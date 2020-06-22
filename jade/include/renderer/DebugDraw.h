#pragma once

#include "Line2D.h"
#include "Shader.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace Jade {
    class DebugDraw {
    public:
        static void BeginFrame();
        static void EndFrame();

        // ------Add Line2D methods---------------------------------------------------------------------------------------
        static void AddLine2D(glm::vec2& from, glm::vec2& to);
        static void AddLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth);
        static void AddLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth, glm::vec3& color);
        static void AddLine2D(glm::vec2& from, glm::vec2& to, float strokeWidth, glm::vec3& color, int lifetime);
        // ---------------------------------------------------------------------------------------------------------------

    private:
        static void Start();
        static int* GetIndicesBuffer();
        static void LoadElementIndices(int index, int* indices);

    private:
        static const int MAX_LINES = 500;
        static Line2D m_Lines[MAX_LINES];
        static float* m_VertexArray2D;
        
        static uint32 m_NumLines;
        static Shader* m_Shader;

        static uint32 m_VAO;
        static uint32 m_VBO;

        static bool m_Started;
    };
}