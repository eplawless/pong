#include "input.h"

void Input::Reset()
{
	for (int key = 0; key < NUM_KEYS; ++key)
	{
		m_isKeyDownByKey[key] = false;
	}
}

void Input::SetKeyDown(unsigned int key)
{
	if (key < NUM_KEYS)
	{
		m_isKeyDownByKey[key] = true;
	}
}

void Input::SetKeyUp(unsigned int key)
{
	if (key < NUM_KEYS)
	{
		m_isKeyDownByKey[key] = false;
	}
}

bool Input::IsKeyDown(unsigned int key) const
{
	if (key >= NUM_KEYS) { return false; }
	return m_isKeyDownByKey[key];
}
