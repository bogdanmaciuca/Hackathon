#pragma once
class Menu {
private:
	struct Option {
		std::wstring nume;
	};
	Graphics::TextFormat text_format;
	float cul3[4] = { 0.4, 0, 0.4, 0.9 };
	float cul2[4] = { 0.4, 0, 0.4, 0.4 };
	float cul1[4] = { 0.49, 0, 1, 0.2 };
	float black[4] = { 0, 0, 0, 1 };
	int pos = 1, menu_num = 0;
	Option options[6];
	bool up_up = 1, up_down = 1, up_enter = 1;
	int get_nextkey = 0;
public:
	struct MenuResults {
		bool music;
		bool difficulty;
		bool restart;
		bool exit;
		bool resume;
		char controls[3];
	};
	void change_button(int i, std::wstring denum) {
		options[i].nume = denum;
	}
	void DrawOptions(Graphics* gfx, int n, int l, int w, int w_opt) {
		int x1, y1;
		x1 = (gfx->GetWindowWidth() - l) / 2;
		y1 = (gfx->GetWindowHeight() - w) / 2;
		gfx->DrawRect(x1, y1, l, w, cul1, 0, true);
		gfx->CreateTextFormat(L"Arial", 40.0f, &text_format);
		text_format.SetAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		int liber = (w - n * w_opt) / (n + 1), posi = liber + y1;

		for (int i = 1; i <= n; i++) {
			if (pos == i)
				gfx->DrawRect(x1 + 30, posi, l - 60, w_opt, cul3, 0, true);
			else
				gfx->DrawRect(x1 + 30, posi, l - 60, w_opt, cul2, 0, true);
			gfx->DrawText(options[i - 1].nume.c_str(), options[i - 1].nume.size(), text_format, 0, posi + w_opt / 7, gfx->GetWindowWidth(), w_opt, black);
			posi += liber + w_opt;
		}
	}

	void ChangeOption(int& button_num, int& w_opt, int& h, bool dif, bool mus, char* contrl) {
		char text[30];
		if (menu_num == 0) {
			button_num = 4;
			size_t outSize;

			options[0].nume = L"Resume";

			options[1].nume = L"Restart";

			options[2].nume = L"Options";
			
			options[3].nume = L"Exit";

			w_opt = 75;
			h = 400;
		}
		if (menu_num == 1) {
			button_num = 4;
			size_t outSize;
			w_opt = 75;

			if (!dif) {
				options[0].nume = L"Difficulty: Easy";
			}
			else {
				options[0].nume = L"Difficuly: Hard";
			}

			if (!dif) {
				options[1].nume = L"Music ON";
			}
			else {
				options[1].nume = L"Music OFF";
			}

			options[2].nume = L"Controls";
			options[3].nume = L"Exit";
			h = 400;
		}
		if (menu_num == 2) {
			button_num = 4;
			size_t outSize;
			w_opt = 75;
			h = 400;

			std::wstring left_key_str;
			std::wstring right_key_str;
			std::wstring jump_key_str;

			if (isalnum(contrl[0])) left_key_str = { (WCHAR)contrl[0] };
			else if (contrl[0] == VK_SPACE) left_key_str = L"SPACE";
			else if (contrl[0] == VK_RETURN) left_key_str = L"ENTER";
			else if (contrl[0] == VK_LSHIFT) left_key_str = L"SHIFT";
			else if (contrl[0] == VK_CONTROL) left_key_str = L"CONTROL";
			if (isalnum(contrl[1])) right_key_str = { (WCHAR)contrl[1] };
			else if (contrl[1] == VK_SPACE) right_key_str = L"SPACE";
			else if (contrl[1] == VK_RETURN) right_key_str = L"ENTER";
			else if (contrl[1] == VK_LSHIFT) right_key_str = L"SHIFT";
			else if (contrl[1] == VK_CONTROL) right_key_str = L"CONTROL";
			if (isalnum(contrl[2])) jump_key_str = { (WCHAR)contrl[2] };
			else if (contrl[2] == VK_SPACE) jump_key_str = L"SPACE";
			else if (contrl[2] == VK_RETURN) jump_key_str = L"ENTER";
			else if (contrl[2] == VK_LSHIFT) jump_key_str = L"SHIFT";
			else if (contrl[2] == VK_CONTROL) jump_key_str = L"CONTROL";

			options[0].nume = L"Move left: " + left_key_str;
			options[1].nume = L"Move right: " + right_key_str;
			options[2].nume = L"Move jump: " + jump_key_str;
			options[3].nume = L"Exit: ";
		}

	}
	MenuResults DrawMenu(Graphics* gfx, bool init_difficulty, bool init_music, char* contrl) {
		Graphics::Sprite background;
		gfx->LoadSprite(L"res/background.png", &background);
		background.width = gfx->GetWindowWidth();
		background.height = gfx->GetWindowHeight();
		gfx->CreateTextFormat(L"Arial", 40.0f, &text_format);
		int h = 400;
		int nrbutoane = 3, w_but;
		bool difficulty = init_difficulty, on_music = init_music;
		MenuResults result = {};
		ChangeOption(nrbutoane, w_but, h, init_difficulty, init_music, contrl);
		while (1) {
			Input input;
			input.Update(gfx->GetWindowX(), gfx->GetWindowY());
			gfx->HandleMessages();
			gfx->BeginFrame();

			if (get_nextkey) {
				int test = input.GetKeyVK();
				if (test != -1 && (isalnum(test) || test == VK_SPACE || test == VK_LSHIFT || test == VK_CONTROL)) {
					contrl[get_nextkey - 1] = test;
					ChangeOption(nrbutoane, w_but, h, difficulty, on_music, contrl);
					get_nextkey = 0;
				}
			}

			if (input.GetKeyDown(VK_DOWN) && up_down) {
				pos++;
				if (pos > nrbutoane)
					pos = 1;
				up_down = 0;
				PlaySound(TEXT("res/but.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			else if (!input.GetKeyDown(VK_DOWN))
				up_down = 1;

			if (input.GetKeyDown(VK_UP) && up_up) {
				pos--;
				if (pos < 1)
					pos = nrbutoane;
				up_up = 0;
				PlaySound(TEXT("res/but.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			else if (!input.GetKeyDown(VK_UP))
				up_up = 1;

			if (input.GetKeyDown(VK_RETURN) && up_enter) {
				if (menu_num == 0)
					if (pos == 1)
					{
						result.resume = 1;
						return result;
					}
					else if (pos == 2)
					{
						result.restart = true;
						return result;
					}
					else if (pos == 3) {
						PlaySound(TEXT("res/but.wav"), NULL, SND_FILENAME | SND_ASYNC);
						menu_num++;
						ChangeOption(nrbutoane, w_but, h, difficulty, on_music, contrl);
					}
					else
					{
						result.exit = true;
						return result;
					}
				else if (menu_num == 1) {
					PlaySound(TEXT("res/but.wav"), NULL, SND_FILENAME | SND_ASYNC);
					if (pos == 1) {
						if (!difficulty) {
							change_button(0, L"Difficulty: Hard");
							difficulty = 1;
							if (difficulty != init_difficulty)
								result.difficulty = 1;
							else
								result.difficulty = 0;
						}
						else {
							change_button(0, L"Difficulty: Easy");
							difficulty = 0;
							if (difficulty != init_difficulty)
								result.difficulty = 1;
							else
								result.difficulty = 0;
						}
					}
					if (pos == 2) {
						if (on_music) {
							change_button(1, L"Music: Off");
							on_music = 0;
							if (on_music != init_music)
								result.music = 1;
							else
								result.music = 0;
						}
						else {
							change_button(1, L"Music: On");
							on_music = 1;
							if (on_music != init_music)
								result.music = 1;
							else
								result.music = 0;
						}
					}
					if (pos == 3) {
						menu_num++;
						ChangeOption(nrbutoane, w_but, h, difficulty, on_music, contrl);
					}
					if (pos == 4) {
						menu_num--;
						ChangeOption(nrbutoane, w_but, h, difficulty, on_music, contrl);
					}
				}
				else {
					PlaySound(TEXT("res/but.wav"), NULL, SND_FILENAME | SND_ASYNC);
					if (pos != 4) {
						get_nextkey = pos;
					}
					else {
						menu_num--;
						ChangeOption(nrbutoane, w_but, h, difficulty, on_music, contrl);
					}
				}
				up_enter = 0;
			}
			else if (!input.GetKeyDown(VK_RETURN))
				up_enter = 1;

			gfx->DrawSprite(background, 0, 0);
			DrawOptions(gfx, nrbutoane, 500, h, w_but);
			gfx->EndFrame();
		}
	}
};