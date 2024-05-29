#include "FFmpegWrapper.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libswresample/swresample.h>
}

#pragma region Ctors

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
void MyEq::FFmpegWrapper::readPackets(FileWriter<uint8_t> fw)
{
    fifo = av_audio_fifo_alloc(AV_SAMPLE_FMT_FLTP, 2, 1);

    // Temporary packet and frame
    packet = av_packet_alloc();
    frame = av_frame_alloc();

    while (true) {
        av_read_frame(formatContext, packet);

        int sendPacket = avcodec_send_packet(codecContext, packet);
        int receiveFrame = avcodec_receive_frame(codecContext, frame);

        if (receiveFrame != 0 || sendPacket != 0) {
            av_packet_unref(packet);
            av_frame_unref(frame);
            break;
        }

        uint8_t* outputBuffer;
        av_samples_alloc(&outputBuffer, nullptr, 2, frame->nb_samples, AV_SAMPLE_FMT_FLTP, 0);
        int numSamplesOutput = swr_convert(swrContext, &outputBuffer, frame->nb_samples,
            (const uint8_t**)frame->data, frame->nb_samples);

        int newNumSamples = numSamplesOutput / 100; // pitchShiftFactor

        // Process audio here (e.g., save to file, play audio, etc.)
        addFilters();

        // save this

        //int calculatedBufferSize = av_samples_get_buffer_size(nullptr, 2, frame->nb_samples, AV_SAMPLE_FMT_FLTP, 0) / 100;
        int calculatedBufferSize = av_samples_get_buffer_size(nullptr, 2, newNumSamples, AV_SAMPLE_FMT_FLTP, 0);
        av_audio_fifo_write(fifo, (void**)&outputBuffer, newNumSamples);
        fw.WriteToFile(outputBuffer, calculatedBufferSize);

        av_packet_unref(packet);
        av_frame_unref(frame);
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
        if (avformat_open_input(&formatContext, inputFileOrDevice.c_str(), nullptr, nullptr) < 0) {
            return -1; // file couldn't be opened
        }

        if (avformat_find_stream_info(formatContext, nullptr) < 0) {
            return -1; // stream info couldn't be found
        }

        // find audio stream
        int audioStreamIndex = -1;
        for (unsigned int i = 0; i < formatContext->nb_streams; i++) {
            if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                audioStreamIndex = i;
                break;
            }
        }

        codec = avcodec_find_decoder(formatContext->streams[audioStreamIndex]->codecpar->codec_id);

        if (codec == NULL) {
            return -1; // codec couldn't be found
        }

        codecContext = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(codecContext, formatContext->streams[audioStreamIndex]->codecpar);
        
        if (avcodec_open2(codecContext, codec, nullptr) < 0) {
            return -1; // codec couldn't be opened
        }

        // log layout description
#if _DEBUG
        char chLayoutDescription[128];
        int sts = av_channel_layout_describe(&formatContext->streams[audioStreamIndex]->codecpar->ch_layout, chLayoutDescription, sizeof(chLayoutDescription));
        std::cout << chLayoutDescription << std::endl;
#endif

        swrContext = swr_alloc();
        swr_alloc_set_opts2(&swrContext, 
            &formatContext->streams[audioStreamIndex]->codecpar->ch_layout, codecContext->sample_fmt, outputSampleRate,
            &formatContext->streams[audioStreamIndex]->codecpar->ch_layout, codecContext->sample_fmt, inputSampleRate,
            3, NULL);

        swr_init(swrContext);
    }
    catch (std::exception ex)
    {
#if _DEBUG
        std::cout << ex.what() << std::endl;
#endif // write to log else
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

void MyEq::FFmpegWrapper::transformInput(std::string input, std::string output)
{
    init(input);
    FileWriter<uint8_t> fw(output);
    readPackets(fw);
    cleanup();
}

#pragma endregion