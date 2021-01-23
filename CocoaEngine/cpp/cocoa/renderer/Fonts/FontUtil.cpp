#include "cocoa/renderer/Fonts/FontUtil.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/Log.h"

#include "stb/stb_image_write.h"

namespace Cocoa
{
	namespace FontUtil
	{
		int GetPixel(int x, int y, uint8* bitmap, int width, int height)
		{
			return  (x < width) && (y < height) && (x >= 0) && (y >= 0) ?
				bitmap[x + (y * width)] :
				0;
		}

		int FindNearestPixel(int pixX, int pixY, uint8* bitmap, int width, int height, int spread)
		{
			int state = GetPixel(pixX, pixY, bitmap, width, height);
			int minX = pixX - spread;
			int maxX = pixX + spread;
			int minY = pixY - spread;
			int maxY = pixY + spread;

			int minDistance = spread * spread;
			for (int y = minY; y <= maxY; y++)
			{
				for (int x = minX; x <= maxX; x++)
				{
					int pixelState = GetPixel(x, y, bitmap, width, height);
					int xSquared = (x - pixX) * (x - pixX);
					int ySquared = (y - pixY) * (y - pixY);
					int distance = xSquared + ySquared;
					if (pixelState != state && distance < minDistance)
					{
						minDistance = distance;
					}
				}
			}
			minDistance = sqrt(minDistance);

			if (state == 0)
			{
				return -minDistance;
			}
			return minDistance;
		}

		SdfBitmapContainer GenerateSdfCodepointBitmap(int codepoint, FT_Face font, int fontSize, int padding, int spread, int upscaleResolution, bool flipVertically)
		{
			// Render a very large sized character to generate the sdf from
			FT_Set_Pixel_Sizes(font, 0, upscaleResolution);
			if (FT_Load_Char(font, codepoint, FT_LOAD_RENDER))
			{
				printf("Could not generate '%c'.\n", codepoint);
				return {
					0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr
				};
			}

			int width = font->glyph->bitmap.width;
			int height = font->glyph->bitmap.rows;
			unsigned char* img = font->glyph->bitmap.buffer;

			float widthScale = (float)width / (float)upscaleResolution;
			float heightScale = (float)height / (float)upscaleResolution;
			int characterWidth = fontSize * widthScale;
			int characterHeight = fontSize * heightScale;
			int bitmapWidth = characterWidth + padding * 2;
			int bitmapHeight = characterHeight + padding * 2;
			float scaleX = (float)width / (float)characterWidth;
			float scaleY = (float)height / (float)characterHeight;
			unsigned char* sdfBitmap = (unsigned char*)malloc(sizeof(unsigned char) * bitmapHeight * bitmapWidth);
			for (int y = -padding; y < bitmapHeight - padding; y++)
			{
				for (int x = -padding; x < bitmapWidth - padding; x++)
				{
					int pixelX = CMath::MapRange(x, -padding, characterWidth + padding, -padding * scaleX, (characterWidth + padding) * scaleX);
					int pixelY = CMath::MapRange(characterHeight - y, -padding, characterHeight + padding, -padding * scaleY, (characterHeight + padding) * scaleY);
					int sdf = FindNearestPixel(pixelX, pixelY, img, width, height, spread);
					float val = (float)sdf / (float)spread;
					val = (val + 1.0f) / 2.0f;
					if (!flipVertically)
					{
						sdfBitmap[(x + padding) + ((y + padding) * bitmapWidth)] = (int)(val * 255.0f);
					}
					else
					{
						sdfBitmap[(x + padding) + ((bitmapHeight - (y + padding) - 1) * bitmapWidth)] = (int)(val * 255.0f);
					}
				}
			}

			FT_Set_Pixel_Sizes(font, 0, 64.0f);
			FT_Load_Char(font, codepoint, FT_LOAD_RENDER);
			return {
				bitmapWidth, bitmapHeight,
				padding, padding,
				(float)(font->glyph->metrics.horiAdvance >> 6) / 64.0f,
				(float)(font->glyph->metrics.horiBearingX >> 6) / (float)64.0f,
				(float)(font->glyph->metrics.horiBearingY >> 6) / (float)64.0f,
				(float)(font->glyph->metrics.width >> 6) / (float)64.0f,
				(float)(font->glyph->metrics.height >> 6) / (float)64.0f,
				sdfBitmap
			};
		}

		void CreateSdfFontTexture(const CPath& fontFile, int fontSize, CharInfo* characterMap, int characterMapSize, int glyphOffset=0)
		{
			FT_Library ft;
			if (FT_Init_FreeType(&ft))
			{
				printf("Could not initialize freetype.\n");
				return;
			}

			FT_Face font;
			if (FT_New_Face(ft, fontFile.Filepath(), 0, &font))
			{
				printf("Could not load font %s.\n", fontFile);
				return;
			}

			int lowResFontSize = fontSize;
			FT_Set_Pixel_Sizes(font, 0, lowResFontSize);
			if (FT_Load_Char(font, 'M', FT_LOAD_RENDER))
			{
				printf("Failed to load glyph.\n");
				return;
			}

			// Estimate a "squarish" width for the font texture, and leave it at that
			int emWidth = font->glyph->bitmap.width;
			int squareLength = sqrt(characterMapSize - glyphOffset);
			int fixedWidth = squareLength * emWidth;

			// Calculate the dimensions of the actual width and height of the font texture
			// and save all the sdf metrics for each codepoint
			int sdfHeight = 0;
			int sdfWidth = 0;
			int rowHeight = 0;
			int x = 0;
			int y = 0;

			SdfBitmapContainer* sdfBitmaps = (SdfBitmapContainer*)malloc(sizeof(SdfBitmapContainer) * (characterMapSize - glyphOffset));
			for (int codepoint = glyphOffset; codepoint < characterMapSize; codepoint++)
			{
				if (FT_Load_Char(font, codepoint, FT_LOAD_RENDER))
				{
					sdfBitmaps[codepoint - glyphOffset] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr };
					printf("Failed to load glyph.\n");
					continue;
				}

				sdfBitmaps[codepoint - glyphOffset] = GenerateSdfCodepointBitmap(codepoint, font, lowResFontSize, 5, 24, 1024);
				SdfBitmapContainer& container = sdfBitmaps[codepoint - glyphOffset];
				int width = container.width;
				int height = container.height;

				rowHeight = CMath::Max(height, rowHeight);
				if (x + width >= fixedWidth)
				{
					x = 0;
					y += rowHeight;
					rowHeight = height;
				}

				characterMap[codepoint - glyphOffset] = {
					(float)x + container.xoff,
					(float)y + container.yoff,
					(float)(x + width - container.xoff),
					(float)(y + height - container.yoff),
					container.advance,
					container.bearingX,
					container.bearingY,
					container.chScaleX,
					container.chScaleY
				};

				sdfWidth = CMath::Max(sdfWidth, x + width);
				sdfHeight = CMath::Max(sdfHeight, y + rowHeight);
				x += width;
			}

			// Write all the image data to the final sdf
			int bitmapLength = sdfWidth * sdfHeight * 4;
			uint8* finalSdf = (uint8*)malloc(sizeof(uint8) * bitmapLength);
			memset(finalSdf, 0, bitmapLength);
			int endBitmap = bitmapLength + 1;

			for (int codepoint = glyphOffset; codepoint < characterMapSize; codepoint++)
			{
				SdfBitmapContainer& sdf = sdfBitmaps[codepoint - glyphOffset];
				int x0 = characterMap[codepoint - glyphOffset].ux0;
				int y0 = characterMap[codepoint - glyphOffset].uy0;
				int x1 = characterMap[codepoint - glyphOffset].ux1;
				int y1 = characterMap[codepoint - glyphOffset].uy1;

				if (!sdf.bitmap) continue;

				// Texture biases give a little wiggle room for sampling the textures, consider adding these as a parameter
				float bottomLeftTextureBias = -0.1f;
				float topRightTextureBias = 1;
				characterMap[codepoint - glyphOffset] = {
					(float)(x0 + bottomLeftTextureBias) / (float)sdfWidth,
					(float)(y0 + bottomLeftTextureBias) / (float)sdfHeight,
					(float)(x1 + topRightTextureBias) / (float)sdfWidth,
					(float)(y1 + topRightTextureBias) / (float)sdfHeight,
					sdf.advance,
					sdf.bearingX,
					sdf.bearingY,
					sdf.chScaleX,
					sdf.chScaleY
				};

				int width = sdf.width;
				int height = sdf.height;
				int xoff = sdf.xoff;
				int yoff = sdf.yoff;
				x = x0 - xoff;
				y = y0 - yoff;
				for (int imgY = 0; imgY < height; imgY++)
				{
					for (int imgX = 0; imgX < width; imgX++)
					{
						unsigned char pixelData = sdf.bitmap[imgX + imgY * width];
						int index = (x + imgX) * 4 + (y + imgY) * sdfWidth * 4;
						Log::Assert(index + 3 < endBitmap, "Index overflow when generating SDF");
						finalSdf[index] = pixelData;
						finalSdf[index + 1] = pixelData;
						finalSdf[index + 2] = pixelData;
						finalSdf[index + 3] = pixelData;
					}
				}

				free(sdf.bitmap);
			}

			printf("Writing png for font\n");
			stbi_write_png("tmp.png", sdfWidth, sdfHeight, 4, finalSdf, sdfWidth * 4);

			free(sdfBitmaps);
			free(finalSdf);
			FT_Done_Face(font);
			FT_Done_FreeType(ft);
		}
	}
}