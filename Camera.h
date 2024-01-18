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
			cameraOrigin{ _origin },
			fovAngle{ _fovAngle }
		{
		}

		Vector3 cameraOrigin = {};
		float fovAngle = 45.f;

		Vector3 forward = { Vector3::UnitZ };
		Vector3 up = { Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch = {};
		float totalYaw = {};


		float nearPlane = 0.1f;
		float farPlane = 1000.f;

		Matrix projectionMatrix = {};
		Matrix invViewMatrix = {};
		Matrix viewMatrix{};
		Matrix worldViewProjectionMatrix{};
		Matrix worldMatrix{};
		float aspectRatioVar{};

		bool canScoreBeIncreased = false;

		void Initialize(float aspectRatio, bool canSpeedBeIncreased, float _fovAngle = 90.f, Vector3 _origin = { 0.f,0.f,0.f })
		{
			fovAngle = _fovAngle;
			aspectRatioVar = aspectRatio;
			cameraOrigin = _origin;
			worldMatrix = CalculateCameraToWorld();
			canScoreBeIncreased = canSpeedBeIncreased;
		}

		void CalculateViewMatrix()
		{
			viewMatrix = CalculateCameraToWorld();

			invViewMatrix = viewMatrix.Inverse();
		}
		void ChangeMovementSpeed(bool canSpeedBeIncreased)
		{
			canScoreBeIncreased = canSpeedBeIncreased;
		}
		void CalculateProjectionMatrix()
		{
			//TODO W3

			projectionMatrix = Matrix::CreatePerspectiveFovLH(fovAngle, aspectRatioVar, nearPlane, farPlane);
			worldViewProjectionMatrix = viewMatrix * projectionMatrix;
		}

		Matrix GetViewMatrix() const 
		{
			return viewMatrix;
		}
		Matrix ViewProjectionMatrix() const 
		{
			return worldViewProjectionMatrix;
		}
		const Vector3& GetOrigin() const { return cameraOrigin; }
		Matrix CalculateCameraToWorld()
		{
			right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();

			worldMatrix = { right,up,forward,cameraOrigin };

			return
			{
				right,
				up,
				forward,
				cameraOrigin
			};
		}
		void Update(const Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();

			float step = 0;
			float rotationSpeed;

			if (!canScoreBeIncreased)
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
				cameraOrigin += (step * deltaTime) * forward.Normalized();
			}
			if (pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN])
			{
				cameraOrigin -= (step * deltaTime) * forward.Normalized();
			}
			if (pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT])
			{

				cameraOrigin += (step * deltaTime) * right.Normalized();
			}
			if (pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT])
			{
				cameraOrigin -= (step * deltaTime) * right.Normalized();
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
				cameraOrigin -= step * deltaTime * forward.Normalized() * static_cast<float>(mouseY);
			}
			//LMB + Mouse Move X
			if (isLeftMousePressed && mouseX)
			{
				totalYaw += TO_RADIANS * rotationSpeed * deltaTime * mouseX;
			}
			//(LMB + RMB + Mouse Move Y)
			if (areBothButtonsPressed && mouseY)
			{
				cameraOrigin += step * deltaTime * up.Normalized() * static_cast<float>(mouseY);
			}

			const Matrix cameraToWorld = this->CalculateCameraToWorld();

			cameraOrigin += cameraToWorld.TransformVector(movementDirection) * deltaTime;

			const Matrix rotMat
			{
			Matrix::CreateRotation(totalPitch,totalYaw, 0.f)
			};

			forward = rotMat.TransformVector(Vector3::UnitZ);

			CalculateViewMatrix();
			CalculateProjectionMatrix();
		}
	};
}
