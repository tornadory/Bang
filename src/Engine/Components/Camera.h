#ifndef CAMERA_H
#define CAMERA_H

#include "Bang/Color.h"
#include "Bang/Component.h"

FORWARD   class Mesh;
FORWARD   class AABox;
FORWARD_T class RectG;
FORWARD   class XMLNode;
FORWARD   class G_Screen;
FORWARD_T class Vector2G;
FORWARD_T class Vector3G;
FORWARD_T class Matrix4G;
FORWARD   class ICloneable;
FORWARD   class MeshRenderer;
FORWARD   class ShaderProgram;

class Camera : public Component
{
    OBJECT(Camera)

public:
    enum ProjectionMode
    {
        Orthographic,
        Perspective
    };

    void SetReplacementShaderProgram(ShaderProgram *replacementShaderProgram);
    ShaderProgram *GetReplacementShaderProgram() const;

    void GetViewMatrix(Matrix4G<float> *view) const;
    void GetProjectionMatrix(Matrix4G<float> *proj) const;

    void SetOrthoHeight(float orthoHeight);

    void SetClearColor(const Color& color);
    void SetFovDegrees(float fovDegrees);
    void SetZNear(float zNear);
    void SetZFar(float zFar);

    void SetProjectionMode(ProjectionMode projMode);

    virtual void Bind() const;

    const Color& GetClearColor() const;
    float GetOrthoWidth() const;
    float GetOrthoHeight() const;
    float GetFovDegrees() const;
    float GetZNear() const;
    float GetZFar() const;
    ProjectionMode GetProjectionMode() const;

    /**
     * @brief Converts a point from world coods to Screen NDC.
     *        NDC: Normalized Device Coordinates [(-1,-1) -> (1,1)]
    **/
    Vector2G<float> WorldToScreenNDCPoint(const Vector3G<float> &position);
    Vector3G<float> ScreenNDCPointToWorld(const Vector2G<float> &screenNDCPos,
                                          float zFromCamera);

    virtual void CloneInto(ICloneable *clone) const override;

    RectG<float> GetScreenBoundingRect(const AABox &bbox);

    virtual void Read(const XMLNode &xmlInfo) override;
    virtual void Write(XMLNode *xmlInfo) const override;

    void SetIdentityMode(bool identityMode);
    bool IsInIdentityMode() const;

protected:
    Camera();
    virtual ~Camera();

private:
    ShaderProgram *m_replacementShaderProgram = nullptr;
    float m_orthoHeight  = 25.0f;
    bool m_identityMode = false;
    Mesh *p_camMesh = nullptr;

    Color m_clearColor = Color(Color(0.3f), 1);
    float m_fovDegrees = 60.0f;
    float m_zNear = 0.1f;
    float m_zFar = 100.0f;

    ProjectionMode m_projMode = ProjectionMode::Perspective;
};

#endif // CAMERA_H
