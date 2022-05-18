#pragma once

struct Platform {
	int x, y;
	char length;
};

class PlatformManager {
private:
	std::random_device rand_dev;
	std::mt19937 generator;
	std::uniform_int_distribution<> disx;
	std::uniform_int_distribution<> disy;
	std::uniform_int_distribution<> len;
	Graphics* gfx;
	Graphics::Sprite *platform_sprite;
	std::vector<Platform> platforms;
public:
	std::vector<Platform> GetPlatformVec() { return platforms; }
	PlatformManager(Graphics* graphics, Graphics::Sprite* _platform_sprite) {
		gfx = graphics;
		platform_sprite = _platform_sprite;
		generator = std::mt19937(rand_dev());
		disx = std::uniform_int_distribution<>(150, 350);
		disy = std::uniform_int_distribution<>(-50, 50);
		len = std::uniform_int_distribution<>(3, 7);
		GeneratePlatforms();
	}
	void GeneratePlatforms() {
		platforms.clear();
		platforms.push_back({ 0, -PLAYER_HEIGHT, 5 });
		for (int i = 0; i < PLATFORM_NUM_ON_SCR; i++) {
			Platform new_platform = {};
			Platform last_platform = platforms[platforms.size() - 1];
			int x = disx(generator);
			int y = disy(generator);
			new_platform.x = last_platform.x + last_platform.length * PLATFORM_WIDTH + x;
			new_platform.y = last_platform.y + y;
			new_platform.length = len(generator);
			platforms.push_back(new_platform);
		}
	}
	// i = index of the "old" platform
	void ReplaceOldPlatform(char i) {
		platforms.erase(platforms.begin() + i, platforms.begin() + i + 1);
		i--;
		Platform new_platform = {};
		Platform last_platform = platforms[platforms.size() - 1];
		int x = disx(generator);
		int y = disy(generator);
		new_platform.x = last_platform.x + last_platform.length * PLATFORM_WIDTH + x;
		new_platform.y = last_platform.y + y;
		new_platform.length = len(generator);
		platforms.push_back(new_platform);
	}
	void Draw(float camera_x) {
		for (int i = 0; i < PLATFORM_NUM_ON_SCR; i++) {
			// If a platform is out of screent (left) erase it and push a new one
			if (platforms[i].x + platforms[i].length * PLATFORM_WIDTH < camera_x) {
				ReplaceOldPlatform(i--);
			}
			for (int j = 0; j < platforms[i].length; j++) {
				gfx->DrawSprite(*platform_sprite, platforms[i].x + j * PLATFORM_WIDTH, platforms[i].y);
			}
		}
	}
};
