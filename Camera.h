#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera(const Vector3& _origin, float _fovAngle) :
			m_CameraOrigin{ _origin },
			m_FovAngle{ _fovAngle }
		{
		}

		Vector3 m_CameraOrigin = {};
		float m_FovAngle = 45.f;

		Vector3 m_Forward = { Vector3::UnitZ };
		Vector3 m_Up = { Vector3::UnitY };
		Vector3 m_Right{ Vector3::UnitX };

		float totalPitch = {};
		float totalYaw = {};


		float m_NearPlane = 0.1f;
		float m_FarPlane = 1000.f;

		Matrix m_ProjectionMatrix = {};
		Matrix m_InvViewMatrix = {};
		Matrix m_ViewMatrix{};
		Matrix m_WorldViewProjectionMatrix{};
		Matrix m_WorldMatrix{};
		float m_AspectRatioVar{};

		bool m_CanScoreBeIncreased = false;

		void Initialize(float aspectRatio, bool canSpeedBeIncreased, float _fovAngle = 90.f, Vector3 _origin = { 0.f,0.f,0.f })
		{
			m_FovAngle = _fovAngle;
			m_AspectRatioVar = aspectRatio;
			m_CameraOrigin = _origin;
			m_WorldMatrix = CalculateCameraToWorld();
			m_CanScoreBeIncreased = canSpeedBeIncreased;
		}

		void CalculateViewMatrix()
		{
			m_ViewMatrix = CalculateCameraToWorld();

			m_InvViewMatrix = m_ViewMatrix.Inverse();
		}
		void ChangeMovementSpeed(bool canSpeedBeIncreased)
		{
			m_CanScoreBeIncreased = canSpeedBeIncreased;
		}
		void CalculateProjectionMatrix()
		{
			//TODO W3

			m_ProjectionMatrix = Matrix::CreatePerspectiveFovLH(m_FovAngle, m_AspectRatioVar, m_NearPlane, m_FarPlane);
			m_WorldViewProjectionMatrix = m_ViewMatrix * m_ProjectionMatrix;
		}

		Matrix GetViewMatrix() const 
		{
			return m_ViewMatrix;
		}
		Matrix ViewProjectionMatrix() const 
		{
			return m_WorldViewProjectionMatrix;
		}
		const Vector3& GetOrigin() const { return m_CameraOrigin; }
		Matrix CalculateCameraToWorld()
		{
			m_Right = Vector3::Cross(Vector3::UnitY, m_Forward).Normalized();
			m_Up = Vector3::Cross(m_Forward, m_Right).Normalized();

			m_WorldMatrix = { m_Right,m_Up,m_Forward,m_CameraOrigin };

			return
			{
				m_Right,
				m_Up,
				m_Forward,
				m_CameraOrigin
			};
		}
		void Update(const Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			float step = 0;
			float rotationSpeed;

			if (!m_CanScoreBeIncreased)
			{
			     step = 10.0f;
				 rotationSpeed = 20.f;

			}
			else
			{
				step = 100.0f;
				rotationSpeed = 100.f;
			}
		

			const Vector3 movementDirection{};

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_UP])
			{
				m_CameraOrigin += (step * deltaTime) * m_Forward.Normalized();
			}
			if (pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN])
			{
				m_CameraOrigin -= (step * deltaTime) * m_Forward.Normalized();
			}
			if (pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT])
			{

				m_CameraOrigin += (step * deltaTime) * m_Right.Normalized();
			}
			if (pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT])
			{
				m_CameraOrigin -= (step * deltaTime) * m_Right.Normalized();
			}

			//Mouse Input
			int mouseX = 0, mouseY = 0;

			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			

			const bool isRightMousePressed{ mouseState == SDL_BUTTON_X1 };
			const bool isLeftMousePressed{ mouseState == SDL_BUTTON_LEFT };
			const bool areBothButtonsPressed{ mouseState == SDL_BUTTON_X2 };

			//RMB + Mouse Move X

			if (isRightMousePressed && mouseX)
			{
				totalYaw += TO_RADIANS * rotationSpeed * deltaTime * mouseX;
			}
			//RMB + Mouse Move Y
			if (isRightMousePressed && mouseY)
			{
				totalPitch += TO_RADIANS * mouseY * rotationSpeed * deltaTime;
			}
			//LMB + Mouse Move Y
			if (isLeftMousePressed && mouseY)
			{
				m_CameraOrigin -= step * deltaTime * m_Forward.Normalized() * static_cast<float>(mouseY);
			}
			//LMB + Mouse Move X
			if (isLeftMousePressed && mouseX)
			{
				totalYaw += TO_RADIANS * rotationSpeed * deltaTime * mouseX;
			}
			//(LMB + RMB + Mouse Move Y)
			if (areBothButtonsPressed && mouseY)
			{
				m_CameraOrigin += step * deltaTime * m_Up.Normalized() * static_cast<float>(mouseY);
			}

			const Matrix cameraToWorld = this->CalculateCameraToWorld();

			m_CameraOrigin += cameraToWorld.TransformVector(movementDirection) * deltaTime;

			const Matrix rotMat
			{
			Matrix::CreateRotation(totalPitch,totalYaw, 0.f)
			};

			m_Forward = rotMat.TransformVector(Vector3::UnitZ);

			CalculateViewMatrix();
			CalculateProjectionMatrix();
		}
	};
}
