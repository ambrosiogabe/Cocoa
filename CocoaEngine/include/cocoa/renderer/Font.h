#pragma once
#include "cocoa/core/AssetManager.h"
#include "cocoa/renderer/TextureHandle.h"
#include "cocoa/file/IFile.h"

#include "stb/stb_truetype.h"

namespace Cocoa
{
	class COCOA Font : public Asset
	{
	public:
		Font(CPath& resourcePath, bool isDefault = false);

		virtual void Load() override;
		virtual void Unload() override;

	private:
		CPath m_Path;
		TextureHandle m_FontTexture;
		stbtt_fontinfo m_FontInfo;
	};
}
