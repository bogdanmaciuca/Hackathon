#include "2d_graphics.h"
#include "input.h"
#include <chrono>
#include <vector>
#include <random>
#include <string>

struct Platform {
	int x, y;
	char length;
};

class PlatformGenerator {
private:
	std::random_device rand_dev;
	std::mt19937 generator;
	std::uniform_int_distribution<> disx;
	std::uniform_int_distribution<> disy;
	std::uniform_int_distribution<> len;
public:
	PlatformGenerator() {
		generator = std::mt19937(rand_dev());
		disx = std::uniform_int_distribution<>(150, 350);
		disy = std::uniform_int_distribution<>(-50, 50);
		len = std::uniform_int_distribution<>(3, 7);
	}
	void GeneratePlatforms(std::vector<Platform>* platforms) {
		platforms->clear();
		platforms->push_back({ 0, -PLAYER_HEIGHT, 5 });
		for (int i = 0; i < PLATFORM_NUM_ON_SCR; i++) {
			Platform new_platform = {};
			Platform last_platform = (*platforms)[platforms->size() - 1];
			int x = disx(generator);
			int y = disy(generator);
			new_platform.x = last_platform.x + last_platform.length * PLATFORM_WIDTH + x;
			new_platform.y = last_platform.y + y;
			new_platform.length = len(generator);
			platforms->push_back(new_platform);
		}
	}
	// i = index of the "old" platform
	void ReplaceOldPlatform(std::vector<Platform>* platforms, char i) {
		platforms->erase(platforms->begin() + i, platforms->begin() + i + 1);
		i--;
		Platform new_platform = {};
		Platform last_platform = (*platforms)[platforms->size() - 1];
		int x = disx(generator);
		int y = disy(generator);
		new_platform.x = last_platform.x + last_platform.length * PLATFORM_WIDTH + x;
		new_platform.y = last_platform.y + y;
		new_platform.length = len(generator);
		platforms->push_back(new_platform);
	}
};

