#include "externalLibs.h"

#include "cocoa/renderer/Shader.h"
#include "cocoa/util/CMath.h"
#include "cocoa/core/Core.h"

namespace Cocoa
{
	namespace NShader
	{
		// Internal Structures
		struct ShaderVariable
		{
			std::string Name;
			uint32 Hash;
			GLint VarLocation;
			uint32 ShaderProgramId;
		};

		// Internal Variables
		static std::vector<ShaderVariable> m_AllShaderVariables = std::vector<ShaderVariable>(10);

		// Forward Declarations
		static GLint GetVariableLocation(const Shader& shader, const char* varName);
		static GLenum ShaderTypeFromString(const std::string& type);
		static std::string ReadFile(const char* filepath);

		Shader createShader()
		{
			return {
				(uint32)-1,
				-1,
				false,
				CPath::create()
			};
		}

		Shader createShader(const CPath& resourceName, bool isDefault)
		{
			// TODO: Should compilation happen on creation?
			return compile(resourceName, isDefault);
		}

		Shader compile(const CPath& filepath, bool isDefault)
		{
			std::string fileSource = ReadFile(filepath.path);

			std::unordered_map<GLenum, std::string> shaderSources;

			const char* typeToken = "#type";
			size_t typeTokenLength = strlen(typeToken);
			size_t pos = fileSource.find(typeToken, 0);
			while (pos != std::string::npos)
			{
				size_t eol = fileSource.find_first_of("\r\n", pos);
				Logger::Assert(eol != std::string::npos, "Syntax error");
				size_t begin = pos + typeTokenLength + 1;
				std::string type = fileSource.substr(begin, eol - begin);
				Logger::Assert(ShaderTypeFromString(type), "Invalid shader type specified.");

				size_t nextLinePos = fileSource.find_first_not_of("\r\n", eol);
				pos = fileSource.find(typeToken, nextLinePos);
				shaderSources[ShaderTypeFromString(type)] = fileSource.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? fileSource.size() - 1 : nextLinePos));
			}

			GLuint program = glCreateProgram();
			Logger::Assert(shaderSources.size() <= 2, "Shader source must be less than 2.");
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
					std::vector<GLchar> infoLogger(maxLength);
					glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLogger[0]);

					// We don't need the shader anymore.
					glDeleteShader(shader);

					Logger::Error("%s", infoLogger.data());
					Logger::Assert(false, "Shader compilation failed!");
					return createShader();
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
				std::vector<GLchar> infoLogger(maxLength);
				glGetProgramInfoLog(program, maxLength, &maxLength, &infoLogger[0]);

				// We don't need the program anymore.
				glDeleteProgram(program);
				// Don't leak shaders either.
				for (auto id : glShaderIDs)
					glDeleteShader(id);

				Logger::Error("%s", infoLogger.data());
				Logger::Assert(false, "Shader linking failed!");
				return NShader::createShader();
			}

			int startIndex = m_AllShaderVariables.size();

			// Get all the active vertex attributes and store them in our map of uniform variable locations
			int numUniforms;
			glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);

			int maxCharLength;
			glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxCharLength);
			if (numUniforms > 0 && maxCharLength > 0)
			{
				char* charBuffer = (char*)AllocMem(sizeof(char) * maxCharLength);

				for (int i = 0; i < numUniforms; i++)
				{
					int length, size;
					GLenum type;
					glGetActiveUniform(program, i, maxCharLength, &length, &size, &type, charBuffer);
					GLint varLocation = glGetUniformLocation(program, charBuffer);
					m_AllShaderVariables.push_back({
						std::string(charBuffer),
						CMath::hashString(charBuffer),
						varLocation,
						program
					});
				}

				FreeMem(charBuffer);
			}

			// Always detach shaders after a successful link.
			for (auto id : glShaderIDs)
				glDetachShader(program, id);

			return Shader{
				program,
				startIndex,
				isDefault,
				filepath
			};
		}

		void destroy(Shader& shader)
		{
			glDeleteProgram(shader.programId);
		}

		void bind(const Shader& shader)
		{
			glUseProgram(shader.programId);
		}

		void unbind(const Shader& shader)
		{
			glUseProgram(0);
		}

		void uploadVec4(const Shader& shader, const char* varName, const glm::vec4& vec4)
		{
			int varLocation = GetVariableLocation(shader, varName);
			glUniform4f(varLocation, vec4.x, vec4.y, vec4.z, vec4.w);
		}

		void uploadVec3(const Shader& shader, const char* varName, const glm::vec3& vec3)
		{
			int varLocation = GetVariableLocation(shader, varName);
			glUniform3f(varLocation, vec3.x, vec3.y, vec3.z);
		}

		void uploadVec2(const Shader& shader, const char* varName, const glm::vec2& vec2)
		{
			int varLocation = GetVariableLocation(shader, varName);
			glUniform2f(varLocation, vec2.x, vec2.y);
		}

		void uploadFloat(const Shader& shader, const char* varName, float value)
		{
			int varLocation = GetVariableLocation(shader, varName);
			glUniform1f(varLocation, value);
		}

		void uploadInt(const Shader& shader, const char* varName, int value)
		{
			int varLocation = GetVariableLocation(shader, varName);
			glUniform1i(varLocation, value);
		}

		void uploadUInt(const Shader& shader, const char* varName, uint32 value)
		{
			int varLocation = GetVariableLocation(shader, varName);
			glUniform1ui(varLocation, value);
		}

		void uploadMat4(const Shader& shader, const char* varName, const glm::mat4& mat4)
		{
			int varLocation = GetVariableLocation(shader, varName);
			glUniformMatrix4fv(varLocation, 1, GL_FALSE, glm::value_ptr(mat4));
		}

		void uploadMat3(const Shader& shader, const char* varName, const glm::mat3& mat3)
		{
			int varLocation = GetVariableLocation(shader, varName);
			glUniformMatrix3fv(varLocation, 1, GL_FALSE, glm::value_ptr(mat3));
		}

		void uploadIntArray(const Shader& shader, const char* varName, int length, const int* array)
		{
			int varLocation = GetVariableLocation(shader, varName);
			glUniform1iv(varLocation, length, array);
		}

		bool isNull(const Shader& shader) 
		{ 
			return shader.programId == -1; 
		}

		void clearAllShaderVariables()
		{
			m_AllShaderVariables.clear();
		}

		// Private functions
		static GLint GetVariableLocation(const Shader& shader, const char* varName)
		{
			Logger::Assert(shader.startIndex >= 0 && shader.startIndex < m_AllShaderVariables.size(), "Invalid shader. Cannot find variable on this shader.");
			uint32 hash = CMath::hashString(varName);

			for (int i = shader.startIndex; i < m_AllShaderVariables.size(); i++)
			{
				const ShaderVariable& shaderVar = m_AllShaderVariables[i];
				if (shaderVar.ShaderProgramId != shader.programId)
				{
					Logger::Warning("Could not find shader variable '%s' for shader '%s'", varName, shader.filepath.path);
					break;
				}

				if (shaderVar.Hash == hash && shaderVar.Name == varName)
				{
					return shaderVar.VarLocation;
				}
			}

			return -1;
		}

		static GLenum ShaderTypeFromString(const std::string& type)
		{
			if (type == "vertex")
				return GL_VERTEX_SHADER;
			else if (type == "fragment" || type == "pixel")
				return GL_FRAGMENT_SHADER;

			Logger::Assert(false, "Unkown shader type.");
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
				Logger::Error("Could not open file: '%s'", filepath);
			}

			return result;
		}
	}
}