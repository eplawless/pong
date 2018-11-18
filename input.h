#pragma once

class Input
{
public: // methods
	void Reset();
	void SetKeyDown(unsigned int key);
	void SetKeyUp(unsigned int key);
	bool IsKeyDown(unsigned int key) const;

private: // members
	static const int NUM_KEYS = 256;
	bool m_isKeyDownByKey[NUM_KEYS];
};
