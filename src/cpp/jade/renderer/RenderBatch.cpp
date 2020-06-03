#include "jade/renderer/RenderBatch.h"
#include "jade/platform/windows/GlFunctions.h"

#include <windows.h>
#include <gl/GL.h>
#include <glm/vec4.hpp>

void RenderBatch::Start() {
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, sizeof(Vertex().position) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, sizeof(Vertex().color) / sizeof(float), GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);
}

void RenderBatch::Add(SpriteRenderer& spr) {
    int index = m_NumSprites;
    m_Sprites[index] = &spr;

    LoadVertexProperties(index);
}

void RenderBatch::LoadVertexProperties(int index) {
    SpriteRenderer* spr = m_Sprites[index];
    
    int offset = index * (sizeof(Vertex) / sizeof(float)) * 4;

    glm::vec4 color = spr->m_Color;
    
    float xAdd = 0.5f;
    float yAdd = 0.5f;
    for (int i=0; i < 4; i++) {
        if (i == 1) {
            yAdd = 0.5f;
        } else if (i == 2) {
            xAdd = -0.5f;
        } else if (i == 3) {
            yAdd = -0.5f;
        }

        // Load position
        m_Vertices[offset] = xAdd;
        m_Vertices[offset + 1] = yAdd;
        m_Vertices[offset + 2] = m_ZIndex;

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
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_NumSprites, m_Vertices);

    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, m_NumSprites * 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
}