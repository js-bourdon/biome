#pragma once

#include "biome_core/Math/Math.h"
#include "biome_render/DXUT/DXUTcamera.h"

namespace biome::render
{
    class FirstPersonCamera final
    {
    public:

        typedef void (*MessageCallback)(void* const pContext, const void* const pMsg);

		FirstPersonCamera() = default;
		~FirstPersonCamera();

        void Init(
            const biome::math::Vector4 worldPosition, 
            const biome::math::Vector4 lookAtWorldPosition, 
            const float fov, 
            const float aspectRatio, 
            const float nearPlane, 
            const float farPlane);

        biome::math::Matrix4x4  GetViewMatrix() const;
        biome::math::Matrix4x4  GetProjMatrix() const;
        biome::math::Vector3    GetWorldPosition() const;
        biome::math::Vector3    GetLookAtDirection() const;
        biome::math::Vector3    GetLookAtPoint() const;
        biome::math::Vector2    GetViewSpaceZParams() const;
        void                    GetClipPlanes(float& near, float& far) const;

        MessageCallback         GetMessageCallback() const { return InternalCallback; }
        void                    FrameMove(float fElapsedTime);

    private:

        static void InternalCallback(void* const pContext, const void* const pMsg);

        void HandleMessages(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    private:

        CFirstPersonCamera m_DxutCamera;
    };
}
