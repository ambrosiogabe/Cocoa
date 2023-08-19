#include "externalLibs.h"

#include "cocoa/renderer/Texture.h"
#include "cocoa/util/JsonExtended.h"
#include "cocoa/core/AssetManager.h"

#include <stb_image.h>

namespace Cocoa
{
	namespace TextureUtil
	{
		const Texture NULL_TEXTURE = create();

		uint32 toGl(WrapMode wrapMode)
		{
			switch (wrapMode)
			{
			case WrapMode::Repeat:
				return GL_REPEAT;
			case WrapMode::None:
				return GL_NONE;
			default:
				Logger::Warning("Unknown glWrapMode '%d'. Defaulting to GL_NONE.", wrapMode);
			}

			return GL_NONE;
		}

		uint32 toGl(FilterMode filterMode)
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
				Logger::Warning("Unknown glFilterMode '%d'. Defaulting to GL_NONE.", filterMode);
			}

			return GL_NONE;
		}

		uint32 toGl(ByteFormat format)
		{
			switch (format)
			{
			case ByteFormat::RGBA8:
				return GL_RGBA8;
			case ByteFormat::RGB8:
				return GL_RGB8;
			case ByteFormat::RGBA:
				return GL_RGBA;
			case ByteFormat::RGB:
				return GL_RGB;
			case ByteFormat::R32UI:
				return GL_R32UI;
			case ByteFormat::RED_INTEGER:
				return GL_RED_INTEGER;
			case ByteFormat::DEPTH24_STENCIL8:
				return GL_DEPTH24_STENCIL8;
			case ByteFormat::None:
				return GL_NONE;
			default:
				Logger::Warning("Unknown glByteFormat '%d'. Defaulting to GL_NONE.", format);
			}

			return GL_NONE;
		}

		uint32 toGlDataType(ByteFormat format)
		{
			switch (format)
			{
			case ByteFormat::RGBA8:
				return GL_FLOAT;
			case ByteFormat::RGB8:
				return GL_FLOAT;
			case ByteFormat::RGBA:
				return GL_FLOAT;
			case ByteFormat::RGB:
				return GL_FLOAT;
			case ByteFormat::R32UI:
				return GL_UNSIGNED_INT;
			case ByteFormat::RED_INTEGER:
				return GL_UNSIGNED_INT;
			case ByteFormat::None:
				return GL_NONE;
			default:
				Logger::Warning("Unknown glByteFormat '%d'. Defaulting to GL_NONE.", format);
			}

			return GL_NONE;
		}

		bool byteFormatIsInt(ByteFormat format)
		{
			switch (format)
			{
			case ByteFormat::RGBA8:
				return false;
			case ByteFormat::RGB8:
				return false;
			case ByteFormat::RGBA:
				return false;
			case ByteFormat::RGB:
				return false;
			case ByteFormat::R32UI:
				return true;
			case ByteFormat::RED_INTEGER:
				return true;
			case ByteFormat::None:
				return false;
			default:
				Logger::Warning("Unknown glByteFormat '%d'. Defaulting to non-int format.", format);
			}

			return false;
		}

		bool byteFormatIsRgb(ByteFormat format)
		{
			switch (format)
			{
			case ByteFormat::RGBA8:
				return false;
			case ByteFormat::RGB8:
				return true;
			case ByteFormat::RGBA:
				return false;
			case ByteFormat::RGB:
				return true;
			case ByteFormat::R32UI:
				return false;
			case ByteFormat::RED_INTEGER:
				return false;
			case ByteFormat::None:
				return false;
			default:
				Logger::Warning("Unknown glByteFormat '%d'. Defaulting to non-rgb format.", format);
			}

			return false;
		}

		static void BindTextureParameters(const Texture& texture)
		{
			if (texture.wrapS != WrapMode::None)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, toGl(texture.wrapS));
			}
			if (texture.wrapT != WrapMode::None)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, toGl(texture.wrapT));
			}
			if (texture.minFilter != FilterMode::None)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, toGl(texture.minFilter));
			}
			if (texture.magFilter != FilterMode::None)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, toGl(texture.magFilter));
			}
		}

		void generate(Texture& texture, const std::filesystem::path& path)
		{
			int channels;

			unsigned char* pixels = stbi_load(path.string().c_str(), &texture.width, &texture.height, &channels, 0);
			Logger::Assert((pixels != nullptr), "STB failed to load image: %s\n-> STB Failure Reason: %s", path.string().c_str(), stbi_failure_reason());

			int bytesPerPixel = channels;
			if (bytesPerPixel == 4)
			{
				texture.internalFormat = ByteFormat::RGBA8;
				texture.externalFormat = ByteFormat::RGBA;
			}
			else if (bytesPerPixel == 3)
			{
				texture.internalFormat = ByteFormat::RGB8;
				texture.externalFormat = ByteFormat::RGB;
			}
			else
			{
				Logger::Warning("Unknown number of channels '%d' in image '%s'.", path.string().c_str(), channels);
				return;
			}

			glGenTextures(1, &texture.graphicsId);
			glBindTexture(GL_TEXTURE_2D, texture.graphicsId);

			BindTextureParameters(texture);

			uint32 internalFormat = toGl(texture.internalFormat);
			uint32 externalFormat = toGl(texture.externalFormat);
			Logger::Assert(internalFormat != GL_NONE && externalFormat != GL_NONE, "Tried to load image from file, but failed to identify internal format for image '%s'", texture.path.string().c_str());
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texture.width, texture.height, 0, externalFormat, GL_UNSIGNED_BYTE, pixels);

			stbi_image_free(pixels);
		}

		void generate(Texture& texture)
		{
			Logger::Assert(texture.internalFormat != ByteFormat::None, "Cannot generate texture without internal format.");
			Logger::Assert(texture.externalFormat != ByteFormat::None, "Cannot generate texture without external format.");
			glGenTextures(1, &texture.graphicsId);
			glBindTexture(GL_TEXTURE_2D, texture.graphicsId);

			BindTextureParameters(texture);

			uint32 internalFormat = toGl(texture.internalFormat);
			uint32 externalFormat = toGl(texture.externalFormat);

			// Here the GL_UNSIGNED_BYTE does nothing since we are just allocating space
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texture.width, texture.height, 0, externalFormat, GL_UNSIGNED_BYTE, nullptr);
		}

		bool isNull(const Texture& texture)
		{
			return texture.graphicsId == NULL_TEXTURE.graphicsId;
		}

		void bind(const Texture& texture)
		{
			glBindTexture(GL_TEXTURE_2D, texture.graphicsId);
		}

		void unbind(const Texture& texture)
		{
			glBindTexture(GL_TEXTURE_2D, texture.graphicsId);
		}

		void destroy(Texture& texture)
		{
			glDeleteTextures(1, &texture.graphicsId);
			texture.graphicsId = -1;
		}

		json serialize(const Texture& texture)
		{
			return {
				{"Filepath", texture.path.string()},
				{"MagFilter", (int)texture.magFilter },
				{"MinFilter", (int)texture.minFilter },
				{"WrapS", (int)texture.wrapS},
				{"WrapT", (int)texture.wrapT},
				{"InternalFormat", (int)texture.internalFormat},
				{"ExternalFormat", (int)texture.externalFormat}
			};
		}

		Texture deserialize(const json& j)
		{
			Texture texture;
			JsonExtended::assignIfNotNull(j, "Filepath", texture.path);
			JsonExtended::assignEnumIfNotNull<FilterMode>(j, "MagFilter", texture.magFilter);
			JsonExtended::assignEnumIfNotNull<FilterMode>(j, "MinFilter", texture.minFilter);
			JsonExtended::assignEnumIfNotNull<WrapMode>(j, "WrapS", texture.wrapS);
			JsonExtended::assignEnumIfNotNull<WrapMode>(j, "WrapT", texture.wrapT);
			JsonExtended::assignEnumIfNotNull<ByteFormat>(j, "InternalFormat", texture.internalFormat);
			JsonExtended::assignEnumIfNotNull<ByteFormat>(j, "ExternalFormat", texture.externalFormat);
			return texture;
		}

		Texture create()
		{
			Texture res;
			res.graphicsId = UINT32_MAX;
			res.width = 0;
			res.height = 0;

			res.magFilter = FilterMode::None;
			res.minFilter = FilterMode::None;
			res.wrapS = WrapMode::None;
			res.wrapT = WrapMode::None;
			res.internalFormat = ByteFormat::None;
			res.externalFormat = ByteFormat::None;

			res.path = "";
			res.isDefault = false;

			return res;
		}
	}
}