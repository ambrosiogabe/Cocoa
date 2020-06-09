#pragma once

#include <glm/matrix.hpp>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <unordered_map>
#include <gl/glcorearb.h>

class Shader {
public:
    Shader(const char* resourceName);

    void Compile(const char* filepath);
    void Bind();
    void Unbind();
    void Delete();

    void UploadVec4(const char* varName, const glm::vec4& vec4);
    void UploadVec3(const char* varName, const glm::vec3& vec3);
    void UploadVec2(const char* varName, const glm::vec2& vec2);
    void UploadFloat(const char* varName, float value);
    void UploadInt(const char* varName, int value);

    void UploadMat4(const char* varName, const glm::mat4& mat4);
    void UploadMat3(const char* varName, const glm::mat3& mat3);

private:
    GLuint GetVariableLocation(const char* varName);

private:
    int m_ShaderProgram;
    bool m_BeingUsed;

    std::unordered_map<const char*, GLuint> m_Variables;
};