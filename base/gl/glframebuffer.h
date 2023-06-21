#pragma once

#include "base/globject.h"

class GLRenderBuffer: public GLObject{
public:
    GLRenderBuffer(int Width, int Height, GLenum format) {
        glGenRenderbuffers(1, &m_ID);
        glBindRenderbuffer(GL_RENDERBUFFER, m_ID);
        glRenderbufferStorage(GL_RENDERBUFFER, format, Width, Height);
    }
    ~GLRenderBuffer(){
        glDeleteBuffers(1, &m_ID);
    }
};

class GLFrameBuffer: public GLObject{
public:
    GLFrameBuffer() {
        glGenFramebuffers(1, &m_ID);
    }
    ~GLFrameBuffer(){
        glDeleteFramebuffers(1, &m_ID);
        SAFE_DELETE(m_Color)
        SAFE_DELETE(m_Depth)
    }

    static void Bind(GLuint id){
        glBindFramebuffer(GL_FRAMEBUFFER, id);
    }

    void AttachColor(GLTexture* color) {
        m_Color = color;

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Color->GetID(), 0);
    }

    void AttachDepth(GLRenderBuffer* depth) {
        m_Depth = depth;

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Depth->GetID());
    }

    GLTexture* GetColor() const {
        return m_Color;
    }

    bool Valid() const {
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE;
    }

public:
    GLTexture* m_Color = nullptr;
    GLRenderBuffer* m_Depth = nullptr;
};