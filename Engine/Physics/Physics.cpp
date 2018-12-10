#include "Physics.h"

#include <cassert>

static uint64_t s_nextHandleId = 1;

Physics &Physics::Get()
{
	static Physics instance;
	return instance;
}

Physics::ObjectHandle Physics::CreateCircle(double x, double y, double radius)
{
	return CreateCircle(x, y, 0, 0, radius);
}

Physics::ObjectHandle Physics::CreateCircle(double x, double y, double vx, double vy, double radius)
{
	ObjectHandle handle = CreateObjectHandle();
	m_circleByHandle[handle] = { { x, y }, { vx, vy }, radius };
	return handle;
}

Physics::ObjectHandle Physics::CreateBox(double top, double left, double width, double height)
{
	return CreateBox(top, left, 0, 0, width, height);
}

Physics::ObjectHandle Physics::CreateBox(double top, double left, double vx, double vy, double width, double height)
{
	ObjectHandle handle = CreateObjectHandle();
	m_boxByHandle[handle] = { { top, left }, { vx, vy }, {width, height} };
	return handle;
}

Physics::Circle &Physics::GetCircle(
	ObjectHandle const &handle)
{
	return m_circleByHandle.at(handle);
}

Physics::Box & Physics::GetBox(ObjectHandle const &handle)
{
	return m_boxByHandle.at(handle);
}

bool Physics::DestroyCircle(ObjectHandle const &handle)
{
	return m_circleByHandle.erase(handle) > 0;
}

bool Physics::DestroyBox(ObjectHandle const &handle)
{
	return m_boxByHandle.erase(handle) > 0;
}

Physics::ObjectHandle Physics::CreateObjectHandle()
{
	return ObjectHandle{ s_nextHandleId++ };
}

