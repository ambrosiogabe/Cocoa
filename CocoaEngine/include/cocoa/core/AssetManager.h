#pragma once
#include "externalLibs.h"
#include "cocoa/file/CPath.h"
#include "cocoa/util/Log.h"
#include "cocoa/renderer/fonts/Font.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/renderer/Shader.h"
#include "cocoa/core/Handle.h"

#include <entt/entt.hpp>

namespace Cocoa
{
	enum class AssetType : char
	{
		None = 0,
		Texture = 1,
		Font = 2,
		Shader = 3
	};

	class COCOA AssetManager
	{
	public:
		static Handle<Texture> LoadTextureFromJson(const json& j, bool isDefault = false, int id = -1);
		static Handle<Texture> LoadTextureFromFile(Texture& texture, const CPath& path, int id = -1);
		static Handle<Texture> GetTexture(const CPath& path);
		static const Texture& GetTexture(uint32 resourceId);

		static Handle<Font> LoadFontFromJson(const CPath& path, const json& j, bool isDefault = false, int id = -1);
		static Handle<Font> LoadFontFromTtfFile(const CPath& fontFile, int fontSize, const CPath& outputFile, int glyphRangeStart, int glyphRangeEnd, int padding, int upscaleResolution);
		static Handle<Font> GetFont(const CPath& path);
		static const Font& GetFont(uint32 resourceId);

		static Handle<Shader> LoadShaderFromFile(const CPath& path, bool isDefault = false, int id = -1);
		static Handle<Shader> GetShader(const CPath& path);
		static const Shader& GetShader(uint32 resourceId);

		static void LoadTexturesFrom(const json& j);
		static void LoadFontsFrom(const json& j);
		static json Serialize();

		static void Clear();
		static void Init(uint32 scene);

		static const std::vector<Texture>& GetAllTextures() { return s_Textures; }
		static const std::vector<Font>& GetAllFonts() { return s_Fonts; }

	public:
		static uint32 s_CurrentScene;
		static uint32 s_ResourceCount;

		static std::vector<Texture> s_Textures;
		static std::vector<Font> s_Fonts;
		static std::vector<Shader> s_Shaders;
	};
}