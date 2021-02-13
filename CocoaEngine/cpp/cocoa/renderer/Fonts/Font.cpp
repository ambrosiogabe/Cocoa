#include "cocoa/renderer/fonts/Font.h"
#include "cocoa/renderer/fonts/FontUtil.h"
#include "cocoa/core/AssetManager.h"
#include "cocoa/util/JsonExtended.h"

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

namespace Cocoa
{
	CharInfo Font::nullCharacter = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	Font Font::nullFont = Font();

	Font::Font()
	{
		m_IsNull = true;
		m_IsDefault = false;
	}

	Font::Font(CPath& resourcePath, bool isDefault)
	{
		m_IsDefault = isDefault;
		m_Path = CPath(resourcePath);
	}

	void Font::Free()
	{
		free(m_CharacterMap);
	}

	const CharInfo& Font::GetCharacterInfo(int codepoint) const
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
		m_CharacterMapSize = glyphRangeEnd - glyphRangeStart;
		FontUtil::CreateSdfFontTexture(fontFile, fontSize, m_CharacterMap, (glyphRangeEnd - glyphRangeStart), outputFile, padding, upscaleResolution, glyphRangeStart);
	}

	json Font::Serialize() const
	{
		json res;
		res["CharacterMapSize"] = m_CharacterMapSize;
		for (int i = 0; i < m_CharacterMapSize; i++)
		{
			const CharInfo& charInfo = m_CharacterMap[i];
			res["CharacterMap"][std::to_string(i)] = {
				{"ux0", charInfo.ux0},
				{"uy0", charInfo.uy0},
				{"ux1", charInfo.ux1},
				{"uy1", charInfo.uy1},
				{"advance", charInfo.advance},
				{"bearingX", charInfo.bearingX},
				{"bearingY", charInfo.bearingY},
				{"chScaleX", charInfo.chScaleX},
				{"chScaleY", charInfo.chScaleY}
			};
		}

		res["FontTextureId"] = m_FontTexture.m_AssetId;
		res["GlyphRangeStart"] = m_GlyphRangeStart;
		res["GlyphRangeEnd"] = m_GlyphRangeEnd;
		res["Filepath"] = m_Path.Path.c_str();
		return res;
	}

	void Font::Deserialize(const json& j)
	{
		JsonExtended::AssignIfNotNull(j, "CharacterMapSize", m_CharacterMapSize);

		m_CharacterMap = (CharInfo*)malloc(sizeof(CharInfo) * m_CharacterMapSize);
		for (int i = 0; i < m_CharacterMapSize; i++)
		{
			float ux0, uy0, ux1, uy1, advance, bearingX, bearingY, chScaleX, chScaleY;
			if (j.contains("CharacterMap") && j["CharacterMap"].contains(std::to_string(i)))
			{
				json subJ = j["CharacterMap"][std::to_string(i)];
				JsonExtended::AssignIfNotNull(subJ, "ux0", ux0);
				JsonExtended::AssignIfNotNull(subJ, "uy0", uy0);
				JsonExtended::AssignIfNotNull(subJ, "ux1", ux1);
				JsonExtended::AssignIfNotNull(subJ, "uy1", uy1);
				JsonExtended::AssignIfNotNull(subJ, "advance", advance);
				JsonExtended::AssignIfNotNull(subJ, "bearingX", bearingX);
				JsonExtended::AssignIfNotNull(subJ, "bearingY", bearingY);
				JsonExtended::AssignIfNotNull(subJ, "chScaleX", chScaleX);
				JsonExtended::AssignIfNotNull(subJ, "chScaleY", chScaleY);
				m_CharacterMap[i] = {
					ux0, uy0,
					ux1, uy1,
					advance,
					bearingX, bearingY,
					chScaleX, chScaleY
				};
			}
		}

		JsonExtended::AssignIfNotNull(j, "FontTextureId", m_FontTexture.m_AssetId);
		JsonExtended::AssignIfNotNull(j, "GlyphRangeStart", m_GlyphRangeStart);
		JsonExtended::AssignIfNotNull(j, "GlyphRangeEnd", m_GlyphRangeEnd);
		JsonExtended::AssignIfNotNull(j, "Filepath", m_Path);
	}
}