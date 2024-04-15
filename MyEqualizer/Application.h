#pragma once

namespace MyEq {
	class UIRenderer {
	public:
		UIRenderer();
		~UIRenderer();
		void RenderUI();

	private:
		void CreateDockSpace(bool& opt_fullscreen, bool& opt_padding);
	};
}