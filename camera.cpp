#include "camera.h"

Camera::Camera()
	: m_positionX(0)
	, m_positionY(0)
	, m_positionZ(0)
	, m_rotationX(0)
	, m_rotationY(0)
	, m_rotationZ(0)
{
}

void Camera::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

DirectX::XMFLOAT3 Camera::GetPosition()
{
	return DirectX::XMFLOAT3{ m_positionX, m_positionY, m_positionZ };
}

DirectX::XMFLOAT3 Camera::GetRotation()
{
	return DirectX::XMFLOAT3{ m_rotationX, m_rotationY, m_rotationZ };
}

void Camera::HandleEvents(GameEventList const &arrEvents)
{
}

void Camera::Update()
{
	float pitch = DirectX::XMConvertToRadians(m_rotationX);
	float yaw = DirectX::XMConvertToRadians(m_rotationY);
	float roll = DirectX::XMConvertToRadians(m_rotationZ);
	DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	DirectX::XMVECTOR up{ 0.0f, 1.0f, 0.0f, 0.0f };
	DirectX::XMVECTOR position{ m_positionX, m_positionY, m_positionZ, 1.0f };
	DirectX::XMVECTOR lookAt{ 0.0f, 0.0f, 1.0f, 1.0f };

	lookAt = DirectX::XMVector3TransformCoord(lookAt, rotation);
	lookAt = DirectX::XMVectorAdd(lookAt, position);
	up = DirectX::XMVector3TransformCoord(up, rotation);

	m_worldToView = DirectX::XMMatrixLookAtLH(position, lookAt, up);
}
