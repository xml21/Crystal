#pragma once

#include <glm/glm.hpp>

namespace Crystal {
	class Camera
	{
	public:
		Camera(const glm::mat4& ProjectionMatrix);

		void Focus();
		void Update(float DeltaTime);

		inline float GetDistance() const { return mDistance; }
		inline void SetDistance(float Distance) { mDistance = Distance; }

		inline void SetProjectionMatrix(const glm::mat4& ProjectionMatrix) { mProjectionMatrix = ProjectionMatrix; }
		inline void SetViewportSize(const uint32_t Width, const uint32_t Height) { mViewportWidth = Width; mViewportHeight = Height; }

		const glm::mat4& GetProjectionMatrix() const { return mProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return mViewMatrix; }

		glm::vec3 GetUpDirection();
		glm::vec3 GetRightDirection();
		glm::vec3 GetForwardDirection();
		const glm::vec3& GetPosition() const { return mPosition; }
		std::pair<float, float> GetPanSpeed() const;
		float GetRotationSpeed() const;
		float GetZoomSpeed() const;

	private:
		void MousePan(const glm::vec2& Delta);
		void MouseRotate(const glm::vec2& Delta);
		void MouseZoom(float Delta);

		glm::vec3 CalculatePosition();
		glm::quat GetOrientation();

		glm::mat4 mProjectionMatrix;
		glm::mat4 mViewMatrix;

		glm::vec3 mPosition = glm::vec3(-1, 1, 1);
		glm::vec3 mRotation = glm::vec3(90.0f, 0.0f, 0.0f);
		glm::vec3 mFocalPoint = glm::vec3(0.0f);

		bool mPanning;
		bool mRotating;

		glm::vec2 mInitialMousePosition;
		glm::vec3 mInitialFocalPoint;
		glm::vec3 mInitialRotation;

		float mDistance = 0.f;
		float mPitch = M_PI / 4.0f;
		float mYaw = 3.0f * (float)M_PI / 4.0f;

		uint32_t mViewportWidth = 1280;
		uint32_t mViewportHeight = 720;
	};
}