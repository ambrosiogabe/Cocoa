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

    public:
        int m_TexSlots[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        const int MAX_BATCH_SIZE = 1000;

    private:
        static std::shared_ptr<Shader> s_Shader;
        std::vector<std::shared_ptr<RenderBatch>> m_Batches;
        Camera* m_Camera;
    };
}