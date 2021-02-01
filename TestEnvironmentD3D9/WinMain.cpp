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
WNDPROC oWndProc;

bool drawMenu = false;
LRESULT APIENTRY hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_F1 || wParam == VK_INSERT)
		{
			drawMenu = !drawMenu;
			return true;
		}
	}

	return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

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
	hr = D3DXCreateFont( pDevice, 15, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T( "Lucida console" ), &pFont );
	if (FAILED( hr ))
	{
		MessageBox( NULL, _T( "Failed to create font." ), _T( "Error" ), MB_ICONERROR | MB_OK );
		return false;
	}

	//hr = D3DXCreateSprite(pDevice, &pSprite);
	//if (FAILED(hr))
	//{
	//	MessageBox(NULL, _T("Failed to create sprite."), _T("Error"), MB_ICONERROR | MB_OK);
	//	return false;
	//}

	MoveWindow(hWnd, 100, 100, 1296, 757, false);
	return true;
}

fennUi::externalHandler exhndlr;
fennUi::basicWindow window1;
fennUi::container ctlButtons;
fennUi::container ct1;
fennUi::container ct2;
fennUi::container ct3;

bool showP1 = false;
bool showP2 = false;
bool showP3 = false;
int iOutPutTest = 0;
float fOutPutTest = 0.f;

bool idc = false;

bool firstFrame = true;
bool checkboxTest = false;
bool constTest = false;
fennUi::packagedText plt = { "" };

INT64 clickCount = 1;
INT64 frameCounter = 0;

IDirect3DVertexBuffer9* g_pVb = nullptr;
IDirect3DTexture9* Primitive = NULL;

void Render(NativeWindow& wnd, HWND hWnd)
{
	auto startTime = execTimer.now();
	frameCounter += 1;

	pDevice->BeginScene();
	pDevice->Clear( 1, nullptr, D3DCLEAR_TARGET, black, 1.0f, 0 );
	//pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	RECT rc_dev = { 0, 0, WND_WIDTH, WND_HEIGHT };
	RECT rc_time = { 0, 25, WND_WIDTH, WND_HEIGHT };

	if (firstFrame) {
		window1.init("FennUI", { 200, 100 }, { 320, 260 }, 0, pDevice);
		window1.resizable = false;

		ctlButtons.init({ 5, 30 }, { 310, 20 }, 1, pDevice);
		ctlButtons.addButton("ESP",  { 0,   0 }, 100, fennUi::buttonMode::BMODE_SINGLE, &showP1);
		ctlButtons.addButton("AIM",  { 105, 0 }, 100, fennUi::buttonMode::BMODE_SINGLE, &showP2);
		ctlButtons.addButton("MISC", { 210, 0 }, 100, fennUi::buttonMode::BMODE_SINGLE, &showP3);

		ct1.init({ 5, 55 }, { 310, 200 }, 2, pDevice);
		ct1.addBasicLabel("ESP", { 5, 5 }, 15);
		ct1.addCheckbox("Friendlies", { 5, 25 }, 125, &idc);
		ct1.addCheckbox("Enemies", { 5, 50 }, 125, &idc);
		ct1.addFloatSlider({ 5, 75 }, 200, 1, &fOutPutTest);
		ct1.addIntSlider({ 5, 100 }, 200, 50, &iOutPutTest);
		ct1.outline = true;
		ct1.enabled = true;

		ct2.init({ 5, 55 }, { 310, 200 }, 3, pDevice);
		ct2.addBasicLabel("this is page 2", { 5, 5 }, 15);
		ct2.addCheckbox("Checkbox", { 5, 25 }, 100, &idc);
		ct2.outline = true;
		ct2.enabled = false;

		window1.addContainer(&ctlButtons);
		window1.addContainer(&ct1);
		window1.addContainer(&ct2);
		firstFrame = false;
	}

	exhndlr.update(hWnd);

	if (drawMenu) {
		window1.draw(&exhndlr);
		if (showP1) {
			ct1.enabled = true;
			ct2.enabled = false;
		}
		else if (showP2) {
			ct1.enabled = false;
			ct2.enabled = true;
		}
	}

	RECT rc = { 100, 100, WND_WIDTH, WND_HEIGHT };
	DrawTextC(szDevice, 100, 115, 15, white, pDevice);

	//background info
	sprintf_s(exTimeTxt, 256, "FPS    : %i", (int)(1000.f / execTime));
	sprintf_s(mps, 64, "MPX: %i | MPY: %i", exhndlr.frameMousePos.x, exhndlr.frameMousePos.y);
	DrawTextC("D3D9 Test Environment | [F1] Menu", 10, 10, 15, white, pDevice);
	DrawTextC(exTimeTxt, 10, 25, 15, white, pDevice);
	fennUi::drawCursor(&exhndlr, pDevice);
	//pSprite->End();
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

	oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(wnd.GetHandle(), GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(hWndProc)));

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

//auto startTime_tmp = execTimer.now();
//auto endTime_tmp = execTimer.now();
//auto exTime_tmp = endTime_tmp - startTime_tmp;
//float execTimeTmp = (std::chrono::duration_cast<std::chrono::microseconds>(exTime_tmp).count());
//std::cout << std::to_string(execTimeTmp) << std::endl;