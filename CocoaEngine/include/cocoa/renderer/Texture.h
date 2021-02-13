#pragma once
#include "cocoa/core/Core.h"
#include "cocoa/file/CPath.h"

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
		uint32 GraphicsId = (uint32)-1;
		int32 Width = 0;
		int32 Height = 0;

		// Texture attributes
		FilterMode MagFilter = FilterMode::None;
		FilterMode MinFilter = FilterMode::None;
		WrapMode WrapS = WrapMode::None;
		WrapMode WrapT = WrapMode::None;
		ByteFormat InternalFormat = ByteFormat::None;
		ByteFormat ExternalFormat = ByteFormat::None;

		CPath Path = CPath();
		bool IsDefault = false;
	};

	namespace TextureUtil
	{
		// Namespace variables
		// NOTE: To make sure this variable is visible to other translation units, declare it as extern
		COCOA extern const Texture NullTexture;

		// Namespace functions
		COCOA json Serialize(const Texture& texture);
		COCOA Texture Deserialize(const json& j);

		COCOA void Bind(const Texture& texture);
		COCOA void Unbind(const Texture& texture);
		COCOA void Delete(Texture& texture);

		// Loads a texture using stb library and generates a texutre using the filter/wrap modes and automatically detects
		// internal/external format, width, height, and alpha channel
		COCOA void Generate(Texture& texture, const CPath& filepath);

		// Allocates memory space on the GPU according to the texture specifications listed here
		COCOA void Generate(Texture& texture);

		COCOA bool IsNull(const Texture& texture);

		COCOA uint32 ToGl(ByteFormat format);
		COCOA uint32 ToGl(WrapMode wrapMode);
		COCOA uint32 ToGl(FilterMode filterMode);
	};
}