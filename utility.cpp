#include "utility.h"

AtExit::AtExit(const std::function<void()> &action)
	: m_action(action)
{
}

AtExit::~AtExit()
{
	if (m_action)
	{
		m_action();
	}
}
