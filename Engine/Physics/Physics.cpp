#include "Physics.h"

#include <cassert>

static uint64_t s_nextHandleId = 1;

Physics &Physics::Get()
{
	static Physics instance;
	return instance;
}

Physics::BallProxy Physics::CreateBall(double x, double y, double radius)
{
	return CreateBall(x, y, 0, 0, radius);
}

Physics::BallProxy Physics::CreateBall(double x, double y, double vx, double vy, double radius)
{
	ObjectHandle handle = CreateObjectHandle(ObjectType::Ball);
	Ball &ball = m_ballByHandle[handle] = { 0, { x, y }, { vx, vy }, radius };
	return BallProxy{ ball, handle, *this };
}

Physics::BoxProxy Physics::CreateBox(double top, double left, double width, double height)
{
	return CreateBox(top, left, 0, 0, width, height);
}

Physics::BoxProxy Physics::CreateBox(double top, double left, double vx, double vy, double width, double height)
{
	ObjectHandle handle = CreateObjectHandle(ObjectType::Box);
	Box &box = m_boxByHandle[handle] = { 0, { top, left }, { vx, vy }, { width, height } };
	return BoxProxy{ box, handle, *this };
}

bool Physics::Destroy(ObjectHandle const &handle)
{
	return m_ballByHandle.erase(handle) > 0 || m_boxByHandle.erase(handle) > 0;
}

Physics::Physics()
	: m_boxByHandle(0, m_hasher)
	, m_ballByHandle(0, m_hasher)
{
}

Physics::ObjectHandle Physics::CreateObjectHandle(ObjectType type)
{
	return ObjectHandle{ s_nextHandleId++, type };
}

