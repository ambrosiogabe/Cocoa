#ifndef COCOA_ENGINE_TEXTURE_H
#define COCOA_ENGINE_TEXTURE_H
#include "cocoa/core/Core.h"
#include "cocoa/file/Path.h"

namespace Cocoa
{
	enum class FilterMode
	{
		None=0,
		Linear,
		Nearest
	};

	enum class WrapMode
	{
		None=0,
		Repeat
	};

	enum class ByteFormat
	{
		None=0,
		RGBA,
		RGBA8,

		RGB,
		RGB8,

		R32UI,
		RED_INTEGER,

		// Depth/Stencil formats
		DEPTH24_STENCIL8
	};

	struct COCOA Texture
	{
		uint32 graphicsId;
		int32 width;
		int32 height;

		// Texture attributes
		FilterMode magFilter;
		FilterMode minFilter;
		WrapMode wrapS;
		WrapMode wrapT;
		ByteFormat internalFormat;
		ByteFormat externalFormat;

		Path path;
		bool isDefault;
	};

	namespace TextureUtil
	{
		// Namespace variables
		// NOTE: To make sure this variable is visible to other translation units, declare it as extern
		COCOA extern const Texture NULL_TEXTURE;

		// Namespace functions
		COCOA void bind(const Texture& texture);
		COCOA void unbind(const Texture& texture);
		COCOA void destroy(Texture& texture);

		// Loads a texture using stb library and generates a texutre using the filter/wrap modes and automatically detects
		// internal/external format, width, height, and alpha channel
		COCOA void generate(Texture& texture, const Path& filepath);

		// Allocates memory space on the GPU according to the texture specifications listed here
		COCOA void generate(Texture& texture);

		COCOA bool isNull(const Texture& texture);

		COCOA uint32 toGl(ByteFormat format);
		COCOA uint32 toGl(WrapMode wrapMode);
		COCOA uint32 toGl(FilterMode filterMode);
		COCOA uint32 toGlDataType(ByteFormat format);
		COCOA bool byteFormatIsInt(ByteFormat format);
		COCOA bool byteFormatIsRgb(ByteFormat format);

		COCOA json serialize(const Texture& texture);
		COCOA Texture deserialize(const json& j);

		COCOA Texture create();
	};
}

#endif
