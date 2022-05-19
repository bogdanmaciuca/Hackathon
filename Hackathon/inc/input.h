#pragma once

enum {
	MAX_KEYS = 256
};

struct Button {
	bool pressed;
	bool down;
	bool released;
};

struct Cursor {
	short int x, y;
};

class Input {
private:
	Button m_keys[MAX_KEYS];
	Cursor m_cursor;
	bool m_any_key;
public:
	Input() {
		ZeroMemory(m_keys, MAX_KEYS * sizeof(Button));
	}
	void Update(int window_x = 0, int window_y = 0) {
		// Buttons
		m_any_key = 0;
		for (int i = 0; i < MAX_KEYS; i++) {
			m_keys[i].pressed = 0;
			m_keys[i].released = 0;
			if (GetAsyncKeyState(i)) {
				if (!m_keys[i].down) {
					m_keys[i].pressed = 1;
					m_any_key = 1;
				}
				m_keys[i].down = 1;
			}
			else {
				if (m_keys[i].down) m_keys[i].released = 1;
				m_keys[i].down = 0;
			}
		}

		// Cursor
		POINT pcursor;
		GetCursorPos(&pcursor);
		m_cursor.x = pcursor.x - window_x;
		m_cursor.y = pcursor.y - window_y;
	}
	// Returns virtual key code of whatever key is pressed that frame
	// Returns -1 if no key is pressed that frame
	int GetKeyVK() {
		for (int i = 0; i < MAX_KEYS; i++) {
			if (m_keys[i].pressed == 1) return i;
		}
		return -1;
	}

	bool GetKeyDown(char key) { return m_keys[key].down; }
	bool GetKeyPressed(char key) { return m_keys[key].pressed; }
	bool GetKeyReleased(char key) { return m_keys[key].released; }
	bool AnyKeyPressed() { return m_any_key; };
	int GetCursorX() { return m_cursor.x; };
	int GetCursorY() { return m_cursor.y; };
};