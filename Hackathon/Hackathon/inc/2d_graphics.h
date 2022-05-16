#pragma once
#include <Windows.h>
#include <d2d1.h>
#include <wincodec.h>
#include <dwrite.h>

#ifdef _DEBUG
#include <iostream>
#endif

#ifdef DrawText
#undef DrawText
#endif

// Standard SafeRelease from MSDN for
template <class T> void SafeRelease(T** ppT) {
	if (*ppT) {
		(*ppT)->Release();
		*ppT = NULL;
	}
}

LRESULT CALLBACK GfxWindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

#define GFX_WINDOW_CLASS_NAME "gfxwnd"
#define GFX_WINDOW_DEFAULT_X 100
#define GFX_WINDOW_DEFAULT_Y 100

class Graphics {
public:
	struct Sprite {
		ID2D1Bitmap* bitmap;
		UINT src_width, src_height;
		float width, height;
	};
	// All frames used for animation must be of equal sizes
	// All frames must be on a single horizontal line
	struct Animation {
		Sprite sprite;
		char index = 0;
		char frame_num = 0;
		float time_per_frame = 0.0f;
		float curr_time = 0.0f;
		UINT src_frame_w = 0, src_frame_h = 0;
	};
	// Each line of frames is considered a state
	struct ComplexAnimation {
		Sprite sprite;
		char index = 0;
		float time_per_frame = 0.0f;
		float curr_time = 0.0f;
		char state = 0;
		char state_num = 0;
		char* frame_num_arr; // For all states
		UINT src_frame_w = 0, src_frame_h = 0;
	};
	class TextFormat {
	public:
		IDWriteTextFormat* dwrite_component;
		~TextFormat() { SafeRelease(&dwrite_component); }
	};
private:
	// Window handle
	HWND m_handle;

	// Window class for all attributes
	WNDCLASSA m_window_class;

	// Window dimensions
	UINT m_window_x, m_window_y;
	UINT m_window_width, m_window_height;

	// Direct2D & DWrite
	ID2D1Factory* m_d2d1_factory = 0;
	ID2D1HwndRenderTarget* m_render_target = 0;
	IDWriteFactory* m_dwrite_factory = 0;
	IWICImagingFactory* m_wic_factory = 0;

	// Virtual camera
	float m_camera_x = 0, m_camera_y = 0;
public:
	UINT GetWindowX() { return m_window_x; }
	UINT GetWindowY() { return m_window_y; }
	UINT GetWindowWidth() { return m_window_width; }
	UINT GetWindowHeight() { return m_window_height; }
	void SetCamera(float cx, float cy) { m_camera_x = cx; m_camera_y = cy; }
	float GetCameraX() { return m_camera_x; }
	float GetCameraY() { return m_camera_y; }

	// WinAPI & Graphics
	void Create(const char* name, UINT width, UINT height, HINSTANCE instance = 0, WNDPROC window_proc = (WNDPROC)GfxWindowProc);
	void Destroy();
	Graphics(); // Empty constructor for performance reasons
	Graphics(const char* name, UINT width, UINT height, HINSTANCE instance = 0, WNDPROC window_proc = (WNDPROC)GfxWindowProc);
	~Graphics();
	void HandleMessages();
	// Graphics
	void BeginFrame();
	void EndFrame();
	// Sprites
	void DrawSprite(const Sprite& sprite, float x, float y, float angle = 0.0f, float opacity = 1.0f);
	bool LoadSprite(const wchar_t* filename, Sprite* sprite); // Returns 0 on failure
	// Animations
	void LoadAnimation(const wchar_t* filename, Animation* animation, float time_per_frame, char frame_num);
	void DrawAnimationFrame(Animation* animation, float delta_time, float x, float y, float angle = 0.0f, float opacity = 1.0f);
	// Complex animations
	void LoadComplexAnim(const wchar_t* filename, ComplexAnimation* animation, float time_per_frame, char state_num, char* frame_num_arr);
	void SetComplexAnimState(ComplexAnimation* c_anim, char state);
	void DrawComplexAnimFrame(ComplexAnimation* c_anim, float delta_time, float x, float y, float angle = 0.0f, float opacity = 1.0f);
	// Text
	void CreateTextFormat(const wchar_t* font_name, float font_size, TextFormat* format, DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE style = DWRITE_FONT_STYLE_NORMAL);
	void DrawText(const wchar_t* text, const UINT &length, const TextFormat& format, float x, float y, float w, float h, float *color);
	// Basic shapes
	void DrawRect(float x, float y, float w, float h, float* color, float angle = 0.0f, bool fill = 0);
	void DrawEllipse(float x, float y, float rad_x, float rad_y, float* color, float angle = 0.0f, bool fill = 0);
	void DrawPolygon(float x, float y, D2D1_POINT_2F* points, UINT points_num, float* color, float angle = 0.0f);
};
