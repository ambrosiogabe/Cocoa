#include "cocoa/renderer/Camera.h"
#include "cocoa/events/Input.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/JsonExtended.h"

namespace Cocoa
{
	namespace NCamera
	{
		// Internal functions
		static void Update(Entity entity, Camera& camera);

		Camera CreateCamera()
		{
			Framebuffer framebuffer;
			framebuffer.Width = 3840;
			framebuffer.Height = 2160;
			framebuffer.IncludeDepthStencil = false;
			Texture color0;
			color0.InternalFormat = ByteFormat::RGB;
			color0.ExternalFormat = ByteFormat::RGB;
			color0.MagFilter = FilterMode::Linear;
			color0.MinFilter = FilterMode::Linear;
			color0.WrapS = WrapMode::Repeat;
			color0.WrapT = WrapMode::Repeat;
			NFramebuffer::AddColorAttachment(framebuffer, color0);
			NFramebuffer::Generate(framebuffer);
			return CreateCamera(framebuffer);
		}

		Camera CreateCamera(Framebuffer framebuffer)
		{
			Camera res;
			// TODO: Make this customizable
			res.ProjectionSize = { 6.0f, 3.0f };
			res.ProjectionNearPlane = 0.5f;
			res.ProjectionFarPlane = 100.0f;
			res.Zoom = 1.0f;
			res.ClearColor = { 0.45f, 0.55f, 0.6f };
			res.Framebuffer = framebuffer;
			AdjustPerspective(res);
			return res;
		}

		static void Update(Entity entity, Camera& camera)
		{
			AdjustPerspective(camera);
			CalculateOrthoViewMatrix(entity, camera);
		}

		void AdjustPerspective(Camera& camera)
		{
			camera.ProjectionMatrix = glm::ortho(
				-camera.ProjectionSize.x * camera.Zoom / 2.0f,
				camera.ProjectionSize.x * camera.Zoom / 2.0f,
				-camera.ProjectionSize.y * camera.Zoom / 2.0f,
				camera.ProjectionSize.y * camera.Zoom / 2.0f,
				camera.ProjectionNearPlane,
				camera.ProjectionFarPlane);

			camera.InverseProjection = glm::inverse(camera.ProjectionMatrix);
		}

		void CalculateViewMatrix(Entity entity, Camera& camera)
		{
			const TransformData& transform = NEntity::GetComponent<TransformData>(entity);
			glm::vec3 cameraForward;
			cameraForward.x = glm::cos(glm::radians(transform.EulerRotation.y)) * glm::cos(glm::radians(transform.EulerRotation.x));
			cameraForward.y = glm::sin(glm::radians(transform.EulerRotation.x));
			cameraForward.z = glm::sin(glm::radians(transform.EulerRotation.y)) * glm::cos(glm::radians(transform.EulerRotation.x));
			cameraForward = glm::normalize(cameraForward);

			glm::vec3 cameraRight = glm::cross(cameraForward, glm::vec3(1, 0, 0));
			glm::vec3 cameraUp = glm::cross(cameraRight, cameraForward);

			glm::vec3 front = glm::vec3(transform.Position.x, transform.Position.y, transform.Position.z) + cameraForward;

			camera.ViewMatrix = glm::lookAt(transform.Position, front, cameraUp);
			camera.InverseView = glm::inverse(camera.ViewMatrix);
		}

		void CalculateOrthoViewMatrix(Entity entity, Camera& camera)
		{
			const TransformData& transform = NEntity::GetComponent<TransformData>(entity);
			glm::vec3 cameraFront = glm::vec3(0, 0, -1) + glm::vec3(transform.Position.x, transform.Position.y, 0.0f);
			glm::vec3 cameraUp = glm::vec3(0, 1.0f, 0);

			camera.ViewMatrix = glm::lookAt(glm::vec3(transform.Position.x, transform.Position.y, 20), cameraFront, cameraUp);
			camera.InverseView = glm::inverse(camera.ViewMatrix);
		}

		void ClearColorRgb(const Camera& camera, int attachment, glm::vec3 color)
		{
			NFramebuffer::ClearColorAttachmentRgb(camera.Framebuffer, attachment, color);
		}
		
		void ClearColorUint32(Camera& camera, int attachment, uint32 color)
		{
			NFramebuffer::ClearColorAttachmentUint32(camera.Framebuffer, attachment, color);
		}

		glm::vec2 ScreenToOrtho(const Camera& camera)
		{
			return Input::ScreenToOrtho(camera);
		}

		void Serialize(json* j, Entity entity, const Camera& camera)
		{
			json res;
			res["Entity"] = NEntity::GetID(entity);
			res["Aspect"] = camera.Aspect;
			res["Zoom"] = camera.Zoom;
			res["Fov"] = camera.Fov;
			res["ProjectionFarPlane"] = camera.ProjectionFarPlane;
			res["ProjectionNearPlane"] = camera.ProjectionNearPlane;
			res["ProjectionSize"] = {
				{ "X", camera.ProjectionSize.x },
				{ "Y", camera.ProjectionSize.y }
			};
			res["Framebuffer"] = NFramebuffer::Serialize(camera.Framebuffer);

			json& jRef = *j;
			int size = jRef["Components"].size();
			jRef["Components"][size]["Camera"] = res;
		}

		void Deserialize(const json& j, Entity entity)
		{
			Framebuffer cameraFramebuffer;
			bool hasFramebuffer = j["Camera"].contains("Framebuffer");
			if (hasFramebuffer)
			{
				cameraFramebuffer = NFramebuffer::Deserialize(j["Camera"]["Framebuffer"]);
				NFramebuffer::Generate(cameraFramebuffer);
			}

			Camera camera = hasFramebuffer ? CreateCamera(cameraFramebuffer) : CreateCamera();
			JsonExtended::AssignIfNotNull(j["Camera"], "Aspect", camera.Aspect);
			JsonExtended::AssignIfNotNull(j["Camera"], "Zoom", camera.Zoom);
			JsonExtended::AssignIfNotNull(j["Camera"], "Fov", camera.Fov);
			JsonExtended::AssignIfNotNull(j["Camera"], "ProjectionFarPlane", camera.ProjectionFarPlane);
			JsonExtended::AssignIfNotNull(j["Camera"], "ProjectionNearPlane", camera.ProjectionNearPlane);
			JsonExtended::AssignIfNotNull(j["Camera"], "ProjectionSize", camera.ProjectionSize);
			NEntity::AddComponent<Camera>(entity, camera);
		}
	}

	namespace CameraSystem
	{
		void Update(SceneData& scene, float dt)
		{
			auto view = scene.Registry.view<Camera>();
			for (auto& rawEntity : view)
			{
				NCamera::Update(NEntity::CreateEntity(rawEntity), view.get<Camera>(rawEntity));
			}
		}

		void Destroy(SceneData& scene)
		{
			auto view = scene.Registry.view<Camera>();
			for (auto& rawEntity : view)
			{
				DeleteEntity(NEntity::CreateEntity(rawEntity));
			}
		}

		void DeleteEntity(Entity entity)
		{
			if (NEntity::HasComponent<Camera>(entity))
			{
				Camera& camera = NEntity::GetComponent<Camera>(entity);
				NFramebuffer::Delete(camera.Framebuffer);
			}
		}
	}
}