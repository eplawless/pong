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

template <typename ResourceType>
class ScopedD3DPointer
{
public: // methods
	ScopedD3DPointer() : m_pResource(nullptr) {}
	ScopedD3DPointer(ResourceType *pResource) : m_pResource(pResource) {}
	~ScopedD3DPointer() { Reset(nullptr); }

	ResourceType *operator->() { return m_pResource; }
	operator bool() { return m_pResource; }
	ResourceType *&Get() { return m_pResource; }
	void Reset() { Reset(nullptr); }
	void Reset(ResourceType *pResource)
	{
		if (m_pResource == pResource) { return; }
		if (m_pResource) { m_pResource->Release(); }
		m_pResource = pResource;
	}

private: // members
	ResourceType *m_pResource;
};