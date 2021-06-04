#include "cocoa/core/AssetManager.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/file/File.h"
#include "cocoa/util/JsonExtended.h"

namespace Cocoa
{
	namespace AssetManager
	{
		static List<Texture> mTextures = List<Texture>();
		static List<Font> mFonts = List<Font>();
		static List<Shader> mShaders = List<Shader>();
		static uint32 mCurrentScene = 0;
		static uint32 mResourceCount = 0;

		const List<Texture>& getAllTextures()
		{
			return mTextures;
		}

		const List<Font>& getAllFonts()
		{
			return mFonts;
		}

		void init(uint32 scene)
		{
			mCurrentScene = scene;
			mResourceCount = 0;
			// TODO: Clear assets somehow?
		}

		const Shader& getShader(uint32 resourceId)
		{
			if (resourceId < (uint32)mShaders.size())
			{
				return mShaders[resourceId];
			}

			return NShader::CreateShader();
		}

		Handle<Shader> getShader(const CPath& path)
		{
			int i = 0;
			for (auto& shader : mShaders)
			{
				if (shader.Filepath == path)
				{
					return NHandle::createHandle<Shader>(i);
				}
				i++;
			}

			return NHandle::createHandle<Shader>();
		}

		Handle<Shader> loadShaderFromFile(const CPath& path, bool isDefault, int id)
		{
			Handle<Shader> shader = getShader(path);
			if (!shader.isNull())
			{
				Logger::Warning("Tried to load asset that has already been loaded '%s'", path.Path);
				return shader;
			}

			CPath absPath = File::GetAbsolutePath(path);
			int index = id;

			// If id is -1, we don't care where you place the texture so long as it gets loaded
			if (index == -1)
			{
				index = mShaders.size();
				mShaders.push(NShader::CreateShader(absPath, isDefault));
			}
			// Otherwise, place the texture in the id location specified, and report error if a texture is already located there for some reason
			else
			{
				Logger::Assert(index < mShaders.size(), "Id must be smaller then shader size.");
				Logger::Assert(NShader::IsNull(mShaders[index]), "Texture slot must be free to place a texture at the specified id.");
				if (NShader::IsNull(mShaders[index]))
				{
					mShaders[index] = NShader::CreateShader(absPath, isDefault);
				}
				else
				{
					Logger::Error("Could not place shader at requested id. The slot is already taken.");
				}
			}

			return NHandle::createHandle<Shader>(index);
		}

		const Texture& getTexture(uint32 resourceId)
		{
			if (resourceId < mTextures.size())
			{
				return mTextures[resourceId];
			}

			return TextureUtil::NullTexture;
		}

		Handle<Texture> getTexture(const CPath& path)
		{
			int i = 0;
			for (auto& tex : mTextures)
			{
				if (tex.Path == path)
				{
					return NHandle::createHandle<Texture>(i);
				}
				i++;
			}

			return NHandle::createHandle<Texture>();
		}

		Handle<Texture> loadTextureFromJson(const json& j, bool isDefault, int id)
		{
			Texture texture = TextureUtil::Deserialize(j);
			texture.IsDefault = isDefault;

			Handle<Texture> textureHandle = getTexture(texture.Path);
			if (!textureHandle.isNull())
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
				index = mTextures.size();
				mTextures.push(texture);
			}
			// Otherwise, place the font in the id location specified, and report error if a font is already located there for some reason
			else
			{
				Logger::Assert(index < mTextures.size(), "Id must be smaller then texture size.");
				Logger::Assert(TextureUtil::IsNull(mTextures[index]), "Texture slot must be free to place a texture at the specified id.");
				if (TextureUtil::IsNull(mTextures[index]))
				{
					mTextures[index] = texture;
				}
				else
				{
					Logger::Error("Could not place texture at requested id. The slot is already taken.");
				}
			}

			return NHandle::createHandle<Texture>(index);
		}

		Handle<Texture> loadTextureFromFile(Texture& texture, const CPath& path, int id)
		{
			Handle<Texture> textureHandle = getTexture(path);
			if (!textureHandle.isNull())
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
				index = mTextures.size();
				mTextures.push(texture);
			}
			// Otherwise, place the texture in the id location specified, and report error if a texture is already located there for some reason
			else
			{
				Logger::Assert(index < mTextures.size(), "Id must be smaller then texture size.");
				Logger::Assert(TextureUtil::IsNull(mTextures[index]), "Texture slot must be free to place a texture at the specified id.");
				if (TextureUtil::IsNull(mTextures[index]))
				{
					mTextures[index] = texture;
				}
				else
				{
					Logger::Error("Could not place texture at requested id. The slot is already taken.");
				}
			}

			return NHandle::createHandle<Texture>(index);
		}

		const Font& getFont(uint32 resourceId)
		{
			if (resourceId < mFonts.size())
			{
				return mFonts[resourceId];
			}

			return Font::NullFont();
		}

		Handle<Font> getFont(const CPath& path)
		{
			int i = 0;
			for (auto& font : mFonts)
			{
				if (font.m_Path == path)
				{
					return NHandle::createHandle<Font>(i);
				}
				i++;
			}

			return NHandle::createHandle<Font>();
		}

		Handle<Font> loadFontFromJson(const CPath& path, const json& j, bool isDefault, int id)
		{
			Handle<Font> font = getFont(path);
			if (!font.isNull())
			{
				Logger::Warning("Tried to load asset that has already been loaded '%s'.", path.Path);
				return font;
			}

			CPath absPath = File::GetAbsolutePath(path);
			int index = id;

			// If id is -1, we don't care where you place the font so long as it gets loaded
			if (index == -1)
			{
				index = mFonts.size();
				mFonts.push(Font{ absPath, isDefault });
			}
			// Otherwise, place the font in the id location specified, and report error if a font is already located there for some reason
			else
			{
				Logger::Assert(index < mFonts.size(), "Id must be smaller then texture size.");
				Logger::Assert(mFonts[index].IsNull(), "Texture slot must be free to place a texture at the specified id.");
				if (mFonts[index].IsNull())
				{
					mFonts[index] = Font{ absPath, isDefault };
				}
				else
				{
					Logger::Error("Could not place font at requested id. The slot is already taken.");
				}
			}

			Font& newFont = mFonts[index];
			newFont.Deserialize(j);
			return NHandle::createHandle<Font>(index);
		}

		Handle<Font> loadFontFromTtfFile(const CPath& fontFile, int fontSize, const CPath& outputFile, int glyphRangeStart, int glyphRangeEnd, int padding, int upscaleResolution)
		{
			Handle<Font> font = getFont(fontFile);
			if (!font.isNull())
			{
				Logger::Warning("Tried to load asset that has already been loaded '%s'.", fontFile.Path);
				return font;
			}

			CPath absPath = File::GetAbsolutePath(fontFile);
			int index = mFonts.size();

			mFonts.push(Font{ absPath, false });
			Font& newFont = mFonts[index];
			newFont.GenerateSdf(fontFile, fontSize, outputFile, glyphRangeStart, glyphRangeEnd, padding, upscaleResolution);

			Texture fontTexSpec;
			fontTexSpec.IsDefault = false;
			fontTexSpec.MagFilter = FilterMode::Linear;
			fontTexSpec.MinFilter = FilterMode::Linear;
			fontTexSpec.WrapS = WrapMode::Repeat;
			fontTexSpec.WrapT = WrapMode::Repeat;
			newFont.m_FontTexture = AssetManager::loadTextureFromFile(fontTexSpec, outputFile);

			return NHandle::createHandle<Font>(index);
		}

		json serialize()
		{
			json res;

			res["SceneID"] = mCurrentScene;

			int i = 0;
			for (auto& assetIt : mTextures)
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
			for (auto& assetIt : mFonts)
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

		void loadTexturesFrom(const json& j)
		{
			uint32 scene = -1;
			JsonExtended::AssignIfNotNull(j, "SceneID", scene);

			if (scene >= 0 && j.contains("Textures"))
			{
				// Note: We shouldn't have to worry about adding in the size of the default textures because they get 'serialized' as null
				// Note: Should we keep it this way though?
				mTextures.resize(j["Textures"].size());
				for (auto it = j["Textures"].begin(); it != j["Textures"].end(); ++it)
				{
					const json& assetJson = it.value();
					if (assetJson.is_null()) continue;

					uint32 resourceId = -1;
					JsonExtended::AssignIfNotNull(assetJson, "ResourceId", resourceId);

					if (resourceId >= 0)
					{
						loadTextureFromJson(assetJson, false, resourceId);
					}
				}
			}
		}

		void loadFontsFrom(const json& j)
		{
			uint32 scene = -1;
			JsonExtended::AssignIfNotNull(j, "SceneID", scene);

			if (scene >= 0 && j.contains("Fonts"))
			{
				mFonts.resize(mFonts.size() + j["Fonts"].size());
				for (auto it = j["Fonts"].begin(); it != j["Fonts"].end(); ++it)
				{
					const json& assetJson = it.value();
					if (assetJson.is_null()) continue;

					uint32 resourceId = -1;
					JsonExtended::AssignIfNotNull(assetJson, "ResourceId", resourceId);

					CPath path = CPath::Create();
					if (assetJson.contains("Filepath"))
					{
						path = CPath::Create(assetJson["Filepath"], false);
					}

					if (resourceId >= 0)
					{
						loadFontFromJson(path, assetJson, false, resourceId);
					}
				}
			}
		}

		void clear()
		{
			// Delete all textures on GPU before clear
			for (auto& tex : mTextures)
			{
				TextureUtil::Delete(tex);
			}
			mTextures.clear();

			// Free all fonts before destroying them
			for (auto& font : mFonts)
			{
				font.Free();
			}
			mFonts.clear();

			// Delete all shaders on clear
			for (auto& shader : mShaders)
			{
				NShader::Delete(shader);
			}
			NShader::ClearAllShaderVariables();
			mShaders.clear();
		}
	}
}