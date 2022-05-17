#define PLATFORM_WIDTH 69
#define PLAYER_WIDTH 69
#define PLAYER_HEIGHT 69
#define PLATFORM_NUM_ON_SCR 10

#include "inc/util.h"
#include "menu.h"
#include "player.h"

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "windowscodecs")
#pragma comment(lib, "ole32")

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cli_args, int cli_args_num) {
	Input input;
	
	Graphics gfx("Hackathon", 800, 600, instance);
	gfx.HandleMessages();

	Graphics::Sprite platform;
	gfx.LoadSprite(L"res/platform.png", &platform);
	platform.width = PLAYER_WIDTH;
	platform.height = PLAYER_HEIGHT;
	Graphics::Sprite background;
	gfx.LoadSprite(L"res/background.png", &background);
	background.width = gfx.GetWindowWidth();
	background.height = gfx.GetWindowHeight();
	Graphics::TextFormat text_format;
	gfx.CreateTextFormat(L"Arial", 40.0f, &text_format, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL);
	float black[4] = { 0, 0, 0, 1 };

	PlatformGenerator platform_gen;
	std::vector<Platform> platforms;
	platform_gen.GeneratePlatforms(&platforms);

	Player p(&gfx);
	UINT score = 0;

	float delta_time = 0.0f;
	bool ignore_next_delta_time = 0;
	while (1) {

		if (ignore_next_delta_time) {
			ignore_next_delta_time = 0;
			delta_time = 0;
		}
		auto start = std::chrono::high_resolution_clock::now();

		p.UpdateInput(&input);

		if (input.GetKeyPressed(VK_ESCAPE)) {
			float last_camera_x = gfx.GetCameraX(), last_camera_y = gfx.GetCameraY();
			gfx.SetCamera(gfx.GetWindowWidth() / 2, gfx.GetWindowHeight() / 2);
			switch (DrawMenu(&gfx)) {
			case 1: // Resume
				ignore_next_delta_time = 1;
				break;
			case 2:
				// Restart
				platform_gen.GeneratePlatforms(&platforms);
				p.Reset();
				score = 0;
				break;
			default:
				break;
			}
			gfx.SetCamera(last_camera_x, last_camera_y);

		}
		p.UpdateInput(&input);
		p.UpdateGravity(platforms, &input, delta_time);
		p.UpdateAnimationState();
		if (p.GetX() / 20 > score) score = p.GetX() / 20;

		if (p.GetY() > 1000.0f) {
			// Restart
			platform_gen.GeneratePlatforms(&platforms);
			p.Reset();
			score = 0;
		}

		input.Update();
		gfx.HandleMessages();
		gfx.SetCamera(p.GetX(), p.GetY());
		gfx.BeginFrame();
		gfx.DrawSprite(background, p.GetX() - gfx.GetWindowWidth() / 2, p.GetY() - gfx.GetWindowHeight() / 2);
		for (int i = 0; i < 10; i++) {
			// If a platform is out of screent (left) erase it and push a new one
			if (platforms[i].x + platforms[i].length * platform.width < p.GetX() - gfx.GetWindowWidth() / 2) {
				platform_gen.ReplaceOldPlatform(&platforms, i--);
			}
			for (int j = 0; j < platforms[i].length; j++) {
				gfx.DrawSprite(platform, platforms[i].x + j * platform.width, platforms[i].y);
			}
		}
		p.Draw(delta_time);
		auto wstr = std::to_wstring(score);
		text_format.SetAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		gfx.DrawText(wstr.c_str(), wstr.size(), text_format, 600, 0, 200, 100, black);
		text_format.SetAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		gfx.EndFrame();

		auto end = std::chrono::high_resolution_clock::now();
		delta_time = ((std::chrono::duration<float, std::milli>)(end - start)).count();
	}
}