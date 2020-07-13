#include "Crpch.h"
#include "Camera.h"

#include "Crystal/Input/Input.h"

#include <glfw/glfw3.h>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Crystal 
{
	Camera::Camera(const glm::mat4& ProjectionMatrix)
		: mProjectionMatrix(ProjectionMatrix)
	{
		mDistance = glm::distance(mPosition, mFocalPoint);
	}

	void Camera::Focus()
	{
		//TODO
	}

	void Camera::Update(float DeltaTime)
	{
		if (Input::IsKeyPressed(GLFW_KEY_LEFT_ALT))
		{
			const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
			glm::vec2 Delta = mouse - mInitialMousePosition;
			mInitialMousePosition = mouse;

			Delta *= DeltaTime;

			if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE))
				MousePan(Delta);
			else if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
				MouseRotate(Delta);
			else if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
				MouseZoom(Delta.y);
		}

		mPosition = CalculatePosition();

		glm::quat Orientation = GetOrientation();
		mRotation = glm::eulerAngles(Orientation) * (180.0f / (float)M_PI);
		mViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 1)) * glm::toMat4(glm::conjugate(Orientation)) * glm::translate(glm::mat4(1.0f), -mPosition);
		mViewMatrix = glm::translate(glm::mat4(1.0f), mPosition) * glm::toMat4(Orientation);
		mViewMatrix = glm::inverse(mViewMatrix);
	}

	glm::vec3 Camera::GetUpDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 Camera::GetRightDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 Camera::GetForwardDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	std::pair<float, float> Camera::GetPanSpeed() const
	{
		float X = std::min(mViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float XFactor = 0.0366f * (X* X) - 0.1778f * X + 0.3021f;

		float Y = std::min(mViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float YFactor = 0.0366f * (Y * Y) - 0.1778f * Y + 0.3021f;

		return { XFactor, YFactor };
	}

	float Camera::GetRotationSpeed() const
	{
		//TODO
		return 0.2f;
	}

	float Camera::GetZoomSpeed() const
	{
		float Distance = mDistance * 0.2f;
		Distance = std::max(Distance, 0.f); //min distance = 0
		float Speed = Distance * Distance;
		Speed = std::min(Speed, 100.0f); // max speed = 100
		return Speed;
	}

	void Camera::MousePan(const glm::vec2& Delta)
	{
		auto[XSpeed, YSpeed] = GetPanSpeed();

		CL_CORE_LOG_TRACE("{0}, {1}", XSpeed, YSpeed);

		mFocalPoint += -GetRightDirection() * Delta.x * XSpeed * mDistance;
		mFocalPoint += GetUpDirection() * Delta.y * YSpeed * mDistance;
	}

	void Camera::MouseRotate(const glm::vec2& Delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		mYaw += yawSign * Delta.x * GetRotationSpeed();
		mPitch += Delta.y * GetRotationSpeed();
	}

	void Camera::MouseZoom(float Delta)
	{
		mDistance = std::max(mDistance - (Delta * GetZoomSpeed()), 1.0f);
	}

	glm::vec3 Camera::CalculatePosition()
	{
		return mFocalPoint - GetForwardDirection() * mDistance;
	}

	glm::quat Camera::GetOrientation()
	{
		return glm::quat(glm::vec3(-mPitch, -mYaw, 0.0f));
	}
}
