#include "cocoa/core/AssetManager.h"
#include "cocoa/util/Log.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/file/IFile.h"
#include "cocoa/util/JsonExtended.h"

namespace Cocoa
{
	std::vector<Texture> AssetManager::s_Textures = std::vector<Texture>();
	std::vector<Font> AssetManager::s_Fonts = std::vector<Font>();
	uint32 AssetManager::s_CurrentScene = 0;
	uint32 AssetManager::s_ResourceCount = 0;

	void AssetManager::Init(uint32 scene)
	{
		s_CurrentScene = scene;
		s_ResourceCount = 0;
		// TODO: Clear assets somehow?
	}

	const Texture& AssetManager::GetTexture(uint32 resourceId)
	{
		if (resourceId < s_Textures.size())
		{
			return s_Textures[resourceId];
		}

		return Texture::nullTexture;
	}

	Handle<Texture> AssetManager::GetTexture(const CPath& path)
	{
		int i = 0;
		for (auto& tex : s_Textures)
		{
			if (tex.GetFilepath() == path)
			{
				return Handle<Texture>(i);
			}
			i++;
		}

		return Handle<Texture>();
	}

	Handle<Texture> AssetManager::LoadTextureFromFile(const CPath& path, bool isDefault)
	{
		Handle<Texture> texture = GetTexture(path);
		if (!texture.IsNull())
		{
			Log::Warning("Tried to load asset that has already been loaded '%s'", path.Filepath());
			return texture;
		}

		CPath absPath = IFile::GetAbsolutePath(path);
		int index = s_Textures.size();
		s_Textures.push_back({ absPath.Filepath(), isDefault });

		Texture& newTexture = s_Textures.at(index);
		newTexture.Load();
		return Handle<Texture>(index);
	}

	const Font& AssetManager::GetFont(uint32 resourceId)
	{
		if (resourceId < s_Fonts.size())
		{
			return s_Fonts[resourceId];
		}

		return Font::nullFont;
	}

	Handle<Font> AssetManager::GetFont(const CPath& path)
	{
		int i = 0;
		for (auto& font : s_Fonts)
		{
			if (font.m_Path == path)
			{
				return Handle<Texture>(i);
			}
			i++;
		}

		return Handle<Font>();
	}

	Handle<Font> AssetManager::LoadFontFromJson(const CPath& path, const json& j, bool isDefault)
	{
		Handle<Font> font = GetFont(path);
		if (!font.IsNull())
		{
			Log::Warning("Tried to load asset that has already been loaded '%s'.", path.Filepath());
			return font;
		}

		CPath absPath = IFile::GetAbsolutePath(path);
		int index = s_Fonts.size();
		s_Fonts.push_back(Font{ absPath, isDefault });

		Font& newFont = s_Fonts.at(index);
		newFont.Deserialize(j);
		return Handle<Texture>(index);
	}

	Handle<Font> AssetManager::LoadFontFromTtfFile(const CPath& fontFile, int fontSize, const CPath& outputFile, int glyphRangeStart, int glyphRangeEnd, int padding, int upscaleResolution)
	{
		Handle<Font> font = GetFont(fontFile);
		if (!font.IsNull())
		{
			Log::Warning("Tried to load asset that has already been loaded '%s'.", fontFile.Filepath());
			return font;
		}

		CPath absPath = IFile::GetAbsolutePath(fontFile);
		int index = s_Fonts.size();
		s_Fonts.push_back(Font{ absPath, false });

		Font& newFont = s_Fonts.at(index);
		newFont.GenerateSdf(fontFile, fontSize, outputFile, glyphRangeStart, glyphRangeEnd, padding, upscaleResolution);
		newFont.m_FontTexture = AssetManager::LoadTextureFromFile(outputFile);
		return Handle<Font>(index);
	}

	json AssetManager::Serialize()
	{
		json res;

		res["SceneID"] = s_CurrentScene;

		int i = 0;
		for (auto& assetIt : s_Textures)
		{
			if (!assetIt.IsDefault())
			{
				json assetSerialized = assetIt.Serialize();
				assetSerialized["ResourceId"] = i;
				res["Textures"][std::to_string(i)] = assetSerialized;
			}
			i++;
		}

		i = 0;
		for (auto& assetIt : s_Fonts)
		{
			if (!assetIt.IsDefault())
			{
				json assetSerialized = assetIt.Serialize();
				assetSerialized["ResourceId"] = i;
				res["Fonts"][std::to_string(i)] = assetSerialized;
			}
			i++;
		}

		return res;
	}

	std::unordered_map<uint32, uint32> AssetManager::LoadTexturesFrom(const json& j)
	{
		std::unordered_map<uint32, uint32> resourceIDMap{};

		uint32 scene = -1;
		JsonExtended::AssignIfNotNull(j["SceneID"], scene);

		// TODO: Switch all json accessors to use .contains(key) instead of [key]
		if (scene >= 0 && j.contains("Textures"))
		{
			for (auto it = j["Textures"].begin(); it != j["Textures"].end(); ++it)
			{
				const json& assetJson = it.value();
				uint32 resourceId = -1;
				JsonExtended::AssignIfNotNull(assetJson["ResourceId"], resourceId);

				CPath path = "";
				if (assetJson.contains("Filepath"))
				{
					path = CPath(assetJson["Filepath"], false);
				}

				if (resourceId >= 0)
				{
					Handle<Texture> tex = LoadTextureFromFile(path);
					resourceIDMap.insert({ resourceId, tex.m_AssetId });
				}
			}
		}

		return resourceIDMap;
	}

	std::unordered_map<uint32, uint32> AssetManager::LoadFontsFrom(const json& j)
	{
		std::unordered_map<uint32, uint32> resourceIDMap{};

		uint32 scene = -1;
		JsonExtended::AssignIfNotNull(j["SceneID"], scene);

		if (scene >= 0 && j.contains("Fonts"))
		{
			for (auto it = j["Fonts"].begin(); it != j["Fonts"].end(); ++it)
			{
				const json& assetJson = it.value();
				uint32 resourceId = -1;
				JsonExtended::AssignIfNotNull(assetJson["ResourceId"], resourceId);

				CPath path = "";
				if (assetJson.contains("Filepath"))
				{
					path = CPath(assetJson["Filepath"], false);
				}

				if (resourceId >= 0)
				{
					Handle<Font> font = LoadFontFromJson(path, assetJson);
					resourceIDMap.insert({ resourceId, font.m_AssetId });
				}
			}
		}

		return resourceIDMap;
	}

	void AssetManager::Clear()
	{
		s_Textures.clear();
		s_Fonts.clear();
	}
}