#include "Bang/Selection.h"

#include "Bang/Scene.h"
#include "Bang/Input.h"
#include "Bang/Camera.h"
#include "Bang/GEngine.h"
#include "Bang/SceneManager.h"
#include "Bang/SelectionFramebuffer.h"

USING_NAMESPACE_BANG

GameObject *Selection::GetOveredGameObject()
{
    if (!Input::IsMouseInsideScreen()) { return nullptr; }

    Scene *scene = SceneManager::GetRootScene();
    List<Camera*> sceneCameras = scene->GetComponentsInChildren<Camera>();
    for (Camera *cam : sceneCameras)
    {
        Vector2i coords = Input::GetMouseCoords();
        SelectionFramebuffer *sfb = cam->GetSelectionFramebuffer();
        if (sfb)
        {
            GameObject *selGo = sfb->GetGameObjectInPosition(coords);
            if (selGo) { return selGo; }
        }
    }
    return nullptr;
}
