#pragma once
#include "cocoa/file/File.h"
#include "cocoa/renderer/fonts/DataStructures.h"
#include "cocoa/core/Handle.h"
#include "cocoa/renderer/Texture.h"

#undef CreateFont

// TODO: I haven't tested changing this to a namespace
// TODO: Test me thoroughly!!!
namespace Cocoa
{
	struct Font
	{
		CPath m_Path;
		Handle<Texture> m_FontTexture;
		CharInfo* m_CharacterMap;
		int m_CharacterMapSize;
		int m_GlyphRangeStart;
		int m_GlyphRangeEnd;
		bool m_IsDefault;
		bool m_IsNull;

		COCOA static Font CreateFont(CPath& resourcePath, bool isDefault = false);
		COCOA static Font CreateFont();
		COCOA static Font NullFont();

		COCOA const CharInfo& GetCharacterInfo(int codepoint) const;
		COCOA void GenerateSdf(const CPath& fontFile, int fontSize, const CPath& outputFile, int glyphRangeStart = 0, int glyphRangeEnd = 'z' + 1, int padding = 5, int upscaleResolution = 4096);
		COCOA void Free();

		COCOA inline bool IsNull() const { return m_IsNull; }
		COCOA inline bool IsDefault() const { return m_IsDefault; }

		COCOA json Serialize() const;
		COCOA void Deserialize(const json& j);
	};
}
