#include <iostream>

#pragma once

namespace MyEq {
	class UIRenderer {
	public:
		UIRenderer();
		~UIRenderer();
		void RenderUI();

	private:
		bool optFullscreen = true;
		bool optFadding = false;
		std::string selectedFile;

		void CreateDockSpace(bool& optFullscreen, bool& optPadding);
		std::string OpenWindowsFile();
	};
}