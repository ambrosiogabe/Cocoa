#pragma once

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
		int xoff, yoff;
		float advance;
		float bearingX, bearingY;
		float chScaleX, chScaleY;
		unsigned char* bitmap;
	};
}