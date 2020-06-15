#include "jade/systems/RenderSystem.h"
#include "jade/platform/windows/GlFunctions.h"
#include "jade/components/components.h"
#include "jade/renderer/Shader.h"

#include <windows.h>
#include <gl/GL.h>
#include <glm/vec4.hpp>
#include <entt/entt.h>
#include <glm/gtx/transform.hpp>

RenderBatch::RenderBatch(RenderSystem* renderer) {
    this->m_Renderer = renderer;
    this->GenerateIndices();

    for (int i=0; i < 16; i++) {
        m_Textures[i] = nullptr;
    }
}

void RenderBatch::Start() {
    m_Shader = new Shader("C:/dev/C++/DungeonCrawler/assets/shaders/SpriteRenderer.glsl");

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(this->m_Indices), this->m_Indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, sizeof(Vertex().position) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, sizeof(Vertex().color) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, sizeof(Vertex().texCoords) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, sizeof(Vertex().texId) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, texId));
    glEnableVertexAttribArray(3);
}

void RenderBatch::Add(entt::entity& entity) {
    int index = m_NumSprites;
    m_Entities[index] = entity;
    m_NumSprites++;

    Sprite* sprite = m_Renderer->GetRegistry().get<SpriteRenderer>(entity).m_Sprite;
    Texture* tex = sprite->m_Texture;
    if (tex != nullptr) {
        if (!HasTexture(tex)) {
            m_Textures[m_NumTextures] = tex;
            m_NumTextures++;
        }
    }

    LoadVertexProperties(index);
}

void RenderBatch::LoadVertexProperties(int index) {
    entt::registry& registry = m_Renderer->GetRegistry();

    entt::entity entity = m_Entities[index];
    int offset = index * (sizeof(Vertex) / sizeof(float)) * 4;

    Transform transform = registry.get<Transform>(entity);
    SpriteRenderer spr = registry.get<SpriteRenderer>(entity);
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, transform.m_Position);
    matrix = glm::rotate(matrix, glm::radians(transform.m_EulerRotation.z), glm::vec3(0, 0, 1));
    matrix = glm::scale(matrix, transform.m_Scale);
    
    glm::vec4 color = spr.m_Color;
    Sprite* sprite = spr.m_Sprite;
    glm::vec2* texCoords = &sprite->m_TexCoords[0];

    int texId = 0;
    if (sprite->m_Texture != nullptr) {
        for (int i=0; i < 16; i++) {
            if (m_Textures[i] == sprite->m_Texture) {
                texId = i + 1;
                break;
            }
        }
    }
    
    float xAdd = 0.5f;
    float yAdd = -0.5f;
    for (int i=0; i < 4; i++) {
        if (i == 1) {
            yAdd = 0.5f;
        } else if (i == 2) {
            xAdd = -0.5f;
        } else if (i == 3) {
            yAdd = -0.5f;
        }

        glm::vec4 currentPos = matrix * glm::vec4(xAdd, yAdd, 0.0f, 1.0f);
        // Load position
        m_Vertices[offset] = currentPos.x;
        m_Vertices[offset + 1] = currentPos.y;
        m_Vertices[offset + 2] = 0.0f;

        // Load color
        m_Vertices[offset + 3] = color.x;
        m_Vertices[offset + 4] = color.y;
        m_Vertices[offset + 5] = color.z;
        m_Vertices[offset + 6] = color.w;

        // Load tex coords
        m_Vertices[offset + 7] = texCoords[i].x;
        m_Vertices[offset + 8] = texCoords[i].y;

        // Load tex id
        m_Vertices[offset + 9] = (float)texId;

        offset += (sizeof(Vertex) / sizeof(float));
    }
}

void RenderBatch::LoadEmptyVertexProperties(int index) {
    int vertexSize = (sizeof(Vertex) / sizeof(float));
    int offset = index * vertexSize * 4;
    for (int i=0; i < 4; i++) {
        for (int j=0; j < vertexSize; j++) {
            m_Vertices[offset + j] = 0;
        }
        offset += vertexSize;
    }
}

void RenderBatch::Render() {
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 4 * m_NumSprites, &m_Vertices[0]);
    
    m_Shader->Bind();
    m_Shader->UploadMat4("uProjection", m_Renderer->GetCamera().GetOrthoProjection());
    m_Shader->UploadMat4("uView", m_Renderer->GetCamera().GetOrthoView());
    for (int i=0; i < 16; i++) {
        if (m_Textures[i] == nullptr) {
            break;
        }
        glActiveTexture(GL_TEXTURE0 + i + 1);
        m_Textures[i]->Bind();
    }
    m_Shader->UploadIntArray("uTextures", 16, m_TexSlots);

    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawElements(GL_TRIANGLES, m_NumSprites * 6, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);

    for (int i=0; i < 16; i++) {
        if (m_Textures[i] == nullptr) {
            break;
        }
        m_Textures[i]->Unbind();
    }
    m_Shader->Unbind();
}

void RenderBatch::GenerateIndices() {
    for (int i=0; i < RenderSystem::MAX_BATCH_SIZE; i++) {
        this->LoadElementIndices(i);
    }
}

void RenderBatch::LoadElementIndices(int index) {
    int offsetArray = 6 * index;
    int offset = 4 * index;

    // Triangle 1
    this->m_Indices[offsetArray] = offset + 3;
    this->m_Indices[offsetArray + 1] = offset + 2;
    this->m_Indices[offsetArray + 2] = offset + 0;

    // Triangle 2
    this->m_Indices[offsetArray + 3] = offset + 0;
    this->m_Indices[offsetArray + 4] = offset + 2;
    this->m_Indices[offsetArray + 5] = offset + 1;
}