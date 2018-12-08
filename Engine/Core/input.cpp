#include "input.h"

void Input::Reset()
{
	for (int key = 0; key < NUM_KEYS; ++key)
	{
		m_isKeyDownByKey[key] = false;
		
	}
}

void Input::SetKeyDown(uint32_t key)
{
	if (key < NUM_KEYS)
	{
		bool wasKeyDown = m_isKeyDownByKey[key];
		m_isKeyDownByKey[key] = true;
		if (!wasKeyDown)
		{
			m_arrKeyEvents.emplace_back(KeyEvent{ KeyEvent::KeyDown, key });
		}
	}
}

void Input::SetKeyUp(uint32_t key)
{
	if (key < NUM_KEYS)
	{
		bool wasKeyDown = m_isKeyDownByKey[key];
		m_isKeyDownByKey[key] = false;
		if (wasKeyDown)
		{
			m_arrKeyEvents.emplace_back(KeyEvent{ KeyEvent::KeyUp, key });
		}
	}
}

bool Input::IsKeyDown(uint32_t key) const
{
	if (key >= NUM_KEYS) { return false; }
	return m_isKeyDownByKey[key];
}

Input::KeyEventList Input::GetAndClearKeyEvents()
{
	KeyEventList result;
	std::swap(result, m_arrKeyEvents);
	return result;
}
