#include "EditorScaleAxis.h"

#include "Toolbar.h"

EditorScaleAxis::EditorScaleAxis(EditorAxis::EditorAxisDirection dir,
                                 GameObject *attachedGameObject)
    : EditorAxis(dir, attachedGameObject)
{
    SetName("EditorScaleAxisGroup" + EditorAxis::GetStringFromDir(dir));

    line = AddComponent<SingleLineRenderer>();
    line->SetDestiny(oAxisDirection);
    line->SetMaterial(material);
    line->SetLineWidth(2.0f);

    line->SetActivateGLStatesBeforeRenderingForSelectionFunction([]()
        {
            glLineWidth(25.0f);
        }
    );
}

EditorScaleAxis::~EditorScaleAxis()
{

}


void EditorScaleAxis::OnUpdate()
{
    EditorAxis::OnUpdate();

    Camera *cam = Canvas::GetInstance()->GetCurrentScene()->GetCamera(); NONULL(cam);
    Transform *camTransform = cam->gameObject->GetComponent<Transform>(); NONULL(camTransform);
    Transform *attTrans = attachedGameObject->GetComponent<Transform>(); NONULL(attTrans);
    Transform *transform = GetComponent<Transform>(); NONULL(transform);
    Vector3 wCamPos = camTransform->GetPosition();

    if (grabbed)
    {
        glm::vec2 sMouseDelta = Input::GetMouseDelta() * glm::vec2(1.0f, -1.0f);
        if (glm::length(sMouseDelta) > 0.0f)
        {
            Vector3 oAxisDir, wAxisDir;
            if (Toolbar::GetInstance()->GetGlobalCoordsMode())
            {
                oAxisDir = attTrans->WorldToLocalDirection(oAxisDirection);
                wAxisDir = oAxisDirection;
            }
            else
            {
                oAxisDir = oAxisDirection;
                wAxisDir = transform->LocalToWorldDirection(oAxisDirection);
            }
            oAxisDir.z *= -1; oAxisDir.Normalize();
            wAxisDir.Normalize();

            // Alignment
            Vector3 wAxisCenter = transform->GetPosition();
            glm::vec2 screenAxisDir = cam->WorldToScreenNDCPoint(wAxisCenter + wAxisDir) -
                                      cam->WorldToScreenNDCPoint(wAxisCenter);
            screenAxisDir = glm::normalize(screenAxisDir);
            float alignment = glm::dot(screenAxisDir, glm::normalize(sMouseDelta));
            //


            Vector3 scaling = Vector3::one + alignment * oAxisDir *
                              glm::length(sMouseDelta) *
                              Vector3::Distance(wCamPos, attTrans->GetPosition()) * 0.001f;

            //TODO: solve problem with negative scaling and depth :/
            attTrans->SetScale(attTrans->GetScale() * scaling);
        }
    }
}

Renderer *EditorScaleAxis::GetAxisRenderer() const
{
    return line;
}
