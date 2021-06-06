#include "cocoa/renderer/Fonts/FontUtil.h"
#include "cocoa/util/CMath.h"

#include "stb/stb_image_write.h"

// TODO: CONSIDER WRITING MY OWN THREAD LIBRARY?
#include <thread>

namespace Cocoa
{
	namespace FontUtil
	{
		int getPixel(int x, int y, uint8* bitmap, int width, int height)
		{
			return  (x < width) && (y < height) && (x >= 0) && (y >= 0) ?
				bitmap[x + (y * width)] :
				0;
		}

		float findNearestPixel(int pixX, int pixY, uint8* bitmap, int width, int height, int spread)
		{
			int state = getPixel(pixX, pixY, bitmap, width, height);
			int minX = pixX - spread;
			int maxX = pixX + spread;
			int minY = pixY - spread;
			int maxY = pixY + spread;

			int minDistance = spread * spread;
			for (int y = minY; y <= maxY; y++)
			{
				for (int x = minX; x <= maxX; x++)
				{
					int pixelstate = getPixel(x, y, bitmap, width, height);
					int xsquared = (x - pixX) * (x - pixX);
					int ysquared = (y - pixY) * (y - pixY);
					int distance = xsquared + ysquared;
					if (pixelstate != state)
					{
						minDistance = CMath::min(distance, minDistance);
					}
				}
			}

			minDistance = (int)sqrt(minDistance);
			float output = (minDistance - 0.5f) / (spread - 0.5f);
			output *= state == 0 ? -1 : 1;
			return (output + 1) * 0.5f;
		}

		SdfBitmapContainer generateSdfCodepointBitmap(int codepoint, FT_Face font, int fontSize, int padding, int upscaleResolution, bool flipVertically)
		{
			int spread = upscaleResolution / 2;

			// Render a very large sized character to generate the sdf from
			FT_Set_Pixel_Sizes(font, 0, upscaleResolution);
			if (FT_Load_Char(font, codepoint, FT_LOAD_RENDER))
			{
				Logger::Warning("Could not generate '%c'.\n", codepoint);
				return {
					0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr
				};
			}

			int width = font->glyph->bitmap.width;
			int height = font->glyph->bitmap.rows;
			unsigned char* img = font->glyph->bitmap.buffer;

			float widthScale = (float)width / (float)upscaleResolution;
			float heightScale = (float)height / (float)upscaleResolution;
			int characterWidth = (int)(fontSize * widthScale);
			int characterHeight = (int)(fontSize * heightScale);
			int bitmapWidth = characterWidth + padding * 2;
			int bitmapHeight = characterHeight + padding * 2;
			float scaleX = (float)width / (float)characterWidth;
			float scaleY = (float)height / (float)characterHeight;
			unsigned char* sdfBitmap = (unsigned char*)AllocMem(sizeof(unsigned char) * bitmapHeight * bitmapWidth);
			Logger::Assert(sdfBitmap != nullptr, "Ran out of memory. Could not allocate memory to generate a font.");

			for (int y = -padding; y < bitmapHeight - padding; y++)
			{
				for (int x = -padding; x < bitmapWidth - padding; x++)
				{
					int pixelX = (int)CMath::mapRange((float)x, -(float)padding, (float)(characterWidth + padding), -padding * scaleX, (characterWidth + padding) * scaleX);
					int pixelY = (int)CMath::mapRange((float)(characterHeight - y), -(float)padding, (float)(characterHeight + padding), -padding * scaleY, (characterHeight + padding) * scaleY);
					float val = findNearestPixel(pixelX, pixelY, img, width, height, spread);
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

			FT_Set_Pixel_Sizes(font, 0, 64);
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

		static void fillSdfBitmaps(int begin, int end, SdfBitmapContainer* arr, const char* fontFile, int fontSize, int padding, int upscaleResolution, int glyphOffset)
		{
			FT_Library ft;
			if (FT_Init_FreeType(&ft))
			{
				Logger::Warning("Could not initialize freetype.\n");
				return;
			}

			FT_Face font;
			if (FT_New_Face(ft, fontFile, 0, &font))
			{
				Logger::Warning("Could not load font %s.\n", fontFile);
				return;
			}

			for (int i = begin; i < end; i++)
			{
				arr[i] = generateSdfCodepointBitmap(i + glyphOffset, font, fontSize, padding, upscaleResolution);
			}

			FT_Done_Face(font);
			FT_Done_FreeType(ft);
		}

		void createSdfFontTexture(const Path& fontFile, int fontSize, CharInfo* characterMap, int characterMapSize, const Path& outputFile, int padding, int upscaleResolution, int glyphOffset)
		{
			FT_Library ft;
			if (FT_Init_FreeType(&ft))
			{
				Logger::Warning("Could not initialize freetype.\n");
				return;
			}

			FT_Face font;
			if (FT_New_Face(ft, fontFile.path, 0, &font))
			{
				Logger::Warning("Could not load font %s.\n", fontFile.path);
				return;
			}

			int lowResFontSize = fontSize;
			FT_Set_Pixel_Sizes(font, 0, lowResFontSize);
			if (FT_Load_Char(font, 'M', FT_LOAD_RENDER))
			{
				Logger::Warning("Failed to load glyph.\n");
				return;
			}

			// Estimate a "squarish" width for the font texture, and leave it at that
			int emWidth = font->glyph->bitmap.width;
			int squareLength = (int)sqrt(characterMapSize - glyphOffset) + 1;
			int fixedWidth = squareLength * emWidth;

			// Calculate the dimensions of the actual width and height of the font texture
			// and save all the sdf metrics for each codepoint
			int sdfHeight = 0;
			int sdfWidth = 0;
			int rowHeight = 0;
			int x = 0;
			int y = 0;

			SdfBitmapContainer* sdfBitmaps = (SdfBitmapContainer*)AllocMem(sizeof(SdfBitmapContainer) * (characterMapSize - glyphOffset));
			// Just to be safe, use up to cores - 2
			const auto processorCount = CMath::max(std::thread::hardware_concurrency() - 2, 1);
			int segmentSize = ('z' + 1) / processorCount;
			int count = 0;
			std::vector<std::thread> threads;
			for (int i = 0; i <= processorCount; i++)
			{
				int end = CMath::min(segmentSize + count, characterMapSize - glyphOffset);
				threads.push_back(std::thread(fillSdfBitmaps, count, end, sdfBitmaps, fontFile.path, lowResFontSize, padding, upscaleResolution, glyphOffset));
				count += segmentSize;
			}

			for (auto& th : threads)
			{
				th.join();
			}

			for (int codepoint = glyphOffset; codepoint < characterMapSize; codepoint++)
			{
				if (FT_Load_Char(font, codepoint, FT_LOAD_RENDER))
				{
					sdfBitmaps[codepoint - glyphOffset] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, nullptr };
					Logger::Warning("Failed to load glyph.\n");
					continue;
				}

				SdfBitmapContainer& container = sdfBitmaps[codepoint];
				int width = container.width;
				int height = container.height;

				rowHeight = CMath::max(height, rowHeight);
				if (x + width >= fixedWidth)
				{
					x = 0;
					y += rowHeight;
					rowHeight = height;
				}

				characterMap[codepoint - glyphOffset] = {
					(float)x + container.xOff,
					(float)y + container.yOff,
					(float)(x + width - container.xOff),
					(float)(y + height - container.yOff),
					container.advance,
					container.bearingX,
					container.bearingY,
					container.chScaleX,
					container.chScaleY
				};

				sdfWidth = CMath::max(sdfWidth, x + width);
				sdfHeight = CMath::max(sdfHeight, y + rowHeight);
				x += width;
			}

			// Write all the image data to the final sdf
			int bitmapLength = sdfWidth * sdfHeight * 4;
			uint8* finalSdf = (uint8*)AllocMem(sizeof(uint8) * bitmapLength);
			Logger::Assert(finalSdf != nullptr, "Out of memory. Could not allocate memory to generate font.");
			memset(finalSdf, 0, bitmapLength);
			int endBitmap = bitmapLength + 1;

			for (int codepoint = glyphOffset; codepoint < characterMapSize; codepoint++)
			{
				SdfBitmapContainer& sdf = sdfBitmaps[codepoint - glyphOffset];
				float x0 = characterMap[codepoint - glyphOffset].ux0;
				float y0 = characterMap[codepoint - glyphOffset].uy0;
				float x1 = characterMap[codepoint - glyphOffset].ux1;
				float y1 = characterMap[codepoint - glyphOffset].uy1;

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
				int xoff = sdf.xOff;
				int yoff = sdf.yOff;
				x = (int)x0 - xoff;
				y = (int)y0 - yoff;
				for (int imgY = 0; imgY < height; imgY++)
				{
					for (int imgX = 0; imgX < width; imgX++)
					{
						unsigned char pixelData = sdf.bitmap[imgX + imgY * width];
						int index = (x + imgX) * 4 + (y + imgY) * sdfWidth * 4;
						Logger::Assert(index + 3 < endBitmap, "Index overflow when generating SDF");
						finalSdf[index] = pixelData;
						finalSdf[index + 1] = pixelData;
						finalSdf[index + 2] = pixelData;
						finalSdf[index + 3] = pixelData;
					}
				}

				FreeMem(sdf.bitmap);
			}

			Logger::Info("Writing png for font at '%s'\n", outputFile.path);
			stbi_write_png(outputFile.path, sdfWidth, sdfHeight, 4, finalSdf, sdfWidth * 4);

			FreeMem(sdfBitmaps);
			FreeMem(finalSdf);
			FT_Done_Face(font);
			FT_Done_FreeType(ft);
		}
	}
}