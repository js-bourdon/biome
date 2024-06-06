//--------------------------------------------------------------------------------------
// File: Camera.h
//
// Helper functions for Direct3D programming.
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=320437
//--------------------------------------------------------------------------------------
#pragma once

#include <DirectXMath.h>


//--------------------------------------------------------------------------------------
// used by CCamera to map WM_KEYDOWN keys
//--------------------------------------------------------------------------------------
enum D3DUtil_CameraKeys
{
    CAM_STRAFE_LEFT = 0,
    CAM_STRAFE_RIGHT,
    CAM_MOVE_FORWARD,
    CAM_MOVE_BACKWARD,
    CAM_MOVE_UP,
    CAM_MOVE_DOWN,
    CAM_RESET,
    CAM_CONTROLDOWN,
    CAM_MAX_KEYS,
    CAM_UNKNOWN     = 0xFF
};

#define KEY_WAS_DOWN_MASK 0x80
#define KEY_IS_DOWN_MASK  0x01

#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_MIDDLE_BUTTON 0x02
#define MOUSE_RIGHT_BUTTON  0x04
#define MOUSE_WHEEL         0x08


//--------------------------------------------------------------------------------------
// Simple base camera class that moves and rotates.  The base class
//       records mouse and keyboard input for use by a derived class, and 
//       keeps common state.
//--------------------------------------------------------------------------------------
class CBaseCamera
{
public:
    CBaseCamera() noexcept;

    // Call these from client and use Get*Matrix() to read new matrices
    virtual LRESULT HandleMessages( _In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam );
    virtual void FrameMove( _In_ float fElapsedTime ) = 0;

    // Functions to change camera matrices
    virtual void Reset();
    virtual void SetViewParams( _In_ DirectX::CXMVECTOR vEyePt, _In_ DirectX::CXMVECTOR vLookatPt );
    virtual void SetProjParams( _In_ float fFOV, _In_ float fAspect, _In_ float fNearPlane, _In_ float fFarPlane );

    // Functions to change behavior
    virtual void SetDragRect( _In_ const RECT& rc ) { m_rcDrag = rc; }
    void SetInvertPitch( _In_ bool bInvertPitch ) { m_bInvertPitch = bInvertPitch; }
    void SetDrag( _In_ bool bMovementDrag, _In_ float fTotalDragTimeToZero = 0.25f )
    {
        m_bMovementDrag = bMovementDrag;
        m_fTotalDragTimeToZero = fTotalDragTimeToZero;
    }
    void SetEnableYAxisMovement( _In_ bool bEnableYAxisMovement ) { m_bEnableYAxisMovement = bEnableYAxisMovement; }
    void SetEnablePositionMovement( _In_ bool bEnablePositionMovement ) { m_bEnablePositionMovement = bEnablePositionMovement; }
    void SetClipToBoundary( _In_ bool bClipToBoundary, _In_opt_ DirectX::XMFLOAT3* pvMinBoundary, _In_opt_ DirectX::XMFLOAT3* pvMaxBoundary )
    {
        m_bClipToBoundary = bClipToBoundary;
        if( pvMinBoundary ) m_vMinBoundary = *pvMinBoundary;
        if( pvMaxBoundary ) m_vMaxBoundary = *pvMaxBoundary;
    }
    void SetScalers( _In_ float fRotationScaler = 0.01f, _In_ float fMoveScaler = 5.0f )
    {
        m_fRotationScaler = fRotationScaler;
        m_fMoveScaler = fMoveScaler;
    }
    void SetNumberOfFramesToSmoothMouseData( _In_ int nFrames ) { if( nFrames > 0 ) m_fFramesToSmoothMouseData = ( float )nFrames; }
    void SetResetCursorAfterMove( _In_ bool bResetCursorAfterMove ) { m_bResetCursorAfterMove = bResetCursorAfterMove; }

    // Functions to get state
    DirectX::XMMATRIX GetViewMatrix() const { return DirectX::XMLoadFloat4x4( &m_mView ); }
    DirectX::XMMATRIX GetProjMatrix() const { return DirectX::XMLoadFloat4x4( &m_mProj ); }
    DirectX::XMVECTOR GetEyePt() const { return DirectX::XMLoadFloat3( &m_vEye ); }
    DirectX::XMVECTOR GetLookAtPt() const { return DirectX::XMLoadFloat3( &m_vLookAt ); }
    float GetNearClip() const { return m_fNearPlane; }
    float GetFarClip() const { return m_fFarPlane; }

    bool IsBeingDragged() const { return ( m_bMouseLButtonDown || m_bMouseMButtonDown || m_bMouseRButtonDown ); }
    bool IsMouseLButtonDown() const { return m_bMouseLButtonDown; }
    bool IsMouseMButtonDown() const { return m_bMouseMButtonDown; }
    bool sMouseRButtonDown() const { return m_bMouseRButtonDown; }

protected:
    // Functions to map a WM_KEYDOWN key to a D3DUtil_CameraKeys enum
    virtual D3DUtil_CameraKeys MapKey( _In_ UINT nKey );

    bool IsKeyDown( _In_ BYTE key ) const { return( ( key & KEY_IS_DOWN_MASK ) == KEY_IS_DOWN_MASK ); }
    bool WasKeyDown( _In_ BYTE key ) const { return( ( key & KEY_WAS_DOWN_MASK ) == KEY_WAS_DOWN_MASK ); }

    DirectX::XMVECTOR XM_CALLCONV ConstrainToBoundary( _In_ DirectX::FXMVECTOR v )
    {
        using namespace DirectX;

        XMVECTOR vMin = XMLoadFloat3( &m_vMinBoundary );
        XMVECTOR vMax = XMLoadFloat3( &m_vMaxBoundary );

        // Constrain vector to a bounding box 
        return XMVectorClamp( v, vMin, vMax );
    }

    void UpdateMouseDelta();
    void UpdateVelocity( _In_ float fElapsedTime );
    void GetInput( _In_ bool bGetKeyboardInput, _In_ bool bGetMouseInput );

    DirectX::XMFLOAT4X4 m_mView;                    // View matrix 
    DirectX::XMFLOAT4X4 m_mProj;                    // Projection matrix

    int m_cKeysDown;                        // Number of camera keys that are down.
    BYTE m_aKeys[CAM_MAX_KEYS];             // State of input - KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK
    DirectX::XMFLOAT3 m_vKeyboardDirection; // Direction vector of keyboard input
    POINT m_ptLastMousePosition;            // Last absolute position of mouse cursor
    int m_nCurrentButtonMask;               // mask of which buttons are down
    int m_nMouseWheelDelta;                 // Amount of middle wheel scroll (+/-) 
    DirectX::XMFLOAT2 m_vMouseDelta;        // Mouse relative delta smoothed over a few frames
    float m_fFramesToSmoothMouseData;       // Number of frames to smooth mouse data over
    DirectX::XMFLOAT3 m_vDefaultEye;        // Default camera eye position
    DirectX::XMFLOAT3 m_vDefaultLookAt;     // Default LookAt position
    DirectX::XMFLOAT3 m_vEye;               // Camera eye position
    DirectX::XMFLOAT3 m_vLookAt;            // LookAt position
    float m_fCameraYawAngle;                // Yaw angle of camera
    float m_fCameraPitchAngle;              // Pitch angle of camera

    RECT m_rcDrag;                          // Rectangle within which a drag can be initiated.
    DirectX::XMFLOAT3 m_vVelocity;          // Velocity of camera
    DirectX::XMFLOAT3 m_vVelocityDrag;      // Velocity drag force
    float m_fDragTimer;                     // Countdown timer to apply drag
    float m_fTotalDragTimeToZero;           // Time it takes for velocity to go from full to 0
    DirectX::XMFLOAT2 m_vRotVelocity;       // Velocity of camera

    float m_fFOV;                           // Field of view
    float m_fAspect;                        // Aspect ratio
    float m_fNearPlane;                     // Near plane
    float m_fFarPlane;                      // Far plane

    float m_fRotationScaler;                // Scaler for rotation
    float m_fMoveScaler;                    // Scaler for movement

    bool m_bMouseLButtonDown;               // True if left button is down 
    bool m_bMouseMButtonDown;               // True if middle button is down 
    bool m_bMouseRButtonDown;               // True if right button is down 
    bool m_bMovementDrag;                   // If true, then camera movement will slow to a stop otherwise movement is instant
    bool m_bInvertPitch;                    // Invert the pitch axis
    bool m_bEnablePositionMovement;         // If true, then the user can translate the camera/model 
    bool m_bEnableYAxisMovement;            // If true, then camera can move in the y-axis
    bool m_bClipToBoundary;                 // If true, then the camera will be clipped to the boundary
    bool m_bResetCursorAfterMove;           // If true, the class will reset the cursor position so that the cursor always has space to move 

    DirectX::XMFLOAT3 m_vMinBoundary;       // Min point in clip boundary
    DirectX::XMFLOAT3 m_vMaxBoundary;       // Max point in clip boundary
};


//--------------------------------------------------------------------------------------
// Simple first person camera class that moves and rotates.
//       It allows yaw and pitch but not roll.  It uses WM_KEYDOWN and 
//       GetCursorPos() to respond to keyboard and mouse input and updates the 
//       view matrix based on input.  
//--------------------------------------------------------------------------------------
class CFirstPersonCamera : public CBaseCamera
{
public:
    CFirstPersonCamera() noexcept;

    // Call these from client and use Get*Matrix() to read new matrices
    virtual void FrameMove( _In_ float fElapsedTime ) override;

    // Functions to change behavior
    void SetRotateButtons( _In_ bool bLeft, _In_ bool bMiddle, _In_ bool bRight, _In_ bool bRotateWithoutButtonDown = false );

    // Functions to get state
    DirectX::XMMATRIX GetWorldMatrix() const { return DirectX::XMLoadFloat4x4( &m_mCameraWorld ); }

    DirectX::XMVECTOR GetWorldRight() const { return DirectX::XMLoadFloat3( reinterpret_cast<const DirectX::XMFLOAT3*>( &m_mCameraWorld._11 ) ); }
    DirectX::XMVECTOR GetWorldUp() const { return DirectX::XMLoadFloat3( reinterpret_cast<const DirectX::XMFLOAT3*>( &m_mCameraWorld._21 ) ); }
    DirectX::XMVECTOR GetWorldAhead() const { return DirectX::XMLoadFloat3( reinterpret_cast<const DirectX::XMFLOAT3*>( &m_mCameraWorld._31 ) ); }
    DirectX::XMVECTOR GetEyePt() const { return DirectX::XMLoadFloat3( reinterpret_cast<const DirectX::XMFLOAT3*>( &m_mCameraWorld._41 ) ); }

protected:
    DirectX::XMFLOAT4X4 m_mCameraWorld; // World matrix of the camera (inverse of the view matrix)

    int m_nActiveButtonMask;            // Mask to determine which button to enable for rotation
    bool m_bRotateWithoutButtonDown;
};

