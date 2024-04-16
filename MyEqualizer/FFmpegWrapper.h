#pragma once

#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libswresample/swresample.h>
}

class FFmpegWrapper {
private:
	AVFormatContext* formatContext = nullptr;
	AVCodecContext* codecContext = nullptr;
	AVCodec* codec = nullptr;
	AVAudioFifo* fifo = nullptr;
	SwrContext* swrContext = nullptr;
	AVPacket* packet = nullptr;
	AVFrame* frame = nullptr;

	bool filtersEnabled = true; // it can be switched later

	std::string inputDeviceName = "UNKNOWN";

	long inputSampleRate = 44100;
	long outputSampleRate = 44100;

	void readDataPacket();
	void writeDataPacket();

	void addPitch(double pitchShiftFactor);
public:
	FFmpegWrapper();
	FFmpegWrapper(std::string inputDevice, long inputSample, long outputSample);

	int init();
	void cleanup();
	void changeInputAudio();

	bool getFiltersEnabled();
	long getInputSampleRate();
	long getOutputSampleRate();
	std::string getInputDeviceName();

	void setFiltersEnabled(bool value);
	void setInputSampleRate(long value);
	void setOutputSampleRate(long value);
	void setInputDeviceName(std::string value);
};
