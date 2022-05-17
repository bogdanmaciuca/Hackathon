#include "inc/util.h"
#include "menu.h"
#include <chrono>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "windowscodecs")
#pragma comment(lib, "ole32")

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cli_args, int cli_args_num) {
	Input input;
	
	Graphics gfx("Hackathon", 800, 600, instance);
	Graphics::Sprite johnny;
	gfx.LoadSprite(L"image.png", &johnny);
	float angle = 0.0f;
	float delta_time = 0.0f;
	while (1) {
		auto start = std::chrono::high_resolution_clock::now();
		angle -= 0.5f;

		if (input.GetKeyPressed(VK_SPACE)) OutputDebugString(L"pressed\n");
		if (input.GetKeyDown(VK_SPACE)) OutputDebugString(L"down\n");
		if (input.GetKeyReleased(VK_SPACE)) OutputDebugString(L"released\n");


		input.Update();
		gfx.HandleMessages();
		gfx.BeginFrame();
		gfx.DrawSprite(johnny, 100, 0, 0);
		gfx.EndFrame();

		auto end = std::chrono::high_resolution_clock::now();
		delta_time = ((std::chrono::duration<float, std::milli>)(end - start)).count();
	}
}