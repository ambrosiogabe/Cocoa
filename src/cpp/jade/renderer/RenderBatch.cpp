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
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4 * RenderSystem::MAX_BATCH_SIZE, nullptr, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * 6 * RenderSystem::MAX_BATCH_SIZE, this->m_Indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, sizeof(Vertex().position) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, sizeof(Vertex().color) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, sizeof(Vertex().texCoords) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, sizeof(Vertex().texId) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, texId));
    glEnableVertexAttribArray(3);
}

void RenderBatch::Add(const Transform& transform, const SpriteRenderer& spr) {
    m_NumSprites++;

    Sprite* sprite = spr.m_Sprite;
    Texture* tex = sprite->m_Texture;
    if (tex != nullptr) {
        if (!HasTexture(tex)) {
            m_Textures[m_NumTextures] = tex;
            m_NumTextures++;
        }
    }

    LoadVertexProperties(transform, spr);
}

void RenderBatch::LoadVertexProperties(const Transform& transform, const SpriteRenderer& spr) {    
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
    
    bool isRotated = transform.m_EulerRotation.z != 0.0f;
    glm::mat4 matrix = glm::mat4(1.0f);
    if (isRotated) {
        matrix = glm::translate(matrix, transform.m_Position);
        matrix = glm::rotate(matrix, glm::radians(transform.m_EulerRotation.z), glm::vec3(0, 0, 1));
        matrix = glm::scale(matrix, transform.m_Scale);
    }

    float xAdd = 1.0f;
    float yAdd = 0.0f;
    for (int i=0; i < 4; i++) {
        if (i == 1) {
            yAdd = 1.0f;
        } else if (i == 2) {
            xAdd = 0.0f;
        } else if (i == 3) {
            yAdd = 0.0f;
        }

        glm::vec4 currentPos = glm::vec4(transform.m_Position.x + (xAdd * transform.m_Scale.x), transform.m_Position.y + (yAdd * transform.m_Scale.y), 0.0f, 1.0f);
        if (isRotated) {
            currentPos = matrix * glm::vec4(xAdd, yAdd, 0.0f, 1.0f);
        }
        // Load Attributes
        m_VertexStackPointer->position = glm::vec3(currentPos);
        m_VertexStackPointer->color = glm::vec4(color);
        m_VertexStackPointer->texCoords = glm::vec2(texCoords[i]);
        m_VertexStackPointer->texId = (float)texId;

        m_VertexStackPointer++;
    }
}

void RenderBatch::LoadEmptyVertexProperties() {
    for (int i=0; i < 4; i++) {
        m_VertexStackPointer = {};
        m_VertexStackPointer++;
    }
}

void RenderBatch::Render() {
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * 4 * m_NumSprites, &m_VertexBufferBase[0]);
    
    for (int i=0; i < this->m_NumTextures; i++) {
        glActiveTexture(GL_TEXTURE0 + i + 1);
        m_Textures[i]->Bind();
    }

    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawElements(GL_TRIANGLES, m_NumSprites * 6, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);

    for (int i=0; i < this->m_NumTextures; i++) {
        m_Textures[i]->Unbind();
    }
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

void RenderBatch::Clear() {
    this->m_VertexStackPointer = m_VertexBufferBase;
    this->m_NumSprites = 0;
    this->m_NumTextures = 0;
}