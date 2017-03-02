#ifndef GLCONTEXT_H
#define GLCONTEXT_H

#include "GL.h"
#include "List.h"
#include "Color.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "TypeMap.h"
#include "Texture.h"
#include "IRenderAgent.h"

class VAO;
class GLContext : public IRenderAgent
{
public:
    GLContext();
    virtual ~GLContext();

    virtual void ApplyToShaderProgram(ShaderProgram *sp) const;

    void Render(const VAO* vao, GL::RenderMode renderMode,
                int elementsCount, int startIndex) const;

    void SetWriteDepth(bool writeDepth);
    void SetTestDepth(bool testDepth);
    void SetWireframe(bool wireframe);
    void SetCullMode(GL::CullMode cullMode);
    void SetModelMatrix(const Matrix4 &model);
    void SetViewMatrix(const Matrix4 &view);
    void SetProjectionMatrix(const Matrix4 &projection);

    bool IsWriteDepth() const;
    bool IsTestDepth() const;
    bool IsWireframe() const;
    GL::CullMode GetCullMode() const;
    const Matrix4 &GetModelMatrix() const;
    const Matrix4 &GetViewMatrix() const;
    const Matrix4 &GetProjectionMatrix() const;

private:
    bool m_writeDepth = true, m_testDepth = true;
    bool m_wireframe = false;
    GL::CullMode m_cullMode = GL::CullMode::None;

    Matrix4 m_modelMatrix, m_viewMatrix, m_projectionMatrix;
};

#endif // GLCONTEXT_H
