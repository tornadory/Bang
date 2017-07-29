#ifndef G_SHADERPROGRAM_H
#define G_SHADERPROGRAM_H

#include "Bang/Map.h"
#include "Bang/GLObject.h"

FORWARD   class Color;
FORWARD   class G_Shader;
FORWARD   class G_Texture;
FORWARD_T class Vector2G;
FORWARD_T class Vector3G;
FORWARD_T class Vector4G;
FORWARD_T class Matrix3G;
FORWARD_T class Matrix4G;

class G_ShaderProgram : public GLObject
{
public:
    G_ShaderProgram();
    virtual ~G_ShaderProgram();

    virtual void Load(const Path &vshaderPath, const Path &fshaderPath);

    bool Link();

    void Bind() const override;
    void UnBind() const override;
    GL::BindTarget GetGLBindTarget() const override;

    bool Set(const String &name, int v) const;
    bool Set(const String &name, float v) const;
    bool Set(const String &name, bool v) const;
    bool Set(const String &name, const Color &c) const;
    bool Set(const String &name, const Matrix3G<float> &m) const;
    bool Set(const String &name, const Matrix4G<float>& m) const;
    bool Set(const String &name, const Vector2G<float>& v) const;
    bool Set(const String &name, const Vector3G<float>& v) const;
    bool Set(const String &name, const Vector4G<float>& v) const;
    bool Set(const String &name, const G_Texture *texture) const;

    void Refresh();
    virtual void SetVertexShader(G_Shader *vertexShader);
    virtual void SetFragmentShader(G_Shader *fragmentShader);

    void SetVertexInputBinding(const String& inputName, uint location);
    void SetFragmentInputBinding(const String& inputName, uint location);

    G_Shader* GetVertexShader() const;
    G_Shader* GetFragmentShader() const;

    GLint GetUniformLocation(const String &name) const;
    GLint GetAttribLocation(const String &name) const;

protected:
    G_Shader *p_vshader = nullptr;
    G_Shader *p_fshader = nullptr;

    mutable Map<String, GLuint> m_nameToLocationCache;
    mutable Map<String, const G_Texture*> m_namesToTexture;

    bool BindTextureToAvailableUnit(const String &texName,
                                    const G_Texture *texture) const;
    void UpdateTextureBindings() const;

    virtual void OnPreLink();
};


#endif // G_SHADERPROGRAM_H
