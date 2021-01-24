#include "cocoa/renderer/fonts/Font.h"
#include "cocoa/renderer/fonts/FontUtil.h"

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

namespace Cocoa
{
	CharInfo Font::nullCharacter = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	Font::Font(CPath& resourcePath, bool isDefault)
	{
		GenerateTypeId<Font>();
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

	void Font::Load()
	{
		if (!IFile::IsFile(m_Path))
		{
			Log::Warning("Attempted to load font with non-existent file: '%s'", m_Path.Filepath());
			return;
		}

		File* file = IFile::OpenFile(m_Path);
		if (file->m_Data.size() <= 0)
		{
			Log::Warning("Attempted to load font with no data: '%s'", m_Path.Filepath());
			return;
		}

		json j = json::parse(file->m_Data);
		if (j["CharacterMap"].is_array())
		{
			int size = j["CharacterMap"].size();
			m_CharacterMap = (CharInfo*)malloc(sizeof(CharInfo) * size);
			m_CharacterMapSize = size;
			for (int i=0; i < size; i++)
			{
				json& component = j["CharacterMap"][i];
				CharInfo& info = m_CharacterMap[i];
				info.ux0 = component["ux0"];
				info.uy0 = component["uy0"];
				info.ux1 = component["ux1"];
				info.uy1 = component["uy1"];
				info.advance = component["advance"];
				info.bearingX = component["bearingX"];
				info.bearingY = component["bearingY"];
				info.chScaleX = component["chScaleX"];
				info.chScaleY = component["chScaleY"];
			}
		}

		if (j["TexturePath"].is_string())
		{
			CPath texturePath = std::string(j["TexturePath"]);
			m_FontTexture = AssetManager::LoadTextureFromFile(texturePath);
		}

		if (j["GlyphRangeStart"].is_number_integer())
		{
			m_GlyphRangeStart = j["GlyphRangeStart"];
		}

		if (j["GlyphRangeEnd"].is_number_integer())
		{
			m_GlyphRangeEnd = j["GlyphRangeEnd"];
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

	void Font::Unload()
	{
	}
}