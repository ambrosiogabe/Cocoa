#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/renderer/Shader.h"
#include "cocoa/renderer/Camera.h"
#include "cocoa/systems/System.h"
#include "cocoa/components/components.h"
#include "cocoa/components/Transform.h"
#include "cocoa/renderer/RenderBatch.h"
#include "cocoa/util/Settings.h"

namespace Cocoa
{
	class COCOA RenderSystem : public System
	{
	public:
		RenderSystem(const char* name, Scene* scene)
			: System(name, scene)
		{
			m_Camera = m_Scene->GetCamera();
		}

		void AddEntity(const Transform& transform, const SpriteRenderer& spr);
		virtual void Render() override;

		Camera& GetCamera() const { return *m_Camera; }

		static void Serialize(json& j, Entity entity, const SpriteRenderer& spriteRenderer);
		static void Deserialize(json& json, Entity entity);
		static void BindShader(std::shared_ptr<Shader> shader) { s_Shader = shader; }
		static void UploadUniform1ui(const char* name, uint32 val) { s_Shader->UploadUInt(name, val); }

	public:
		int m_TexSlots[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
		const int MAX_BATCH_SIZE = 1000;

	private:
		static std::shared_ptr<Shader> s_Shader;
		std::vector<std::shared_ptr<RenderBatch>> m_Batches;
		Camera* m_Camera;
	};
}