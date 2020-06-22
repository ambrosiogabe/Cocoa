#pragma once

#include "core/Core.h"
#include "util/Log.h"

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

class Line2D {
public:
    Line2D()
        : m_From(glm::vec2()), m_To(glm::vec2()), m_Color(glm::vec3()), m_Stroke(0.0f), m_Lifetime(0) {}

    Line2D(glm::vec2 from, glm::vec2 to, glm::vec3 color, float stroke, int lifetime)
        : m_From(from), m_To(to), m_Color(color), m_Stroke(stroke), m_Lifetime(lifetime) {
        
        glm::vec2 line = from - to;
        glm::vec2 normal = glm::vec2(line.y, -line.x);
        normal = glm::normalize(normal);

        float halfStroke = stroke / 2.0f;
        m_Verts[0] = from + (normal * halfStroke);
        m_Verts[1] = from - (normal * halfStroke);
        m_Verts[2] = m_Verts[0] - line;
        m_Verts[3] = m_Verts[1] - line;
    }

    int BeginFrame()                     { return m_Lifetime--; }
    const glm::vec2& GetFrom()     const { return m_From; }
    const glm::vec2& GetTo()       const { return m_To; }
    const glm::vec2& GetStart()    const { return m_From; }
    const glm::vec2& GetEnd()      const { return m_To; }
    const glm::vec3& GetColor()    const { return m_Color; }
    float GetStroke()              const { return m_Stroke; }
    const glm::vec2* GetVerts()    const { return &m_Verts[0]; }
    //float lengthSquared()       const { return glm::vec2.Length(m_From - m_To);}


private:
    glm::vec2 m_From;
    glm::vec2 m_To;
    glm::vec3 m_Color;
    glm::vec2 m_Verts[4]{};
    float m_Stroke;
    uint32 m_Lifetime;
};