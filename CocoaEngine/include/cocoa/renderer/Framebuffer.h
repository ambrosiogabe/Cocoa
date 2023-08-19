#ifndef COCOA_ENGINE_FRAMEBUFFER_H
#define COCOA_ENGINE_FRAMEBUFFER_H
#include "cocoa/renderer/Texture.h"
#include "cocoa/core/Handle.h"

namespace Cocoa
{
	struct Framebuffer
	{
		uint32 fbo = (uint32)-1;
		int32 width = 0;
		int32 height = 0;

		// Depth/Stencil attachment (optional)
		uint32 rbo = (uint32)-1;
		ByteFormat depthStencilFormat = ByteFormat::None;
		bool includeDepthStencil = true;

		// Color attachments
		std::vector<Texture> colorAttachments; // TODO: All color attachments will be resized to match the framebuffer size (perhaps this should be changed in the future...?)
	};

	namespace NFramebuffer
	{
		COCOA void destroy(Framebuffer& framebuffer, bool clearColorAttachmentSpecs = true);
		COCOA void generate(Framebuffer& framebuffer);
		COCOA void regenerate(Framebuffer& framebuffer);

		COCOA const Texture& getColorAttachment(const Framebuffer& framebuffer, int index);
		COCOA void addColorAttachment(Framebuffer& framebuffer, const Texture& textureSpec); // TODO: The order the attachments are added will be the index they get (change this in the future too...?)
		COCOA void clearColorAttachmentRgb(const Framebuffer& framebuffer, int colorAttachment, glm::vec3 clearColor);
		COCOA void clearColorAttachmentUint32(const Framebuffer& framebuffer, int colorAttachment, uint32 clearColor);
		COCOA uint32 readPixelUint32(const Framebuffer& framebuffer, int colorAttachment, int x, int y);

		COCOA void bind(const Framebuffer& framebuffer);
		COCOA void unbind(const Framebuffer& framebuffer);

		COCOA json serialize(const Framebuffer& framebuffer);
		COCOA Framebuffer deserialize(const json& j);
	};
}

#endif
