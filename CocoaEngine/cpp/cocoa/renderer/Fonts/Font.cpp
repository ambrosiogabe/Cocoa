#include "cocoa/renderer/fonts/Font.h""
#include "cocoa/renderer/fonts/FontUtil.h"
#include "cocoa/util/JsonExtended.h"

#undef CreateFont;

namespace Cocoa
{
	static CharInfo mNullCharacter = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	static Font mNullFontObj;

	Font Font::createFont()
	{
		Font res;
		res.isNull = true;
		res.isDefault = false;
		return res;
	}

	Font Font::createFont(CPath& resourcePath, bool isDefault)
	{
		Font res;
		res.isDefault = isDefault;
		res.path = CPath(resourcePath);
		return res;
	}

	Font Font::nullFont()
	{
		return mNullFontObj;
	}

	void Font::free() const
	{
		FreeMem(characterMap);
	}

	const CharInfo& Font::getCharacterInfo(int codepoint) const
	{
		if (codepoint < characterMapSize && codepoint >= 0)
		{
			return characterMap[codepoint];
		}
		else
		{
			return mNullCharacter;
		}
	}

	void Font::generateSdf(const CPath& fontFile, int fontSize, const CPath& outputFile, int glyphRangeStart, int glyphRangeEnd, int padding, int upscaleResolution)
	{
		glyphRangeStart = glyphRangeStart;
		glyphRangeEnd = glyphRangeEnd;
		characterMap = (CharInfo*)AllocMem(sizeof(CharInfo) * (glyphRangeEnd - glyphRangeStart));
		characterMapSize = glyphRangeEnd - glyphRangeStart;
		FontUtil::createSdfFontTexture(fontFile, fontSize, characterMap, (glyphRangeEnd - glyphRangeStart), outputFile, padding, upscaleResolution, glyphRangeStart);
	}

	json Font::serialize() const
	{
		json res;
		res["CharacterMapSize"] = characterMapSize;
		for (int i = 0; i < characterMapSize; i++)
		{
			const CharInfo& charInfo = characterMap[i];
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

		res["FontTextureId"] = fontTexture.assetId;
		res["GlyphRangeStart"] = glyphRangeStart;
		res["GlyphRangeEnd"] = glyphRangeEnd;
		res["Filepath"] = path.path;
		return res;
	}

	void Font::deserialize(const json& j)
	{
		JsonExtended::AssignIfNotNull(j, "CharacterMapSize", characterMapSize);

		characterMap = (CharInfo*)AllocMem(sizeof(CharInfo) * characterMapSize);
		for (int i = 0; i < characterMapSize; i++)
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
				characterMap[i] = {
					ux0, uy0,
					ux1, uy1,
					advance,
					bearingX, bearingY,
					chScaleX, chScaleY
				};
			}
		}

		JsonExtended::AssignIfNotNull(j, "FontTextureId", fontTexture.assetId);
		JsonExtended::AssignIfNotNull(j, "GlyphRangeStart", glyphRangeStart);
		JsonExtended::AssignIfNotNull(j, "GlyphRangeEnd", glyphRangeEnd);
		JsonExtended::AssignIfNotNull(j, "Filepath", path);
	}
}