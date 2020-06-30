#include "jade/renderer/DebugDraw.h"
#include "jade/core/Application.h"

#include <glad/glad.h>

namespace Jade {
    Line2D DebugDraw::m_Lines[MAX_LINES] {};
    float* DebugDraw::m_VertexArray2D = new float[MAX_LINES * 6 * 4];
    uint32 DebugDraw::m_NumLines = 0;
    Shader* DebugDraw::m_Shader = nullptr;
    uint32 DebugDraw::m_VAO = 0;
    uint32 DebugDraw::m_VBO = 0;
    bool DebugDraw::m_Started = false;

    void DebugDraw::Start() {
        DebugDraw::m_Shader = new Shader("assets/shaders/debugLine2D.glsl");

        // Generate and bind vertex array
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        // Create a buffer object and allocate space
        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_LINES * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

        // Create another buffer object for the indices, then upload
        uint32 ebo;
        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        // TODO: double check that these sizes are correct
        int* indices = GetIndicesBuffer();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * MAX_LINES * 6, indices, GL_STATIC_DRAW);
        delete indices;

        // Enable the attrib pointers
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 6 * sizeof(float), 0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, false, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    int* DebugDraw::GetIndicesBuffer() {
        int* indices = new int[MAX_LINES * 6];
        for (int i=0; i < MAX_LINES; i++) {
            LoadElementIndices(i, indices);
        }
        return indices;
    }

    void DebugDraw::LoadElementIndices(int index, int* indices) {
        int offsetArray = 6 * index;
        int offset = 4 * index;

        // Triangle 1
        indices[offsetArray] = offset + 3;
        indices[offsetArray + 1] = offset + 2;
        indices[offsetArray + 2] = offset + 1;

        // Triangle 2
        indices[offsetArray + 3] = offset + 1;
        indices[offsetArray + 4] = offset + 2;
        indices[offsetArray + 5] = offset + 0;
    }   

    void DebugDraw::BeginFrame() {
        if (!m_Started) {
            Start();
            m_Started = true;
        }

        // Remove dead lines
        for (uint32 i=0; i < m_NumLines; i++) {
            if (m_Lines[i].BeginFrame() <= 0) {
                for (uint32 j=i; j < m_NumLines - 1; j++) {
                    m_Lines[j] = m_Lines[j + 1];
                }
                i--;
                m_NumLines--;
            }
        }
    }

    void DebugDraw::EndFrame() {
        int index = 0;
        for (uint32 lineIndex=0; lineIndex < m_NumLines; lineIndex++) {
            Line2D line = m_Lines[lineIndex];
            const glm::vec2* verts = line.GetVerts();
            const glm::vec3& color = line.GetColor();
            
            for (int i=0; i < 4; i++) {
                m_VertexArray2D[index] = verts[i].x;
                m_VertexArray2D[index + 1] = verts[i].y;
                m_VertexArray2D[index + 2] = -10.0f;
                
                m_VertexArray2D[index + 3] = color.r;
                m_VertexArray2D[index + 4] = color.g;
                m_VertexArray2D[index + 5] = color.b;
                index += 6;
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * m_NumLines * 6 * 4, m_VertexArray2D);

        // Use shader
        m_Shader->Bind();
        m_Shader->UploadMat4("uProjection", Application::Get()->GetScene()->GetCamera()->GetOrthoProjection());
        m_Shader->UploadMat4("uView", Application::Get()->GetScene()->GetCamera()->GetOrthoView());

        // Bind vao
        glBindVertexArray(m_VAO);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glDrawElements(GL_TRIANGLES, m_NumLines * 6, GL_UNSIGNED_INT, 0);

        // Disable location
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glBindVertexArray(0);
    }



    // ------Add Line2D methods---------------------------------------------------------------------------------------

    void DebugDraw::AddLine2D(glm::vec2& from, glm::vec2& to) {
        glm::vec3 green = glm::vec3(0, 1, 0);
        AddLine2D(from, to, 1.0f, green, 1);
    }

    void DebugDraw::AddLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth) {
        glm::vec3 green = glm::vec3(0, 1, 0);
        AddLine2D(from, to, strokeWidth, green, 1);
    }

    void DebugDraw::AddLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth, glm::vec3& color) {
        AddLine2D(from, to, strokeWidth, color, 1);
    }

    void DebugDraw::AddLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth, glm::vec3& color, int lifetime) {
        if (m_NumLines >= MAX_LINES) return;
        m_Lines[m_NumLines] = Line2D(from, to, color, strokeWidth, lifetime);
        m_NumLines++;
    }

    // ---------------------------------------------------------------------------------------------------------------

}