#include "Bang/VBO.h"

#include "Bang/GL.h"

USING_NAMESPACE_BANG

VBO::VBO()
{
    GL::GenBuffers(1, &m_idGL);
}

VBO::~VBO()
{
    GL::DeleteBuffers(1, &m_idGL);
}

void VBO::Fill(const void *data, int dataSize, GL::UsageHint usage)
{
    Bind();
    GL::BufferData(GL::BindTarget::VBO, dataSize, data, usage);
    UnBind();
}

GL::BindTarget VBO::GetGLBindTarget() const
{
    return GL::BindTarget::VBO;
}

void VBO::Bind() const
{
    GL::Bind(this);
}
void VBO::UnBind() const
{
    GL::UnBind(this);
}
