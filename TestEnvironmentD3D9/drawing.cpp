#include "drawing.h"
#include <vector>
#include <iostream>

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* dev)
{
	D3DRECT BarRect = { x, y, x + w, y + h };
	dev->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, color, 0, 0);
}

ID3DXLine* LineL;
void DrawLine(int x1, int y1, int x2, int y2, int thickness, D3DCOLOR color, IDirect3DDevice9* dev) {

	if (!LineL) {
		D3DXCreateLine(dev, &LineL);
	}

	D3DXVECTOR2 Line[2];
	Line[0] = D3DXVECTOR2(x1, y1);
	Line[1] = D3DXVECTOR2(x2, y2);
	LineL->SetWidth(thickness);
	LineL->Draw(Line, 2, color);
}

void drawCircleD3D(float x, float y, float radius, int sides, float width, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice)
{
	float angle = D3DX_PI * 2 / sides;
	float _cos = cos(angle);
	float _sin = sin(angle);
	float x1 = radius, y1 = 0, x2, y2;

	for (int i = 0; i < sides; i++)
	{
		x2 = _cos * x1 - _sin * y1 + x;
		y2 = _sin * x1 + _cos * y1 + y;
		x1 += x;
		y1 += y;
		DrawLine(x1, y1, x2, y2, width, color, pDevice);
		x1 = x2 - x;
		y1 = y2 - y;
	}
}

bool initFonts = true;
std::vector< ID3DXFont* > fonts = {};
void DrawTextC(const char* text, float x, float y, int size, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice) {
	RECT rect;
	size -= 1;
	if (size < 1 || size > 20) {
		return;
	}

	if (initFonts) {
		for (int i = 0; i < 20; i++) {
			ID3DXFont* tmpFont;
			D3DXCreateFont(pDevice, i, 0, FW_BOLD, 1, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Consolas", &tmpFont);
			std::cout << i << std::endl;
			fonts.push_back(tmpFont);
		}
		initFonts = false;
	}

	//SetRect(&rect, x + 1, y + 1, x + 1, y + 1);
	//FontF->DrawTextA(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));


	SetRect(&rect, x, y, x, y);
	fonts[size]->DrawTextA(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, color);
}