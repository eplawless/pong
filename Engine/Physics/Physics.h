#pragma once

#include <cstdint>
#include <map>
#include "../Libraries/nonstd/optional.hpp"

class Physics
{
public: // types
	struct ObjectHandle 
	{ 
		uint64_t id; 
		bool operator<(ObjectHandle const &other) const { return id < other.id; }
	};

	struct Vector2D {
		double x;
		double y;
	};

	struct Box {
		Vector2D position;
		Vector2D velocity;
		Vector2D size;
	};

	struct Circle
	{
		Vector2D position;
		Vector2D velocity;
		double radius;
	};

public: // static methods
	static Physics &Get();

public: // methods
	ObjectHandle CreateCircle(double x, double y, double radius);
	ObjectHandle CreateCircle(double x, double y, double vx, double vy, double radius);
	ObjectHandle CreateBox(double top, double left, double width, double height);
	Circle &GetCircle(ObjectHandle const &handle);
	Box &GetBox(ObjectHandle const &handle);
	bool DestroyCircle(ObjectHandle const &handle);
	bool DestroyBox(ObjectHandle const &handle);

private: // methods
	Physics() = default;
	Physics(Physics const &other) = delete;
	Physics &operator=(Physics const &other) = delete;

	ObjectHandle CreateObjectHandle();

private: // members
	std::map<ObjectHandle, Circle> m_circleByHandle;
	std::map<ObjectHandle, Box> m_boxByHandle;
};