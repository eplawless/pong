#pragma once

#include <functional>
#include <Windows.h>

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

template <typename T>
int sign(T value)
{
	return (T(0) < value) - (value < T(0));
}

struct Point2D
{
	double x;
	double y;
};

using Vector2D = Point2D;

struct Box2D
{
	double positionX;
	double positionY;
	double sizeX;
	double sizeY;

	Point2D GetCenter() const
	{
		return Point2D{ 
			positionX + 0.5f * sizeX, 
			positionY + 0.5f * sizeY 
		};
	}

	bool Intersects(Box2D const &other) const
	{
		return !(positionX > (other.positionX + other.sizeX)
			|| other.positionX > (positionX + sizeX)
			|| positionY > (other.positionY + other.sizeY)
			|| other.positionY > (positionY + sizeY)
		);
	}
};
