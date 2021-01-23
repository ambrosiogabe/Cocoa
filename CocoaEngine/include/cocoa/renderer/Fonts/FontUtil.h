#pragma once
// TODO: Rename this file to font, restructure project to be *functional*
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "DataStructures.h"
#include "cocoa/file/CPath.h"

// TODO: Fix include of freetype here. Must be a problem in premake setup
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Cocoa
{
	namespace FontUtil
	{
		int GetPixel(int x, int y, uint8* bitmap, int width, int height);

		int FindNearestPixel(int pixX, int pixY, uint8* bitmap, int width, int height, int spread);

		SdfBitmapContainer GenerateSdfCodepointBitmap(int codepoint, FT_Face font, int fontSize, int padding = 5, int spread = 128, int upscaleResolution = 4096, bool flipVertically = false);

		void CreateSdfFontTexture(const CPath& filepath);
	}
}