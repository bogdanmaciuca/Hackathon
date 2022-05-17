#pragma once

Graphics::TextFormat text_format;
float gray[] = { 0.8,0.8,0.8,1 }, darkgray[] = { 0.5,0.5,0.5,1 }, green[] = { 0,1,0,1 }, black[] = { 0,0,0,1 };
int pos = 1, nrmeniu = 0;
struct Opt
{
	wchar_t nume[20];
	int nr;
};
Opt denumiri[6];
bool up_up = 1, up_down = 1, up_enter = 1;

void col_rect(Graphics* gfx, int n, int l, int w, int w_opt)
{
	int x1, y1;
	x1 = (800 - l) / 2;
	y1 = (600 - w) / 2;
	gfx->DrawRect(x1, y1, l, w, darkgray, 0, true);
	gfx->CreateTextFormat(L"Arial", 40.0f, &text_format);
	int liber = (w - n * w_opt) / (n + 1), posi = liber + y1;

	for (int i = 1; i <= n; i++) {
		if (pos == i)
			gfx->DrawRect(x1 + 30, posi, l - 60, w_opt, green, 0, true);
		else
			gfx->DrawRect(x1 + 30, posi, l - 60, w_opt, gray, 0, true);
		gfx->DrawText(denumiri[i - 1].nume, denumiri[i - 1].nr, text_format, x1 + 250 - denumiri[i - 1].nr * 11, posi + w_opt / 7, l - 60, w_opt, black);
		posi += liber + w_opt;
	}
}

void meniu_schimb(int& nrbuton, int& w_opt)
{
	char text[30];
	if (nrmeniu == 0) {
		nrbuton = 3;
		size_t outSize;

		strcpy_s(text, "Resume");
		mbstowcs_s(&outSize, denumiri[0].nume, text, 6);
		denumiri[0].nr = 6;

		strcpy_s(text, "Options");
		mbstowcs_s(&outSize, denumiri[1].nume, text, 7);
		denumiri[1].nr = 7;

		strcpy_s(text, "Restart");
		mbstowcs_s(&outSize, denumiri[2].nume, text, 7);
		denumiri[2].nr = 7;
		w_opt = 75;
	}
	if (nrmeniu == 1)
	{
		nrbuton = 4;
		size_t outSize;
		w_opt = 50;

		strcpy_s(text, "Chestie1");
		mbstowcs_s(&outSize, denumiri[0].nume, text, 8);
		denumiri[0].nr = 8;

		strcpy_s(text, "Chestie2");
		mbstowcs_s(&outSize, denumiri[1].nume, text, 8);
		denumiri[1].nr = 8;

		strcpy_s(text, "Chestie3");
		mbstowcs_s(&outSize, denumiri[2].nume, text, 8);
		denumiri[2].nr = 8;

		strcpy_s(text, "Back");
		mbstowcs_s(&outSize, denumiri[3].nume, text, 4);
		denumiri[3].nr = 4;
	}
}

char DrawMenu(Graphics* gfx) {
	int result = 0;
	while (1) {
		gfx->CreateTextFormat(L"Arial", 40.0f, &text_format);
		Input input;
		input.Update(gfx->GetWindowX(), gfx->GetWindowY());
		gfx->HandleMessages();
		gfx->BeginFrame();

		int nrbutoane = 3, w_but;
		meniu_schimb(nrbutoane, w_but);
		if (input.GetKeyDown(VK_DOWN) && up_down)
		{
			pos++;
			if (pos > nrbutoane)
				pos = 1;
			up_down = 0;
		}
		else if (!input.GetKeyDown(VK_DOWN))
			up_down = 1;

		if (input.GetKeyDown(VK_UP) && up_up)
		{
			pos--;
			if (pos < 1)
				pos = nrbutoane;
			up_up = 0;
		}
		else if (!input.GetKeyDown(VK_UP))
			up_up = 1;

		if (input.GetKeyDown(VK_RETURN)&&up_enter)
		{
			if (nrmeniu == 0)
				if (pos == 1)
					return 1;
				else if (pos == 2)
					nrmeniu = 1;
				else
					return 2;
			else {
				if (pos == 4)
					nrmeniu--;
			}
			up_enter = 0;
		}
		else if(!input.GetKeyDown(VK_RETURN))
			up_enter = 1;

		if (result)
			return (int)result;
		col_rect(gfx, nrbutoane, 500, 300, w_but);
		gfx->EndFrame();
	}
}