#pragma once

#include "glm/glm.hpp"

namespace Crystal
{
	class OrthographicCamera
	{
		public:
			OrthographicCamera(float left, float right, float top, float bottom);

			inline glm::mat4 GetProjectionMatrix() const { return mProjectionMatrix; }
			inline glm::mat4 GetViewMatrix() const { return mViewMatrix; }
			inline glm::mat4 GetViewProjectionMatrix() const { return mViewProjectionMatrix; }
			inline glm::vec3 GetPosition() const { return mPosition; }
			inline float GetRotation() const { return mRotation; }

			void SetProjectionMatrix(glm::mat4 val) { mProjectionMatrix = val; }
			void SetViewMatrix(glm::mat4 val) { mViewMatrix = val; }
			void SetViewProjectionMatrix(glm::mat4 val) { mViewProjectionMatrix = val; }
			void SetPosition(glm::vec3 val) { mPosition = val; SetViewProjectionMatrixPosition();}
			void SetRotation(float val) { mRotation = val; SetViewProjectionMatrixRotation();}

	private:
			glm::mat4 mProjectionMatrix;
			glm::mat4 mViewMatrix;
			glm::mat4 mViewProjectionMatrix;
			glm::vec3 mPosition = {0.0f, 0.0f, 0.0f};

			glm::mat4 mVPTranslation;
			glm::mat4 mVPRotation;

			float mRotation = 0.0f;

			void SetViewProjectionMatrixRotation();
			void SetViewProjectionMatrixPosition();
	};
}
