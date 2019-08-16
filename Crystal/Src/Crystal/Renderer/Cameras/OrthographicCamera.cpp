#include "Crpch.h"
#include "OrthographicCamera.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Crystal
{

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: mProjectionMatrix(glm::ortho(left, right, bottom, top)), mViewMatrix(1.0f)
	{
		mViewProjectionMatrix = mViewMatrix * mProjectionMatrix;

		SetPosition({ 0.0f, 0.0f, 0.0f });
		SetRotation(0.0f);
	}

	void OrthographicCamera::SetViewProjectionMatrixRotation()
	{
		mVPRotation = glm::rotate(glm::mat4(1.0f), glm::radians(mRotation), glm::vec3(0,0,1));

		glm::mat4 Transform = mVPTranslation * mVPRotation;

		mViewMatrix = glm::inverse(Transform);
		mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
	}

	void OrthographicCamera::SetViewProjectionMatrixPosition()
	{
		mVPTranslation = glm::translate(glm::mat4(1.0f), mPosition);

		glm::mat4 Transform = mVPTranslation * mVPRotation;

		mViewMatrix = glm::inverse(Transform);
		mViewProjectionMatrix = mProjectionMatrix * mViewMatrix;
	}
}