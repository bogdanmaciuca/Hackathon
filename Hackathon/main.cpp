#define PLATFORM_WIDTH 69
#define PLAYER_WIDTH 69
#define PLAYER_HEIGHT 69
#define PLATFORM_NUM_ON_SCR 10

enum {
	MODE_NORMAL,
	MODE_RANDOM_JUMP,
	MODE_NO_LEFT,
	MODE_BUNNYHOP,
	MODE_WIND,
	MODE_GLIDING,
	MODE_BIG_JUMPS,
	MODE_SPEED,
	MODE_SLIPPERY
};

#include <chrono>
#include <vector>
#include <random>
#include <string>
#include <fstream>
#include "inc/2d_graphics.h"
#include "inc/input.h"
#include "menu.h"
#include "platforms.h"
#include "player.h"

#pragma comment(lib, "d2d1")
#pragma comment(lib, "dwrite")
#pragma comment(lib, "windowscodecs")
#pragma comment(lib, "ole32")
#pragma comment(lib, "winmm")

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cli_args, int cli_args_num) {
	UINT highscore = 0;
	// Loading high score
	std::fstream file;
	file.open("res/highscore.hs");
	file >> highscore;
	file.close();

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
	Graphics::Animation debuf_anim;
	gfx.LoadAnimation(L"res/debuf.png", &debuf_anim, 20.0f, 26);
	debuf_anim.sprite.width = gfx.GetWindowWidth();
	//debuf_anim.sprite.height = debuf_anim.sprite.width / debuf_anim.sprite.src_width * debuf_anim.src_frame_h;
	debuf_anim.sprite.height = 100;
	Graphics::TextFormat text_format;
	gfx.CreateTextFormat(L"Arial", 40.0f, &text_format, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL);
	float black[] = { 0, 0, 0, 1 };
	float white[] = { 1, 1, 1, 1 };
	
	PlatformManager platforms(&gfx, &platform);

	char mode = MODE_NORMAL;
	Player p(&gfx, &mode);
	UINT score = 0;

	// Jump mode
	float jump_mode_curr_time = 0;
	float jump_mode_time = 2000;
	float jump_mode_rad = 0;
	std::random_device rand_dev;
	std::mt19937 generator;
	std::uniform_real_distribution<> jump_mode_rand_dist(1000.0f, 2000.0f);

	float delta_time = 0.0f;
	bool ignore_next_delta_time = 0;
	bool difficulty= 0, on_music =1;

	PlaySound(TEXT("res/music.wav"), NULL, SND_LOOP | SND_ASYNC);
	bool should_loop = true;
	Graphics::Sprite first_screen;
	gfx.LoadSprite(L"res/first.png", &first_screen);
	first_screen.width = gfx.GetWindowWidth();
	first_screen.height = gfx.GetWindowHeight();
	while (should_loop)
	{
		input.Update();
		should_loop = !input.AnyKeyPressed();
		gfx.BeginFrame();
		gfx.DrawSprite(first_screen,0,0);
		gfx.EndFrame();
	}
	while (1) {
		// Handles a menu glitch
		if (ignore_next_delta_time) {
			ignore_next_delta_time = 0;
			delta_time = 0;
		}
		auto start = std::chrono::high_resolution_clock::now();

		p.UpdateInput(&input);

		if (input.GetKeyPressed(VK_ESCAPE)) {
			PlaySound(NULL, 0, 0);
			float last_camera_x = gfx.GetCameraX(), last_camera_y = gfx.GetCameraY();
			gfx.SetCamera(gfx.GetWindowWidth() / 2, gfx.GetWindowHeight() / 2);
			MenuResults menu_act = DrawMenu(&gfx,difficulty, on_music);
			if (menu_act.resume)
				ignore_next_delta_time = 1;
			else if (menu_act.restart)
			{
				if (score > highscore) highscore = score;
				file.open("res/highscore.hs", std::ios::out);
				file << highscore;
				file.close();
				platforms.GeneratePlatforms();
				p.Reset();
				score = 0;
			}
			else;///exit

			if (menu_act.difficulty)
			{
				difficulty = !difficulty;
				//.....
			}
			else if (menu_act.music)			
				on_music = !on_music;			
			if (on_music)
				PlaySound(TEXT("res/music.wav"), NULL, SND_LOOP | SND_ASYNC);
			else
				PlaySound(NULL, 0, 0);
			
			gfx.SetCamera(last_camera_x, last_camera_y);
		}
		p.UpdateInput(&input, mode == MODE_NO_LEFT);
		p.UpdateGravity(platforms.GetPlatformVec(), &input, delta_time, mode == MODE_RANDOM_JUMP);
		p.UpdateAnimationState();
		if (p.GetX() / 20 > score) score = p.GetX() / 20;

		if (p.GetY() > 1000.0f) {
			// Restart
			if (score > highscore) highscore = score;
			file.open("res/highscore.hs", std::ios::out);
			file << highscore;
			file.close();
			platforms.GeneratePlatforms();
			p.Reset();
			score = 0;
		}

		switch (mode) {
		case MODE_NORMAL:
			p.SetNormalStats();
			break;
		case MODE_RANDOM_JUMP: {
			p.SetRandomJumpStats();
			// Draw a circle every frame with a disc inside to represent the time left until next jump
			// Also pick a random time between 1 and 2 seconds when the timer finishes
			if (jump_mode_curr_time > jump_mode_time) {
				jump_mode_curr_time = 0;
				p.Jump();
				jump_mode_time = jump_mode_rand_dist(generator);
			}
			jump_mode_rad = jump_mode_curr_time / jump_mode_time * 100;
			jump_mode_curr_time += delta_time;
		} break;
		case MODE_NO_LEFT: // Done in player input function
			p.SetNoLeftStats();
			break;
		case MODE_BUNNYHOP:
			p.SetBunnyhopStats();
			p.Jump();
			break;
		case MODE_WIND:
			break;
		case MODE_GLIDING:
			p.SetGlidingStats();
			break;
		case MODE_BIG_JUMPS:
			p.SetBigJumpsStats();
			break;
			//case MODE_SPEED
		case MODE_SLIPPERY:
			p.SetSlipperyStats();
			break;
		default:
			break;
		}

		input.Update();
		gfx.HandleMessages();
		gfx.SetCamera(p.GetX(), p.GetY());
		gfx.BeginFrame();
		gfx.DrawSprite(background, p.GetX() - gfx.GetWindowWidth() / 2, p.GetY() - gfx.GetWindowHeight() / 2);
		
		p.Draw(delta_time);
		auto wstr = std::to_wstring(score) + L"\n" + std::to_wstring(highscore);
		text_format.SetAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		gfx.DrawText(wstr.c_str(), wstr.size(), text_format, 600, 0, 200, 200, black);
		text_format.SetAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		platforms.Draw(p.GetX() - gfx.GetWindowWidth() / 2);
		if (mode == MODE_RANDOM_JUMP) {
			gfx.DrawEllipse(700, 500, 100, 100, white);
			gfx.DrawEllipse(700, 500, jump_mode_rad, jump_mode_rad, white, 0, 1);
		}
		gfx.DrawAnimationFrame(&debuf_anim, delta_time, p.GetX() - gfx.GetWindowWidth() / 2, p.GetY() - gfx.GetWindowHeight() / 2 + 100);
		gfx.EndFrame();

		auto end = std::chrono::high_resolution_clock::now();
		delta_time = ((std::chrono::duration<float, std::milli>)(end - start)).count();
	}
}