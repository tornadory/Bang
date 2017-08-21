#ifndef GL_H
#define GL_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <stack>

#include "Bang/Map.h"
#include "Bang/Color.h"
#include "Bang/String.h"
#include "Bang/Matrix4.h"

#define GL_CheckError() ( GL::CheckError(__LINE__, __FUNCTION__, __FILE__) )

using GLId = GLuint;

FORWARD class G_VAO;
FORWARD class GLObject;
FORWARD class G_Texture;
FORWARD class G_ShaderProgram;

class GL
{
public:
    enum class Primitives
    {
        Points    = GL_POINTS,
        Lines     = GL_LINES,
        LineStrip = GL_LINE_STRIP,
        Triangles = GL_TRIANGLES,
        Quads     = GL_QUADS
    };

    enum class CullMode
    {
        Front        = GL_FRONT,
        Back         = GL_BACK,
        FrontAndBack = GL_FRONT_AND_BACK,
        None         = GL_NONE
    };

    enum class BindTarget
    {
        None           = 0,
        Texture2D      = GL_TEXTURE_2D,
        ShaderProgram  ,
        Framebuffer    = GL_FRAMEBUFFER,
        VAO            ,
        VBO
    };

    enum class ViewProjMode
    {
        UseBoth            = 0,
        OnlyFixAspectRatio = 1,
        IgnoreBoth         = 2
    };

    static void ClearError();
    static bool CheckError(int line = 0, const String &func = "",
                           const String &file = "");
    static bool CheckFramebufferError();

    static void ClearColorBuffer(const Color& clearColor = Color::Zero,
                                 bool clearR = true, bool clearG = true,
                                 bool clearB = true, bool clearA = true);
    static void ClearDepthBuffer(float clearDepth = 1.0f);
    static void ClearStencilBuffer();

    static void Enable (GLenum glEnum);
    static void Disable(GLenum glEnum);

    static void SetViewport(int x, int y, int width, int height);
    static void SetLineWidth(float lineWidth);

    static void SetColorMask(bool maskR, bool maskG, bool maskB, bool maskA);
    static void SetViewProjMode(ViewProjMode mode);
    static void SetStencilWrite(bool writeStencil);
    static void SetStencilOp(GLenum zPassOp);
    static void SetStencilValue(Byte value);
    static void SetStencilTest(bool testStencil);
    static void SetDepthWrite(bool writeDepth);
    static void SetDepthTest(bool testDepth);
    static void SetWireframe(bool wireframe);
    static void SetCullMode(const GL::CullMode cullMode);
    static void SetModelMatrix(const Matrix4 &model);
    static void SetViewMatrix(const Matrix4 &view);
    static void SetProjectionMatrix(const Matrix4 &projection);
    static void SetZNearFar(float zNear, float zFar);

    static void ApplyToShaderProgram(G_ShaderProgram *sp);
    static void Render(const G_VAO* vao,
                       GL::Primitives renderMode,
                       int elementsCount,
                       int startElementIndex = 0);

    static GLenum GetStencilOp();
    static Byte GetStencilValue();
    static bool IsColorMaskR();
    static bool IsColorMaskG();
    static bool IsColorMaskB();
    static bool IsColorMaskA();
    static bool IsStencilWrite();
    static bool IsStencilTest();
    static bool IsDepthWrite();
    static bool IsDepthTest();
    static bool IsWireframe();
    static GL::CullMode GetCullMode();
    static const Matrix4 &GetModelMatrix();
    static const Matrix4 &GetViewMatrix();
    static const Matrix4 &GetProjectionMatrix();

    static void Bind(const GLObject *bindable);
    static void Bind(BindTarget bindTarget, GLId glId);
    static void UnBind(const GLObject *bindable);
    static void UnBind(BindTarget bindTarget);
    static GLId GetBoundId(GL::BindTarget bindTarget);
    static bool IsBound(const GLObject *bindable);
    static bool IsBound(BindTarget bindTarget, GLId glId);

    static GL* GetActive();

    GL();

private:
    Byte m_stencilValue = 0;
    GLenum m_stencilOp = GL_KEEP;
    bool m_colorMaskR = true, m_colorMaskG = true,
         m_colorMaskB = true, m_colorMaskA = true;
    bool m_writeDepth = true, m_testDepth = true;
    bool m_testStencil = false;
    bool m_wireframe = false;
    GL::CullMode m_cullMode = GL::CullMode::None;
    Map<GL::BindTarget, std::stack<GLId> > m_glBoundIds;

    GL::ViewProjMode m_viewProjMode = GL::ViewProjMode::UseBoth;
    Matrix4 m_modelMatrix, m_viewMatrix, m_projectionMatrix;
    float m_zNear, m_zFar;

    static void _Bind(BindTarget bindTarget, GLId glId);
    void OnBind(GL::BindTarget bindTarget, GLId glId);
    void OnUnBind(GL::BindTarget bindTarget);

    friend class GraphicPipeline;
};

#endif // GL_H
