#include <pch.h>
#include "biome_render/FirstPersonCamera.h"

using namespace biome::render;
using namespace biome::math;

FirstPersonCamera::~FirstPersonCamera()
{
	CURSORINFO cinfo = {};
	cinfo.cbSize = sizeof(CURSORINFO);
	if (GetCursorInfo(&cinfo) && cinfo.flags == 0)
	{
		ShowCursor(true);
	}
}

void FirstPersonCamera::Init(Vector4 worldPosition, Vector4 lookAtWorldPosition, float fov, float aspectRatio, float nearPlane, float farPlane)
{
	m_DxutCamera.SetViewParams(worldPosition, lookAtWorldPosition);
	m_DxutCamera.SetProjParams(fov, aspectRatio, nearPlane, farPlane);
	m_DxutCamera.SetRotateButtons(true, false, false, false);
	m_DxutCamera.SetResetCursorAfterMove(true);
	m_DxutCamera.SetScalers(0.01f, 500.0f);
	ShowCursor(false);
}

Matrix4x4 FirstPersonCamera::GetViewMatrix() const
{
	return m_DxutCamera.GetViewMatrix();
}

Matrix4x4 FirstPersonCamera::GetProjMatrix() const
{
	return m_DxutCamera.GetProjMatrix();
}

Vector3 FirstPersonCamera::GetWorldPosition() const
{
	Vector3 posWS;
	DirectX::XMStoreFloat3(&posWS, m_DxutCamera.GetEyePt());
	return posWS;
}

Vector3 FirstPersonCamera::GetLookAtDirection() const
{
	Vector3 lookAtWS;
	DirectX::XMStoreFloat3(&lookAtWS, m_DxutCamera.GetWorldAhead());
	return lookAtWS;
}

Vector3 FirstPersonCamera::GetLookAtPoint() const
{
	Vector3 lookAtWS;
	DirectX::XMStoreFloat3(&lookAtWS, m_DxutCamera.GetLookAtPt());
	return lookAtWS;
}

Vector2 FirstPersonCamera::GetViewSpaceZParams() const
{
	float nearClip, farClip;
	GetClipPlanes(nearClip, farClip);
	float rcpNear = 1.0f / nearClip;
	float rcpFar = 1.0f / farClip;
	return Vector2 { (rcpFar - rcpNear), rcpNear };
}

void FirstPersonCamera::GetClipPlanes(float& nearClip, float& farClip) const
{
	nearClip = m_DxutCamera.GetNearClip();
	farClip = m_DxutCamera.GetFarClip();
}

void FirstPersonCamera::InternalCallback(void* const pContext, const void* const pMsg)
{
	const MSG* const pTypedMsg = static_cast<const MSG*>(pMsg);
	FirstPersonCamera* const pCamera = static_cast<FirstPersonCamera*>(pContext);
	pCamera->HandleMessages(pTypedMsg->hwnd, pTypedMsg->message, pTypedMsg->wParam, pTypedMsg->lParam);
}

void FirstPersonCamera::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_DxutCamera.HandleMessages(hWnd, uMsg, wParam, lParam);
}

void FirstPersonCamera::FrameMove(float fElapsedTime)
{
	m_DxutCamera.FrameMove(fElapsedTime);
}