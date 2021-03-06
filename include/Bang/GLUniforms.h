#ifndef GLUNIFORMS_H
#define GLUNIFORMS_H

#include "Bang/GL.h"
#include "Bang/Array.h"
#include "Bang/TypeMap.h"
#include "Bang/UniformBuffer.h"

NAMESPACE_BANG_BEGIN

FORWARD class ShaderProgram;

class GLUniforms
{
public:
    struct Matrices
    {
        Matrix4 model;
        Matrix4 normal;
        Matrix4 view;
        Matrix4 viewInv;
        Matrix4 proj;
        Matrix4 projInv;
        Matrix4 pvm;
    };

    struct Camera
    {
        float zNear;
        float zFar;
    };

    struct Viewport
    {
        Vector2 minPos;
        Vector2 size;
    };

    template <class T>
    struct GLSLVar
    {
        String name = ""; T value;
        GLSLVar(const String &_name, const T &_value)
            : name(_name), value(_value) {}
        GLSLVar() {}
    };

    template <class T>
    static T GetUniform(GLId program, int uniformLocation);

    template <class T>
    static T GetUniform(GLId program, const String &uniformName);

    template <class T>
    static T GetUniform(const String &uniformName);

    template <class T>
    static GLSLVar<T> GetUniformAt(GLId shaderProgramId, GLuint uniformIndex);

    template <class T>
    static UniformBuffer<T>* CreateBuffer();

    template <class T>
    static UniformBuffer<T>* GetBuffer();

    template <class T>
    static void RemoveBuffer();

    static void SetAllUniformsToShaderProgram(ShaderProgram *sp);
    static void BindAllUniformBuffersToShader(const ShaderProgram *sp);

    static void SetModelMatrix(const Matrix4 &model);
    static void SetViewMatrix(const Matrix4 &view);
    static void SetProjectionMatrix(const Matrix4 &projection);
    static void UpdatePVMMatrix();

    void SetViewProjMode(GL::ViewProjMode viewProjMode);
    GL::ViewProjMode GetViewProjMode() const;

    static UniformBuffer<Camera>* GetCameraBuffer();
    static UniformBuffer<Viewport>* GetViewportBuffer();

private:
    TypeMap<IUniformBuffer*> m_uniformBuffers;

    Matrices m_matrices;
    int m_bindingPointsUsed = 0;
    GL::ViewProjMode m_viewProjMode = GL::ViewProjMode::IgnoreBothAndModel;

    GLUniforms();
    virtual ~GLUniforms();

    template <class T>
    UniformBuffer<T>* _CreateBuffer();

    template <class T>
    UniformBuffer<T>* _GetBuffer();

    template <class T>
    void _RemoveBuffer();

    void RemoveBuffer(IUniformBuffer *buffer);

    static Matrices *GetMatrices();
    static GLUniforms *GetActive();

    friend class GL;
};

NAMESPACE_BANG_END

#include "Bang/GLUniforms.tcc"

#endif // GLUNIFORMS_H

