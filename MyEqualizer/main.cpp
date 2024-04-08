#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/audio_fifo.h>
#include <libswresample/swresample.h>
}


void init() {
	AVFormatContext* formatContext = nullptr;
	AVCodecContext* codecContext = nullptr;
	AVCodec* codec = nullptr;
}

int main() {
	
	// initialize ffmpeg and qt
	init();


	return 0;
}
