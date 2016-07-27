#include "Light.h"

#include "Transform.h"
#include "GameObject.h"

Light::Light()
{
    #ifdef BANG_EDITOR
    m_inspectorInfo.AddSlotInfos(
    {
        new InspectorVFloatSWInfo("Intensity", 1)
       ,new InspectorVFloatSWInfo("Color", 3)
    });
    #endif
}

void Light::SetUniformsBeforeApplyingLight() const
{
    ShaderProgram *sp = m_lightMaterial->GetShaderProgram();
    sp->SetUniformFloat("B_light_intensity",      m_intensity,                          false);
    sp->SetUniformVec3 ("B_light_color",          m_color,                              false);
    sp->SetUniformVec3 ("B_light_forward_world",  gameObject->transform->GetForward(),  false);
    sp->SetUniformVec3 ("B_light_position_world", gameObject->transform->GetPosition(), false);
}

void Light::ApplyLight(GBuffer *gbuffer) const
{
    SetUniformsBeforeApplyingLight();
    gbuffer->RenderPassWithMaterial(m_lightMaterial);
}

const std::string Light::ToString() const
{
    return "Light";
}

void Light::CloneInto(ICloneable *clone) const
{
    Component::CloneInto(clone);
    Light *l = static_cast<Light*>(clone);
    l->SetColor(GetColor());
}

ICloneable *Light::Clone() const
{
    Light *l = new Light();
    CloneInto(l);
    return l;
}

void Light::SetColor(const Vector3 &color)
{
    m_color = color;
}

void Light::SetIntensity(float intensity)
{
    m_intensity = intensity;
}

Vector3 Light::GetColor() const
{
    return m_color;
}

float Light::GetIntensity() const
{
    return m_intensity;
}

#ifdef BANG_EDITOR
InspectorWidgetInfo *Light::OnInspectorInfoNeeded()
{
    Component::OnInspectorInfoNeeded();

    m_inspectorInfo.GetSlotInfo("Intensity")->SetFloat(m_intensity);
    m_inspectorInfo.GetSlotInfo("Color")->SetVector3(m_color);

    return &m_inspectorInfo;
}

void Light::OnInspectorInfoChanged(InspectorWidgetInfo *info)
{
    m_intensity = info->GetFloat("Intensity");
    m_color = info->GetVector3("Color");
}
#endif

void Light::WriteInternal(std::ostream &f) const
{
    Component::WriteInternal(f);
    FileWriter::WriteFloat(m_intensity, f);
    FileWriter::WriteVector3(m_color, f);
    FileWriter::WriteFilepath(m_lightMaterial->GetFilepath(), f);
}

void Light::ReadInternal(std::istream &f)
{
    Component::ReadInternal(f);
    m_intensity = FileReader::ReadFloat(f);
    m_color = FileReader::ReadVec3(f);

    std::string materialFilepath = FileReader::ReadString(f);
    m_lightMaterial = AssetsManager::GetAsset<Material>(materialFilepath);

}
