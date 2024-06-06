//--------------------------------------------------------------------------------------
// File: DXUTcamera.cpp
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.monitorInfocrosoft.com/fwlink/?LinkId=320437
//--------------------------------------------------------------------------------------
#include <pch.h>
//#include "DXUT.h"
#include "DXUTcamera.h"
//#include "DXUTres.h"

using namespace DirectX;

//======================================================================================
// CBaseCamera
//======================================================================================

//--------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------
CBaseCamera::CBaseCamera() noexcept :
    m_mView {},
    m_mProj {},
    m_cKeysDown(0),
    m_aKeys {},
    m_vKeyboardDirection(0, 0, 0),
    m_ptLastMousePosition { 0, 0 },
    m_nCurrentButtonMask(0),
    m_nMouseWheelDelta(0),
    m_vMouseDelta(0, 0),
    m_fFramesToSmoothMouseData(2.0f),
    m_vDefaultEye(0, 0, 0),
    m_vDefaultLookAt(0, 0, 0),
    m_vEye(0, 0, 0),
    m_vLookAt(0, 0, 0),
    m_fCameraYawAngle(0.0f),
    m_fCameraPitchAngle(0.0f),
    m_rcDrag {},
    m_vVelocity(0, 0, 0),
    m_vVelocityDrag(0, 0, 0),
    m_fDragTimer(0.0f),
    m_fTotalDragTimeToZero(0.25),
    m_vRotVelocity(0, 0),
    m_fFOV(0),
    m_fAspect(0),
    m_fNearPlane(0),
    m_fFarPlane(1),
    m_fRotationScaler(0.01f),
    m_fMoveScaler(5.0f),
    m_bMouseLButtonDown(false),
    m_bMouseMButtonDown(false),
    m_bMouseRButtonDown(false),
    m_bMovementDrag(false),
    m_bInvertPitch(false),
    m_bEnablePositionMovement(true),
    m_bEnableYAxisMovement(true),
    m_bClipToBoundary(false),
    m_bResetCursorAfterMove(false),
    m_vMinBoundary(-1, -1, -1),
    m_vMaxBoundary(1, 1, 1)
{
    // Setup the view matrix
    SetViewParams(g_XMZero, g_XMIdentityR2);

    // Setup the projection matrix
    SetProjParams(XM_PI / 4, 1.0f, 1.0f, 1000.0f);

    GetCursorPos(&m_ptLastMousePosition);

    SetRect(&m_rcDrag, LONG_MIN, LONG_MIN, LONG_MAX, LONG_MAX);
}


//--------------------------------------------------------------------------------------
// Client can call this to change the position and direction of camera
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
void CBaseCamera::SetViewParams(CXMVECTOR vEyePt, CXMVECTOR vLookatPt)
{
    XMStoreFloat3(&m_vEye, vEyePt);
    XMStoreFloat3(&m_vDefaultEye, vEyePt);

    XMStoreFloat3(&m_vLookAt, vLookatPt);
    XMStoreFloat3(&m_vDefaultLookAt, vLookatPt);

    // Calc the view matrix
    XMMATRIX mView = XMMatrixLookAtLH(vEyePt, vLookatPt, g_XMIdentityR1);
    XMStoreFloat4x4(&m_mView, mView);

    XMMATRIX mInvView = XMMatrixInverse(nullptr, mView);

    // The axis basis vectors and camera position are stored inside the 
    // position matrix in the 4 rows of the camera's world matrix.
    // To figure out the yaw/pitch of the camera, we just need the Z basis vector
    XMFLOAT3 zBasis;
    XMStoreFloat3(&zBasis, mInvView.r[2]);

    m_fCameraYawAngle = atan2f(zBasis.x, zBasis.z);
    float fLen = sqrtf(zBasis.z * zBasis.z + zBasis.x * zBasis.x);
    m_fCameraPitchAngle = -atan2f(zBasis.y, fLen);
}


//--------------------------------------------------------------------------------------
// Calculates the projection matrix based on input params
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
void CBaseCamera::SetProjParams(float fFOV, float fAspect, float fNearPlane, float fFarPlane)
{
    // Set attributes for the projection matrix
    m_fFOV = fFOV;
    m_fAspect = fAspect;
    m_fNearPlane = fNearPlane;
    m_fFarPlane = fFarPlane;

    XMMATRIX mProj = XMMatrixPerspectiveFovLH(fFOV, fAspect, fNearPlane, fFarPlane);
    XMStoreFloat4x4(&m_mProj, mProj);
}


//--------------------------------------------------------------------------------------
// Call this from your message proc so this class can handle window messages
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
LRESULT CBaseCamera::HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_KEYDOWN:
    {
        // Map this key to a D3DUtil_CameraKeys enum and update the
        // state of m_aKeys[] by adding the KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK mask
        // only if the key is not down
        D3DUtil_CameraKeys mappedKey = MapKey((UINT)wParam);
        if (mappedKey != CAM_UNKNOWN)
        {
            _Analysis_assume_(mappedKey < CAM_MAX_KEYS);
            if (FALSE == IsKeyDown(m_aKeys[mappedKey]))
            {
                m_aKeys[mappedKey] = KEY_WAS_DOWN_MASK | KEY_IS_DOWN_MASK;
                ++m_cKeysDown;
            }
        }
        break;
    }

    case WM_KEYUP:
    {
        // Map this key to a D3DUtil_CameraKeys enum and update the
        // state of m_aKeys[] by removing the KEY_IS_DOWN_MASK mask.
        D3DUtil_CameraKeys mappedKey = MapKey((UINT)wParam);
        if (mappedKey != CAM_UNKNOWN && (DWORD)mappedKey < 8)
        {
            m_aKeys[mappedKey] &= ~KEY_IS_DOWN_MASK;
            --m_cKeysDown;
        }
        break;
    }

    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_LBUTTONDBLCLK:
    {
        // Compute the drag rectangle in screen coord.
        POINT ptCursor =
        {
            (short)LOWORD(lParam), (short)HIWORD(lParam)
        };

        // Update member var state
        if ((uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK) && PtInRect(&m_rcDrag, ptCursor))
        {
            m_bMouseLButtonDown = true; m_nCurrentButtonMask |= MOUSE_LEFT_BUTTON;
        }
        if ((uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONDBLCLK) && PtInRect(&m_rcDrag, ptCursor))
        {
            m_bMouseMButtonDown = true; m_nCurrentButtonMask |= MOUSE_MIDDLE_BUTTON;
        }
        if ((uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONDBLCLK) && PtInRect(&m_rcDrag, ptCursor))
        {
            m_bMouseRButtonDown = true; m_nCurrentButtonMask |= MOUSE_RIGHT_BUTTON;
        }

        // Capture the mouse, so if the mouse button is 
        // released outside the window, we'll get the WM_LBUTTONUP message
        SetCapture(hWnd);
        GetCursorPos(&m_ptLastMousePosition);
        return TRUE;
    }

    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    case WM_LBUTTONUP:
    {
        // Update member var state
        if (uMsg == WM_LBUTTONUP)
        {
            m_bMouseLButtonDown = false; m_nCurrentButtonMask &= ~MOUSE_LEFT_BUTTON;
        }
        if (uMsg == WM_MBUTTONUP)
        {
            m_bMouseMButtonDown = false; m_nCurrentButtonMask &= ~MOUSE_MIDDLE_BUTTON;
        }
        if (uMsg == WM_RBUTTONUP)
        {
            m_bMouseRButtonDown = false; m_nCurrentButtonMask &= ~MOUSE_RIGHT_BUTTON;
        }

        // Release the capture if no mouse buttons down
        if (!m_bMouseLButtonDown &&
            !m_bMouseRButtonDown &&
            !m_bMouseMButtonDown)
        {
            ReleaseCapture();
        }
        break;
    }

    case WM_CAPTURECHANGED:
    {
        if ((HWND)lParam != hWnd)
        {
            if ((m_nCurrentButtonMask & MOUSE_LEFT_BUTTON) ||
                (m_nCurrentButtonMask & MOUSE_MIDDLE_BUTTON) ||
                (m_nCurrentButtonMask & MOUSE_RIGHT_BUTTON))
            {
                m_bMouseLButtonDown = false;
                m_bMouseMButtonDown = false;
                m_bMouseRButtonDown = false;
                m_nCurrentButtonMask &= ~MOUSE_LEFT_BUTTON;
                m_nCurrentButtonMask &= ~MOUSE_MIDDLE_BUTTON;
                m_nCurrentButtonMask &= ~MOUSE_RIGHT_BUTTON;
                ReleaseCapture();
            }
        }
        break;
    }

    case WM_MOUSEWHEEL:
        // Update member var state
        m_nMouseWheelDelta += (short)HIWORD(wParam);
        break;
    }

    return FALSE;
}


//--------------------------------------------------------------------------------------
// Figure out the velocity based on keyboard input & drag if any
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
void CBaseCamera::GetInput(bool bGetKeyboardInput, bool bGetMouseInput)
{
    m_vKeyboardDirection = XMFLOAT3(0, 0, 0);
    if (bGetKeyboardInput)
    {
        // Update acceleration vector based on keyboard state
        if (IsKeyDown(m_aKeys[CAM_MOVE_FORWARD]))
            m_vKeyboardDirection.z += 1.0f;
        if (IsKeyDown(m_aKeys[CAM_MOVE_BACKWARD]))
            m_vKeyboardDirection.z -= 1.0f;
        if (m_bEnableYAxisMovement)
        {
            if (IsKeyDown(m_aKeys[CAM_MOVE_UP]))
                m_vKeyboardDirection.y += 1.0f;
            if (IsKeyDown(m_aKeys[CAM_MOVE_DOWN]))
                m_vKeyboardDirection.y -= 1.0f;
        }
        if (IsKeyDown(m_aKeys[CAM_STRAFE_RIGHT]))
            m_vKeyboardDirection.x += 1.0f;
        if (IsKeyDown(m_aKeys[CAM_STRAFE_LEFT]))
            m_vKeyboardDirection.x -= 1.0f;
    }

    if (bGetMouseInput)
    {
        UpdateMouseDelta();
    }
}


//--------------------------------------------------------------------------------------
// Figure out the mouse delta based on mouse movement
//--------------------------------------------------------------------------------------
void CBaseCamera::UpdateMouseDelta()
{
    // Get current position of mouse
    POINT ptCurMousePos;
    GetCursorPos(&ptCurMousePos);

    // Calc how far it's moved since last frame
    POINT ptCurMouseDelta;
    ptCurMouseDelta.x = ptCurMousePos.x - m_ptLastMousePosition.x;
    ptCurMouseDelta.y = ptCurMousePos.y - m_ptLastMousePosition.y;

    // Record current position for next time
    m_ptLastMousePosition = ptCurMousePos;

    if (m_bResetCursorAfterMove /*&& DXUTIsActive()*/)
    {
        const HWND foregroundHwnd = GetForegroundWindow();
        const HWND activeHwnd = GetActiveWindow();

        if (foregroundHwnd == activeHwnd)
        {
            const HMONITOR monitorHnd = MonitorFromWindow(activeHwnd, MONITOR_DEFAULTTOPRIMARY);
            MONITORINFO monitorInfo = {};
            monitorInfo.cbSize = sizeof(MONITORINFO);
            if (GetMonitorInfo(monitorHnd, &monitorInfo))
            {
                // Set position of camera to center of desktop, 
                // so it always has room to move.  This is very useful
                // if the cursor is hidden.  If this isn't done and cursor is hidden, 
                // then invisible cursor will hit the edge of the screen 
                // and the user can't tell what happened
                POINT ptCenter = {};
                ptCenter.x = (monitorInfo.rcMonitor.left + monitorInfo.rcMonitor.right) / 2;
                ptCenter.y = (monitorInfo.rcMonitor.top + monitorInfo.rcMonitor.bottom) / 2;
                SetCursorPos(ptCenter.x, ptCenter.y);
                m_ptLastMousePosition = ptCenter;
            }
        }
    }

    // Smooth the relative mouse data over a few frames so it isn't 
    // jerky when moving slowly at low frame rates.
    float fPercentOfNew = 1.0f / m_fFramesToSmoothMouseData;
    float fPercentOfOld = 1.0f - fPercentOfNew;
    m_vMouseDelta.x = m_vMouseDelta.x * fPercentOfOld + ptCurMouseDelta.x * fPercentOfNew;
    m_vMouseDelta.y = m_vMouseDelta.y * fPercentOfOld + ptCurMouseDelta.y * fPercentOfNew;

    m_vRotVelocity.x = m_vMouseDelta.x * m_fRotationScaler;
    m_vRotVelocity.y = m_vMouseDelta.y * m_fRotationScaler;
}


//--------------------------------------------------------------------------------------
// Figure out the velocity based on keyboard input & drag if any
//--------------------------------------------------------------------------------------
void CBaseCamera::UpdateVelocity(_In_ float fElapsedTime)
{
    XMVECTOR vMouseDelta = XMLoadFloat2(&m_vMouseDelta);
    XMVECTOR vRotVelocity = vMouseDelta * m_fRotationScaler;

    XMStoreFloat2(&m_vRotVelocity, vRotVelocity);

    XMVECTOR vKeyboardDirection = XMLoadFloat3(&m_vKeyboardDirection);
    XMVECTOR vAccel = vKeyboardDirection;

    // Normalize vector so if moving 2 dirs (left & forward), 
    // the camera doesn't move faster than if moving in 1 dir
    vAccel = XMVector3Normalize(vAccel);

    // Scale the acceleration vector
    vAccel *= m_fMoveScaler;

    if (m_bMovementDrag)
    {
        // Is there any acceleration this frame?
        if (XMVectorGetX(XMVector3LengthSq(vAccel)) > 0)
        {
            // If so, then this means the user has pressed a movement key
            // so change the velocity immediately to acceleration 
            // upon keyboard input.  This isn't normal physics
            // but it will give a quick response to keyboard input
            XMStoreFloat3(&m_vVelocity, vAccel);

            m_fDragTimer = m_fTotalDragTimeToZero;

            XMStoreFloat3(&m_vVelocityDrag, vAccel / m_fDragTimer);
        }
        else
        {
            // If no key being pressed, then slowly decrease velocity to 0
            if (m_fDragTimer > 0)
            {
                // Drag until timer is <= 0
                XMVECTOR vVelocity = XMLoadFloat3(&m_vVelocity);
                XMVECTOR vVelocityDrag = XMLoadFloat3(&m_vVelocityDrag);

                vVelocity -= vVelocityDrag * fElapsedTime;

                XMStoreFloat3(&m_vVelocity, vVelocity);

                m_fDragTimer -= fElapsedTime;
            }
            else
            {
                // Zero velocity
                m_vVelocity = XMFLOAT3(0, 0, 0);
            }
        }
    }
    else
    {
        // No drag, so immediately change the velocity
        XMStoreFloat3(&m_vVelocity, vAccel);
    }
}


//--------------------------------------------------------------------------------------
// Maps a windows virtual key to an enum
//--------------------------------------------------------------------------------------
D3DUtil_CameraKeys CBaseCamera::MapKey(_In_ UINT nKey)
{
    // This could be upgraded to a method that's user-definable but for 
    // simplicity, we'll use a hardcoded mapping.
    switch (nKey)
    {
    case VK_CONTROL:
        return CAM_CONTROLDOWN;
    case VK_LEFT:
        return CAM_STRAFE_LEFT;
    case VK_RIGHT:
        return CAM_STRAFE_RIGHT;
    case VK_UP:
        return CAM_MOVE_FORWARD;
    case VK_DOWN:
        return CAM_MOVE_BACKWARD;
    case VK_PRIOR:
        return CAM_MOVE_UP;        // pgup
    case VK_NEXT:
        return CAM_MOVE_DOWN;      // pgdn

    case 'A':
        return CAM_STRAFE_LEFT;
    case 'D':
        return CAM_STRAFE_RIGHT;
    case 'W':
        return CAM_MOVE_FORWARD;
    case 'S':
        return CAM_MOVE_BACKWARD;
    case 'Q':
        return CAM_MOVE_DOWN;
    case 'E':
        return CAM_MOVE_UP;

    case VK_NUMPAD4:
        return CAM_STRAFE_LEFT;
    case VK_NUMPAD6:
        return CAM_STRAFE_RIGHT;
    case VK_NUMPAD8:
        return CAM_MOVE_FORWARD;
    case VK_NUMPAD2:
        return CAM_MOVE_BACKWARD;
    case VK_NUMPAD9:
        return CAM_MOVE_UP;
    case VK_NUMPAD3:
        return CAM_MOVE_DOWN;

    case VK_HOME:
        return CAM_RESET;
    }

    return CAM_UNKNOWN;
}


//--------------------------------------------------------------------------------------
// Reset the camera's position back to the default
//--------------------------------------------------------------------------------------
void CBaseCamera::Reset()
{
    XMVECTOR vDefaultEye = XMLoadFloat3(&m_vDefaultEye);
    XMVECTOR vDefaultLookAt = XMLoadFloat3(&m_vDefaultLookAt);

    SetViewParams(vDefaultEye, vDefaultLookAt);
}


//======================================================================================
// CFirstPersonCamera
//======================================================================================

CFirstPersonCamera::CFirstPersonCamera() noexcept :
    m_mCameraWorld {},
    m_nActiveButtonMask(0x07),
    m_bRotateWithoutButtonDown(false)
{
}


//--------------------------------------------------------------------------------------
// Update the view matrix based on user input & elapsed time
//--------------------------------------------------------------------------------------
void CFirstPersonCamera::FrameMove(_In_ float fElapsedTime)
{
    if (IsKeyDown(m_aKeys[CAM_RESET]))
    {
        Reset();
    }

    // Get keyboard/mouse/gamepad input
    GetInput(m_bEnablePositionMovement, (m_nActiveButtonMask & m_nCurrentButtonMask) || m_bRotateWithoutButtonDown);

    //// Get the mouse movement (if any) if the mouse button are down
    //if( (m_nActiveButtonMask & m_nCurrentButtonMask) || m_bRotateWithoutButtonDown )
    //    UpdateMouseDelta( fElapsedTime );

    // Get amount of velocity based on the keyboard input and drag (if any)
    UpdateVelocity(fElapsedTime);

    // Simple euler method to calculate position delta
    XMVECTOR vVelocity = XMLoadFloat3(&m_vVelocity);
    XMVECTOR vPosDelta = vVelocity * fElapsedTime;

    // If rotating the camera 
    if ((m_nActiveButtonMask & m_nCurrentButtonMask)
        || m_bRotateWithoutButtonDown)
    {
        // Update the pitch & yaw angle based on mouse movement
        float fYawDelta = m_vRotVelocity.x;
        float fPitchDelta = m_vRotVelocity.y;

        // Invert pitch if requested
        if (m_bInvertPitch)
            fPitchDelta = -fPitchDelta;

        m_fCameraPitchAngle += fPitchDelta;
        m_fCameraYawAngle += fYawDelta;

        // LimonitorInfot pitch to straight up or straight down
        m_fCameraPitchAngle = std::max(-XM_PI / 2.0f, m_fCameraPitchAngle);
        m_fCameraPitchAngle = std::min(+XM_PI / 2.0f, m_fCameraPitchAngle);
    }

    // Make a rotation matrix based on the camera's yaw & pitch
    XMMATRIX mCameraRot = XMMatrixRotationRollPitchYaw(m_fCameraPitchAngle, m_fCameraYawAngle, 0);

    // Transform vectors based on camera's rotation matrix
    XMVECTOR vWorldUp = XMVector3TransformCoord(g_XMIdentityR1, mCameraRot);
    XMVECTOR vWorldAhead = XMVector3TransformCoord(g_XMIdentityR2, mCameraRot);

    // Transform the position delta by the camera's rotation 
    if (!m_bEnableYAxisMovement)
    {
        // If restricting Y movement, do not include pitch
        // when transformonitorInfong position delta vector.
        mCameraRot = XMMatrixRotationRollPitchYaw(0.0f, m_fCameraYawAngle, 0.0f);
    }
    XMVECTOR vPosDeltaWorld = XMVector3TransformCoord(vPosDelta, mCameraRot);

    // Move the eye position 
    XMVECTOR vEye = XMLoadFloat3(&m_vEye);
    vEye += vPosDeltaWorld;
    if (m_bClipToBoundary)
        vEye = ConstrainToBoundary(vEye);
    XMStoreFloat3(&m_vEye, vEye);

    // Update the lookAt position based on the eye position
    XMVECTOR vLookAt = vEye + vWorldAhead;
    XMStoreFloat3(&m_vLookAt, vLookAt);

    // Update the view matrix
    XMMATRIX mView = XMMatrixLookAtLH(vEye, vLookAt, vWorldUp);
    XMStoreFloat4x4(&m_mView, mView);

    XMMATRIX mCameraWorld = XMMatrixInverse(nullptr, mView);
    XMStoreFloat4x4(&m_mCameraWorld, mCameraWorld);
}


//--------------------------------------------------------------------------------------
// Enable or disable each of the mouse buttons for rotation drag.
//--------------------------------------------------------------------------------------
_Use_decl_annotations_
void CFirstPersonCamera::SetRotateButtons(bool bLeft, bool bMiddle, bool bRight, bool bRotateWithoutButtonDown)
{
    m_nActiveButtonMask = (bLeft ? MOUSE_LEFT_BUTTON : 0) |
        (bMiddle ? MOUSE_MIDDLE_BUTTON : 0) |
        (bRight ? MOUSE_RIGHT_BUTTON : 0);
    m_bRotateWithoutButtonDown = bRotateWithoutButtonDown;
}

