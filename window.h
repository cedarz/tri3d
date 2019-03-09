#ifndef _TRI3D_WINDOW_H_
#define _TRI3D_WINDOW_H_


#include <Windows.h>
#include <tchar.h>

class Window {
public:
	Window(int width, int height);
	void SetWindowCaption(const wchar_t *text);
	void MainLoop();

private:
	//TCHAR appName[] = _T("tri3d");
	HWND hwnd_{0};
	int width_ = 0;
	int height_ = 0;
	bool full_screen_ { false };

	HDC screen_dc_{ NULL };
	unsigned char * frame_buffer_ { nullptr };
};


#endif // !_TRI3D_WINDOW_H_
