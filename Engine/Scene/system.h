#pragma once

#include "../Core/input.h"
#include <cstdint>

class System
{
public: // methods
	System(const char *name);
	virtual ~System();

	virtual void Start() = 0;
	virtual void Update(uint64_t usdt, Input const &input) = 0;
	virtual void Stop() = 0;

protected: // members
	const char *m_name;
};