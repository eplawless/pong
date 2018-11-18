#pragma once

#include <functional>

class AtExit
{
public: // methods
	AtExit(const std::function<void()> &action);
	~AtExit();

private: // members
	std::function<void()> m_action;
};
