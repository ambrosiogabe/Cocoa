#pragma once
#include "externalLibs.h"
#include "jade/core/Core.h"

#include "jade/renderer/Shader.h"
#include "jade/renderer/Camera.h"
#include "jade/systems/System.h"
#include "jade/components/components.h"
#include "jade/components/Transform.h"
#include "jade/renderer/RenderBatch.h"
#include "jade/util/Settings.h"

namespace Jade {
    class RenderSystem : public System {
    public:
        RenderSystem(Camera* camera, const char* name)
            : System(name) {
            m_Camera = camera;
            m_Shader = new Shader(Settings::General::s_EngineAssetsPath + "shaders/SpriteRenderer.glsl");
        }

        void AddEntity(const Transform& transform, const SpriteRenderer& spr);
        virtual void Render(entt::registry& registry) override;
        virtual void ImGui(entt::registry& registry) override;

        Camera& GetCamera() const { return *m_Camera; }

        static void Serialize(entt::entity entity, const SpriteRenderer& spriteRenderer);
        static void Deserialize(json& json, entt::registry& registry, entt::entity entity);

    public:
        int m_TexSlots[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        const int MAX_BATCH_SIZE = 1000;

    private:
        Shader* m_Shader;
        std::vector<std::shared_ptr<RenderBatch>> m_Batches;
        Camera* m_Camera;
    };
}