#include "Bang/EditorFloor.h"

#include "Bang/Scene.h"
#include "Bang/Camera.h"
#include "Bang/Vector3.h"
#include "Bang/Material.h"
#include "Bang/Transform.h"
#include "Bang/PointLight.h"
#include "Bang/SceneManager.h"
#include "Bang/AssetsManager.h"
#include "Bang/SingleLineRenderer.h"

EditorFloor::EditorFloor()
{
    m_material = new Material(*AssetsManager::Load<Material>(
                                  "Materials/G_DefaultNoSP.bmat", true));
    m_material->SetDiffuseColor(Color::White * 0.7f);

    const int GridSize2 = 2 * (c_gridSize + 1);
    m_lineRenderers.Resize(GridSize2);
    for (int i = 0; i < GridSize2; ++i)
    {
        m_lineRenderers[i] = AddComponent<SingleLineRenderer>();
        m_lineRenderers[i]->SetMaterial(m_material);
        //m_lineRenderers[i]->SetEnabled(false);
    }

    GetHideFlags()->SetOn(HideFlag::HideAndDontSave);
}

EditorFloor::~EditorFloor()
{
    if (m_material) { delete m_material; }
}

void EditorFloor::OnEditorUpdate()
{
    GameObject::OnEditorUpdate();

    Camera *cam  = SceneManager::GetActiveScene()->GetCamera();
    Transform *camt = cam->gameObject->transform;

    const float LineSize2 = (c_gridSize * c_tileSize) / 2.0f; // Half line length

    int indexOffsetX = int(camt->GetPosition().x / c_tileSize);
    int indexOffsetZ = int(camt->GetPosition().z / c_tileSize);

    for (int i = 0; i <= c_gridSize; ++i)
    {
        SingleLineRenderer *lr = m_lineRenderers[i];

        int j = i - c_gridSize / 2;   // [ -GridSize/2, GridSize/2 ]
        j += indexOffsetX;

        Vector3 center = Vector3(c_tileSize * j, 0, c_tileSize * indexOffsetZ);

        lr->SetOrigin ( Vector3(0, 0, -LineSize2) + center );
        lr->SetDestiny( Vector3(0, 0,  LineSize2) + center );
    }

    for (int i = 0; i <= c_gridSize; ++i)
    {
        SingleLineRenderer *lr = m_lineRenderers[i + c_gridSize + 1];

        int j = i - c_gridSize / 2;   // [ -GridSize/2, GridSize/2 ]
        j += indexOffsetZ;

        Vector3 center = Vector3(c_tileSize * indexOffsetX, 0, c_tileSize * j);

        lr->SetOrigin ( Vector3(-LineSize2, 0, 0) + center );
        lr->SetDestiny( Vector3( LineSize2, 0, 0) + center );
    }
}

#include "Bang/Hierarchy.h"
void EditorFloor::OnDrawGizmos(bool depthed, bool overlay)
{
    Scene *scene = SceneManager::GetActiveScene();

    Rect rect;
    GameObject *selected =
            Hierarchy::GetInstance()->GetFirstSelectedGameObject();
    if (selected)
    {
        AABox box;
        box = selected->GetAABBox(true);
        Gizmos::SetColor(Color::Red);
        //Gizmos::RenderSimpleBox(box);
        Gizmos::SetColor(Color::Green);
        rect = selected->GetBoundingScreenRect(scene->GetCamera(), true);
        //rect = box.GetAABoundingScreenRect(scene->GetCamera());
        //rect = Rect(-0.5f, 0.5f, -0.5f, 0.5f);
        //Gizmos::RenderRect(rect);
        //PointLight *pointLight = selected->GetComponent<PointLight>();
        //if (pointLight)
        //{
        //    rect = pointLight->GetRenderRect(scene->GetCamera());
            //Gizmos::RenderRect(rect);
        //}
    }

    //rect = scene->GetBoundingScreenRect(scene->GetCamera(), true);
    //Gizmos::RenderRect(rect);
}
