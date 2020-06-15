#include "jade/renderer/Framebuffer.h"
#include "jade/renderer/Texture.h"
#include "jade/util/Log.h"

#include <gl/gl3w.h>

void Framebuffer::Init() {
    glGenFramebuffers(1, &m_ID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_ID);

    // Create texture to render data too and attach it to framebuffer
    m_Texture = new Texture(m_Width, m_Height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Texture->GetId(), 0);

    // Create renderbuffer to store depth_stencil info
    unsigned int rboID;
    glGenRenderbuffers(1, &rboID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, m_Width, m_Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboID);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        Log::Assert(false, "Framebuffer is not complete.");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}