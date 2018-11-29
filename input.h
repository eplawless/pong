#pragma once

#include <cstdint>
#include <vector>

class Input
{
public: // types
	struct KeyEvent
	{
		enum Type { KeyUp, KeyDown };
		Type type;
		uint32_t key;
	};
	using KeyEventList = std::vector<KeyEvent>;

public: // methods
	void Reset();
	void SetKeyDown(uint32_t key);
	void SetKeyUp(uint32_t key);
	bool IsKeyDown(uint32_t key) const;

	KeyEventList GetAndClearKeyEvents();

private: // members
	KeyEventList m_arrKeyEvents;
	static const int NUM_KEYS = 256;
	bool m_isKeyDownByKey[NUM_KEYS];
};
