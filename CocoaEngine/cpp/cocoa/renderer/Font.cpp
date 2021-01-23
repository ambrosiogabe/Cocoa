#include "cocoa/renderer/Font.h"

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace Cocoa
{
	Font::Font(CPath& resourcePath, bool isDefault)
	{
		GenerateTypeId<Font>();
		m_IsDefault = isDefault;
		m_Path = CPath(resourcePath);
	}

	void Font::Load()
	{
		if (!IFile::IsFile(m_Path))
		{
			Log::Warning("Attempted to load font with non-existent file: '%s'", m_Path.Filepath());
			return;
		}

		// TODO: Add file support for unicode files, that way we don't have to manually open the file here
		unsigned char* fontBuffer;

		FILE* fontFile = fopen(m_Path.Filepath(), "rb");
		fseek(fontFile, 0, SEEK_END);
		int size = ftell(fontFile);
		fseek(fontFile, 0, SEEK_SET);

		fontBuffer = (unsigned char*)malloc(size);
		fread(fontBuffer, size, 1, fontFile);
		fclose(fontFile);

		if (!stbtt_InitFont(&m_FontInfo, fontBuffer, 0))
		{
			Log::Warning("Invalid font file, unable to initialize: '%s'", m_Path.Filepath());
			return;
		}

		int bitmapWidth = 512;
		int bitmapHeight = 128;
		int lineHeight = 64;

		// Create a bitmap for the phrase
		unsigned char* bitmap = (unsigned char*)calloc(bitmapWidth * bitmapHeight, sizeof(unsigned char));

		// Calculate font scaling
		float scale = stbtt_ScaleForPixelHeight(&m_FontInfo, lineHeight);

		int ascent, descent, lineGap;
		stbtt_GetFontVMetrics(&m_FontInfo, &ascent, &descent, &lineGap);

		ascent *= scale;
		descent *= scale;

		std::string word = "the quick brown fox";
		int x = 0;
		for (char character : word)
		{
			// How wide is this character
			int ax;
			int lsb;
			stbtt_GetCodepointHMetrics(&m_FontInfo, character, &ax, &lsb);

			// Get bounding box for character (may be offset to account for chars that dip above or below the line
			int cX1, cY1, cX2, cY2;
			stbtt_GetCodepointBitmapBox(&m_FontInfo, character, scale, scale, &cX1, &cY1, &cX2, &cY2);

			// Compute y (different characters have different heights)
			int y = ascent + cY1;

			// Render character (stride and offset are important here)
			int byteOffset = x + (lsb * scale) + (y * bitmapWidth);
			stbtt_MakeCodepointBitmap(&m_FontInfo, bitmap + byteOffset, cX2 - cX1, cY2 - cY1, bitmapWidth, scale, scale, character);

			// Advance x
			x += ax * scale;

			// Add kerning
			int kern;
			kern = stbtt_GetCodepointKernAdvance(&m_FontInfo, character, character + 1);
			x += kern * scale;
		}

		// Create texture and load the generated image
		stbi_write_png("out.png", bitmapWidth, bitmapHeight, 1, bitmap, bitmapWidth);

		// Free resources
		free(fontBuffer);
		free(bitmap);
	}

	void Font::Unload()
	{
	}
}