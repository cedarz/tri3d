#include "window.h"

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE) {
				DestroyWindow(hWnd);
			}
			break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
Window::Window(int width, int height) : width_(width), height_(height) {
	// get handle to exe file
	HINSTANCE hInstance = GetModuleHandle(0);
	const char* cls_name = __TEXT("Window");
	// Register Class
	//{ CS_BYTEALIGNCLIENT, (WNDPROC)screen_events, 0, 0, 0,
	//	NULL, NULL, NULL, NULL, _T("SCREEN3.1415926") };
	WNDCLASS wc;
	wc.style			= CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= NULL;
	wc.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH); //(HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName	= 0;
	wc.lpszClassName	= cls_name;


	RegisterClass(&wc);
	RECT clientSize; //{0, 0, width_, height_};
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = width_;
	clientSize.bottom = height_;

	DWORD style = WS_POPUP;
	if (!full_screen_) {
		style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	}
	hwnd_ = CreateWindow(cls_name, __TEXT("tri3D"), style, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, NULL, NULL, hInstance, NULL);

	HDC hdc = GetDC(hwnd_);
	screen_dc_ = CreateCompatibleDC(hdc);
	// https://stackoverflow.com/questions/29433674/why-releasedc
	ReleaseDC(hwnd_, hdc);

	// bitmapinfo
	BITMAPINFO bmap = {0};
	bmap.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmap.bmiHeader.biWidth = width_;
	bmap.bmiHeader.biHeight = -height_;
	bmap.bmiHeader.biPlanes = 1;
	bmap.bmiHeader.biBitCount = 32;
	bmap.bmiHeader.biCompression = BI_RGB;
	bmap.bmiHeader.biSizeImage = width_ * height_ * 4;
	bmap.bmiColors->rgbBlue = 0;
	bmap.bmiColors->rgbGreen = 0;
	bmap.bmiColors->rgbRed = 0;
	bmap.bmiColors->rgbReserved = 0;

	LPVOID ptr;
	HBITMAP screen_bmap_ = CreateDIBSection(screen_dc_, &bmap, DIB_RGB_COLORS, &ptr, 0, 0);
	

	HBITMAP screen_oldbmap = (HBITMAP)SelectObject(screen_dc_, screen_bmap_);
	frame_buffer_ = (unsigned int *)ptr;
	memset(frame_buffer_, 0, width_ * height_ * 4);

	RECT rect{0, 0, width_, height_};
	AdjustWindowRect(&rect, GetWindowLong(hwnd_, GWL_STYLE), 0);
	int wx = rect.right - rect.left;
	int wy = rect.bottom - rect.top;
	int sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	int sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
	if(sy < 0) sy = 0;
	SetWindowPos(hwnd_, NULL, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));

	SetForegroundWindow(hwnd_);
	ShowWindow(hwnd_, SW_SHOWNORMAL);

	render_ = std::make_shared<Renderer>(width_, height_, frame_buffer_);
}

void Window::SetWindowCaption(const wchar_t * text)
{
	DWORD_PTR dwResult;
	SendMessageTimeoutA(hwnd_, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(text), 
		SMTO_ABORTIFHUNG, 2000, &dwResult);
}

void Window::MainLoop() {
	DWORD cur_time = GetTickCount();
	DWORD pre_time = GetTickCount();
	MSG msg{0};

	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, 0, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			cur_time = GetTickCount();
			if (cur_time - pre_time > 30) {
				pre_time = cur_time;
				// render here?
				Render();
				Update();
			}
		}
	}
}

void Window::Update() {
	HDC hdc = GetDC(hwnd_);
	BitBlt(hdc, 0, 0, width_, height_, screen_dc_, 0, 0, SRCCOPY);
	ReleaseDC(hwnd_, hdc);
}

void Window::Render() {
	render_->render(frame_buffer_);
}