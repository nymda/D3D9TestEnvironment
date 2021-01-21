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
		//ShowCursor(FALSE);
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		ScreenToClient(hWnd, &cursorPos);
		if (cursorPos.x > 1279) {
			cursorPos.x = 1279;
		}
		else if (cursorPos.x < 0) {
			cursorPos.x = 0;
		}
		if (cursorPos.y > 719) {
			cursorPos.y = 719;
		}
		else if (cursorPos.y < 0) {
			cursorPos.y = 0;
		}
		return cursorPos;
	}

	struct vec2 {
		float x;
		float y;
	};


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

	void drawCursor(externalHandler* pmg, LPDIRECT3DDEVICE9 pDev) {
		DrawFilledRect(pmg->frameMousePos.x - 1, pmg->frameMousePos.y - 1, 3, 3, red, pDev);
	}
	bool isPointInRegion(vec2 point, vec2 top, int width, int height) {
		if (point.x > top.x && point.y > top.y && point.x < (top.x + width) && point.y < (top.y + height)) {
			return true;
		}
		return false;
	}
	bool mouseHasJustClicked(externalHandler* pmg) {
		if (pmg->mouseDown != pmg->lastMouseDown) {
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

			DrawTextC(label, relPos.x + 5, relPos.y + 2, 18, black, pDev);
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

			if (*val) {
				DrawFilledRect(relPos.x + 2, relPos.y + 2, size.y - 4, size.y - 4, green, pDev);
			}
			else {
				DrawFilledRect(relPos.x + 2, relPos.y + 2, size.y - 4, size.y - 4, red, pDev);
			}




			DrawTextC(label, relPos.x + (size.y - 4) + 5, relPos.y + 2, 18, black, pDev);
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

	class basicWindow {
		vec2 position;
		vec2 size;
		const char* label;
		vec2 mouseStoredOffset;
		int uid;
		LPDIRECT3DDEVICE9 pDev;
		std::vector<button> objButtons = {};
		std::vector<checkbox> objCheckbox = {};
		std::vector<flabel> objLabels = {};

	public:
		void init(const char* label, vec2 startPosition, vec2 startSize, int uid, LPDIRECT3DDEVICE9 pDev) {
			this->position = startPosition;
			this->label = label;
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

		void addLabel(packagedText* plt, vec2 pos, int fontSize) {
			flabel tmp;
			tmp.text = plt;
			tmp.position = pos;
			tmp.fontSize = fontSize;
			objLabels.push_back(tmp);
		}

		void draw(externalHandler* ehnd) {
			if (ehnd->mouseDown) {
				if (ehnd->focusedId == -1 && mouseHasJustClicked(ehnd) && isPointInRegion(ehnd->frameMousePos, { this->position.x + this->size.x - 20,  this->position.y + this->size.y - 20 }, 20, 20)) {
					this->mouseStoredOffset = getOffset(ehnd->frameMousePos, this->position);
					ehnd->focusedId = uid;
					ehnd->focusedFunction = 1;
				}
				if (ehnd->focusedId == uid && ehnd->focusedFunction == 1) {
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

				if (ehnd->focusedId == -1 && mouseHasJustClicked(ehnd) && isPointInRegion(ehnd->frameMousePos, position, size.x, size.y)) {
					this->mouseStoredOffset = getOffset(ehnd->frameMousePos, this->position);
					ehnd->focusedId = uid;
					ehnd->focusedFunction = 2;
				}
				if (ehnd->focusedId == uid && ehnd->focusedFunction == 2) {
					position.x = (ehnd->frameMousePos.x - mouseStoredOffset.x);
					position.y = (ehnd->frameMousePos.y - mouseStoredOffset.y);
				}
			}

			DrawFilledRect(position.x, position.y, size.x, size.y, grey, pDev);
			DrawFilledRect(position.x + size.x - 20, position.y + size.y - 20, 20, 20, white, pDev);
			DrawTextC(label, position.x + 5, position.y + 2, 20, white, pDev);
			DrawLine(position.x, position.y + 25, position.x + size.x, position.y + 25, 2, white, pDev);

			int yoff = 32;

			for (button i : objButtons) {
				i.draw(ehnd, pDev, position, yoff);
				yoff += 25;
			}
			for (checkbox i : objCheckbox) {
				i.draw(ehnd, pDev, position, yoff);
				yoff += 25;
			}
			for (flabel i : objLabels) {
				i.draw(ehnd, pDev, position);
				yoff += 25;
			}
		}
	};


	/*void testBox(preservationManager* pmg, vec2 position, vec2 size, LPDIRECT3DDEVICE9 pDev) {
		if (isPointInRegion(pmg->frameMousePos, position, size.x, size.y) && pmg->mouseDown) {
			DrawFilledRect(position.x, position.y, size.x, size.y, red, pDev);
		}
		else if (isPointInRegion(pmg->frameMousePos, position, size.x, size.y)) {
			DrawFilledRect(position.x, position.y, size.x, size.y, green, pDev);
		}
		else {
			DrawFilledRect(position.x, position.y, size.x, size.y, blue, pDev);
		}
	}*/

	/*class preservationManager {
	public:
		std::vector<preservedInfo> presArray = { };
		vec2 frameMousePos = { 0, 0 };
		bool mouseDown = false;
		bool lastMouseDown = false;
		vec2 mouseStoredOffset = { 0, 0 };
		int singleWindowHandler = -1;
		int singleWindowFunction = -1;

		void update(HWND hWnd) {
			lastMouseDown = mouseDown;
			POINT tmp = GetMousePos(hWnd);
			frameMousePos = { (float)tmp.x, (float)tmp.y };
			mouseDown = (((GetAsyncKeyState(VK_LBUTTON) >> 15) & 0x0001) == 0x0001);
			if (!mouseDown) {
				singleWindowHandler = -1;
				singleWindowFunction = -1;
			}
		}
	};*/

	/*void drawWindow(preservationManager* pmg, int winID, const char* title, vec2 startPos, vec2 startSize, LPDIRECT3DDEVICE9 pDev) {
		int windowId = winID;
		const char* wndTitle = title;
		bool existsInPreservCache = false;
		int uid = 0;

		for (auto i : pmg->presArray) {
			if (i.uid == winID) {
				existsInPreservCache = true;

				if (pmg->mouseDown) {
					if (pmg->singleWindowHandler == -1 && mouseHasJustClicked(pmg) && isPointInRegion(pmg->frameMousePos, { i.position.x + i.size.x - 20, i.position.y + i.size.y - 20 }, 20, 20)) {
						pmg->mouseStoredOffset = getOffset(pmg->frameMousePos, i.position);
						pmg->singleWindowHandler = uid;
						pmg->singleWindowFunction = 1;
					}
					if (pmg->singleWindowHandler == uid && pmg->singleWindowFunction == 1) {
						vec2 newSize = { 0, 0 };

						newSize.x = ((pmg->frameMousePos.x + 10) - pmg->presArray[uid].position.x);
						newSize.y = ((pmg->frameMousePos.y + 10) - pmg->presArray[uid].position.y);

						if (newSize.x > 50 && newSize.y > 50) {
							pmg->presArray[uid].size = newSize;
						}
					}

					if (pmg->singleWindowHandler == -1 && mouseHasJustClicked(pmg) && isPointInRegion(pmg->frameMousePos, i.position, i.size.x, i.size.y)) {
						pmg->mouseStoredOffset = getOffset(pmg->frameMousePos, i.position);
						pmg->singleWindowHandler = uid;
						pmg->singleWindowFunction = 2;
					}
					if (pmg->singleWindowHandler == uid && pmg->singleWindowFunction == 2) {
						pmg->presArray[uid].position.x = (pmg->frameMousePos.x - pmg->mouseStoredOffset.x);
						pmg->presArray[uid].position.y = (pmg->frameMousePos.y - pmg->mouseStoredOffset.y);
					}
				}

				DrawFilledRect(i.position.x, i.position.y, i.size.x, i.size.y, grey, pDev);
				DrawFilledRect(i.position.x + i.size.x - 20, i.position.y + i.size.y - 20, 20, 20, white, pDev);
				DrawTextC(title, i.position.x + 5, i.position.y + 5, white, pDev);
				DrawLine(i.position.x, i.position.y + 25, i.position.x + i.size.x, i.position.y + 25, 2, white, pDev);

				testBox(pmg, { i.position.x + 5, i.position.y + 30 }, { 50, 25 }, pDev);
			}
			else {
				uid += 1;
			}
		}
		if (!existsInPreservCache) {
			pmg->presArray.push_back({ winID, startPos, startSize });
		}
	}*/
}

