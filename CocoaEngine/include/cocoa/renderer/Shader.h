#pragma once
#include "externalLibs.h"
#include "cocoa/file/CPath.h"

typedef unsigned int GLuint;

namespace Cocoa
{
	class COCOA Shader
	{
	public:
		Shader();
		Shader(const CPath& resourceName, bool isDefault=false);

		void Compile(const char* filepath);
		void Bind() const;
		void Unbind() const;
		void Delete();

		void UploadVec4(const char* varName, const glm::vec4& vec4) const;
		void UploadVec3(const char* varName, const glm::vec3& vec3) const;
		void UploadVec2(const char* varName, const glm::vec2& vec2) const;
		void UploadFloat(const char* varName, float value) const;
		void UploadInt(const char* varName, int value) const;
		void UploadIntArray(const char* varName, int size, int* array) const;
		void UploadUInt(const char* varName, uint32 value) const;

		void UploadMat4(const char* varName, const glm::mat4& mat4) const;
		void UploadMat3(const char* varName, const glm::mat3& mat3) const;

		const CPath& GetPath() const { return m_Path; }
		bool IsNull() const { return m_ShaderProgram == -1; }

	public:
		static Shader nullShader;

	private:
		GLuint GetVariableLocation(const char* varName) const;

	private:
		CPath m_Path;
		int m_ShaderProgram;
		bool m_IsDefault;

		std::unordered_map<std::string, GLuint> m_Variables;
	};
}