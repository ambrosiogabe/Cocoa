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

		Camera createCamera()
		{
			Framebuffer framebuffer;
			framebuffer.width = 3840;
			framebuffer.height = 2160;
			framebuffer.includeDepthStencil = false;
			Texture color0;
			color0.internalFormat = ByteFormat::RGB;
			color0.externalFormat = ByteFormat::RGB;
			color0.magFilter = FilterMode::Linear;
			color0.minFilter = FilterMode::Linear;
			color0.wrapS = WrapMode::Repeat;
			color0.wrapT = WrapMode::Repeat;
			NFramebuffer::addColorAttachment(framebuffer, color0);
			NFramebuffer::generate(framebuffer);
			return createCamera(framebuffer);
		}

		Camera createCamera(Framebuffer framebuffer)
		{
			Camera res;
			// TODO: Make this customizable
			res.projectionSize = { 6.0f, 3.0f };
			res.projectionNearPlane = 0.5f;
			res.projectionFarPlane = 100.0f;
			res.zoom = 1.0f;
			res.clearColor = { 0.45f, 0.55f, 0.6f };
			res.framebuffer = framebuffer;
			adjustPerspective(res);
			return res;
		}

		static void Update(Entity entity, Camera& camera)
		{
			adjustPerspective(camera);
			calculateOrthoViewMatrix(entity, camera);
		}

		void adjustPerspective(Camera& camera)
		{
			camera.projectionMatrix = glm::ortho(
				-camera.projectionSize.x * camera.zoom / 2.0f,
				camera.projectionSize.x * camera.zoom / 2.0f,
				-camera.projectionSize.y * camera.zoom / 2.0f,
				camera.projectionSize.y * camera.zoom / 2.0f,
				camera.projectionNearPlane,
				camera.projectionFarPlane);

			camera.inverseProjection = glm::inverse(camera.projectionMatrix);
		}

		void calculateViewMatrix(Entity entity, Camera& camera)
		{
			const TransformData& transform = NEntity::getComponent<TransformData>(entity);
			glm::vec3 cameraForward;
			cameraForward.x = glm::cos(glm::radians(transform.eulerRotation.y)) * glm::cos(glm::radians(transform.eulerRotation.x));
			cameraForward.y = glm::sin(glm::radians(transform.eulerRotation.x));
			cameraForward.z = glm::sin(glm::radians(transform.eulerRotation.y)) * glm::cos(glm::radians(transform.eulerRotation.x));
			cameraForward = glm::normalize(cameraForward);

			glm::vec3 cameraRight = glm::cross(cameraForward, glm::vec3(1, 0, 0));
			glm::vec3 cameraUp = glm::cross(cameraRight, cameraForward);

			glm::vec3 front = glm::vec3(transform.position.x, transform.position.y, transform.position.z) + cameraForward;

			camera.viewMatrix = glm::lookAt(transform.position, front, cameraUp);
			camera.inverseView = glm::inverse(camera.viewMatrix);
		}

		void calculateOrthoViewMatrix(Entity entity, Camera& camera)
		{
			const TransformData& transform = NEntity::getComponent<TransformData>(entity);
			glm::vec3 cameraFront = glm::vec3(0, 0, -1) + glm::vec3(transform.position.x, transform.position.y, 0.0f);
			glm::vec3 cameraUp = glm::vec3(0, 1.0f, 0);

			camera.viewMatrix = glm::lookAt(glm::vec3(transform.position.x, transform.position.y, 20), cameraFront, cameraUp);
			camera.inverseView = glm::inverse(camera.viewMatrix);
		}

		void clearColorRgb(const Camera& camera, int attachment, glm::vec3 color)
		{
			NFramebuffer::clearColorAttachmentRgb(camera.framebuffer, attachment, color);
		}

		void clearColorUint32(Camera& camera, int attachment, uint32 color)
		{
			NFramebuffer::clearColorAttachmentUint32(camera.framebuffer, attachment, color);
		}

		glm::vec2 screenToOrtho(const Camera& camera)
		{
			return Input::screenToOrtho(camera);
		}

		void serialize(json* j, Entity entity, const Camera& camera)
		{
			json res;
			res["Entity"] = NEntity::getId(entity);
			res["Aspect"] = camera.aspect;
			res["Zoom"] = camera.zoom;
			res["Fov"] = camera.fov;
			res["ProjectionFarPlane"] = camera.projectionFarPlane;
			res["ProjectionNearPlane"] = camera.projectionNearPlane;
			res["ProjectionSize"] = {
				{ "X", camera.projectionSize.x },
				{ "Y", camera.projectionSize.y }
			};
			res["ClearColor"] = CMath::serialize(camera.clearColor);
			res["Framebuffer"] = NFramebuffer::serialize(camera.framebuffer);

			json& jRef = *j;
			int size = jRef["Components"].size();
			jRef["Components"][size]["Camera"] = res;
		}

		void deserialize(const json& j, Entity entity)
		{
			Framebuffer cameraFramebuffer;
			bool hasFramebuffer = j["Camera"].contains("Framebuffer");
			if (hasFramebuffer)
			{
				cameraFramebuffer = NFramebuffer::deserialize(j["Camera"]["Framebuffer"]);
				NFramebuffer::generate(cameraFramebuffer);
			}

			Camera camera = hasFramebuffer ? createCamera(cameraFramebuffer) : createCamera();
			JsonExtended::assignIfNotNull(j["Camera"], "Aspect", camera.aspect);
			JsonExtended::assignIfNotNull(j["Camera"], "Zoom", camera.zoom);
			JsonExtended::assignIfNotNull(j["Camera"], "Fov", camera.fov);
			JsonExtended::assignIfNotNull(j["Camera"], "ProjectionFarPlane", camera.projectionFarPlane);
			JsonExtended::assignIfNotNull(j["Camera"], "ProjectionNearPlane", camera.projectionNearPlane);
			JsonExtended::assignIfNotNull(j["Camera"], "ProjectionSize", camera.projectionSize);
			JsonExtended::assignIfNotNull(j["Camera"], "ClearColor", camera.clearColor);
			NEntity::addComponent<Camera>(entity, camera);
		}
	}

	namespace CameraSystem
	{
		void update(SceneData& scene, float dt)
		{
			auto view = scene.registry.view<Camera>();
			for (auto& rawEntity : view)
			{
				NCamera::Update(NEntity::createEntity(rawEntity), view.get<Camera>(rawEntity));
			}
		}

		void destroy(SceneData& scene)
		{
			auto view = scene.registry.view<Camera>();
			for (auto& rawEntity : view)
			{
				deleteEntity(NEntity::createEntity(rawEntity));
			}
		}

		void deleteEntity(Entity entity)
		{
			if (NEntity::hasComponent<Camera>(entity))
			{
				Camera& camera = NEntity::getComponent<Camera>(entity);
				NFramebuffer::destroy(camera.framebuffer);
			}
		}
	}
}