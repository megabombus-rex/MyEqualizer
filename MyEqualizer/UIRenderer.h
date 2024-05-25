#include <iostream>
#include "FFmpegWrapper.h"

#pragma once

namespace MyEq {
	class UIRenderer {
	public:
		UIRenderer(FFmpegWrapper* wrapper);
		UIRenderer();
		~UIRenderer();
		void RenderUI();
		
		FFmpegWrapper* wrapper;
	private:
		bool optFullscreen = true;
		bool optFadding = false;
		std::string selectedFile;

		void CreateDockSpace(bool& optFullscreen, bool& optPadding);
		std::string OpenWindowsFile();
	};
}