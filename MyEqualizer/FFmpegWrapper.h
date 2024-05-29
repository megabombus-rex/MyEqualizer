#pragma once

#include <iostream>
#include "FileWriter.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libswresample/swresample.h>
}

#define MAX_AUDIO_FRAME_SIZE 192000 // 1 second of 48khz 32bit audio

namespace MyEq {

	class FFmpegWrapper {
	private:
		AVFormatContext*		formatContext = NULL;
		AVCodecContext*			codecContext = NULL;
		const AVCodec*			codec = NULL;
		const AVChannelLayout*	layout = NULL;
		AVAudioFifo*			fifo = NULL;
		SwrContext*				swrContext = NULL;
		AVPacket*				packet = NULL;
		AVFrame*				frame = NULL;

		bool filtersEnabled = true; // it can be switched later

		std::string inputDeviceName = "UNKNOWN";

		long inputSampleRate = 44100;
		long outputSampleRate = 44100;

		void readPackets(FileWriter<uint8_t> fw);
		void addFilters();

	public:
		FFmpegWrapper(std::string inputDevice = "UNKNOWN", long inputSample = 44100, long outputSample = 44100);

		int init(std::string inputFileOrDevice);
		void cleanup();

		bool getFiltersEnabled();
		long getInputSampleRate();
		long getOutputSampleRate();
		std::string getInputDeviceName();

		void setFiltersEnabled(bool value);
		void setInputSampleRate(long value);
		void setOutputSampleRate(long value);
		void setInputDeviceName(std::string value);

		void transformInput(std::string input, std::string output);
	};

}
