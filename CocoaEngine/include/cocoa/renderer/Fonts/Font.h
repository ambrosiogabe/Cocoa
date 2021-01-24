#pragma once
#include "cocoa/core/AssetManager.h"
#include "cocoa/renderer/TextureHandle.h"
#include "cocoa/file/IFile.h"
#include "cocoa/renderer/fonts/DataStructures.h"

namespace Cocoa
{
	class COCOA Font : public Asset
	{
	public:
		Font(CPath& resourcePath, bool isDefault = false);
		~Font();

		virtual void Load() override;
		virtual void Unload() override;

		const CharInfo& GetCharacterInfo(int codepoint);
		void GenerateSdf(const CPath& fontFile, int fontSize, const CPath& outputFile, int glyphRangeStart = 0, int glyphRangeEnd = 'z' + 1, int padding = 5, int upscaleResolution = 4096);

	private:
		static CharInfo nullCharacter;

		CPath m_Path;
		TextureHandle m_FontTexture;
		CharInfo* m_CharacterMap = nullptr;
		int m_CharacterMapSize = 0;
		int m_GlyphRangeStart = 0;
		int m_GlyphRangeEnd = 0;
	};
}
