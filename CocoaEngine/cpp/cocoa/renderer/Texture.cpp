#include "externalLibs.h"

#include "cocoa/renderer/Texture.h"
#include "cocoa/util/Log.h"
#include "cocoa/util/JsonExtended.h"
#include "cocoa/core/AssetManager.h"

#include <stb_image.h>

namespace Cocoa
{
	namespace TextureUtil
	{
		const Texture NullTexture = {};

		static uint32 ToGl(WrapMode wrapMode)
		{
			switch (wrapMode)
			{
			case WrapMode::Repeat:
				return GL_REPEAT;
			case WrapMode::None:
				return GL_NONE;
			default:
				Log::Warning("Unknown glWrapMode '%d'", wrapMode);
			}

			return GL_NONE;
		}

		static uint32 ToGl(FilterMode filterMode)
		{
			switch (filterMode)
			{
			case FilterMode::Linear:
				return GL_LINEAR;
			case FilterMode::Nearest:
				return GL_NEAREST;
			case FilterMode::None:
				return GL_NONE;
			default:
				Log::Warning("Unknown glFilterMode '%d'", filterMode);
			}

			return GL_NONE;
		}

		static uint32 ToGlInternal(ByteFormat format)
		{
			switch (format)
			{
			case ByteFormat::RGBA8:
				return GL_RGBA8;
			case ByteFormat::RGB8:
				return GL_RGB8;
			case ByteFormat::RGB:
				return GL_RGB;
			case ByteFormat::None:
				return GL_NONE;
			default:
				Log::Warning("Unknown glInternalFormat '%d'", format);
			}

			return GL_NONE;
		}

		static uint32 ToGlExternal(ByteFormat format)
		{
			switch (format)
			{
			case ByteFormat::RGBA8:
				return GL_RGBA;
			case ByteFormat::RGB8:
				return GL_RGB;
			case ByteFormat::RGB:
				return GL_RGB;
			case ByteFormat::None:
				return GL_NONE;
			default:
				Log::Warning("Unknown glExternalFormat '%d'", format);
			}

			return GL_NONE;
		}

		static void BindTextureParameters(const Texture& texture)
		{
			if (texture.WrapS != WrapMode::None)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, ToGl(texture.WrapS));
			}
			if (texture.WrapT != WrapMode::None)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, ToGl(texture.WrapT));
			}
			if (texture.MinFilter != FilterMode::None)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, ToGl(texture.MinFilter));
			}
			if (texture.MagFilter != FilterMode::None)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, ToGl(texture.MagFilter));
			}
		}

		void Generate(Texture& texture, const CPath& path)
		{
			int channels;

			unsigned char* pixels = stbi_load(path.Filepath(), &texture.Width, &texture.Height, &channels, 0);
			Log::Assert((pixels != nullptr), "STB failed to load image: %s\n-> STB Failure Reason: %s", path.Filepath(), stbi_failure_reason());

			int bytesPerPixel = channels;
			if (bytesPerPixel == 4)
			{
				texture.InternalFormat = ByteFormat::RGBA8;
			}
			else if (bytesPerPixel == 3)
			{
				texture.InternalFormat = ByteFormat::RGB8;
			}
			else
			{
				Log::Warning("Unknown number of channels '%d' in image '%s'.", path.Filepath(), channels);
				return;
			}

			glGenTextures(1, &texture.GraphicsId);
			glBindTexture(GL_TEXTURE_2D, texture.GraphicsId);

			BindTextureParameters(texture);

			uint32 internalFormat = ToGlInternal(texture.InternalFormat);
			uint32 externalFormat = ToGlExternal(texture.InternalFormat);
			Log::Assert(internalFormat != GL_NONE && externalFormat != GL_NONE, "Tried to load image from file, but failed to identify internal format for image '%s'", texture.Path.Filepath());
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texture.Width, texture.Height, 0, externalFormat, GL_UNSIGNED_BYTE, pixels);

			stbi_image_free(pixels);
		}

		void Generate(Texture& texture)
		{
			Log::Assert(texture.InternalFormat != ByteFormat::None, "Cannot generate texture without internal format.");
			glGenTextures(1, &texture.GraphicsId);
			glBindTexture(GL_TEXTURE_2D, texture.GraphicsId);

			BindTextureParameters(texture);

			uint32 internalFormat = ToGlInternal(texture.InternalFormat);
			uint32 externalFormat = ToGlExternal(texture.InternalFormat);

			// Here the GL_UNSIGNED_BYTE does nothing since we are just allocating space
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texture.Width, texture.Height, 0, externalFormat, GL_UNSIGNED_BYTE, nullptr);
		}

		bool IsNull(const Texture& texture)
		{
			return texture.GraphicsId == NullTexture.GraphicsId;
		}

		void Bind(const Texture& texture)
		{
			glBindTexture(GL_TEXTURE_2D, texture.GraphicsId);
		}

		void Unbind(const Texture& texture)
		{
			glBindTexture(GL_TEXTURE_2D, texture.GraphicsId);
		}

		void Delete(Texture& texture)
		{
			glDeleteTextures(1, &texture.GraphicsId);
			texture.GraphicsId = -1;
		}

		json Serialize(const Texture& texture)
		{
			return {
				{"Filepath", texture.Path.Filepath() },
				{"MagFilter", (int)texture.MagFilter },
				{"MinFilter", (int)texture.MinFilter },
				{"WrapS", (int)texture.WrapS},
				{"WrapT", (int)texture.WrapT}
			};
		}

		Texture Deserialize(const json& j)
		{
			Texture texture;
			JsonExtended::AssignIfNotNull(j, "Filepath", texture.Path);
			JsonExtended::AssignEnumIfNotNull<FilterMode>(j, "MagFilter", texture.MagFilter);
			JsonExtended::AssignEnumIfNotNull<FilterMode>(j, "MinFilter", texture.MinFilter);
			JsonExtended::AssignEnumIfNotNull<WrapMode>(j, "WrapS", texture.WrapS);
			JsonExtended::AssignEnumIfNotNull<WrapMode>(j, "WrapT", texture.WrapT);
			return texture;
		}
	}
}