#include "cocoa/renderer/Camera.h"
#include "cocoa/events/Input.h"

namespace Cocoa
{
	namespace NCamera
	{
		Camera CreateCamera(glm::vec3& position)
		{
			Camera res;
			res.Transform = Transform::CreateTransform(position, glm::vec3(1.0f), glm::vec3(0.0f));
			// TODO: Make this customizable
			res.ProjectionSize = { 6.0f, 3.0f };
			res.ProjectionNearPlane = 0.5f;
			res.ProjectionFarPlane = 100.0f;
			AdjustPerspective(res);
			return res;
		}

		void Update(Camera& camera)
		{
			AdjustPerspective(camera);
			CalculateOrthoViewMatrix(camera);
		}

		void AdjustPerspective(Camera& camera)
		{
			camera.ProjectionMatrix = glm::ortho(
				-camera.ProjectionSize.x * camera.Zoom / 2.0f, 
				camera.ProjectionSize.x * camera.Zoom / 2.0f, 
				-camera.ProjectionSize.y * camera.Zoom / 2.0f, 
				camera.ProjectionSize.y  * camera.Zoom / 2.0f, 
				camera.ProjectionNearPlane, 
				camera.ProjectionFarPlane);

			camera.InverseProjection = glm::inverse(camera.ProjectionMatrix);
		}

		void CalculateViewMatrix(Camera& camera)
		{
			glm::vec3 cameraForward;
			cameraForward.x = glm::cos(glm::radians(camera.Transform.EulerRotation.y)) * glm::cos(glm::radians(camera.Transform.EulerRotation.x));
			cameraForward.y = glm::sin(glm::radians(camera.Transform.EulerRotation.x));
			cameraForward.z = glm::sin(glm::radians(camera.Transform.EulerRotation.y)) * glm::cos(glm::radians(camera.Transform.EulerRotation.x));
			cameraForward = glm::normalize(cameraForward);

			glm::vec3 cameraRight = glm::cross(cameraForward, glm::vec3(1, 0, 0));
			glm::vec3 cameraUp = glm::cross(cameraRight, cameraForward);

			glm::vec3 front = glm::vec3(camera.Transform.Position.x, camera.Transform.Position.y, camera.Transform.Position.z) + cameraForward;

			camera.ViewMatrix = glm::lookAt(camera.Transform.Position, front, cameraUp);
			camera.InverseView = glm::inverse(camera.ViewMatrix);
		}

		void CalculateOrthoViewMatrix(Camera& camera)
		{
			glm::vec3 cameraFront = glm::vec3(0, 0, -1) + glm::vec3(camera.Transform.Position.x, camera.Transform.Position.y, 0.0f);
			glm::vec3 cameraUp = glm::vec3(0, 1.0f, 0);

			camera.ViewMatrix = glm::lookAt(glm::vec3(camera.Transform.Position.x, camera.Transform.Position.y, 20), cameraFront, cameraUp);
			camera.InverseView = glm::inverse(camera.ViewMatrix);
		}

		glm::vec2 ScreenToOrtho(const Camera& camera)
		{
			return Input::ScreenToOrtho(camera);
		}
	}
}