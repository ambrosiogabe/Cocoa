#include "cocoa/renderer/Camera.h"
#include "cocoa/events/Input.h"
#include "cocoa/util/CMath.h"

namespace Cocoa
{
	namespace NCamera
	{
		// Internal functions
		static void Update(Camera& camera);

		Camera CreateCamera()
		{
			Camera res;
			res.Transform = Transform::CreateTransform(glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
			// TODO: Make this customizable
			res.ProjectionSize = { 6.0f, 3.0f };
			res.ProjectionNearPlane = 0.5f;
			res.ProjectionFarPlane = 100.0f;
			res.Zoom = 1.0f;
			
			res.Framebuffer.Width = 3840;
			res.Framebuffer.Height = 2160;
			res.Framebuffer.IncludeDepthStencil = false;
			Texture color0;
			color0.InternalFormat = ByteFormat::RGB;
			color0.ExternalFormat = ByteFormat::RGB;
			color0.MagFilter = FilterMode::Linear;
			color0.MinFilter = FilterMode::Linear;
			color0.WrapS = WrapMode::Repeat;
			color0.WrapT = WrapMode::Repeat;
			NFramebuffer::AddColorAttachment(res.Framebuffer, color0);
			NFramebuffer::Generate(res.Framebuffer);

			AdjustPerspective(res);
			return res;
		}

		Camera CreateCamera(glm::vec3& position, Framebuffer framebuffer)
		{
			Camera res;
			res.Transform = Transform::CreateTransform(position, glm::vec3(1.0f), glm::vec3(0.0f));
			// TODO: Make this customizable
			res.ProjectionSize = { 6.0f, 3.0f };
			res.ProjectionNearPlane = 0.5f;
			res.ProjectionFarPlane = 100.0f;
			res.Framebuffer = framebuffer;
			res.Zoom = 1.0f;
			AdjustPerspective(res);
			return res;
		}

		static void Update(Camera& camera)
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

		json Serialize(const Camera& camera)
		{
			json res;
			Transform::Serialize(res, camera.Transform);
			res["Aspect"] = camera.Aspect;
			res["Zoom"] = camera.Zoom;
			res["Fov"] = camera.Fov;
			res["ProjectionFarPlane"] = camera.ProjectionFarPlane;
			res["ProjectionNearPlane"] = camera.ProjectionNearPlane;
			res["ProjectionSize"] = {
				{ "X", camera.ProjectionSize.x },
				{ "Y", camera.ProjectionSize.y }
			};

			return res;
		}

		void Deserialize(const json& j, Camera& camera)
		{
			Transform::Deserialize(j, camera.Transform);
			camera.Aspect = j["Aspect"];
			camera.Zoom = j["Zoom"];
			camera.Fov = j["Fov"];
			camera.ProjectionFarPlane = j["ProjectionFarPlane"];
			camera.ProjectionNearPlane = j["ProjectionNearPlane"];
			camera.ProjectionSize = CMath::DeserializeVec2(j["ProjectionSize"]);
		}
	}

	namespace CameraSystem
	{
		void Update(SceneData& scene, float dt) 
		{
			auto view = scene.Registry.view<Camera>();
			for (auto& rawEntity : view)
			{
				NCamera::Update(view.get<Camera>(rawEntity));
			}
		}

		void Destroy(SceneData& scene)
		{
			auto view = scene.Registry.view<Camera>();
			for (auto& rawEntity : view)
			{
				Camera& camera = view.get<Camera>(rawEntity);
				NFramebuffer::Delete(camera.Framebuffer);
			}
		}
	}
}