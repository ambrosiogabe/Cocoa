#pragma once
#include "cocoa/file/IFile.h"
#include "cocoa/renderer/fonts/DataStructures.h"
#include "cocoa/core/Handle.h"
#include "cocoa/renderer/Texture.h"

namespace Cocoa
{
	class COCOA Font
	{
	public:
		Font(CPath& resourcePath, bool isDefault = false);
		Font();

		const CharInfo& GetCharacterInfo(int codepoint) const;
		void GenerateSdf(const CPath& fontFile, int fontSize, const CPath& outputFile, int glyphRangeStart = 0, int glyphRangeEnd = 'z' + 1, int padding = 5, int upscaleResolution = 4096);
		void Free();

		inline bool IsNull() const { return m_IsNull; }
		inline bool IsDefault() const { return m_IsDefault; }

		json Serialize() const;
		void Deserialize(const json& j);

	public:
		static Font nullFont;
		static CharInfo nullCharacter;

		CPath m_Path;
		Handle<Texture> m_FontTexture;
		CharInfo* m_CharacterMap = nullptr;
		int m_CharacterMapSize = 0;
		int m_GlyphRangeStart = 0;
		int m_GlyphRangeEnd = 0;
		bool m_IsDefault;
		bool m_IsNull = false;
	};
}
