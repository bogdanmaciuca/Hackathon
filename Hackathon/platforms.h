#pragma once

struct Platform {
	int x, y;
	char length;
};

void GetCoefForMode(char mode, float *mode_coef_x, float *mode_coef_y, float *mode_len_coef) {
	switch (mode) {
	case MODE_NORMAL:
		*mode_coef_x = 0.8f, *mode_coef_y = 1, *mode_len_coef = 1.0f;
		break;
	case MODE_RANDOM_JUMP:
		*mode_coef_x = 0.75f, *mode_coef_y = 1.5f, *mode_len_coef = 1.25f;
		break;
	case MODE_NO_LEFT:
		*mode_coef_x = 1.5f, *mode_coef_y = 1, *mode_len_coef = 1.0f;
		break;
	case MODE_BUNNYHOP:
		*mode_coef_x = 1.5f, *mode_coef_y = 1.5f, *mode_len_coef = 0.75f;
		break;
	case MODE_WIND:
		*mode_coef_x = 2.0f, *mode_coef_y = 1, *mode_len_coef = 1.25f;
		break;
	case MODE_GLIDING:
		*mode_coef_x = 2.0f, *mode_coef_y = 0.75f, *mode_len_coef = 1.0f;
		break;
	case MODE_BIG_JUMPS:
		*mode_coef_x = 3.0f, *mode_coef_y = 2.0f, *mode_len_coef = 1.0f;
		break;
	case MODE_SPEED:
		*mode_coef_x = 2.0f, *mode_coef_y = 1.0f, *mode_len_coef = 1.5f;
		break;
	case MODE_SLIPPERY:
		*mode_coef_x = 1.0f, *mode_coef_y = 1.0f, *mode_len_coef = 2.75f;
		break;
	default:
		*mode_coef_x = 1, *mode_coef_y = 1, *mode_len_coef = 1.0f;
		break;
	}
}

class PlatformManager {
private:
	std::random_device rand_dev;
	std::mt19937 generator;
	std::uniform_int_distribution<> disx;
	std::uniform_int_distribution<> disy;
	std::uniform_int_distribution<> len;
	Graphics* gfx;
	std::vector<Platform> platforms;
	Graphics::Sprite platform_sprites[9];
	Graphics::Sprite platform_sprite;
public:
	std::vector<Platform> GetPlatformVec() { return platforms; }
	PlatformManager(Graphics* graphics) {
		gfx = graphics;
		
		gfx->LoadSprite(L"res/platform.png", &platform_sprite);
		platform_sprite.width = PLAYER_WIDTH;
		platform_sprite.height = PLAYER_HEIGHT;
		
		gfx->LoadSprite(L"res/platform_normal.png", &platform_sprites[MODE_NORMAL]);
		gfx->LoadSprite(L"res/platform_random_jump.png", &platform_sprites[MODE_RANDOM_JUMP]);
		gfx->LoadSprite(L"res/platform_no_left.png", &platform_sprites[MODE_NO_LEFT]);
		gfx->LoadSprite(L"res/platform_bunnyhop.png", &platform_sprites[MODE_BUNNYHOP]);
		gfx->LoadSprite(L"res/platform_wind.png", &platform_sprites[MODE_WIND]);
		gfx->LoadSprite(L"res/platform_gliding.png", &platform_sprites[MODE_GLIDING]);
		gfx->LoadSprite(L"res/platform_big_jumps.png", &platform_sprites[MODE_BIG_JUMPS]);
		gfx->LoadSprite(L"res/platform_speed.png", &platform_sprites[MODE_SPEED]);
		gfx->LoadSprite(L"res/platform_slippery.png", &platform_sprites[MODE_SLIPPERY]);
		for (int i = 0; i < 9; i++) {
			platform_sprites[i].width = PLAYER_WIDTH;
			platform_sprites[i].height = PLAYER_HEIGHT;
		}

		generator = std::mt19937(rand_dev());
		disx = std::uniform_int_distribution<>(200, 300);
		disy = std::uniform_int_distribution<>(-75, 75);
		len = std::uniform_int_distribution<>(12, 16);
		GeneratePlatforms(MODE_NORMAL);
	}
	void GeneratePlatforms(char mode) {
		platforms.clear();
		platforms.push_back({ 0, -PLAYER_HEIGHT, 5 });
		for (int i = 0; i < PLATFORM_NUM_ON_SCR - 1; i++) {
			Platform new_platform = {};
			Platform last_platform = platforms[platforms.size() - 1];
			float mode_coef_x, mode_coef_y, mode_coef_len;
			GetCoefForMode(mode, &mode_coef_x, &mode_coef_y, &mode_coef_len);
			int x = disx(generator) * mode_coef_x;
			int y = disy(generator) * mode_coef_y;
			new_platform.x = last_platform.x + last_platform.length * PLATFORM_WIDTH + x;
			new_platform.y = last_platform.y + y;
			new_platform.length = len(generator) * mode_coef_len;
			platforms.push_back(new_platform);
		}
	}
	// i = index of the "old" platform
	void ReplaceOldPlatform(char i, char mode) {
		platforms.erase(platforms.begin() + i, platforms.begin() + i + 1);
		i--;
		Platform new_platform = {};
		Platform last_platform = platforms[platforms.size() - 1];
		float mode_coef_x, mode_coef_y, mode_coef_len;
		GetCoefForMode(mode, &mode_coef_x, &mode_coef_y, &mode_coef_len);
		int x = disx(generator) * mode_coef_x;
		int y = disy(generator) * mode_coef_y;
		new_platform.x = last_platform.x + last_platform.length * PLATFORM_WIDTH + x;
		new_platform.y = last_platform.y + y;
		new_platform.length = len(generator) * mode_coef_len;
		platforms.push_back(new_platform);
	}
	void Draw(float camera_x, char mode, bool difficulty) {
		for (int i = 0; i < PLATFORM_NUM_ON_SCR; i++) {
			// If a platform is out of screent (left) erase it and push a new one
			if (platforms[i].x + platforms[i].length * PLATFORM_WIDTH < camera_x) {
				ReplaceOldPlatform(i--, mode);
			}
			for (int j = 0; j < platforms[i].length; j++) {
				gfx->DrawSprite(difficulty ? platform_sprite : platform_sprites[mode], platforms[i].x + j * PLATFORM_WIDTH, platforms[i].y);
			}
		}
	}
};
