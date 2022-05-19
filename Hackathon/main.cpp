#define PLATFORM_WIDTH 50
#define PLAYER_WIDTH 50
#define PLAYER_HEIGHT 50
#define PLATFORM_NUM_ON_SCR 2

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

void SaveControls(char *controls) {
	std::ofstream file("res/controls", std::ios::out | std::ios::trunc);
	for (int i = 0; i < 3; i++) {
		if (isalnum(controls[i])) file << controls[i] << " ";
		else if (controls[i] == VK_SPACE) file << "SPACE ";
		else if (controls[i] == VK_RETURN) file << "ENTER ";
		else if (controls[i] == VK_LSHIFT) file << "SHIFT ";
		else if (controls[i] == VK_CONTROL) file << "CTRL ";
	}
}

void ReadControls(char* controls) {
	std::ifstream file("res/controls");
	std::string str;
	for (int i = 0; i < 3; i++) {
		file >> str;
		if (str.size() == 1) {
			if (isalnum(str[0])) controls[i] = str[0];
		}
		else {
			if (str == "SPACE") controls[i] = VK_SPACE;
			if (str == "ENTER") controls[i] = VK_RETURN;
			if (str == "SHIFT") controls[i] = VK_LSHIFT;
			if (str == "CONTROL") controls[i] = VK_CONTROL;
		}
	}
	file.close();
}

char GetModeFromFile() {
	char result = MODE_NORMAL;
	std::string str;
	std::ifstream file("res/mode_to_load");
	file >> str;
	if (str == "MODE_NORMAL") return MODE_NORMAL;
	else if (str == "MODE_RANDOM_JUMP") result = MODE_RANDOM_JUMP;
	else if (str == "MODE_NO_LEFT") result = MODE_NO_LEFT;
	else if (str == "MODE_BUNNYHOP") result = MODE_BUNNYHOP;
	else if (str == "MODE_WIND") result = MODE_WIND;
	else if (str == "MODE_GLIDING") result = MODE_GLIDING;
	else if (str == "MODE_BIG_JUMPS") result = MODE_BIG_JUMPS;
	else if (str == "MODE_SPEED") result = MODE_SPEED;
	else if (str == "MODE_SLIPPERY") result = MODE_SLIPPERY;
	file.close();
	return result;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cli_args, int cli_args_num) {
	bool change_modes_rand = 0;

	std::fstream file;

	// Loading the controls
	char controls[3] = {};
	ReadControls(controls);

	UINT highscore = 0;
	// Loading high score
	file.open("res/highscore.hs");
	file >> highscore;
	file.close();

	Input input;

	Graphics gfx("Hackathon", 800, 600, instance);
	gfx.HandleMessages();

	Graphics::Sprite platform_sprites[9];

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

	PlatformManager platforms(&gfx);

	char mode;
	if (change_modes_rand) mode = MODE_NORMAL;
	else mode = GetModeFromFile();
	Player p(&gfx, &mode, controls);
	UINT score = 0;
	short int camera_max_offset = 40.0f;

	// Jump mode
	float jump_mode_curr_time = 0;
	float jump_mode_time = 2000;
	float jump_mode_rad = 0;
	std::random_device rand_dev;
	std::mt19937 generator;
	std::uniform_real_distribution<> jump_mode_rand_dist(1000.0f, 2000.0f);

	float delta_time = 0.0f;
	bool ignore_next_delta_time = 0;
	bool difficulty = 0, on_music = 0;

	PlaySound(TEXT("res/music.wav"), NULL, SND_LOOP | SND_ASYNC);
	// Controls screen
	bool should_loop = true;
	Graphics::Sprite first_screen;
	gfx.LoadSprite(L"res/controls.png", &first_screen);
	first_screen.width = gfx.GetWindowWidth();
	first_screen.height = gfx.GetWindowHeight();
	while (should_loop) {
		input.Update();
		should_loop = !input.AnyKeyPressed();
		gfx.BeginFrame();
		gfx.DrawSprite(first_screen, 0, 0);
		gfx.EndFrame();
	}

	// Units to next debuf
	short int units_to_next_debuf = 0;
	short int units_between_debufs = 150;
	std::uniform_int_distribution<> mode_dist(0, 8);
	bool should_draw_debuf = 1;
	
	Menu menu;

	float camera_speed = 0.004f;
	while (1) {
		auto start = std::chrono::high_resolution_clock::now();

		if (ignore_next_delta_time) {
			ignore_next_delta_time = 0;
			delta_time = 0;
		}
		p.UpdateInput(&input);

		// Menu
		if (input.GetKeyPressed(VK_ESCAPE)) {
			PlaySound(NULL, 0, 0);
			float last_camera_x = gfx.GetCameraX(), last_camera_y = gfx.GetCameraY();
			gfx.SetCamera(gfx.GetWindowWidth() / 2, gfx.GetWindowHeight() / 2);
			Menu::MenuResults menu_act = menu.DrawMenu(&gfx, difficulty, on_music, controls);
			SaveControls(controls);
			if (menu_act.difficulty) difficulty = !difficulty;
			if (difficulty == 0) units_between_debufs = 150; // Easy
			else units_between_debufs = 75; // Hard

			if (menu_act.resume)
				ignore_next_delta_time = 1;
			else if (menu_act.restart) {
				if (score > highscore) highscore = score;
				file.open("res/highscore.hs", std::ios::out);
				file << highscore;
				file.close();
				platforms.GeneratePlatforms(mode);
				p.Reset(1);
				score = 0;
				if (change_modes_rand)
					mode = MODE_NORMAL;
			}
			else {
				gfx.~Graphics();
				exit(0);
			}

			if (menu_act.difficulty) {
				difficulty = !difficulty;
			}
			else if (menu_act.music)
				on_music = !on_music;
			if (on_music)
				PlaySound(TEXT("res/music.wav"), NULL, SND_LOOP | SND_ASYNC);
			else
				PlaySound(NULL, 0, 0);

			gfx.SetCamera(last_camera_x, last_camera_y);
		}
		
		if (p.GetX() / 20 > score) score = p.GetX() / 20;
		units_to_next_debuf = (int)(p.GetX() / 20) % units_between_debufs + 2;

		if (p.GetY() > 1000.0f) {
			// Restart
			if (score > highscore) highscore = score;
			file.open("res/highscore.hs", std::ios::out);
			file << highscore;
			file.close();
			if (change_modes_rand)
				mode = MODE_NORMAL;
			platforms.GeneratePlatforms(mode);
			p.Reset(1);
			score = 0;
		}

		// Mode handling
		if (!should_draw_debuf && units_to_next_debuf > units_between_debufs - 25) {
			debuf_anim.index = 0;
		}
		if (units_to_next_debuf > units_between_debufs) {
			units_to_next_debuf = 0;
			if (change_modes_rand)
				mode = mode_dist(generator);
		}
		should_draw_debuf = 0;
		if (debuf_anim.index < debuf_anim.frame_num - 1)
			should_draw_debuf = 1;

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
			p.SetWindStats();
			break;
		case MODE_GLIDING:
			p.SetGlidingStats();
			if (input.GetKeyDown(VK_SPACE))
				p.SetLowerGravity();
			else
				p.SetNormalGravity();
			break;
		case MODE_BIG_JUMPS:
			p.SetBigJumpsStats();
			break;
		case MODE_SPEED:
			p.SetSpeedStats();
			break;
		case MODE_SLIPPERY:
			p.SetSlipperyStats();
			break;
		default:
			break;
		}

		p.UpdateInput(&input, mode == MODE_NO_LEFT);
		p.UpdateGravity(platforms.GetPlatformVec(), &input, delta_time, mode == MODE_RANDOM_JUMP);
		p.UpdateAnimationState();

		// Smart camera movement
		float cam_X = gfx.GetCameraX() + gfx.GetWindowWidth() / 2, cam_Y = gfx.GetCameraY() + gfx.GetWindowHeight() / 2;
		float dist = sqrt(pow(cam_X - p.GetX() - 250, 2) + pow(cam_Y - p.GetY(), 2));
		if (cam_X - (p.GetX() + 250) > 30)
			cam_X -= delta_time * camera_speed * dist;
		else if (cam_X - (p.GetX() + 250) < -30)
			cam_X += delta_time * camera_speed * dist;

		if (cam_Y - p.GetY() > 80)
			cam_Y -= delta_time * camera_speed * dist;
		else if (cam_Y - p.GetY() < -80)
			cam_Y += delta_time * camera_speed * dist;
		gfx.SetCamera(cam_X, cam_Y);

		input.Update();
		gfx.HandleMessages();
		gfx.BeginFrame();
		gfx.DrawSprite(background, gfx.GetCameraX(), gfx.GetCameraY());

		p.Draw(delta_time);
		auto wstr = std::to_wstring(score) + L"\n" + std::to_wstring(highscore);
		text_format.SetAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
		gfx.DrawText(wstr.c_str(), wstr.size(), text_format, 600, 0, 200, 200, black);
		text_format.SetAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		platforms.Draw(p.GetX() - gfx.GetWindowWidth() / 2, mode, difficulty);
		if (mode == MODE_RANDOM_JUMP) {
			gfx.DrawEllipse(700, 500, 100, 100, white);
			gfx.DrawEllipse(700, 500, jump_mode_rad, jump_mode_rad, white, 0, 1);
		}
		if (should_draw_debuf)
			gfx.DrawAnimationFrame(&debuf_anim, delta_time, gfx.GetCameraX(), gfx.GetCameraY());
		gfx.EndFrame();

		auto end = std::chrono::high_resolution_clock::now();
		delta_time = ((std::chrono::duration<float, std::milli>)(end - start)).count();
	}
}