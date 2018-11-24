#pragma once

#include <DirectXMath.h>

class Camera
{
public: // methods
	Camera();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetRotation();
	DirectX::XMMATRIX GetViewMatrix() { return m_worldToView; }
	void Update();

private: // members
	float m_positionX;
	float m_positionY;
	float m_positionZ;
	float m_rotationX;
	float m_rotationY;
	float m_rotationZ;
	DirectX::XMMATRIX m_worldToView;
};
