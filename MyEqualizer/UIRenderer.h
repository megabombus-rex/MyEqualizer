#pragma once

namespace MyEq {
	class UIRenderer {
	public:
		UIRenderer();
		~UIRenderer();
		void RenderUI();

	private:
		bool opt_fullscreen = true;
		bool opt_padding = false;

		void CreateDockSpace(bool& opt_fullscreen, bool& opt_padding);
	};
}