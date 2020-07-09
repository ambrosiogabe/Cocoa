#pragma once

#include "jade/renderer/Shader.h"
#include "jade/renderer/Camera.h"
#include "jade/systems/System.h"
#include "jade/components/components.h"
#include "jade/components/Transform.h"
#include "jade/core/Core.h"

#include <vector>
#include <entt.h>

namespace Jade {
    struct Vertex {
        glm::vec3 position;
        glm::vec4 color;
        glm::vec2 texCoords;
        float texId;
    };

    class RenderBatch;
    class RenderSystem : public System {
    public:
        RenderSystem(Camera* camera, const char* name)
            : System(name) {
            m_Camera = camera;
            m_Shader = new Shader("assets/shaders/SpriteRenderer.glsl");
        }

        void AddEntity(const Transform& transform, const SpriteRenderer& spr);
        virtual void Render(entt::registry& registry) override;
        virtual void ImGui(entt::registry& registry) override;

        Camera& GetCamera() const { return *m_Camera; }

    public:
        int m_TexSlots[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
        static const int MAX_BATCH_SIZE;

    private:
        Shader* m_Shader;
        std::vector<RenderBatch*> m_Batches;
        Camera* m_Camera;
    };

    class RenderBatch {
    public:
        RenderBatch(RenderSystem* renderer);

        void Clear();
        void Start();
        void Add(const Transform& transform, const SpriteRenderer& spr);
        void Render();

        bool const HasRoom() {
            return m_NumSprites < RenderSystem::MAX_BATCH_SIZE;
        }

        int const NumSprites() {
            return m_NumSprites;
        }

        bool const HasTextureRoom() {
            return m_NumTextures < 16;
        }

        bool const HasTexture(Texture* tex) {
            for (int i=0; i < 16; i++) {
                if (m_Textures[i] == tex) {
                    return true;
                }
            }
            return false;
        }

    private:
        void LoadVertexProperties(const Transform& transform, const SpriteRenderer& spr);
        void LoadEmptyVertexProperties();
        
        void LoadElementIndices(int index);
        void GenerateIndices();

    private:
        RenderSystem* m_Renderer;
        
        Vertex* m_VertexBufferBase = new Vertex[RenderSystem::MAX_BATCH_SIZE * 4];
        Vertex* m_VertexStackPointer = &m_VertexBufferBase[0];
        uint32* m_Indices = new uint32[RenderSystem::MAX_BATCH_SIZE * 6];
        Texture* m_Textures[16];

        uint32 m_VAO, m_VBO, m_EBO;
        uint16 m_ZIndex = 0;
        // TODO: make these like uint16s
        uint16 m_NumSprites = 0;
        uint16 m_NumTextures = 0;

    };
}