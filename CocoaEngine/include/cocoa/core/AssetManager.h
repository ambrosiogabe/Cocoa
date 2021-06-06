#ifndef COCOA_ENGINE_ASSET_MANAGER_H
#define COCOA_ENGINE_ASSET_MANAGER_H
#include "externalLibs.h"
#include "cocoa/file/Path.h"
#include "cocoa/renderer/Fonts/Font.h"
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
		COCOA Handle<Texture> loadTextureFromJson(const json& j, bool isDefault = false, int id = -1);
		COCOA Handle<Texture> loadTextureFromFile(Texture& texture, const Path& path, int id = -1);
		COCOA Handle<Texture> getTexture(const Path& path);
		COCOA const Texture& getTexture(uint32 resourceId);

		COCOA Handle<Font> loadFontFromJson(const Path& path, const json& j, bool isDefault = false, int id = -1);
		COCOA Handle<Font> loadFontFromTtfFile(const Path& fontFile, int fontSize, const Path& outputFile, int glyphRangeStart, int glyphRangeEnd, int padding, int upscaleResolution);
		COCOA Handle<Font> getFont(const Path& path);
		COCOA const Font& getFont(uint32 resourceId);

		COCOA Handle<Shader> loadShaderFromFile(const Path& path, bool isDefault = false, int id = -1);
		COCOA Handle<Shader> getShader(const Path& path);
		COCOA const Shader& getShader(uint32 resourceId);

		COCOA void loadTexturesFrom(const json& j);
		COCOA void loadFontsFrom(const json& j);
		COCOA json serialize();

		COCOA void clear();
		COCOA void init(uint32 scene);

		COCOA const List<Texture>& getAllTextures();
		COCOA const List<Font>& getAllFonts();
	}
}

#endif
