#ifndef COCOA_ENGINE_FONT_H
#define COCOA_ENGINE_FONT_H
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
		CPath path;
		Handle<Texture> fontTexture;
		CharInfo* characterMap;
		int characterMapSize;
		int glyphRangeStart;
		int glyphRangeEnd;
		bool isDefault;
		bool isNull;

		COCOA static Font createFont(CPath& resourcePath, bool isDefault = false);
		COCOA static Font createFont();
		COCOA static Font nullFont();

		COCOA const CharInfo& getCharacterInfo(int codepoint) const;
		COCOA void generateSdf(const CPath& fontFile, int fontSize, const CPath& outputFile, int glyphRangeStart = 0, int glyphRangeEnd = 'z' + 1, int padding = 5, int upscaleResolution = 4096);
		COCOA void free() const;

		COCOA json serialize() const;
		COCOA void deserialize(const json& j);
	};
}

#endif
