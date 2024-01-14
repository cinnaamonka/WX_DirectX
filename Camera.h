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
			origin{ _origin },
			fovAngle{ _fovAngle }
		{
		}

		Vector3 origin{};
		float fovAngle{ 45.f };
		float fov{ tanf((fovAngle * TO_RADIANS) / 2.f) };

		Vector3 forward{ Vector3::UnitZ };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{};
		float totalYaw{};


		float nearPlane = 0.1f;
		float farPlane = 100.f;

		Matrix projectionMatrix{};
		Matrix invViewMatrix{};
		Matrix viewMatrix{};
		Matrix worldViewProectionMatrix{};
		Matrix worldMatrix{};
		float aspectRatioVar{};

		void Initialize(float aspectRatio, float _fovAngle = 90.f, Vector3 _origin = { 0.f,0.f,0.f })
		{
			fovAngle = _fovAngle;
			fov = tanf((fovAngle * TO_RADIANS) / 2.f);
			aspectRatioVar = aspectRatio;
			origin = _origin;
			worldMatrix = CalculateCameraToWorld();
		}

		void CalculateViewMatrix()
		{
			// ONB - CameraToWorldMatrix
			//ONB => invViewMatrix

			viewMatrix = CalculateCameraToWorld();

			//Inverse(ONB) => ViewMatrix
			invViewMatrix = viewMatrix.Inverse();

			//ViewMatrix => Matrix::CreateLookAtLH(...) [not implemented yet]
			//viewMatrix = Matrix::CreateLookAtLH(origin, forward, up);
		}

		void CalculateProjectionMatrix()
		{
			//TODO W3

			projectionMatrix = Matrix::CreatePerspectiveFovLH(fovAngle, aspectRatioVar, nearPlane, farPlane);
			worldViewProectionMatrix = viewMatrix * projectionMatrix;
		}

		Matrix GetViewMatrix() const 
		{
			return viewMatrix;
		}
		Matrix ViewProjectionMatrix() const 
		{
			return worldViewProectionMatrix;
		}
		const Vector3& GetOrigin() const { return origin; }
		Matrix CalculateCameraToWorld()
		{
			right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();

			worldMatrix = { right,up,forward,origin };

			return
			{
				right,
				up,
				forward,
				origin
			};
		}
		void Update(const Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();
			const float step = 15.0f;

			const Vector3 movementDirection{};

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			if (pKeyboardState[SDL_SCANCODE_W] || pKeyboardState[SDL_SCANCODE_UP])
			{
				origin += (step * deltaTime) * forward.Normalized();
			}
			if (pKeyboardState[SDL_SCANCODE_S] || pKeyboardState[SDL_SCANCODE_DOWN])
			{
				origin -= (step * deltaTime) * forward.Normalized();
			}
			if (pKeyboardState[SDL_SCANCODE_D] || pKeyboardState[SDL_SCANCODE_RIGHT])
			{

				origin += (step * deltaTime) * right.Normalized();
			}
			if (pKeyboardState[SDL_SCANCODE_A] || pKeyboardState[SDL_SCANCODE_LEFT])
			{
				origin -= (step * deltaTime) * right.Normalized();
			}



			//Mouse Input
			int mouseX = 0, mouseY = 0;

			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			const float rotationSpeed = 20.f;

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
				origin -= step * deltaTime * forward.Normalized() * static_cast<float>(mouseY);
			}
			//LMB + Mouse Move X
			if (isLeftMousePressed && mouseX)
			{
				totalYaw += TO_RADIANS * rotationSpeed * deltaTime * mouseX;
			}
			//(LMB + RMB + Mouse Move Y)
			if (areBothButtonsPressed && mouseY)
			{
				origin += step * deltaTime * up.Normalized() * static_cast<float>(mouseY);
			}

			const Matrix cameraToWorld = this->CalculateCameraToWorld();

			origin += cameraToWorld.TransformVector(movementDirection) * deltaTime;


			const Matrix rotMat
			{
			Matrix::CreateRotation(totalPitch,totalYaw, 0.f)
			};


			forward = rotMat.TransformVector(Vector3::UnitZ);

			//Update Matrices
			CalculateViewMatrix();
			CalculateProjectionMatrix(); //Try to optimize this - should only be called once or when fov/aspectRatio changes
		}
	
	};
	
}
