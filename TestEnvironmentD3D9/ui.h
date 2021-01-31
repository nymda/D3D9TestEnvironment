#pragma once
#include <d3dx9.h>
#include <d3dx9.h>
#include "drawing.h"
#include <vector>
#include <iostream>

namespace fennUi {
	//run on a loop - as in imgui



	enum buttonMode {
		BMODE_SINGLE,
		BMODE_CONSTANT
	};

	POINT GetMousePos(HWND hWnd)
	{
		ShowCursor(FALSE);
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		ScreenToClient(hWnd, &cursorPos);
		return cursorPos;
	}

	struct preservedInfo {
		int uid;
		vec2 position;
		vec2 size;
	};

	class externalHandler {
	public:
		vec2 frameMousePos = { 0, 0 };
		bool mouseDown = false;
		bool lastMouseDown = false;
		int focusedId = -1;
		int focusedFunction = -1;
		bool mouseIsOnControl = false;
		bool controlIsInUse = false;

		void update(HWND hWnd) {
			lastMouseDown = mouseDown;
			POINT tmp = GetMousePos(hWnd);
			frameMousePos = { (float)tmp.x, (float)tmp.y };
			mouseDown = (((GetAsyncKeyState(VK_LBUTTON) >> 15) & 0x0001) == 0x0001);
			if (!mouseDown) {
				focusedId = -1;
				focusedFunction = -1;
			}
		}
	};

	void DrawLine2(IDirect3DDevice9* m_pD3Ddev, float X, float Y, float X2, float Y2, D3DCOLOR Color)
	{
		Vert pVertex[2] = { { X, Y, 0.0f, 1.0f, Color }, { X2, Y2, 0.0f, 1.0f, Color } };
		m_pD3Ddev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		m_pD3Ddev->DrawPrimitiveUP(D3DPT_LINELIST, 1, &pVertex, sizeof(Vert));
	}

	void drawCursor(externalHandler* pmg, LPDIRECT3DDEVICE9 pDev) {

		float X = pmg->frameMousePos.x;
		float Y = pmg->frameMousePos.y;

		Vert cursorMain_W = { X, Y, 0.0f, 1.0f, white };
		Vert cursorBL_W = { X + 6, Y + 16, 0.0f, 1.0f, white };
		Vert cursorC_W = { X + 9, Y + 9, 0.0f, 1.0f, white };
		Vert cursorBR_W = { X + 16, Y + 6, 0.0f, 1.0f, white };

		Vert cursorMain_B = { X, Y, 0.0f, 1.0f, black };
		Vert cursorBL_B = { X + 6, Y + 16, 0.0f, 1.0f, black };
		Vert cursorC_B = { X + 9, Y + 9, 0.0f, 1.0f, black };
		Vert cursorBR_B = { X + 16, Y + 6, 0.0f, 1.0f, black };

		Vert pVertex[6] = {
			cursorMain_W,
			cursorBL_W,
			cursorC_W,
			cursorMain_W,
			cursorBR_W,
			cursorC_W,
		};

		Vert pVertexOutline[5] = {
			cursorMain_B,
			cursorBL_B,
			cursorC_B,
			cursorBR_B,
			cursorMain_B,
		};

		pDev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		pDev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 6, pVertex, sizeof(Vert));
		pDev->DrawPrimitiveUP(D3DPT_LINESTRIP, 5, pVertexOutline, sizeof(Vert));
	}

	bool isPointInRegion(vec2 point, vec2 top, int width, int height) {
		if (point.x > top.x && point.y > top.y && point.x < (top.x + width) && point.y < (top.y + height)) {
			return true;
		}
		return false;
	}

	bool mouseHasJustClicked(externalHandler* pmg) {
		if (pmg->mouseDown != pmg->lastMouseDown && !pmg->lastMouseDown) {
			return true;
		}
		return false;
	}

	vec2 getOffset(vec2 cursor, vec2 wndPos) {
		return { cursor.x - wndPos.x, cursor.y - wndPos.y };
	}

	//easy-ish struct for handling strings. 128 bytes.
	struct packagedText {
		char labelText[128];
	};

	class button {
	public:
		vec2 position;
		vec2 size;
		vec2 relPos;
		const char* label;
		buttonMode mode;
		bool* val;

		bool isHover(externalHandler* ehnd) {
			if (isPointInRegion(ehnd->frameMousePos, relPos, size.x, size.y)) {
				return true;
			}
		}

		bool isClicked(externalHandler* ehnd) {
			if (isHover(ehnd) && ehnd->mouseDown) {
				return true;
			}
		}

		void draw(externalHandler* ehnd, LPDIRECT3DDEVICE9 pDev, vec2 parentPosition, int yOffset) {
			relPos = { parentPosition.x + position.x, parentPosition.y + position.y };

			if (this->isHover(ehnd)) {
				ehnd->mouseIsOnControl = true;
			}

			if (this->isHover(ehnd) && this->isClicked(ehnd)) {
				DrawFilledRect(relPos.x, relPos.y, size.x, size.y, darkGrey, pDev);
				if (mode == BMODE_SINGLE) {
					if (mouseHasJustClicked(ehnd)) {
						*val = true;
					}
					else {
						*val = false;
					}
				}
				else if (mode == BMODE_CONSTANT) {
					*val = true;
				}
			}
			else if (this->isHover(ehnd)) {
				DrawFilledRect(relPos.x, relPos.y, size.x, size.y, lightGrey, pDev);
			}
			else {
				DrawFilledRect(relPos.x, relPos.y, size.x, size.y, white, pDev);
			}

			if (!this->isClicked(ehnd)) {
				*val = false;
			}

			DrawTextC(label, relPos.x + 5, relPos.y + 2, 16, black, pDev);

			drawRect(relPos, size.x, size.y - 1, black, pDev);
		}
	};

	class checkbox {
	public:
		vec2 position;
		vec2 size;
		vec2 relPos;
		bool* val;
		const char* label;

		bool isHover(externalHandler* ehnd) {
			if (isPointInRegion(ehnd->frameMousePos, relPos, size.x, size.y)) {
				return true;
			}
		}

		bool isClicked(externalHandler* ehnd) {
			if (isHover(ehnd) && ehnd->mouseDown) {
				return true;
			}
		}

		void draw(externalHandler* ehnd, LPDIRECT3DDEVICE9 pDev, vec2 parentPosition, int yOffset) {
			relPos = { parentPosition.x + position.x, parentPosition.y + position.y };

			if (this->isHover(ehnd)) {
				ehnd->mouseIsOnControl = true;
			}

			if (this->isHover(ehnd) && this->isClicked(ehnd)) {
				DrawFilledRect(relPos.x, relPos.y, size.x, size.y, darkGrey, pDev);
				if (mouseHasJustClicked(ehnd)) {
					*val = !*val;
				}
			}
			else if (this->isHover(ehnd)) {
				DrawFilledRect(relPos.x, relPos.y, size.x, size.y, lightGrey, pDev);
			}
			else {
				DrawFilledRect(relPos.x, relPos.y, size.x, size.y, white, pDev);
			}

			DrawFilledRect(relPos.x + 2, relPos.y + 2, size.y - 4, size.y - 4, grey, pDev);

			if (*val) { DrawFilledRect(relPos.x + 4, relPos.y + 4, size.y - 8, size.y - 8, white, pDev); }

			DrawTextC(label, relPos.x + (size.y - 4) + 5, relPos.y + 2, 18, black, pDev);

			drawRect(relPos, size.x, size.y - 1, black, pDev);
		}
	};

	class floatSlider {
		float sliderPosOffset = 0;
		bool hold = false;

	public:
		const char* label;
		vec2 position;
		vec2 size;
		vec2 relPos;
		float max;
		float current;

		bool isHover(externalHandler* ehnd) {
			if (isPointInRegion(ehnd->frameMousePos, relPos, size.x, size.y)) {
				return true;
			}
		}

		bool isClicked(externalHandler* ehnd) {
			if (isHover(ehnd) && ehnd->mouseDown) {
				return true;
			}
		}

		void draw(externalHandler* ehnd, LPDIRECT3DDEVICE9 pDev, vec2 parentPosition) {
			relPos = { parentPosition.x + position.x, parentPosition.y + position.y };
			float pixelValue = max / (size.x - 20);

			if (this->isHover(ehnd) || hold) {
				ehnd->mouseIsOnControl = true;
				DrawFilledRect(relPos.x, relPos.y, size.x, size.y, lightGrey, pDev);
				if (ehnd->mouseDown) {
					hold = true;
					float newPosOffset = (ehnd->frameMousePos.x - 10) - relPos.x;
					if (newPosOffset < 0.f) {
						newPosOffset = 0.f;
						current = 0.f;
					}
					else if (newPosOffset > size.x - 21) {
						newPosOffset = size.x - 21;
						current = max;
					}
					else {
						current = (pixelValue * newPosOffset);
					}
					
					sliderPosOffset = newPosOffset;
				}
			}
			else {
				DrawFilledRect(relPos.x, relPos.y, size.x, size.y, white, pDev);
			}

			if (!ehnd->mouseDown) {
				hold = false;
			}
			
			DrawFilledRect(relPos.x + 1.f + sliderPosOffset, relPos.y + 1.f, 20, size.y - 2.f, grey, pDev);
			DrawTextC(std::to_string(current).c_str(), relPos.x + 2, relPos.y, 20, black, pDev);
			drawRect(relPos, size.x, size.y - 1, black, pDev);
		}
	};

	class intSlider {
		float sliderPosOffset = 0;
		bool hold = false;

	public:
		const char* label;
		vec2 position;
		vec2 size;
		vec2 relPos;
		int max;
		int current;

		bool isHover(externalHandler* ehnd) {
			if (isPointInRegion(ehnd->frameMousePos, relPos, size.x, size.y)) {
				return true;
			}
		}

		bool isClicked(externalHandler* ehnd) {
			if (isHover(ehnd) && ehnd->mouseDown) {
				return true;
			}
		}

		void draw(externalHandler* ehnd, LPDIRECT3DDEVICE9 pDev, vec2 parentPosition) {
			relPos = { parentPosition.x + position.x, parentPosition.y + position.y };
			float pixelValue = max / (size.x - 20);

			if (this->isHover(ehnd) || hold) {
				ehnd->mouseIsOnControl = true;
				DrawFilledRect(relPos.x, relPos.y, size.x, size.y, lightGrey, pDev);
				if (ehnd->mouseDown) {
					hold = true;
					float newPosOffset = (ehnd->frameMousePos.x - 10) - relPos.x;
					if (newPosOffset < 0.f) {
						newPosOffset = 0.f;
						current = 0;
					}
					else if (newPosOffset > size.x - 21) {
						newPosOffset = size.x - 21;
						current = max;
					}
					else {
						current = (int)floor(pixelValue * newPosOffset);
					}

					sliderPosOffset = newPosOffset;
				}
			}
			else {
				DrawFilledRect(relPos.x, relPos.y, size.x, size.y, white, pDev);
			}

			if (!ehnd->mouseDown) {
				hold = false;
			}

			DrawFilledRect(relPos.x + 1.f + sliderPosOffset, relPos.y + 1.f, 20, size.y - 2.f, grey, pDev);
			DrawTextC(std::to_string(current).c_str(), relPos.x + 2, relPos.y, 20, black, pDev);
			drawRect(relPos, size.x, size.y - 1, black, pDev);
		}
	};

	class flabel {
	public:
		packagedText* text;
		vec2 position;
		vec2 relPos;
		int fontSize;

		void draw(externalHandler* ehnd, LPDIRECT3DDEVICE9 pDev, vec2 parentPosition) {
			relPos = { parentPosition.x + position.x, parentPosition.y + position.y };
			DrawTextC(text->labelText, relPos.x, relPos.y, fontSize, white, pDev);
		}
	};

	class blabel {
	public:
		const char* text;
		vec2 position;
		vec2 relPos;
		int fontSize;

		void draw(externalHandler* ehnd, LPDIRECT3DDEVICE9 pDev, vec2 parentPosition) {
			relPos = { parentPosition.x + position.x, parentPosition.y + position.y };
			DrawTextC(text, relPos.x, relPos.y, fontSize, white, pDev);
		}
	};

	class container {
		LPDIRECT3DDEVICE9 pDev;
		vec2 position;
		vec2 relPos;
		vec2 size;
		int uid;

		std::vector<button> objButtons = {};
		std::vector<checkbox> objCheckbox = {};
		std::vector<floatSlider> objFloatSliders = {};
		std::vector<intSlider> objIntSliders = {};
		std::vector<flabel> objLabels = {};
		std::vector<blabel> objBasicLabels = {};

	public:
		bool enabled = true;
		bool outline = false;

		void init( vec2 startPosition, vec2 startSize, int uid, LPDIRECT3DDEVICE9 pDev) {
			this->position = startPosition;
			this->size = startSize;
			this->uid = uid;
			this->pDev = pDev;
		}

		void addButton(const char* label, vec2 pos, vec2 size, buttonMode mode, bool* out) {
			button tmp;
			tmp.label = label;
			tmp.position = pos;
			tmp.size = size;
			tmp.mode = mode;
			tmp.val = out;
			objButtons.push_back(tmp);
		}

		void addCheckbox(const char* label, vec2 pos, vec2 size, bool* out) {
			checkbox tmp;
			tmp.label = label;
			tmp.position = pos;
			tmp.size = size;
			tmp.val = out;
			objCheckbox.push_back(tmp);
		}

		void addFloatSlider(const char* label, vec2 pos, vec2 size, float max) {
			floatSlider tmp;
			tmp.label = label;
			tmp.position = pos;
			tmp.size = size;
			tmp.max = max;
			objFloatSliders.push_back(tmp);
		}

		void addIntSlider(const char* label, vec2 pos, vec2 size, int max) {
			intSlider tmp;
			tmp.label = label;
			tmp.position = pos;
			tmp.size = size;
			tmp.max = max;
			objIntSliders.push_back(tmp);
		}

		void addLabel(packagedText* plt, vec2 pos, int fontSize) {
			flabel tmp;
			tmp.text = plt;
			tmp.position = pos;
			tmp.fontSize = fontSize;
			objLabels.push_back(tmp);
		}


		void addBasicLabel(const char* plt, vec2 pos, int fontSize) {
			blabel tmp;
			tmp.text = plt;
			tmp.position = pos;
			tmp.fontSize = fontSize;
			objBasicLabels.push_back(tmp);
		}


		void draw(externalHandler* ehnd, vec2 parentPosition) {
			if (!enabled) {
				return;
			}
			relPos = { parentPosition.x + position.x, parentPosition.y + position.y };
			DrawFilledRect(relPos.x, relPos.y, size.x, size.y, grey, pDev);

			for (button& i : objButtons) {
				i.draw(ehnd, pDev, relPos, 0);
			}
			for (checkbox& i : objCheckbox) {
				i.draw(ehnd, pDev, relPos, 0);
			}
			for (floatSlider& i : objFloatSliders) {
				i.draw(ehnd, pDev, relPos);
			}
			for (intSlider& i : objIntSliders) {
				i.draw(ehnd, pDev, relPos);
			}
			for (flabel& i : objLabels) {
				i.draw(ehnd, pDev, relPos);
			}
			for (blabel& i : objBasicLabels) {
				i.draw(ehnd, pDev, relPos);
			}

			if (outline) {
				drawRect(relPos, size.x, size.y, black, pDev);
			}
		}
	};

	class basicWindow {
		vec2 position;
		vec2 size;
		const char* label;
		vec2 mouseStoredOffset;
		int uid;
		LPDIRECT3DDEVICE9 pDev;
		std::vector<container*> containers = {};

	public:
		bool resizable = true;

		void init(const char* label, vec2 startPosition, vec2 startSize, int uid, LPDIRECT3DDEVICE9 pDev) {
			this->position = startPosition;
			this->label = label;
			this->size = startSize;
			this->uid = uid;
			this->pDev = pDev;
		}

		void addContainer(container* container) {
			this->containers.push_back(container);
		}

		void draw(externalHandler* ehnd) {
			if (ehnd->mouseDown) {
				if (resizable && ehnd->focusedId == -1 && mouseHasJustClicked(ehnd) && isPointInRegion(ehnd->frameMousePos, { this->position.x + this->size.x - 20,  this->position.y + this->size.y - 20 }, 20, 20)) {
					this->mouseStoredOffset = getOffset(ehnd->frameMousePos, this->position);
					ehnd->focusedId = uid;
					ehnd->focusedFunction = 1;
				}
				if (ehnd->focusedId == uid && ehnd->focusedFunction == 1 && resizable) {
					vec2 newSize = { 0, 0 };

					newSize.x = ((ehnd->frameMousePos.x + 10) - this->position.x);
					newSize.y = ((ehnd->frameMousePos.y + 10) - this->position.y);

					if (newSize.x > 50) {
						this->size.x = newSize.x;
					}
					if (newSize.y > 50) {
						this->size.y = newSize.y;
					}
				}

				if (ehnd->focusedId == -1 && mouseHasJustClicked(ehnd) && isPointInRegion(ehnd->frameMousePos, position, size.x, size.y) && !ehnd->mouseIsOnControl) {
					this->mouseStoredOffset = getOffset(ehnd->frameMousePos, this->position);
					ehnd->focusedId = uid;
					ehnd->focusedFunction = 2;
				}
				if (ehnd->focusedId == uid && ehnd->focusedFunction == 2 && !ehnd->mouseIsOnControl) {
					position.x = (ehnd->frameMousePos.x - mouseStoredOffset.x);
					position.y = (ehnd->frameMousePos.y - mouseStoredOffset.y);
				}
			}

			DrawFilledRect(position.x, position.y, size.x, size.y, grey, pDev);
			if (resizable) { DrawFilledRect(position.x + size.x - 20, position.y + size.y - 20, 20, 20, white, pDev); }
			DrawTextC(label, position.x + 5, position.y + 2, 20, white, pDev);
			DrawLine(position.x, position.y + 25, position.x + size.x, position.y + 25, 1, white, pDev);

			ehnd->mouseIsOnControl = false;
			for (container* cnt : containers) {
				cnt->draw(ehnd, position);
			}

			drawRect(position, size.x, size.y, white, pDev);
		}
	};
}