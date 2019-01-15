#pragma once

#include <cstdint>
#include <functional>
#include <unordered_map>
#include "../Libraries/nonstd/optional.hpp"

class Physics
{
public: // types
	enum class ObjectType 
	{ 
		Box, 
		Ball 
	};

	struct ObjectHandle 
	{ 
		uint64_t id;
		ObjectType type;
	};

	struct ObjectHandleHasher
	{
		size_t operator()(ObjectHandle const &handle) const
		{
			return std::hash<uint64_t>()(handle.id);
		}
	};

protected: // types
	struct Vector 
	{
		double x = 0;
		double y = 0;
	};

	struct Box 
	{
		uint16_t refCount = 0;
		Vector position;
		Vector velocity;
		Vector size;
	};

	struct Ball
	{
		uint16_t refCount = 0;
		Vector position;
		Vector velocity;
		double radius = 0;
	};

	struct RefCounted
	{
		RefCounted(uint16_t &refCount, ObjectHandle handle, Physics &physics)
			: m_refCount(refCount)
			, m_handle(handle)
			, m_physics(physics)
		{
			++m_refCount;
		}
		RefCounted(RefCounted const &other)
			: m_refCount(other.m_refCount)
			, m_handle(other.m_handle)
			, m_physics(other.m_physics)
		{
			++m_refCount;
		}
		RefCounted &operator=(RefCounted const &) = delete;
		virtual ~RefCounted()
		{
			if (--m_refCount == 0)
			{
				m_physics.Destroy(m_handle);
			}
		}

	protected: // members
		uint16_t &m_refCount;
		ObjectHandle m_handle;
		Physics &m_physics;
	};

public: // types
	struct BoxProxy : public RefCounted
	{
		BoxProxy(Box &box, ObjectHandle handle, Physics &physics)
			: position(box.position)
			, velocity(box.velocity)
			, size(box.size)
			, RefCounted(box.refCount, handle, physics)
		{
		}
		BoxProxy(BoxProxy const &other)
			: position(other.position)
			, velocity(other.velocity)
			, size(other.size)
			, RefCounted(other)
		{
		}
		
		Vector &position;
		Vector &velocity;
		Vector &size;
	};

	struct BallProxy : public RefCounted
	{
		BallProxy(Ball &ball, ObjectHandle handle, Physics &physics)
			: position(ball.position)
			, velocity(ball.velocity)
			, radius(ball.radius)
			, RefCounted(ball.refCount, handle, physics)
		{
		}
		BallProxy(BallProxy const &other)
			: position(other.position)
			, velocity(other.velocity)
			, radius(other.radius)
			, RefCounted(other)
		{
		}

		Vector &position;
		Vector &velocity;
		double &radius;
	};

public: // static methods
	static Physics &Get();

public: // methods
	BallProxy CreateBall(double x, double y, double radius);
	BallProxy CreateBall(double x, double y, double vx, double vy, double radius);
	BoxProxy CreateBox(double top, double left, double width, double height);
	BoxProxy CreateBox(double top, double left, double vx, double vy, double width, double height);

private: // methods
	Physics();
	Physics(Physics const &other) = delete;
	Physics &operator=(Physics const &other) = delete;
	ObjectHandle CreateObjectHandle(ObjectType type);
	bool Destroy(ObjectHandle const &handle);

private: // members
	ObjectHandleHasher m_hasher;
	std::vector<ObjectHandle> m_arrHandles;
	std::unordered_map<ObjectHandle, Ball, ObjectHandleHasher> m_ballByHandle;
	std::unordered_map<ObjectHandle, Box, ObjectHandleHasher> m_boxByHandle;
};

inline bool operator==(
	Physics::ObjectHandle const &lhs,
	Physics::ObjectHandle const &rhs)
{
	return lhs.id == rhs.id;
}

inline bool operator<(
	Physics::ObjectHandle const &lhs, 
	Physics::ObjectHandle const &rhs) 
{ 
	return lhs.id < rhs.id;
}

namespace std
{
	template <>
	struct hash<Physics::ObjectHandle>
	{
		size_t operator()(Physics::ObjectHandle const &handle) const
		{
			return Physics::ObjectHandleHasher()(handle);
		}
	};
}
