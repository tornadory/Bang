#include "EditorAxisGroup.h"

EditorAxisGroup::EditorAxisGroup() : EditorGameObject("EditorAxisGroup")
{
    etag = new EditorTranslateAxisGroup();
    erag = new EditorRotateAxisGroup();
    esag = new EditorScaleAxisGroup();

    AddChild(etag);
    AddChild(erag);
    AddChild(esag);

    DisableAllGroups();

    AddComponent<Transform>();
}

void EditorAxisGroup::DisableAllGroups()
{
    etag->SetEnabled(false);
    erag->SetEnabled(false);
    esag->SetEnabled(false);
}

void EditorAxisGroup::OnUpdate()
{
    Toolbar::TransformMode tm = Toolbar::GetInstance()->
                                GetSelectedTransformMode();

    if (tm == Toolbar::TransformMode::Translate)
    {
        DisableAllGroups();
        etag->SetEnabled(true);
    }
    else if (tm == Toolbar::TransformMode::Rotate)
    {
        DisableAllGroups();
        erag->SetEnabled(true);
    }
    else if (tm == Toolbar::TransformMode::Scale)
    {
        DisableAllGroups();
        esag->SetEnabled(true);
    }

    //Attached GameObject positioning and scaling
    Camera *cam = Canvas::GetInstance()->GetCurrentScene()->GetCamera();
    GameObject *attGameObject = GetAttachedGameObject();
    if (attGameObject)
    {
        Transform *st = attGameObject->GetComponent<Transform>();
        if (st)
        {
            Transform *t = GetComponent<Transform>();
            Box bbox = attGameObject->GetObjectBoundingBox();

            t->SetLocalPosition( bbox.GetCenter() );

            if (Toolbar::GetInstance()->GetGlobalCoordsMode())
            {
                t->SetLocalRotation( st->GetLocalRotation().Inversed() );
            }
            else
            {
                t->SetLocalRotation( Quaternion() );
            }

            Vector3 camPos = cam->GetOwner()->GetComponent<Transform>()->GetPosition();
            Vector3 attPos = attGameObject->GetComponent<Transform>()->GetPosition();
            float distanceToCamera = Vector3::Distance(camPos, attPos);

            t->SetLocalScale((1.0f / st->GetScale()) * distanceToCamera * sizeBoost);
        }
    }
}


