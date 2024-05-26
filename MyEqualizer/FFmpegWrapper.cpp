#include "FFmpegWrapper.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libswresample/swresample.h>
}

#pragma region Ctors


MyEq::FFmpegWrapper::FFmpegWrapper()
{
    filtersEnabled = true;

    inputSampleRate = 44100;
    outputSampleRate = 44100;

    inputDeviceName = "UNKNOWN";
}

MyEq::FFmpegWrapper::FFmpegWrapper(std::string inputDevice, long inputSample, long outputSample)
{
    filtersEnabled = true;

    inputDeviceName = inputDevice;
    inputSampleRate = inputSample;
    outputSampleRate = outputSample;
}

#pragma endregion

#pragma region Private Methods

// call this only after init and before cleanup
// this can be async as it shouldn't block the UI thread
void MyEq::FFmpegWrapper::readPackets()
{
    // Allocate and initialize audio FIFO buffer
    AVAudioFifo* fifo = av_audio_fifo_alloc(AV_SAMPLE_FMT_FLTP, 2, 1);

    // Temporary packet and frame
    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();

    while (true) {
        // Read audio packet
        av_read_frame(formatContext, packet);

        // Decode audio packet
        int sendPacket = avcodec_send_packet(codecContext, packet);
        int receiveFrame = avcodec_receive_frame(codecContext, frame);

        if (receiveFrame != 0 || sendPacket != 0) {
            av_packet_unref(packet);
            av_frame_unref(frame);
            break;
        }

        // Resample audio frame
        uint8_t* outputBuffer;
        av_samples_alloc(&outputBuffer, nullptr, 2, frame->nb_samples, AV_SAMPLE_FMT_FLTP, 0);
        int numSamplesOutput = swr_convert(swrContext, &outputBuffer, frame->nb_samples,
            (const uint8_t**)frame->data, frame->nb_samples);

        int newNumSamples = numSamplesOutput / 100; // pitchShiftFactor

        // Process audio here (e.g., save to file, play audio, etc.)
        addFilters();

        // save this
        av_audio_fifo_write(fifo, (void**)&outputBuffer, newNumSamples);
        av_packet_unref(packet);
        av_frame_unref(frame);

        // break conditions - EOF, empty samples, to check
    }

    av_audio_fifo_free(fifo);
    av_frame_free(&frame);
    av_packet_free(&packet);
}

#pragma endregion


#pragma region Public Methods

int MyEq::FFmpegWrapper::init(std::string inputFileOrDevice)
{
    try {
        avformat_open_input(&formatContext, inputFileOrDevice.c_str(), nullptr, nullptr);
        avformat_find_stream_info(formatContext, nullptr);
        int audioStreamIndex = -1;
        for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
            if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                audioStreamIndex = i;
                break;
            }
        }

        codec = avcodec_find_decoder(formatContext->streams[audioStreamIndex]->codecpar->codec_id);
        codecContext = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(codecContext, formatContext->streams[audioStreamIndex]->codecpar);
        avcodec_open2(codecContext, codec, nullptr);

        // Set up resampler

        layout = av_channel_layout_standard(NULL);

        swrContext = swr_alloc();
        swr_alloc_set_opts2(&swrContext, 
            layout, codecContext->sample_fmt, outputSampleRate, 
            layout, codecContext->sample_fmt, inputSampleRate,
            3, NULL);
        //swr_alloc_set_opts2(&swrContext, );

        swr_init(swrContext);
    }
    catch (std::exception ex)
    {
        std::cout << ex.what() << std::endl;
        return 1;
    }

    return 0;
}

void MyEq::FFmpegWrapper::cleanup()
{
    swr_free(&swrContext);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
}

// call this method to add filters to the packets
// use flags to know which filters to apply
void MyEq::FFmpegWrapper::addFilters()
{
}

#pragma region Getters & Setters


bool MyEq::FFmpegWrapper::getFiltersEnabled()
{
    return filtersEnabled;
}

long MyEq::FFmpegWrapper::getInputSampleRate()
{
    return inputSampleRate;
}

long MyEq::FFmpegWrapper::getOutputSampleRate()
{
    return outputSampleRate;
}

std::string MyEq::FFmpegWrapper::getInputDeviceName()
{
    return inputDeviceName;
}

void MyEq::FFmpegWrapper::setFiltersEnabled(bool value)
{
    filtersEnabled = value;
}

void MyEq::FFmpegWrapper::setInputSampleRate(long value)
{
    inputSampleRate = value;
}

void MyEq::FFmpegWrapper::setOutputSampleRate(long value)
{
    outputSampleRate = value;
}

void MyEq::FFmpegWrapper::setInputDeviceName(std::string value)
{
    inputDeviceName = value;
}

#pragma endregion

void MyEq::FFmpegWrapper::pitchInput(double pitchRate, std::string input)
{
    init(input);
    cleanup();
}

#pragma endregion