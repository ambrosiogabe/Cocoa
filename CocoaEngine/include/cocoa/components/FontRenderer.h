#ifndef COCOA_ENGINE_FONT_RENDERER_H
#define COCOA_ENGINE_FONT_RENDERER_H
#include "externalLibs.h"
#include "cocoa/renderer/Fonts/Font.h"
#include "cocoa/core/Handle.h"

namespace Cocoa
{
	struct FontRenderer
	{
		glm::vec4 color = glm::vec4(1, 1, 1, 1);
		int zIndex = 0;
		Handle<Font> font;
		std::string text;
		int fontSize;
	};
}

#endif
