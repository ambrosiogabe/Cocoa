#include "cocoa/core/AssetManager.h"
#include "cocoa/util/Log.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/file/File.h"
#include "cocoa/util/JsonExtended.h"

namespace Cocoa
{
	std::vector<Texture> AssetManager::s_Textures = std::vector<Texture>();
	std::vector<Font> AssetManager::s_Fonts = std::vector<Font>();
	std::vector<Shader> AssetManager::s_Shaders = std::vector<Shader>();
	uint32 AssetManager::s_CurrentScene = 0;
	uint32 AssetManager::s_ResourceCount = 0;

	void AssetManager::Init(uint32 scene)
	{
		s_CurrentScene = scene;
		s_ResourceCount = 0;
		// TODO: Clear assets somehow?
	}

	const Shader& AssetManager::GetShader(uint32 resourceId)
	{
		if (resourceId < s_Shaders.size())
		{
			return s_Shaders[resourceId];
		}

		return NShader::CreateShader();
	}

	Handle<Shader> AssetManager::GetShader(const CPath& path)
	{
		int i = 0;
		for (auto& shader : s_Shaders)
		{
			if (shader.Filepath == path)
			{
				return Handle<Shader>(i);
			}
			i++;
		}

		return Handle<Shader>();
	}

	Handle<Shader> AssetManager::LoadShaderFromFile(const CPath& path, bool isDefault, int id)
	{
		Handle<Shader> shader = GetShader(path);
		if (!shader.IsNull())
		{
			Log::Warning("Tried to load asset that has already been loaded '%s'", path.Path.c_str());
			return shader;
		}

		CPath absPath = File::GetAbsolutePath(path);
		int index = id;

		// If id is -1, we don't care where you place the texture so long as it gets loaded
		if (index == -1)
		{
			index = s_Shaders.size();
			s_Shaders.emplace_back(NShader::CreateShader(absPath, isDefault));
		}
		// Otherwise, place the texture in the id location specified, and report error if a texture is already located there for some reason
		else
		{
			Log::Assert(index < s_Shaders.size(), "Id must be smaller then shader size.");
			Log::Assert(NShader::IsNull(s_Shaders[index]), "Texture slot must be free to place a texture at the specified id.");
			if (NShader::IsNull(s_Shaders[index]))
			{
				s_Shaders[index] = NShader::CreateShader(absPath, isDefault);
			}
			else
			{
				Log::Error("Could not place shader at requested id. The slot is already taken.");
			}
		}

		return Handle<Shader>(index);
	}

	const Texture& AssetManager::GetTexture(uint32 resourceId)
	{
		if (resourceId < s_Textures.size())
		{
			return s_Textures[resourceId];
		}

		return TextureUtil::NullTexture;
	}

	Handle<Texture> AssetManager::GetTexture(const CPath& path)
	{
		int i = 0;
		for (auto& tex : s_Textures)
		{
			if (tex.Path == path)
			{
				return Handle<Texture>(i);
			}
			i++;
		}

		return Handle<Texture>();
	}

	Handle<Texture> AssetManager::LoadTextureFromJson(const json& j, bool isDefault, int id)
	{
		Texture texture = TextureUtil::Deserialize(j);
		texture.IsDefault = isDefault;

		Handle<Texture> textureHandle = GetTexture(texture.Path);
		if (!textureHandle.IsNull())
		{
			Log::Warning("Tried to load asset that has already been loaded '%s'.", texture.Path.Path.c_str());
			return textureHandle;
		}

		CPath absPath = File::GetAbsolutePath(texture.Path);
		int index = id;

		// Make sure to generate texture *before* pushing back since we are pushing back a copy
		TextureUtil::Generate(texture, texture.Path);

		// If id is -1, we don't care where you place the font so long as it gets loaded
		if (index == -1)
		{
			index = s_Textures.size();
			s_Textures.push_back(texture);
		}
		// Otherwise, place the font in the id location specified, and report error if a font is already located there for some reason
		else
		{
			Log::Assert(index < s_Textures.size(), "Id must be smaller then texture size.");
			Log::Assert(TextureUtil::IsNull(s_Textures[index]), "Texture slot must be free to place a texture at the specified id.");
			if (TextureUtil::IsNull(s_Textures[index]))
			{
				s_Textures[index] = texture;
			}
			else
			{
				Log::Error("Could not place texture at requested id. The slot is already taken.");
			}
		}

		return Handle<Texture>(index);
	}

	Handle<Texture> AssetManager::LoadTextureFromFile(Texture& texture, const CPath& path, int id)
	{
		Handle<Texture> textureHandle = GetTexture(path);
		if (!textureHandle.IsNull())
		{
			Log::Warning("Tried to load asset that has already been loaded '%s'", path.Path.c_str());
			return textureHandle;
		}

		CPath absPath = File::GetAbsolutePath(path);
		int index = id;
		texture.Path = path;
		TextureUtil::Generate(texture, path);

		// If id is -1, we don't care where you place the texture so long as it gets loaded
		if (index == -1)
		{
			index = s_Textures.size();
			s_Textures.push_back(texture);
		}
		// Otherwise, place the texture in the id location specified, and report error if a texture is already located there for some reason
		else
		{
			Log::Assert(index < s_Textures.size(), "Id must be smaller then texture size.");
			Log::Assert(TextureUtil::IsNull(s_Textures[index]), "Texture slot must be free to place a texture at the specified id.");
			if (TextureUtil::IsNull(s_Textures[index]))
			{
				s_Textures[index] = texture;
			}
			else
			{
				Log::Error("Could not place texture at requested id. The slot is already taken.");
			}
		}

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
				return Handle<Font>(i);
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
			Log::Warning("Tried to load asset that has already been loaded '%s'.", path.Path.c_str());
			return font;
		}

		CPath absPath = File::GetAbsolutePath(path);
		int index = id;

		// If id is -1, we don't care where you place the font so long as it gets loaded
		if (index == -1)
		{
			index = s_Fonts.size();
			s_Fonts.emplace_back(Font{ absPath, isDefault });
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
		return Handle<Font>(index);
	}

	Handle<Font> AssetManager::LoadFontFromTtfFile(const CPath& fontFile, int fontSize, const CPath& outputFile, int glyphRangeStart, int glyphRangeEnd, int padding, int upscaleResolution)
	{
		Handle<Font> font = GetFont(fontFile);
		if (!font.IsNull())
		{
			Log::Warning("Tried to load asset that has already been loaded '%s'.", fontFile.Path.c_str());
			return font;
		}

		CPath absPath = File::GetAbsolutePath(fontFile);
		int index = s_Fonts.size();

		s_Fonts.push_back(Font{ absPath, false });
		Font& newFont = s_Fonts.at(index);
		newFont.GenerateSdf(fontFile, fontSize, outputFile, glyphRangeStart, glyphRangeEnd, padding, upscaleResolution);

		Texture fontTexSpec;
		fontTexSpec.IsDefault = false;
		fontTexSpec.MagFilter = FilterMode::Linear;
		fontTexSpec.MinFilter = FilterMode::Linear;
		fontTexSpec.WrapS = WrapMode::Repeat;
		fontTexSpec.WrapT = WrapMode::Repeat;
		newFont.m_FontTexture = AssetManager::LoadTextureFromFile(fontTexSpec, outputFile);

		return Handle<Font>(index);
	}

	json AssetManager::Serialize()
	{
		json res;

		res["SceneID"] = s_CurrentScene;

		int i = 0;
		for (auto& assetIt : s_Textures)
		{
			if (!assetIt.IsDefault)
			{
				json assetSerialized = TextureUtil::Serialize(assetIt);
				assetSerialized["ResourceId"] = i;
				res["Textures"][i] = assetSerialized;
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
				res["Fonts"][i] = assetSerialized;
			}
			i++;
		}

		return res;
	}

	void AssetManager::LoadTexturesFrom(const json& j)
	{
		uint32 scene = -1;
		JsonExtended::AssignIfNotNull(j, "SceneID", scene);

		if (scene >= 0 && j.contains("Textures"))
		{
			// Note: We shouldn't have to worry about adding in the size of the default textures because they get 'serialized' as null
			// Note: Should we keep it this way though?
			s_Textures.resize(j["Textures"].size());
			for (auto it = j["Textures"].begin(); it != j["Textures"].end(); ++it)
			{
				const json& assetJson = it.value();
				if (assetJson.is_null()) continue;

				uint32 resourceId = -1;
				JsonExtended::AssignIfNotNull(assetJson, "ResourceId", resourceId);

				if (resourceId >= 0)
				{
					LoadTextureFromJson(assetJson, false, resourceId);
				}
			}
		}
	}

	void AssetManager::LoadFontsFrom(const json& j)
	{
		uint32 scene = -1;
		JsonExtended::AssignIfNotNull(j, "SceneID", scene);

		if (scene >= 0 && j.contains("Fonts"))
		{
			s_Fonts.resize(s_Fonts.size() + j["Fonts"].size());
			for (auto it = j["Fonts"].begin(); it != j["Fonts"].end(); ++it)
			{
				const json& assetJson = it.value();
				if (assetJson.is_null()) continue;

				uint32 resourceId = -1;
				JsonExtended::AssignIfNotNull(assetJson, "ResourceId", resourceId);

				CPath path = NCPath::CreatePath();
				if (assetJson.contains("Filepath"))
				{
					path = NCPath::CreatePath(assetJson["Filepath"], false);
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
		// Delete all textures on GPU before clear
		for (auto& tex : s_Textures)
		{
			TextureUtil::Delete(tex);
		}
		s_Textures.clear();

		// Free all fonts before destroying them
		for (auto& font : s_Fonts)
		{
			font.Free();
		}
		s_Fonts.clear();

		// Delete all shaders on clear
		for (auto& shader : s_Shaders)
		{
			NShader::Delete(shader);
		}
		NShader::ClearAllShaderVariables();
		s_Shaders.clear();
	}
}