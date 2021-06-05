#include "externalLibs.h"

#include "cocoa/renderer/Framebuffer.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/util/JsonExtended.h"
#include "cocoa/core/AssetManager.h"

namespace Cocoa
{
	namespace NFramebuffer
	{
		void generate(Framebuffer& framebuffer)
		{
			Logger::Assert(framebuffer.fbo == (uint32)-1, "Cannot generate framebuffer that has with Fbo id == (uint32)-1.");
			Logger::Assert(framebuffer.rbo == (uint32)-1, "Cannot generate framebuffer that has with Rbo id == (uint32)-1.");

			glGenFramebuffers(1, &framebuffer.fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);

			if (framebuffer.colorAttachments.size() > 1)
			{
				Logger::Assert(framebuffer.colorAttachments.size() < 8, "Too many framebuffer attachments. Only 8 attachments supported.");
				static GLenum colorBufferAttachments[8] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7 };
				glDrawBuffers(framebuffer.colorAttachments.size(), colorBufferAttachments);
			}

			// Create texture to render data to and attach it to framebuffer
			for (int i = 0; i < framebuffer.colorAttachments.size(); i++)
			{
				Texture& texture = framebuffer.colorAttachments[i];
				texture.width = framebuffer.width;
				texture.height = framebuffer.height;
				TextureUtil::generate(texture);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture.graphicsId, 0);
			}

			if (framebuffer.includeDepthStencil)
			{
				Logger::Assert(framebuffer.depthStencilFormat != ByteFormat::None, "Cannot add depth stencil information with no byte format.");

				// Create renderbuffer to store depth_stencil info
				glGenRenderbuffers(1, &framebuffer.rbo);
				glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.rbo);
				uint32 glDepthFormat = TextureUtil::toGl(framebuffer.depthStencilFormat);
				glRenderbufferStorage(GL_RENDERBUFFER, glDepthFormat, framebuffer.width, framebuffer.height);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.rbo);
			}

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				Logger::Assert(false, "Framebuffer is not complete.");
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void regenerate(Framebuffer& framebuffer)
		{
			Logger::Assert(framebuffer.fbo != (uint32)-1, "Cannot regenerate framebuffer that has with Fbo id != (uint32)-1.");
			if (framebuffer.includeDepthStencil)
			{
				Logger::Assert(framebuffer.rbo != (uint32)-1, "Cannot regenerate framebuffer that has with Rbo id != (uint32)-1.");
			}

			destroy(framebuffer, false);
			generate(framebuffer);
		}

		void destroy(Framebuffer& framebuffer, bool clearColorAttachmentSpecs)
		{
			Logger::Assert(framebuffer.fbo != (uint32)-1, "Tried to delete invalid framebuffer.");
			glDeleteFramebuffers(1, &framebuffer.fbo);
			framebuffer.fbo = (uint32)-1;

			for (int i = 0; i < framebuffer.colorAttachments.size(); i++)
			{
				Texture& texture = framebuffer.colorAttachments[i];
				TextureUtil::destroy(texture);
			}

			if (clearColorAttachmentSpecs)
			{
				framebuffer.colorAttachments.clear();
			}

			if (framebuffer.includeDepthStencil)
			{
				Logger::Assert(framebuffer.rbo != (uint32)-1, "Tried to delete invalid renderbuffer.");
				glDeleteRenderbuffers(1, &framebuffer.rbo);
				framebuffer.rbo = (uint32)-1;
			}
		}

		void addColorAttachment(Framebuffer& framebuffer, const Texture& textureSpecification)
		{
			framebuffer.colorAttachments.push_back(textureSpecification);
		}

		const Texture& getColorAttachment(const Framebuffer& framebuffer, int index)
		{
			return framebuffer.colorAttachments.at(index);
		}

		void clearColorAttachmentRgb(const Framebuffer& framebuffer, int colorAttachment, glm::vec3 clearColor)
		{
			Logger::Assert(colorAttachment >= 0 && colorAttachment < framebuffer.colorAttachments.size(), "Index out of bounds. Color attachment does not exist '%d'.", colorAttachment);
			const Texture& texture = framebuffer.colorAttachments[colorAttachment];
			Logger::Assert(TextureUtil::byteFormatIsRgb(texture.internalFormat) && TextureUtil::byteFormatIsRgb(texture.externalFormat), "Cannot clear non-rgb texture as if it were a rgb texture.");

			uint32 externalFormat = TextureUtil::toGl(texture.externalFormat);
			uint32 formatType = TextureUtil::toGlDataType(texture.externalFormat);
			glClearTexImage(texture.graphicsId, 0, externalFormat, formatType, &clearColor);
		}

		void clearColorAttachmentUint32(const Framebuffer& framebuffer, int colorAttachment, uint32 clearColor)
		{
			Logger::Assert(colorAttachment >= 0 && colorAttachment < framebuffer.colorAttachments.size(), "Index out of bounds. Color attachment does not exist '%d'.", colorAttachment);
			const Texture& texture = framebuffer.colorAttachments[colorAttachment];
			Logger::Assert(TextureUtil::byteFormatIsInt(texture.internalFormat) && TextureUtil::byteFormatIsInt(texture.externalFormat), "Cannot clear non-uint texture as if it were a uint texture.");

			uint32 externalFormat = TextureUtil::toGl(texture.externalFormat);
			uint32 formatType = TextureUtil::toGlDataType(texture.externalFormat);
			glClearTexImage(texture.graphicsId, 0, externalFormat, formatType, &clearColor);
		}

		uint32 readPixelUint32(const Framebuffer& framebuffer, int colorAttachment, int x, int y)
		{
			Logger::Assert(colorAttachment >= 0 && colorAttachment < framebuffer.colorAttachments.size(), "Index out of bounds. Color attachment does not exist '%d'.", colorAttachment);
			const Texture& texture = framebuffer.colorAttachments[colorAttachment];
			Logger::Assert(TextureUtil::byteFormatIsInt(texture.internalFormat) && TextureUtil::byteFormatIsInt(texture.externalFormat), "Cannot read non-uint texture as if it were a uint texture.");

			// If we are requesting an out of bounds pixel, return max uint32 which should be a good flag I guess
			// TODO: Create clearColor member variable in color attachments and return that instead here
			if (x < 0 || y < 0 || x >= framebuffer.colorAttachments[colorAttachment].width || y >= framebuffer.colorAttachments[colorAttachment].height)
			{
				return (uint32)-1;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
			glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachment);

			// 128 bits should be big enough for 1 pixel of any format
			// TODO: Come up with generic way to get any type of pixel data
			static uint32 pixel;
			uint32 externalFormat = TextureUtil::toGl(texture.externalFormat);
			uint32 formatType = TextureUtil::toGlDataType(texture.externalFormat);
			glReadPixels(x, y, 1, 1, externalFormat, formatType, &pixel);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return pixel;
		}

		void bind(const Framebuffer& framebuffer)
		{
			Logger::Assert(framebuffer.fbo != (uint32)-1, "Tried to bind invalid framebuffer.");
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
		}

		void unbind(const Framebuffer& framebuffer)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		json serialize(const Framebuffer& framebuffer)
		{
			json res;
			res["Width"] = framebuffer.width;
			res["Height"] = framebuffer.height;
			res["IncludeDepthStencil"] = framebuffer.includeDepthStencil;
			res["DepthStencilFormat"] = framebuffer.depthStencilFormat;
			res["ColorAttachments"] = {};

			for (int i = 0; i < framebuffer.colorAttachments.size(); i++)
			{
				res["ColorAttachments"][i] = TextureUtil::serialize(framebuffer.colorAttachments[i]);
			}

			return res;
		}

		Framebuffer deserialize(const json& j)
		{
			Framebuffer res;
			JsonExtended::assignIfNotNull(j, "Width", res.width);
			JsonExtended::assignIfNotNull(j, "Height", res.height);
			JsonExtended::assignIfNotNull(j, "IncludeDepthStencil", res.includeDepthStencil);

			if (j["ColorAttachments"].size() > 0)
			{
				const int colorAttachmentSize = j["ColorAttachments"].size();
				for (int i = 0; i < colorAttachmentSize; i++)
				{
					if (j["ColorAttachments"][i].is_object())
					{
						addColorAttachment(res, TextureUtil::deserialize(j["ColorAttachments"][i]));
					}
					else
					{
						Logger::Warning("Invalid color attachment '%d' when deserializing framebuffer.", i);
					}
				}
			}

			return res;
		}
	}
}