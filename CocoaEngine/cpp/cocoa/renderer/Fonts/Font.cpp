#include "cocoa/renderer/fonts/Font.h"
#include "cocoa/renderer/fonts/FontUtil.h"

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

namespace Cocoa
{
	CharInfo Font::nullCharacter = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	Font Font::nullFont = {};

	Font::Font()
	{
		m_IsNull = true;
	}

	Font::Font(CPath& resourcePath, bool isDefault)
	{
		m_IsDefault = isDefault;
		m_Path = CPath(resourcePath);
	}

	Font::~Font()
	{
		if (m_CharacterMap)
		{
			free(m_CharacterMap);
		}
	}

	const CharInfo& Font::GetCharacterInfo(int codepoint)
	{
		if (codepoint < m_CharacterMapSize && codepoint >= 0)
		{
			return m_CharacterMap[codepoint];
		}
		else
		{
			return Font::nullCharacter;
		}
	}

	void Font::GenerateSdf(const CPath& fontFile, int fontSize, const CPath& outputFile, int glyphRangeStart, int glyphRangeEnd, int padding, int upscaleResolution)
	{
		m_GlyphRangeStart = glyphRangeStart;
		m_GlyphRangeEnd = glyphRangeEnd;
		m_CharacterMap = (CharInfo*)malloc(sizeof(CharInfo) * (glyphRangeEnd - glyphRangeStart));
		FontUtil::CreateSdfFontTexture(fontFile, fontSize, m_CharacterMap, (glyphRangeEnd - glyphRangeStart), outputFile, padding, upscaleResolution, glyphRangeStart);
		
	}
}