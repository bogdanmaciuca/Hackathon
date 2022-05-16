#include "inc/2d_graphics.h"
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "windowscodecs")
#pragma comment(lib, "ole32")

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cli_args, int cli_args_num) {
	Graphics gfx("Hackathon", 800, 600, instance);
	Graphics::Sprite johnny;
	gfx.LoadSprite(L"image.png", &johnny);
	float angle = 0.0f;
	while (1) {
		angle -= 0.5f;
		gfx.HandleMessages();
		gfx.BeginFrame();
		gfx.DrawSprite(johnny, 100, 50, angle);
		gfx.EndFrame();
	}
}