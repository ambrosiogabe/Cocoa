#pragma once
// TODO: Rename this file to font, restructure project to be *functional*
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "DataStructures.h"
#include "cocoa/file/CPath.h"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Cocoa
{
	namespace FontUtil
	{
		COCOA int GetPixel(int x, int y, uint8* bitmap, int width, int height);

		COCOA float FindNearestPixel(int pixX, int pixY, uint8* bitmap, int width, int height, int spread);

		COCOA SdfBitmapContainer GenerateSdfCodepointBitmap(int codepoint, FT_Face font, int fontSize, int padding = 5, int upscaleResolution = 4096, bool flipVertically = false);

		COCOA void CreateSdfFontTexture(const CPath& fontFile, int fontSize, CharInfo* characterMap, int characterMapSize, const CPath& outputFile, 
			int padding = 5, int upscaleResolution = 4096, int glyphOffset = 0);
	}
}