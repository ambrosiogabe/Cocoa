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

	Handle<Texture> AssetManager::LoadTextureFromFile(const CPath& path, bool isDefault, int id)
	{
		Handle<Texture> texture = GetTexture(path);
		if (!texture.IsNull())
		{
			Log::Warning("Tried to load asset that has already been loaded '%s'", path.Filepath());
			return texture;
		}

		CPath absPath = IFile::GetAbsolutePath(path);
		int index = id;

		// If id is -1, we don't care where you place the texture so long as it gets loaded
		if (index == -1)
		{
			index = s_Textures.size();
			s_Textures.push_back({ absPath.Filepath(), isDefault });
		}
		// Otherwise, place the texture in the id location specified, and report error if a texture is already located there for some reason
		else
		{
			Log::Assert(index < s_Textures.size(), "Id must be smaller then texture size.");
			Log::Assert(s_Textures[index].IsNull(), "Texture slot must be free to place a texture at the specified id.");
			if (s_Textures[index].IsNull())
			{
				s_Textures[index] = { absPath.Filepath(), isDefault };
			}
			else
			{
				Log::Error("Could not place texture at requested id. The slot is already taken.");
			}
		}

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

	Handle<Font> AssetManager::LoadFontFromJson(const CPath& path, const json& j, bool isDefault, int id)
	{
		Handle<Font> font = GetFont(path);
		if (!font.IsNull())
		{
			Log::Warning("Tried to load asset that has already been loaded '%s'.", path.Filepath());
			return font;
		}

		CPath absPath = IFile::GetAbsolutePath(path);
		int index = id;

		// If id is -1, we don't care where you place the font so long as it gets loaded
		if (index == -1)
		{
			index = s_Fonts.size();
			s_Fonts.push_back(Font{ absPath, isDefault });
		}
		// Otherwise, place the font in the id location specified, and report error if a font is already located there for some reason
		else
		{
			Log::Assert(index < s_Fonts.size(), "Id must be smaller then texture size.");
			Log::Assert(s_Fonts[index].IsNull(), "Texture slot must be free to place a texture at the specified id.");
			if (s_Fonts[index].IsNull())
			{
				s_Fonts[index] = Font{ absPath, isDefault };
			}
			else
			{
				Log::Error("Could not place font at requested id. The slot is already taken.");
			}
		}

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

	void AssetManager::LoadTexturesFrom(const json& j)
	{
		uint32 scene = -1;
		JsonExtended::AssignIfNotNull(j["SceneID"], scene);

		// TODO: Switch all json accessors to use .contains(key) instead of [key]
		if (scene >= 0 && j.contains("Textures"))
		{
			s_Textures.resize(s_Textures.size() + j["Textures"].size());
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
					LoadTextureFromFile(path, false, resourceId);
				}
			}
		}
	}

	void AssetManager::LoadFontsFrom(const json& j)
	{
		uint32 scene = -1;
		JsonExtended::AssignIfNotNull(j["SceneID"], scene);

		if (scene >= 0 && j.contains("Fonts"))
		{
			s_Fonts.resize(s_Fonts.size() + j["Fonts"].size());
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
					LoadFontFromJson(path, assetJson, false, resourceId);
				}
			}
		}
	}

	void AssetManager::Clear()
	{
		s_Textures.clear();
		s_Fonts.clear();
	}
}