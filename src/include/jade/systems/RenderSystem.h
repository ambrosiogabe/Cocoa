#pragma once

#include "jade/renderer/Shader.h"
#include "jade/renderer/Camera.h"

#include <vector>
#include <entt/entt.h>

typedef unsigned int uint;

class RenderBatch;
class RenderSystem {
public:
    RenderSystem(Camera* camera, entt::registry& registry) 
        : m_Camera(camera), m_Registry(registry) {}

    void AddActor(entt::actor& actor);
    void Render();

    Camera& GetCamera() const { return *m_Camera; }
    entt::registry& GetRegistry() const { return m_Registry; }

public:
    static const int MAX_BATCH_SIZE;

private:
    std::vector<RenderBatch*> m_Batches;
    Camera* m_Camera;
    entt::registry& m_Registry;
};

const int RenderSystem::MAX_BATCH_SIZE = 1000;


struct Vertex {
    union {
        float position[3];
        struct {
            float x;
            float y;
            float z;
        };
    };
    union {
        float color[4];
        struct {
            float r;
            float g;
            float b;
            float a;
        };
    };
    union {
        float texCoords[2];
        struct {
            float uv0;
            float uv1;
        };
    };
    float texId;
};

class RenderBatch {
public:
    RenderBatch(RenderSystem* renderer);

    void Clear();
    void Start();
    void Add(entt::actor& spr);
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
        for (int i=0; i < 8; i++) {
            if (m_Textures[i] == tex) {
                return true;
            }
        }
        return false;
    }

private:
    void LoadVertexProperties(int index);
    void LoadEmptyVertexProperties(int index);
    
    void LoadElementIndices(int index);
    void GenerateIndices();

private:
    entt::entity m_Actors[RenderSystem::MAX_BATCH_SIZE];
    Shader* m_Shader;
    RenderSystem* m_Renderer;
    
    float m_Vertices[RenderSystem::MAX_BATCH_SIZE * 4 * (sizeof(Vertex) / sizeof(float))];
    int m_Indices[RenderSystem::MAX_BATCH_SIZE * 6];
    Texture* m_Textures[16];
    int m_TexSlots[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    uint m_VAO, m_VBO, m_EBO;
    int m_ZIndex = 0;
    // TODO: make these like uint16s
    int m_NumSprites = 0;
    int m_NumTextures = 0;
};