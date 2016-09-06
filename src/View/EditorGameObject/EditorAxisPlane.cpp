#include "EditorAxisPlane.h"

#include "EditorAxis.h"
#include "MeshRenderer.h"

EditorAxisPlane::EditorAxisPlane(EditorAxis *axis1, EditorAxis *axis2)
    : m_axis1(axis1), m_axis2(axis2)
{
    m_material = AssetsManager::LoadAsset<Material>("Assets/Engine/Materials/D2G_Line.bmat");
    m_material = new Material(*m_material);

    m_planeRenderer = AddComponent<MeshRenderer>();
    m_planeRenderer->SetMesh(MeshFactory::GetPlane());
    m_planeRenderer->SetMaterial(m_material);

    SetTransformAccordingToAxis();
}

EditorAxisPlane::~EditorAxisPlane()
{
    delete m_material;
}

void EditorAxisPlane::OnUpdate()
{
}

void EditorAxisPlane::SetTransformAccordingToAxis()
{
    NONULL(m_axis1); NONULL(m_axis2);
    Vector3 pos = Vector3::zero;
    Vector3 scale = Vector3::one;

    transform->SetLocalPosition(pos);
    transform->SetLocalScale(scale);
}