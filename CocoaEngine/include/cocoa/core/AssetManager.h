#pragma once
#include "externalLibs.h"
#include "cocoa/file/CPath.h"
#include "cocoa/util/Log.h"
#include "cocoa/renderer/fonts/Font.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/core/Handle.h"

#include <entt/entt.hpp>

namespace Cocoa
{
	enum class AssetType : char
	{
		None=0,
		Texture=1,
		Font=2
	};

	class COCOA AssetManager
	{
	public:		
		static Handle<Texture> LoadTextureFromFile(const CPath& path, bool isDefault = false);
		static Handle<Texture> GetTexture(const CPath& path);
		static const Texture& GetTexture(uint32 resourceId);

		static Handle<Font> LoadFontFromJson(const CPath& path, const json& j, bool isDefault = false);
		static Handle<Font> LoadFontFromTtfFile(const CPath& fontFile, int fontSize, const CPath& outputFile, int glyphRangeStart, int glyphRangeEnd, int padding, int upscaleResolution);
		static Handle<Font> GetFont(const CPath& path);
		static const Font& GetFont(uint32 resourceId);

		static std::unordered_map<uint32, uint32> LoadTexturesFrom(const json& j);
		static std::unordered_map<uint32, uint32> LoadFontsFrom(const json& j);
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
	};
}