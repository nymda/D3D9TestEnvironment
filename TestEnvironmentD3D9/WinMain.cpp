#include <stdio.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "NativeWindow.h"
#include <chrono>
#include <string>
#include "drawing.h"
#include "ui.h"
#include <iostream>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

float execTime = 0.f;
std::chrono::high_resolution_clock execTimer;

IDirect3D9* pD3d = nullptr;
IDirect3DDevice9* pDevice = nullptr;
ID3DXFont* pFont = nullptr;
ID3DXSprite* pSprite = nullptr;
char szDevice[256]{ 0 };
char exTimeTxt[256]{ 0 };
char mps[128]{ 0 };

bool InitD3D(HWND hWnd, UINT uWidth, UINT uHeight)
{
	pD3d = Direct3DCreate9( D3D_SDK_VERSION );
	D3DPRESENT_PARAMETERS dp{ 0 };
	dp.BackBufferWidth = uWidth;
	dp.BackBufferHeight = uHeight;
	dp.hDeviceWindow = hWnd;
	dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	dp.Windowed = TRUE;

	HRESULT hr = pD3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &dp, &pDevice );
	if (FAILED( hr ) || !pDevice)
	{
		MessageBox( NULL, _T( "Failed to create D3D Device." ), _T( "Error" ), MB_ICONERROR | MB_OK );
		return false;
	}

	sprintf_s( szDevice, 256, "pDevice: 0x%p", &pDevice );

	// Create font to draw string
	hr = D3DXCreateFont( pDevice, 21, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T( "Consolas" ), &pFont );
	if (FAILED( hr ))
	{
		MessageBox( NULL, _T( "Failed to create font." ), _T( "Error" ), MB_ICONERROR | MB_OK );
		return false;
	}

	MoveWindow(hWnd, 100, 100, 1296, 757, false);
	return true;
}

fennUi::externalHandler exhndlr;
fennUi::basicWindow window1;
bool firstFrame = true;
bool checkboxTest = false;
bool singleTest = false;
bool constTest = false;
fennUi::packagedText plt = { "" };

INT64 frameCounter = 0;
void Render(NativeWindow& wnd, HWND hWnd)
{
	auto startTime = execTimer.now();
	frameCounter += 1;

	pDevice->BeginScene();
	pDevice->Clear( 1, nullptr, D3DCLEAR_TARGET, 0x00111111, 1.0f, 0 );

	RECT rc_dev = { 0, 0, WND_WIDTH, WND_HEIGHT };
	RECT rc_time = { 0, 25, WND_WIDTH, WND_HEIGHT };

	exhndlr.update(hWnd);

	if (firstFrame) {
		window1.init("Window 1", { 200, 100 }, { 200, 200 }, 0, pDevice);
		window1.addButton("Single", { 5, 32 }, { 100, 20 }, fennUi::buttonMode::BMODE_SINGLE, &singleTest);
		window1.addButton("Constant", { 5, 57 }, { 100, 20 }, fennUi::buttonMode::BMODE_CONSTANT, &constTest);
		window1.addCheckbox("Checkbox", { 5, 83 }, { 100, 20 }, &checkboxTest);
		window1.addLabel(&plt, { 110, 31 }, 18);
		firstFrame = false;
	}

	window1.draw(&exhndlr);
	
	if (singleTest) {
		std::cout << "Frame: " << frameCounter << " SINGLE PRESS" << std::endl;
		plt = { "clicked!" };
	}
	if (constTest) {
		std::cout << "Frame: " << frameCounter << " CONSTANT PRESS" << std::endl;
		plt = { "Held!" };
	}
	else {
		plt = { "Idle" };
	}


	//end of hook code

	//background info
	sprintf_s(exTimeTxt, 256, "FPS    : %i", (int)(1000.f / execTime));
	sprintf_s(mps, 64, "MPX: %i | MPY: %i", exhndlr.frameMousePos.x, exhndlr.frameMousePos.y);
	DrawTextC(szDevice, 10, 10, 15, white, pDevice);
	DrawTextC(exTimeTxt, 10, 30, 15, white, pDevice);

	//mouse cursor
	fennUi::drawCursor(&exhndlr, pDevice);

	pDevice->EndScene();
	pDevice->Present( 0, 0, 0, 0 );

	auto endTime = execTimer.now();
	auto exTime = endTime - startTime;
	execTime = (std::chrono::duration_cast<std::chrono::microseconds>(exTime).count() / 1000.f);
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	AllocConsole();
	std::wstring strW = L"UI test";
	SetConsoleTitle(strW.c_str());
	HWND console = GetConsoleWindow();
	DeleteMenu(GetSystemMenu(console, false), SC_CLOSE, MF_BYCOMMAND);
	SetWindowLong(console, GWL_STYLE, GetWindowLong(console, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	std::cout.precision(2);
	SMALL_RECT tmp = { 0, 0, 120, 15 };
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), true, &tmp);

	NativeWindow wnd;

	wnd.Create( hInstance, nCmdShow );

	if (!InitD3D( wnd.GetHandle(), WND_WIDTH, WND_HEIGHT ))
		return 1;

	MSG m;
	while (true)
	{
		while (PeekMessage( &m, NULL, 0, 0, PM_REMOVE ) && m.message != WM_QUIT)
		{
			TranslateMessage( &m );
			DispatchMessage( &m );
		}
		if (m.message == WM_QUIT)
			break;

		Render(wnd, wnd.GetHandle());
	}

	return 0;
}