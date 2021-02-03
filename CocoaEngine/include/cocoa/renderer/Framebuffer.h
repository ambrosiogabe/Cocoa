#pragma once

#include "cocoa/renderer/Texture.h"
#include "cocoa/core/Handle.h"

namespace Cocoa
{
	enum class TextureFormat
	{
		None = 0,

		// Color
		RGBA8,

		// Depth/Stencil
		DEPTH24STENCIL8
	};

	struct TextureSpecification
	{
		TextureSpecification() = default;
		TextureSpecification(TextureFormat format)
			: m_TextureFormat(format) { }

		TextureFormat m_TextureFormat;
	};

	struct FramebufferAttachment
	{

	};

	class COCOA Framebuffer
	{
	public:
		Framebuffer(int width, int height)
			: m_Width(width), m_Height(height)
		{
			Init();
		}

		Framebuffer() {}

		void Init();

		Framebuffer& AddAttachment(TextureSpecification textureSpecification);
		Framebuffer& Generate();

		Texture* GetTexture() const { return m_Texture; }
		unsigned int GetId() const { return m_ID; }
		int GetWidth() const { return m_Width; }
		int GetHight() const { return m_Height; }

	public:
		int m_Width = 0;
		int m_Height = 0;
		unsigned int m_ID = 0;
		Texture* m_Texture = nullptr;

	private:
		std::vector<Handle<Texture>> m_ColorAttachments;
		Handle<Texture> m_DepthAttachment;
	};
}