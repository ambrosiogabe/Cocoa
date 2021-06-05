#ifndef COCOA_ENGINE_SHADER_H
#define COCOA_ENGINE_SHADER_H
#include "externalLibs.h"
#include "cocoa/file/CPath.h"

typedef unsigned int GLuint;

namespace Cocoa
{
	struct Shader
	{
		uint32 programId;
		int startIndex; // This is the start index in the global shader variables vector
		bool isDefault;
		CPath filepath;
	};

	namespace NShader
	{
		COCOA Shader createShader();
		COCOA Shader createShader(const CPath& resourceName, bool isDefault=false);

		COCOA Shader compile(const CPath& filepath, bool isDefault=false);
		COCOA void bind(const Shader& shader);
		COCOA void unbind(const Shader& shader);
		COCOA void destroy(Shader& shader);

		COCOA void uploadVec4(const Shader& shader, const char* varName, const glm::vec4& vec4);
		COCOA void uploadVec3(const Shader& shader, const char* varName, const glm::vec3& vec3);
		COCOA void uploadVec2(const Shader& shader, const char* varName, const glm::vec2& vec2);
		COCOA void uploadFloat(const Shader& shader, const char* varName, float value);
		COCOA void uploadInt(const Shader& shader, const char* varName, int value);
		COCOA void uploadIntArray(const Shader& shader, const char* varName, int size, const int* array);
		COCOA void uploadUInt(const Shader& shader, const char* varName, uint32 value);

		COCOA void uploadMat4(const Shader& shader, const char* varName, const glm::mat4& mat4);
		COCOA void uploadMat3(const Shader& shader, const char* varName, const glm::mat3& mat3);

		COCOA bool isNull(const Shader& shader);
		COCOA void clearAllShaderVariables();
	};
}

#endif
