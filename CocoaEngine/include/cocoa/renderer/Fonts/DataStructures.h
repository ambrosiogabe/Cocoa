#ifndef COCOA_ENGINE_FONTS_DATA_STRUCTURES_H
#define COCOA_ENGINE_FONTS_DATA_STRUCTURES_H

namespace Cocoa
{
	struct CharInfo
	{
		float ux0, uy0;
		float ux1, uy1;
		float advance;
		float bearingX, bearingY;
		float chScaleX, chScaleY;
	};

	struct SdfBitmapContainer
	{
		int width, height;
		int xOff, yOff;
		float advance;
		float bearingX, bearingY;
		float chScaleX, chScaleY;
		unsigned char* bitmap;
	};
}

#endif
