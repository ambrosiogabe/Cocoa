#include "externalLibs.h"

#include "cocoa/renderer/Shader.h"
#include "cocoa/util/Log.h"
#include "cocoa/core/Core.h"

namespace Cocoa
{
	Shader Shader::nullShader = Shader();

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex")
			return GL_VERTEX_SHADER;
		else if (type == "fragment" || type == "pixel")
			return GL_FRAGMENT_SHADER;

		Log::Assert(false, "Unkown shader type.");
		return 0;
	}

	static std::string ReadFile(const char* filepath)
	{
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
			Log::Error("Could not open file: '%s'", filepath);
		}

		return result;
	}

	Shader::Shader()
	{
		m_ShaderProgram = -1;
		m_IsDefault = false;
	}

	Shader::Shader(const CPath& resourceName, bool isDefault)
	{
		m_Path = resourceName;
		m_IsDefault = isDefault;
	}

	void Shader::Compile(const char* filepath)
	{
		std::string fileSource = ReadFile(filepath);

		std::unordered_map<GLenum, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = fileSource.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = fileSource.find_first_of("\r\n", pos);
			Log::Assert(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = fileSource.substr(begin, eol - begin);
			Log::Assert(ShaderTypeFromString(type), "Invalid shader type specified.");

			size_t nextLinePos = fileSource.find_first_not_of("\r\n", eol);
			pos = fileSource.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = fileSource.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? fileSource.size() - 1 : nextLinePos));
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
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
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

		m_ShaderProgram = program;
		
		// Get all the active vertex attributes and store them in our map of uniform variable locations
		int numUniforms;
		glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

		int maxCharLength;
		glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxCharLength);
		if (numUniforms > 0 && maxCharLength > 0)
		{
			char* charBuffer = (char*)malloc(sizeof(char) * maxCharLength);

			for (int i = 0; i < numUniforms; i++)
			{
				int length, size;
				GLenum type;
				glGetActiveUniform(program, i, maxCharLength, &length, &size, &type, charBuffer);
				int varLocation = glGetUniformLocation(m_ShaderProgram, charBuffer);
				m_Variables.insert({ std::string(charBuffer), varLocation });
			}

			free(charBuffer);
		}

		// Always detach shaders after a successful link.
		for (auto id : glShaderIDs)
			glDetachShader(program, id);
	}

	void Shader::Delete()
	{
		glDeleteProgram(m_ShaderProgram);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_ShaderProgram);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

	GLuint Shader::GetVariableLocation(const char* varName) const
	{
		if (m_Variables.find(varName) != m_Variables.end())
		{
			return m_Variables.at(varName);
		}

		return -1;
	}

	void Shader::UploadVec4(const char* varName, const glm::vec4& vec4) const
	{
		int varLocation = GetVariableLocation(varName);
		glUniform4f(varLocation, vec4.x, vec4.y, vec4.z, vec4.w);
	}

	void Shader::UploadVec3(const char* varName, const glm::vec3& vec3) const
	{
		int varLocation = GetVariableLocation(varName);
		glUniform3f(varLocation, vec3.x, vec3.y, vec3.z);
	}

	void Shader::UploadVec2(const char* varName, const glm::vec2& vec2) const
	{
		int varLocation = GetVariableLocation(varName);
		glUniform2f(varLocation, vec2.x, vec2.y);
	}

	void Shader::UploadFloat(const char* varName, float value) const
	{
		int varLocation = GetVariableLocation(varName);
		glUniform1f(varLocation, value);
	}

	void Shader::UploadInt(const char* varName, int value) const
	{
		int varLocation = GetVariableLocation(varName);
		glUniform1i(varLocation, value);
	}

	void Shader::UploadUInt(const char* varName, uint32 value) const
	{
		int varLocation = GetVariableLocation(varName);
		glUniform1ui(varLocation, value);
	}

	void Shader::UploadMat4(const char* varName, const glm::mat4& mat4) const
	{
		int varLocation = GetVariableLocation(varName);
		glUniformMatrix4fv(varLocation, 1, GL_FALSE, glm::value_ptr(mat4));
	}

	void Shader::UploadMat3(const char* varName, const glm::mat3& mat3) const
	{
		int varLocation = GetVariableLocation(varName);
		glUniformMatrix3fv(varLocation, 1, GL_FALSE, glm::value_ptr(mat3));
	}

	void Shader::UploadIntArray(const char* varName, int length, int* array) const
	{
		int varLocation = GetVariableLocation(varName);
		glUniform1iv(varLocation, length, array);
	}

}