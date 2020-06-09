#include "jade/systems/RenderSystem.h"
#include "jade/platform/windows/GlFunctions.h"
#include "jade/components/components.h"
#include "jade/renderer/Shader.h"

#include <windows.h>
#include <gl/GL.h>
#include <glm/vec4.hpp>
#include <entt/entt.h>
#include <glm/gtx/transform.hpp>

int indices[] = {
    3,  2, 0,
    0, 2, 1,

    7, 6, 4,
    4, 6, 5,

    11, 10, 8,
    8, 10, 9
};

RenderBatch::RenderBatch() {
    this->GenerateIndices();
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
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, sizeof(Vertex().position) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, sizeof(Vertex().color) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);
}

void RenderBatch::Add(entt::actor& actor) {
    int index = m_NumSprites;
    m_Actors[index] = &actor;

    LoadVertexProperties(index);
    
    m_NumSprites++;
}

void RenderBatch::LoadVertexProperties(int index) {
    entt::actor* actor = m_Actors[index];

    int offset = index * (sizeof(Vertex) / sizeof(float)) * 4;

    Transform transform = actor->get<Transform>();
    SpriteRenderer spr = actor->get<SpriteRenderer>();
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::translate(matrix, transform.m_Position);
    matrix = glm::rotate(matrix, transform.m_EulerRotation.z, glm::vec3(0, 0, 1));
    
    glm::vec4 color = spr.m_Color;
    
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

        glm::vec4 currentPos = glm::vec4(xAdd, yAdd, 0.0f, 1.0f);// * matrix;
        // Load position
        m_Vertices[offset] = currentPos.x;
        m_Vertices[offset + 1] = currentPos.y;
        m_Vertices[offset + 2] = 0.0f;

        // Load color
        m_Vertices[offset + 3] = color.x;
        m_Vertices[offset + 4] = color.y;
        m_Vertices[offset + 5] = color.z;
        m_Vertices[offset + 6] = color.w;

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
    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    //glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, m_NumSprites * 6, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
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