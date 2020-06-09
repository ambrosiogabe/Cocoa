#include "jade/renderer/Shader.h"
#include "jade/platform/windows/GlFunctions.h"
#include "jade/util/Log.h"

#include <windows.h>
#include <fstream>
#include <unordered_map>
#include <array>

static GLenum ShaderTypeFromString(const std::string& type)
{
    if (type == "vertex") 
        return GL_VERTEX_SHADER;
    else if (type == "fragment" || type == "pixel")
        return GL_FRAGMENT_SHADER;

    Log::Assert(false, "Unkown shader type.");
    return 0;
}

static std::string ReadFile(const char* filepath) {
    std::string result;
    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (in)
    {
        in.seekg(0, std::ios::end);
        result.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&result[0], result.size());
        in.close();
    }
    else
    {
        Log::Error("Could not open file.");
    }

    return result;
}

Shader::Shader(const char* resourceName) {
    Compile(resourceName);
}

void Shader::Compile(const char* filepath) {
    std::string source = ReadFile(filepath);

    std::unordered_map<GLenum, std::string> shaderSources;

    const char* typeToken = "#type";
    size_t typeTokenLength = strlen(typeToken);
    size_t pos = source.find(typeToken, 0);
    while (pos != std::string::npos)
    {
        size_t eol = source.find_first_of("\r\n", pos);
        Log::Assert(eol != std::string::npos, "Syntax error");
        size_t begin = pos + typeTokenLength + 1;
        std::string type = source.substr(begin, eol - begin);
        Log::Assert(ShaderTypeFromString(type), "Invalid shader type specified.");

        size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        pos = source.find(typeToken, nextLinePos);
        shaderSources[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
    }

    GLuint program = glCreateProgram();
    Log::Assert(shaderSources.size() <= 2, "Shader source must be less than 2.");
    std::array<GLenum, 2> glShaderIDs;
    int glShaderIDIndex = 0;

    for (auto& kv : shaderSources)
    {
        GLenum shaderType = kv.first;
        const std::string& source = kv.second;

        // Create an empty vertex shader handle
        GLuint shader = glCreateShader(shaderType);

        // Send the vertex shader source code to GL
        // Note that std::string's .c_str is NULL character terminated.
        const GLchar* sourceCStr = source.c_str();
        glShaderSource(shader, 1, &sourceCStr, 0);

        // Compile the vertex shader
        glCompileShader(shader);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE)
        {
            GLint maxLength = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

            // We don't need the shader anymore.
            glDeleteShader(shader);

            Log::Error("%s", infoLog.data());
            Log::Assert(false, "Shader compilation failed!");
            return;
        }

        glAttachShader(program, shader);
        glShaderIDs[glShaderIDIndex++] = shader;
    }

    // Link our program
    glLinkProgram(program);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int*)& isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(program);
        // Don't leak shaders either.
        for (auto id : glShaderIDs) 
            glDeleteShader(id);

        Log::Error("%s", infoLog.data());
        Log::Assert(false, "Shader linking failed!");
        return;
    }

    // Always detach shaders after a successful link.
    for (auto id : glShaderIDs)
        glDetachShader(program, id);

    m_ShaderProgram = program;
}

void Shader::Bind() {
    glUseProgram(m_ShaderProgram);
}

void Shader::Unbind() {
    glUseProgram(0);
}