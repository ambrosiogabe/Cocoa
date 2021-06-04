#include "cocoa/core/AssetManager.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/file/File.h"
#include "cocoa/util/JsonExtended.h"

namespace Cocoa
{
	namespace AssetManager
	{
		List<Texture> m_Textures = List<Texture>();
		List<Font> m_Fonts = List<Font>();
		List<Shader> m_Shaders = List<Shader>();
		uint32 m_CurrentScene = 0;
		uint32 m_ResourceCount = 0;

		const List<Texture>& GetAllTextures()
		{
			return m_Textures;
		}

		const List<Font>& GetAllFonts()
		{
			return m_Fonts;
		}

		void AssetManager::Init(uint32 scene)
		{
			m_CurrentScene = scene;
			m_ResourceCount = 0;
			// TODO: Clear assets somehow?
		}

		const Shader& AssetManager::GetShader(uint32 resourceId)
		{
			if (resourceId < m_Shaders.size())
			{
				return m_Shaders[resourceId];
			}

			return NShader::CreateShader();
		}

		Handle<Shader> AssetManager::GetShader(const CPath& path)
		{
			int i = 0;
			for (auto& shader : m_Shaders)
			{
				if (shader.Filepath == path)
				{
					return NHandle::CreateHandle<Shader>(i);
				}
				i++;
			}

			return NHandle::CreateHandle<Shader>();
		}

		Handle<Shader> AssetManager::LoadShaderFromFile(const CPath& path, bool isDefault, int id)
		{
			Handle<Shader> shader = GetShader(path);
			if (!shader.IsNull())
			{
				Logger::Warning("Tried to load asset that has already been loaded '%s'", path.Path);
				return shader;
			}

			CPath absPath = File::GetAbsolutePath(path);
			int index = id;

			// If id is -1, we don't care where you place the texture so long as it gets loaded
			if (index == -1)
			{
				index = m_Shaders.size();
				m_Shaders.push(NShader::CreateShader(absPath, isDefault));
			}
			// Otherwise, place the texture in the id location specified, and report error if a texture is already located there for some reason
			else
			{
				Logger::Assert(index < m_Shaders.size(), "Id must be smaller then shader size.");
				Logger::Assert(NShader::IsNull(m_Shaders[index]), "Texture slot must be free to place a texture at the specified id.");
				if (NShader::IsNull(m_Shaders[index]))
				{
					m_Shaders[index] = NShader::CreateShader(absPath, isDefault);
				}
				else
				{
					Logger::Error("Could not place shader at requested id. The slot is already taken.");
				}
			}

			return NHandle::CreateHandle<Shader>(index);
		}

		const Texture& AssetManager::GetTexture(uint32 resourceId)
		{
			if (resourceId < m_Textures.size())
			{
				return m_Textures[resourceId];
			}

			return TextureUtil::NullTexture;
		}

		Handle<Texture> AssetManager::GetTexture(const CPath& path)
		{
			int i = 0;
			for (auto& tex : m_Textures)
			{
				if (tex.Path == path)
				{
					return NHandle::CreateHandle<Texture>(i);
				}
				i++;
			}

			return NHandle::CreateHandle<Texture>();
		}

		Handle<Texture> AssetManager::LoadTextureFromJson(const json& j, bool isDefault, int id)
		{
			Texture texture = TextureUtil::Deserialize(j);
			texture.IsDefault = isDefault;

			Handle<Texture> textureHandle = GetTexture(texture.Path);
			if (!textureHandle.IsNull())
			{
				Logger::Warning("Tried to load asset that has already been loaded '%s'.", texture.Path.Path);
				return textureHandle;
			}

			CPath absPath = File::GetAbsolutePath(texture.Path);
			int index = id;

			// Make sure to generate texture *before* pushing back since we are pushing back a copy
			TextureUtil::Generate(texture, texture.Path);

			// If id is -1, we don't care where you place the font so long as it gets loaded
			if (index == -1)
			{
				index = m_Textures.size();
				m_Textures.push(texture);
			}
			// Otherwise, place the font in the id location specified, and report error if a font is already located there for some reason
			else
			{
				Logger::Assert(index < m_Textures.size(), "Id must be smaller then texture size.");
				Logger::Assert(TextureUtil::IsNull(m_Textures[index]), "Texture slot must be free to place a texture at the specified id.");
				if (TextureUtil::IsNull(m_Textures[index]))
				{
					m_Textures[index] = texture;
				}
				else
				{
					Logger::Error("Could not place texture at requested id. The slot is already taken.");
				}
			}

			return NHandle::CreateHandle<Texture>(index);
		}

		Handle<Texture> AssetManager::LoadTextureFromFile(Texture& texture, const CPath& path, int id)
		{
			Handle<Texture> textureHandle = GetTexture(path);
			if (!textureHandle.IsNull())
			{
				Logger::Warning("Tried to load asset that has already been loaded '%s'", path.Path);
				return textureHandle;
			}

			CPath absPath = File::GetAbsolutePath(path);
			int index = id;
			texture.Path = path;
			TextureUtil::Generate(texture, path);

			// If id is -1, we don't care where you place the texture so long as it gets loaded
			if (index == -1)
			{
				index = m_Textures.size();
				m_Textures.push(texture);
			}
			// Otherwise, place the texture in the id location specified, and report error if a texture is already located there for some reason
			else
			{
				Logger::Assert(index < m_Textures.size(), "Id must be smaller then texture size.");
				Logger::Assert(TextureUtil::IsNull(m_Textures[index]), "Texture slot must be free to place a texture at the specified id.");
				if (TextureUtil::IsNull(m_Textures[index]))
				{
					m_Textures[index] = texture;
				}
				else
				{
					Logger::Error("Could not place texture at requested id. The slot is already taken.");
				}
			}

			return NHandle::CreateHandle<Texture>(index);
		}

		const Font& AssetManager::GetFont(uint32 resourceId)
		{
			if (resourceId < m_Fonts.size())
			{
				return m_Fonts[resourceId];
			}

			return Font::NullFont();
		}

		Handle<Font> AssetManager::GetFont(const CPath& path)
		{
			int i = 0;
			for (auto& font : m_Fonts)
			{
				if (font.m_Path == path)
				{
					return NHandle::CreateHandle<Font>(i);
				}
				i++;
			}

			return NHandle::CreateHandle<Font>();
		}

		Handle<Font> AssetManager::LoadFontFromJson(const CPath& path, const json& j, bool isDefault, int id)
		{
			Handle<Font> font = GetFont(path);
			if (!font.IsNull())
			{
				Logger::Warning("Tried to load asset that has already been loaded '%s'.", path.Path);
				return font;
			}

			CPath absPath = File::GetAbsolutePath(path);
			int index = id;

			// If id is -1, we don't care where you place the font so long as it gets loaded
			if (index == -1)
			{
				index = m_Fonts.size();
				m_Fonts.push(Font{ absPath, isDefault });
			}
			// Otherwise, place the font in the id location specified, and report error if a font is already located there for some reason
			else
			{
				Logger::Assert(index < m_Fonts.size(), "Id must be smaller then texture size.");
				Logger::Assert(m_Fonts[index].IsNull(), "Texture slot must be free to place a texture at the specified id.");
				if (m_Fonts[index].IsNull())
				{
					m_Fonts[index] = Font{ absPath, isDefault };
				}
				else
				{
					Logger::Error("Could not place font at requested id. The slot is already taken.");
				}
			}

			Font& newFont = m_Fonts[index];
			newFont.Deserialize(j);
			return NHandle::CreateHandle<Font>(index);
		}

		Handle<Font> AssetManager::LoadFontFromTtfFile(const CPath& fontFile, int fontSize, const CPath& outputFile, int glyphRangeStart, int glyphRangeEnd, int padding, int upscaleResolution)
		{
			Handle<Font> font = GetFont(fontFile);
			if (!font.IsNull())
			{
				Logger::Warning("Tried to load asset that has already been loaded '%s'.", fontFile.Path);
				return font;
			}

			CPath absPath = File::GetAbsolutePath(fontFile);
			int index = m_Fonts.size();

			m_Fonts.push(Font{ absPath, false });
			Font& newFont = m_Fonts[index];
			newFont.GenerateSdf(fontFile, fontSize, outputFile, glyphRangeStart, glyphRangeEnd, padding, upscaleResolution);

			Texture fontTexSpec;
			fontTexSpec.IsDefault = false;
			fontTexSpec.MagFilter = FilterMode::Linear;
			fontTexSpec.MinFilter = FilterMode::Linear;
			fontTexSpec.WrapS = WrapMode::Repeat;
			fontTexSpec.WrapT = WrapMode::Repeat;
			newFont.m_FontTexture = AssetManager::LoadTextureFromFile(fontTexSpec, outputFile);

			return NHandle::CreateHandle<Font>(index);
		}

		json AssetManager::Serialize()
		{
			json res;

			res["SceneID"] = m_CurrentScene;

			int i = 0;
			for (auto& assetIt : m_Textures)
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
			for (auto& assetIt : m_Fonts)
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
				m_Textures.resize(j["Textures"].size());
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
				m_Fonts.resize(m_Fonts.size() + j["Fonts"].size());
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
			for (auto& tex : m_Textures)
			{
				TextureUtil::Delete(tex);
			}
			m_Textures.clear();

			// Free all fonts before destroying them
			for (auto& font : m_Fonts)
			{
				font.Free();
			}
			m_Fonts.clear();

			// Delete all shaders on clear
			for (auto& shader : m_Shaders)
			{
				NShader::Delete(shader);
			}
			NShader::ClearAllShaderVariables();
			m_Shaders.clear();
		}
	}
}