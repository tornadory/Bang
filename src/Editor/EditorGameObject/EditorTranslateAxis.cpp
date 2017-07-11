#include "Bang/EditorTranslateAxis.h"

#include "Bang/Debug.h"
#include "Bang/Scene.h"
#include "Bang/Input.h"
#include "Bang/Camera.h"
#include "Bang/Matrix4.h"
#include "Bang/Material.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/EditorState.h"
#include "Bang/MeshFactory.h"
#include "Bang/MeshRenderer.h"
#include "Bang/SceneManager.h"
#include "Bang/GraphicPipeline.h"
#include "Bang/SingleLineRenderer.h"
#include "Bang/SelectionFramebuffer.h"

EditorTranslateAxis::EditorTranslateAxis(EditorAxis::EditorAxisDirection dir,
                                         GameObject *attachedGameObject)
    : EditorAxis(dir, attachedGameObject)
{
    SetName("EditorTranslateAxis" + EditorAxis::GetStringFromDir(dir));

    m_line = AddComponent<SingleLineRenderer>();
    m_line->SetDestiny(m_oAxisDirection);
    m_line->SetMaterial(m_material);
    m_line->SetLineWidth(2.0f);
    m_line->SetRenderLayer(Renderer::RenderLayer::Gizmos);

    m_axisCap = MeshFactory::GetConeGameObject();
    m_axisCap->GetHideFlags()->SetOn(HideFlag::HideAndDontSave);
    m_axisCap->SetParent(this);
    if (dir == EditorAxis::EditorAxisDirection::X)
    {
        m_axisCap->transform->SetLocalEuler(Vector3(90.0f, 90.0f, 0.0f));
    }
    else if (dir == EditorAxis::EditorAxisDirection::Y)
    {
        m_axisCap->transform->SetLocalEuler(Vector3(0.0f, 0.0f, 0.0f));
    }
    else if (dir == EditorAxis::EditorAxisDirection::Z)
    {
        m_axisCap->transform->SetLocalEuler(Vector3(-90.0f, 0.0f, 0.0f));
    }
    m_axisCap->transform->SetLocalPosition(m_oAxisDirection);
    m_axisCap->transform->SetLocalScale(Vector3(1,2,1) * 0.25f);
    m_axisCap->GetComponent<MeshRenderer>()->SetMaterial(m_material);
    m_axisCap->GetComponent<MeshRenderer>()->
            SetRenderLayer(Renderer::RenderLayer::Gizmos);

    GetHideFlags()->SetOn(HideFlag::HideAndDontSave);
}

EditorTranslateAxis::~EditorTranslateAxis()
{
}


void EditorTranslateAxis::OnEditorUpdate()
{
    EditorAxis::OnEditorUpdate();

    Camera *cam = SceneManager::GetActiveScene()->GetCamera(); ENSURE(cam);
    Transform *camTransform = cam->gameObject->transform;
    GameObject *ago = m_attachedGameObject;
    Vector3 wCamPos = camTransform->GetPosition();

    SetEnabled(EditorState::GetCurrentTransformMode() ==
               EditorState::TransformMode::Translate);
    if (m_grabbed)
    {
        Vector2 sMouseDelta = Input::GetMouseDelta() * Vector2(1.0f, -1.0f);
        if (sMouseDelta.Length() > 0.0f)
        {
            Vector3 wAxisDir, parentAxisDir;
            if (EditorState::IsUsingGlobalCoords())
            {
                wAxisDir = m_oAxisDirection;
                if (ago->parent)
                {
                    parentAxisDir = ago->parent->transform->
                            WorldToLocalVector(m_oAxisDirection);
                }
            }
            else
            {
                wAxisDir = ago->transform->LocalToWorldVector(m_oAxisDirection);
                parentAxisDir = ago->transform->LocalToParentDirection(m_oAxisDirection);
            }
            wAxisDir.Normalize();
            parentAxisDir.Normalize();

            // Alignment
            Vector3 wAxisCenter = transform->GetPosition();
            Vector2 screenAxisDir =
                    cam->WorldToScreenNDCPoint(wAxisCenter + wAxisDir) -
                    cam->WorldToScreenNDCPoint(wAxisCenter);
            screenAxisDir.Normalize();
            float alignment = Vector2::Dot(screenAxisDir,
                                           sMouseDelta.Normalized());
            //

            Vector3 worldMove = alignment *
                                parentAxisDir *
                                sMouseDelta.Length() *
                                Vector3::Distance(wCamPos,
                                                  ago->transform->GetPosition())
                                                    * 0.002f;
            worldMove *= 1.0f / ago->parent->transform->GetScale();
            ago->transform->TranslateLocal(worldMove);
        }
    }
}

void EditorTranslateAxis::OnDrawGizmos(bool depthed, bool overlay)
{
    EditorAxis::OnDrawGizmos(depthed, overlay);

    // Bigger grab for selecting
    GraphicPipeline *gp = GraphicPipeline::GetActive();
    bool selectionPass = gp->GetSelectionFramebuffer()->IsPassing();
    m_line->SetLineWidth(!selectionPass ? 2.0f : 25.0f);

    if (overlay && !depthed)
    {
        Gizmos::Render(m_line);
        Gizmos::Render(m_axisCap->GetComponent<Renderer>());
    }
}

Renderer *EditorTranslateAxis::GetAxisRenderer() const
{
    return m_line;
}
