#pragma once

#include "jade/systems/RenderSystem.h"
#include "jade/components/components.h"

#include <vector>

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
};

typedef unsigned int uint;

class RenderSystem;
class RenderBatch {
public:
    RenderBatch() {}

    void Clear();
    void Start();
    void Add(SpriteRenderer& spr);
    void Render();

    int const NumSprites() {
        return m_NumSprites;
    }

private:
    void LoadVertexProperties(int index);
    void LoadEmptyVertexProperties(int index);

private:
    SpriteRenderer* m_Sprites[RenderSystem::MAX_BATCH_SIZE];
    float m_Vertices[RenderSystem::MAX_BATCH_SIZE * 4 * (sizeof(Vertex) / sizeof(float))];

    uint m_VAO, m_VBO;
    int m_ZIndex;
    int m_NumSprites;
};