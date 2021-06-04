#pragma once
#include "externalLibs.h"
#include "cocoa/file/CPath.h"
#include "cocoa/renderer/fonts/Font.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/renderer/Shader.h"

namespace Cocoa
{
	enum class AssetType : char
	{
		None = 0,
		Texture = 1,
		Font = 2,
		Shader = 3
	};

	namespace AssetManager
	{
		COCOA Handle<Texture> LoadTextureFromJson(const json& j, bool isDefault = false, int id = -1);
		COCOA Handle<Texture> LoadTextureFromFile(Texture& texture, const CPath& path, int id = -1);
		COCOA Handle<Texture> GetTexture(const CPath& path);
		COCOA const Texture& GetTexture(uint32 resourceId);

		COCOA Handle<Font> LoadFontFromJson(const CPath& path, const json& j, bool isDefault = false, int id = -1);
		COCOA Handle<Font> LoadFontFromTtfFile(const CPath& fontFile, int fontSize, const CPath& outputFile, int glyphRangeStart, int glyphRangeEnd, int padding, int upscaleResolution);
		COCOA Handle<Font> GetFont(const CPath& path);
		COCOA const Font& GetFont(uint32 resourceId);

		COCOA Handle<Shader> LoadShaderFromFile(const CPath& path, bool isDefault = false, int id = -1);
		COCOA Handle<Shader> GetShader(const CPath& path);
		COCOA const Shader& GetShader(uint32 resourceId);

		COCOA void LoadTexturesFrom(const json& j);
		COCOA void LoadFontsFrom(const json& j);
		COCOA json Serialize();

		COCOA void Clear();
		COCOA void Init(uint32 scene);

		COCOA const List<Texture>& GetAllTextures();
		COCOA const List<Font>& GetAllFonts();
	}
}