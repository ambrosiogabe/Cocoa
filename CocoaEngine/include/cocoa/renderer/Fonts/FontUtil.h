#ifndef COCOA_ENGINE_FONT_UTIL_H
#define COCOA_ENGINE_FONT_UTIL_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "DataStructures.h"

#include <filesystem>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Cocoa
{
	namespace FontUtil
	{
		COCOA int getPixel(int x, int y, uint8* bitmap, int width, int height);

		COCOA float findNearestPixel(int pixX, int pixY, uint8* bitmap, int width, int height, int spread);

		COCOA SdfBitmapContainer generateSdfCodepointBitmap(int codepoint, FT_Face font, int fontSize, int padding = 5, int upscaleResolution = 4096, bool flipVertically = false);

		COCOA void createSdfFontTexture(const std::filesystem::path& fontFile, int fontSize, CharInfo* characterMap, int characterMapSize, const std::filesystem::path& outputFile,
			int padding = 5, int upscaleResolution = 4096, int glyphOffset = 0);
	}
}

#endif
