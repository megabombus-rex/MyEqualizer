#include "FFmpegWrapper.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libswresample/swresample.h>
}

#pragma region Ctors


FFmpegWrapper::FFmpegWrapper()
{
    filtersEnabled = true;

    inputSampleRate = 44100;
    outputSampleRate = 44100;

    inputDeviceName = "UNKNOWN";
}

FFmpegWrapper::FFmpegWrapper(std::string inputDevice, long inputSample, long outputSample)
{
    filtersEnabled = true;

    inputDeviceName = inputDeviceName
}

#pragma endregion

#pragma region Public Methods

int FFmpegWrapper::init()
{
    try {
        avformat_open_input(&formatContext, inputDeviceName.c_str(), nullptr, nullptr);
        avformat_find_stream_info(formatContext, nullptr);
        int audioStreamIndex = -1;
        for (int i = 0; i < formatContext->nb_streams; i++) {
            if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                audioStreamIndex = i;
                break;
            }
        }


    }
    catch (std::exception ex)
    {
        std::cout << ex.what() << std::endl;
        return 1;
    }

    return 0;
}

void FFmpegWrapper::cleanup()
{
    av_audio_fifo_free(fifo);
    swr_free(&swrContext);
    avcodec_close(codecContext);
    avformat_close_input(&formatContext);
    av_frame_free(&frame);
    av_packet_free(&packet);
}

void FFmpegWrapper::changeInputAudio()
{
}

#pragma region Getters & Setters


bool FFmpegWrapper::getFiltersEnabled()
{
    return filtersEnabled;
}

long FFmpegWrapper::getInputSampleRate()
{
    return inputSampleRate;
}

long FFmpegWrapper::getOutputSampleRate()
{
    return outputSampleRate;
}

std::string FFmpegWrapper::getInputDeviceName()
{
    return inputDeviceName;
}

void FFmpegWrapper::setFiltersEnabled(bool value)
{
    filtersEnabled = value;
}

void FFmpegWrapper::setInputSampleRate(long value)
{
    inputSampleRate = value;
}

void FFmpegWrapper::setOutputSampleRate(long value)
{
    outputSampleRate = value;
}

void FFmpegWrapper::setInputDeviceName(std::string value)
{
    inputDeviceName = value;
}

#pragma endregion


#pragma endregion

#pragma region Private Methods

void FFmpegWrapper::readDataPacket()
{
}

void FFmpegWrapper::writeDataPacket()
{
}

void FFmpegWrapper::addPitch(double pitchShiftFactor)
{
}

#pragma endregion