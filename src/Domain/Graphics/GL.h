#ifndef GL_H
#define GL_H

#include "Matrix4.h"
#include "ShaderProgram.h"

class GLContext;
class GL
{
public:
    enum class RenderMode
    {
        Points = GL_POINTS,
        Lines = GL_LINES,
        LineStrip = GL_LINE_STRIP,
        Triangles = GL_TRIANGLES,
        Quads = GL_QUADS
    };

    enum class CullMode
    {
        Front = GL_FRONT,
        Back = GL_BACK,
        FrontAndBack = GL_FRONT_AND_BACK,
        None = GL_NONE
    };

    static void SetWireframe(bool wireframe);
    static void SetCullMode(const GL::CullMode cullMode);
    static void SetModelMatrix(const Matrix4 &model);
    static void SetViewMatrix(const Matrix4 &view);
    static void SetProjectionMatrix(const Matrix4 &projection);

    static void ApplyToShaderProgram(ShaderProgram *sp);
    static void Apply();
    static void Reset();

    static bool IsWireframe();
    static GL::CullMode GetCullMode();
    static const Matrix4 &GetModelMatrix();
    static const Matrix4 &GetViewMatrix();
    static const Matrix4 &GetProjectionMatrix();

    static GLContext* GetGLContext();

private:
    GL();
};

#endif // GL_H