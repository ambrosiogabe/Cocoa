#pragma once

class Shader {
public:
    Shader(const char* resourceName);

    void Compile(const char* filepath);
    void Bind();
    void Unbind();
    void Delete();

private:
    int m_ShaderProgram;
    bool m_BeingUsed;
};