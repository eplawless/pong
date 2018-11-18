#pragma once

class System
{
public: // methods
	System(const char *name);
	virtual ~System();

	virtual void Start() = 0;
	virtual void Update() = 0;
	virtual void Stop() = 0;

protected: // members
	const char *m_name;
};