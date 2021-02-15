#pragma once
#include "externalLibs.h"
#include "cocoa/renderer/fonts/Font.h"
#include "cocoa/core/Handle.h"

namespace Cocoa
{
	struct FontRenderer
	{
		glm::vec4 m_Color = glm::vec4(1, 1, 1, 1);
		int m_ZIndex = 0;
		Handle<Font> m_Font;
		std::string text;
		int fontSize;
	};
}