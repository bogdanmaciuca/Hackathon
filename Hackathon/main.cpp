#include "inc/util.h"
#include "menu.h"
#include <chrono>
#include <vector>
#include <random>

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "windowscodecs")
#pragma comment(lib, "ole32")

struct Platform {
	int x, y;
	char length;
};

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cli_args, int cli_args_num) {
	Input input;
	
	Graphics gfx("Hackathon", 800, 600, instance);
	
	Graphics::Sprite platform;
	gfx.LoadSprite(L"platform.png", &platform);
	platform.width = 69;
	platform.height = platform.src_height * platform.width / platform.src_width;

	std::random_device rand_dev;
	std::mt19937 generator(rand_dev());
	std::uniform_int_distribution<> disx(150, 350);
	std::uniform_int_distribution<> disy(-50, 50);
	std::uniform_int_distribution<> len(3, 7);

	std::vector<Platform> platforms = { {0, 0, 5} };
	platforms.reserve(1024);
	for (int i = 1; i < 1024; i++) {
		Platform new_platform = {};
		Platform last_platform = platforms[platforms.size() - 1];
		int x = disx(generator);
		int y = disy(generator);
		new_platform.x = last_platform.x + last_platform.length * platform.width + x;
		new_platform.y = last_platform.y + y;
		new_platform.length = len(generator);
		platforms.push_back(new_platform);
	}

	Player p(&gfx);

	float delta_time = 0.0f;
	while (1) {
		auto start = std::chrono::high_resolution_clock::now();

		p.UpdateInput(&input);

		if (input.GetKeyPressed(VK_ESCAPE)) {
			float last_camera_x = gfx.GetCameraX(), last_camera_y = gfx.GetCameraY();
			gfx.SetCamera(gfx.GetWindowWidth() / 2, gfx.GetWindowHeight() / 2);
			DrawMenu(&gfx);
			gfx.SetCamera(last_camera_x, last_camera_y);

		}
		input.Update();
		gfx.HandleMessages();
		gfx.SetCamera(p.GetX(), p.GetY());
		gfx.BeginFrame();
		for (int i = 0; i < 1024; i++) {
			for (int j = 0; j < platforms[i].length; j++) {
				gfx.DrawSprite(platform, platforms[i].x + j * platform.width, platforms[i].y);
			}
		}
		p.Draw(delta_time);
		gfx.EndFrame();

		auto end = std::chrono::high_resolution_clock::now();
		delta_time = ((std::chrono::duration<float, std::milli>)(end - start)).count();
	}
}