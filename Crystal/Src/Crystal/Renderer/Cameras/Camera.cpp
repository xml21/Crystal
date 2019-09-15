#include "Crpch.h"
#include "Camera.h"

#include "Crystal/Input/Input.h"

#include <glfw/glfw3.h>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#define M_PI 3.14159f

namespace Crystal 
{
	Camera::Camera(const glm::mat4& ProjectionMatrix)
		: mProjectionMatrix(ProjectionMatrix)
	{
		//Sensibility defaults ----------------------------
		mPanSpeed = 0.15f;
		mRotationSpeed = 0.3f;
		mZoomSpeed = 1.0f;

		mPosition = { -5, 5, 5 };
		mRotation = glm::vec3(90.0f, 0.0f, 0.0f);

		mFocalPoint = glm::vec3(0.0f);
		mDistance = glm::distance(mPosition, mFocalPoint);

		mYaw = 3.0f * (float)M_PI / 4.0f;
		mPitch = M_PI / 4.0f;
		//Sensibility defaults ----------------------------
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

	void Camera::MousePan(const glm::vec2& Delta)
	{
		mFocalPoint += -GetRightDirection() * Delta.x * mPanSpeed * mDistance;
		mFocalPoint += GetUpDirection() * Delta.y * mPanSpeed * mDistance;
	}

	void Camera::MouseRotate(const glm::vec2& Delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		mYaw += yawSign * Delta.x * mRotationSpeed;
		mPitch += Delta.y * mRotationSpeed;
	}

	void Camera::MouseZoom(float Delta)
	{
		mDistance -= Delta * mZoomSpeed;
		if (mDistance < 1.0f)
		{
			mFocalPoint += GetForwardDirection();
			mDistance = 1.0f;
		}
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
