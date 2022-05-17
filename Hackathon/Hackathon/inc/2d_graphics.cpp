#include "2d_graphics.h"

LRESULT CALLBACK GfxWindowProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
	switch (message) {
	case WM_DESTROY:
		exit(0);
	default:
		break;
	}
	return DefWindowProcA(window, message, wparam, lparam);
}

void Graphics::Create(
	const char* name,
	UINT width, UINT height,
	HINSTANCE instance,
	WNDPROC window_proc
) {
	// Initializing the window class
	ZeroMemory(&m_window_class, sizeof(WNDCLASSA)); // Setting all attributes to default
	m_window_class.lpfnWndProc = window_proc;
	m_window_class.lpszClassName = GFX_WINDOW_CLASS_NAME;
	m_window_class.hInstance = instance;
	RegisterClassA(&m_window_class);

	// Initializing the window handle
	m_handle = CreateWindowA(
		GFX_WINDOW_CLASS_NAME, name,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		GFX_WINDOW_DEFAULT_X, GFX_WINDOW_DEFAULT_Y,
		width, height, 0, 0, instance, 0
	);

	// Direct 2D
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_d2d1_factory);
	m_d2d1_factory->CreateHwndRenderTarget(
		D2D1::RenderTargetProperties(),
		D2D1::HwndRenderTargetProperties(m_handle, D2D1::SizeU(width, height)),
		&m_render_target
	);

	// DWrite
	DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_dwrite_factory)
	);

	// WIC factory
	CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL, CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		reinterpret_cast<void**>(&m_wic_factory)
	);
}

void Graphics::Destroy() {
	// Releasing all resources
	SafeRelease(&m_d2d1_factory);
	SafeRelease(&m_render_target);
	SafeRelease(&m_wic_factory);
}

Graphics::Graphics() {} // Empty constructor for performance reasons

Graphics::Graphics(
	const char* name,
	UINT width, UINT height,
	HINSTANCE instance,
	WNDPROC window_proc
) {
	Create(name, width, height, instance, window_proc);
}
Graphics::~Graphics() { Destroy(); }

void Graphics::HandleMessages() {
	RECT r;
	GetWindowRect(m_handle, &r);
	m_window_x = r.left;
	m_window_y = r.top;
	m_window_width = r.right - r.left;
	m_window_height = r.bottom - r.top;

	MSG message;
	if (PeekMessageA(&message, m_handle, 0, 0, PM_REMOVE)) {
		DispatchMessageA(&message);
		TranslateMessage(&message);
	}
}

bool Graphics::LoadSprite(const wchar_t* filename, Sprite* sprite) {
	bool debug = 0;
#ifdef _DEBUG
	debug = 1;
#endif

	IWICBitmapDecoder* decoder = 0;
	IWICBitmapFrameDecode* source = 0;
	IWICFormatConverter* converter = 0;
	HRESULT hr = m_wic_factory->CreateDecoderFromFilename(
		filename, 0, GENERIC_READ,
		WICDecodeMetadataCacheOnLoad, &decoder
	);
	if (FAILED(hr)) {
		//if (debug) std::cout << "HRESULT: " << hr << "\n";
		return 0;
	}

	hr = decoder->GetFrame(0, &source);
	if (FAILED(hr)) {
		//if (debug) std::cout << "HRESULT: " << hr << "\n";
		return 0;
	}

	m_wic_factory->CreateFormatConverter(&converter);
	if (FAILED(hr)) {
		//if (debug) std::cout << "HRESULT: " << hr << "\n";
		return 0;
	}

	hr = converter->Initialize(
		source, GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone, NULL, 0.0f,
		WICBitmapPaletteTypeMedianCut
	);
	if (FAILED(hr)) {
		//if (debug) std::cout << "HRESULT: " << hr << "\n";
		return 0;
	}

	hr = m_render_target->CreateBitmapFromWicBitmap(converter, NULL, &sprite->bitmap);
	if (FAILED(hr)) {
		//if (debug) std::cout << "HRESULT: " << hr << "\n";
		return 0;
	}

	D2D1_SIZE_U bitmap_size = sprite->bitmap->GetPixelSize();
	sprite->src_width = bitmap_size.width;
	sprite->src_height = bitmap_size.height;
	sprite->width = bitmap_size.width;
	sprite->height = bitmap_size.height;

	SafeRelease(&decoder);
	SafeRelease(&source);
	SafeRelease(&converter);

	return 1;
}

void Graphics::LoadAnimation(const wchar_t* filename, Animation* animation, float time_per_frame, char frame_num) {
	LoadSprite(filename, &animation->sprite);
	animation->frame_num = frame_num;
	animation->time_per_frame = time_per_frame;
	animation->src_frame_w = animation->sprite.src_width / frame_num;
	animation->src_frame_h = animation->sprite.src_height;

	// Setting the default drawing size
	animation->sprite.width = animation->src_frame_w;
	animation->sprite.height = animation->src_frame_h;
}

void Graphics::LoadComplexAnim(const wchar_t* filename, ComplexAnimation* animation, float time_per_frame, char state_num, char* frame_num_arr) {
	LoadSprite(filename, &animation->sprite);
	animation->state_num = state_num;
	animation->frame_num_arr = frame_num_arr;
	animation->time_per_frame = time_per_frame;
	// Cycle through the frame number array to find the biggest number
	char largest_frame_num = 0;
	for (int i = 0; i < state_num; i++) {
		if (largest_frame_num < frame_num_arr[i])
			largest_frame_num = frame_num_arr[i];
	}
	animation->src_frame_w = (float)animation->sprite.src_width / largest_frame_num;
	animation->src_frame_h = (float)animation->sprite.src_height / state_num;

	// Setting the default drawing size
	animation->sprite.width = animation->src_frame_w;
	animation->sprite.height = animation->src_frame_h;
}

void Graphics::BeginFrame() {
	m_render_target->BeginDraw();
	m_render_target->Clear();
}

void Graphics::EndFrame() {
	m_render_target->EndDraw();
}

void Graphics::DrawSprite(const Sprite& sprite, float x, float y, float angle, float opacity) {
	// Getting screen coordinates from absolute coordinates
	float rel_x = x - m_camera_x, rel_y = y - m_camera_y;

	// Set rotation matrix
	D2D1_POINT_2F center;
	center.x = rel_x + sprite.width / 2;
	center.y = rel_y + sprite.height / 2;
	m_render_target->SetTransform(D2D1::Matrix3x2F::Rotation(angle, center));

	m_render_target->DrawBitmap(
		sprite.bitmap,
		D2D1::RectF(rel_x, rel_y, rel_x + sprite.width, rel_y + sprite.height),
		opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		D2D1::RectF(0, 0, sprite.src_width, sprite.src_height)
	);
}

void Graphics::DrawAnimationFrame(Animation* animation, float delta_time, float x, float y, float angle, float opacity) {
	// Handling the frame index
	animation->curr_time += delta_time;
	if (animation->curr_time > animation->time_per_frame) {
		animation->index++;
		animation->curr_time = 0;
	}
	if (animation->index >= animation->frame_num) animation->index = 0;

	// No need to get the y coordinate for simple animations as it is always 0
	float src_x = animation->index * animation->src_frame_w;

	// Getting screen coordinates from absolute coordinates
	float rel_x = x - m_camera_x, rel_y = y - m_camera_y;

	// Set rotation matrix
	D2D1_POINT_2F center;
	center.x = rel_x + animation->sprite.width / 2;
	center.y = rel_y + animation->sprite.height / 2;
	m_render_target->SetTransform(D2D1::Matrix3x2F::Rotation(angle, center));

	m_render_target->DrawBitmap(
		animation->sprite.bitmap,
		D2D1::RectF(x, y, x + animation->sprite.width, y + animation->sprite.height),
		opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		D2D1::RectF(src_x, 0, src_x + animation->src_frame_w, animation->src_frame_h)
	);
}

void Graphics::SetComplexAnimState(ComplexAnimation* c_anim, char state) {
	c_anim->state = state;
	c_anim->index = 0;
	c_anim->curr_time = 0.0f;
}

void Graphics::DrawComplexAnimFrame(ComplexAnimation* c_anim, float delta_time, float x, float y, float angle, float opacity) {
	// Handling the frame index
	c_anim->curr_time += delta_time;
	if (c_anim->curr_time > c_anim->time_per_frame) {
		c_anim->index++;
		c_anim->curr_time = 0;
	}
	if (c_anim->index >= c_anim->frame_num_arr[c_anim->state]) c_anim->index = 0;

	float src_x = c_anim->index * c_anim->src_frame_w;
	float src_y = c_anim->state * c_anim->src_frame_h;

	// Getting screen coordinates from absolute coordinates
	float rel_x = x - m_camera_x, rel_y = y - m_camera_y;

	// Set rotation matrix
	D2D1_POINT_2F center;
	center.x = rel_x + c_anim->src_frame_w / 2;
	center.y = rel_y + c_anim->src_frame_h / 2;
	m_render_target->SetTransform(D2D1::Matrix3x2F::Rotation(angle, center));

	m_render_target->DrawBitmap(
		c_anim->sprite.bitmap,
		D2D1::RectF(x, y, x + c_anim->sprite.width, y + c_anim->sprite.height),
		opacity, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		D2D1::RectF(src_x, src_y, src_x + c_anim->src_frame_w, src_y + c_anim->src_frame_h)
	);
}

void Graphics::CreateTextFormat(const wchar_t* font_name, float font_size, TextFormat* format,
	DWRITE_FONT_WEIGHT weight, DWRITE_FONT_STYLE style) {
	m_dwrite_factory->CreateTextFormat(
		font_name, 0, weight, style,
		DWRITE_FONT_STRETCH_NORMAL, font_size, 
		L"en-us", &format->dwrite_component
	);
}

void Graphics::DrawText(const wchar_t* text, const UINT& length, const TextFormat& format, float x, float y, float w, float h, float *color) {
	ID2D1SolidColorBrush* brush;
	m_render_target->CreateSolidColorBrush(
		D2D1::ColorF(color[0], color[1], color[2], color[3]), &brush);
	m_render_target->DrawTextW(
		text, length, (format).dwrite_component,
		D2D1::RectF(x, y, x + w, y + h), brush);
	SafeRelease(&brush);
}


void Graphics::DrawRect(float x, float y, float w, float h, float* color, float angle, bool fill) {
	// Getting screen coordinates from absolute coordinates
	float rel_x = x - m_camera_x, rel_y = y - m_camera_y;
	ID2D1SolidColorBrush* brush;
	m_render_target->CreateSolidColorBrush(
		D2D1::ColorF(color[0], color[1], color[2], color[3]), &brush);

	// Set rotation matrix
	D2D1_POINT_2F center;
	center.x = rel_x + w / 2;
	center.y = rel_y + h / 2;
	m_render_target->SetTransform(D2D1::Matrix3x2F::Rotation(angle, center));

	if (fill)
		m_render_target->FillRectangle(
			D2D1::RectF(rel_x, rel_y, rel_x + w, rel_y + h), brush);
	else
		m_render_target->DrawRectangle(
			D2D1::RectF(rel_x, rel_y, rel_x + w, rel_y + h), brush);

	SafeRelease(&brush);
}

void Graphics::DrawEllipse(float x, float y, float rad_x, float rad_y, float* color, float angle, bool fill) {
	// Getting screen coordinates from absolute coordinates
	float rel_x = x - m_camera_x, rel_y = y - m_camera_y;

	ID2D1SolidColorBrush* brush;
	m_render_target->CreateSolidColorBrush(
		D2D1::ColorF(color[0], color[1], color[2], color[3]), &brush);

	// Set rotation matrix
	D2D1_POINT_2F center;
	center.x = rel_x + rad_x;
	center.y = rel_y + rad_y;
	m_render_target->SetTransform(D2D1::Matrix3x2F::Rotation(angle, center));

	if (fill)
		m_render_target->FillEllipse(
			D2D1::Ellipse(D2D1::Point2F(rel_x, rel_y), rad_x, rad_y), brush);
	else
		m_render_target->DrawEllipse(
			D2D1::Ellipse(D2D1::Point2F(rel_x, rel_y), rad_x, rad_y), brush);

	SafeRelease(&brush);
}

void Graphics::DrawPolygon(float x, float y, D2D1_POINT_2F* points, UINT points_num, float* color, float angle) {
	// Getting screen coordinates from absolute coordinates
	float rel_x = x - m_camera_x, rel_y = y - m_camera_y;


	// Set rotation matrix
	D2D1_POINT_2F center = { 0, 0 };
	for (int i = 0; i < points_num; i++) {
		center.x += points[i].x;
		center.x += points[i].y;
	}
	center.x = center.x / points_num + rel_x;
	center.y = center.y / points_num + rel_y;
	m_render_target->SetTransform(D2D1::Matrix3x2F::Rotation(angle, center));

	ID2D1SolidColorBrush* brush;
	m_render_target->CreateSolidColorBrush(
		D2D1::ColorF(color[0], color[1], color[2], color[3]), &brush);

	for (int i = 0; i < points_num - 1; i++) {
		m_render_target->DrawLine(
			D2D1::Point2F(points[i].x + rel_x, points[i].y + rel_y),
			D2D1::Point2F(points[i + 1].x + rel_x, points[i + 1].y + rel_y),
			brush
		);
	}
	m_render_target->DrawLine(
		D2D1::Point2F(points[0].x + rel_x, points[0].y + rel_y),
		D2D1::Point2F(points[points_num - 1].x + rel_x, points[points_num - 1].y + rel_y),
		brush
	);

	SafeRelease(&brush);
}
