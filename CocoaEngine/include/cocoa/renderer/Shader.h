#pragma once
#include "externalLibs.h"
#include "cocoa/file/CPath.h"

typedef unsigned int GLuint;

namespace Cocoa
{
	struct Shader
	{
		uint32 ProgramId;
		int StartIndex; // This is the start index in the global shader variables vector
		bool IsDefault;
		CPath Filepath;
	};

	namespace NShader
	{
		COCOA Shader CreateShader();
		COCOA Shader CreateShader(const CPath& resourceName, bool isDefault=false);

		COCOA Shader Compile(const CPath& filepath, bool isDefault=false);
		COCOA void Bind(const Shader& shader);
		COCOA void Unbind(const Shader& shader);
		COCOA void Delete(Shader& shader);

		COCOA void UploadVec4(const Shader& shader, const char* varName, const glm::vec4& vec4);
		COCOA void UploadVec3(const Shader& shader, const char* varName, const glm::vec3& vec3);
		COCOA void UploadVec2(const Shader& shader, const char* varName, const glm::vec2& vec2);
		COCOA void UploadFloat(const Shader& shader, const char* varName, float value);
		COCOA void UploadInt(const Shader& shader, const char* varName, int value);
		COCOA void UploadIntArray(const Shader& shader, const char* varName, int size, const int* array);
		COCOA void UploadUInt(const Shader& shader, const char* varName, uint32 value);

		COCOA void UploadMat4(const Shader& shader, const char* varName, const glm::mat4& mat4);
		COCOA void UploadMat3(const Shader& shader, const char* varName, const glm::mat3& mat3);

		COCOA bool IsNull(const Shader& shader);
		COCOA void ClearAllShaderVariables();
	};
}