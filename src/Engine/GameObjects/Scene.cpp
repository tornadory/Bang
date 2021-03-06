#include "Bang/Scene.h"

#include "Bang/Debug.h"
#include "Bang/Camera.h"
#include "Bang/Gizmos.h"
#include "Bang/XMLNode.h"
#include "Bang/GEngine.h"
#include "Bang/UICanvas.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/SceneManager.h"
#include "Bang/DebugRenderer.h"
#include "Bang/RectTransform.h"
#include "Bang/GameObjectFactory.h"

USING_NAMESPACE_BANG

Scene::Scene() : GameObject("Scene")
{
    m_gizmos = new Gizmos();
    m_localObjectManager = new ObjectManager();
    p_debugRenderer = GameObject::Create<DebugRenderer>();
}

Scene::~Scene()
{
    delete m_gizmos;
    delete m_behaviourManager;
    delete m_localObjectManager;
    GameObject::Destroy(p_debugRenderer);
}

void Scene::OnPreStart()
{
    m_behaviourManager = CreateBehaviourManager();
    GameObject::OnPreStart();
}

void Scene::Update()
{
    GameObject::Update();
}

void Scene::Render(RenderPass rp, bool renderChildren)
{
    GameObject::Render(rp, renderChildren);

    if (rp == RenderPass::Scene) { GetDebugRenderer()->Render(true); }
    else if (rp == RenderPass::Overlay) { GetDebugRenderer()->Render(false); }
}

void Scene::OnResize(int newWidth, int newHeight)
{
    InvalidateCanvas();
}

Gizmos *Scene::GetGizmos() const { return m_gizmos; }
DebugRenderer *Scene::GetDebugRenderer() const { return p_debugRenderer; }

BehaviourManager *Scene::CreateBehaviourManager() const
{
    return new BehaviourManager();
}

void Scene::SetCamera(Camera *cam)
{
    if (p_camera) { p_camera = nullptr; }

    p_camera = cam;
    if (p_camera)
    {
        p_camera->EventEmitter<IDestroyListener>::RegisterListener(this);
    }
}

void Scene::SetFirstFoundCamera()
{
    Camera *sceneCamera = GetComponentInChildren<Camera>();
    // if (!sceneCamera) { Debug_Warn("No camera found!"); }
    SetCamera(sceneCamera);
}

void Scene::InvalidateCanvas()
{
    List<UICanvas*> canvases = GetComponentsInChildren<UICanvas>(true);
    for (UICanvas *canvas : canvases) { canvas->Invalidate(); }
}

ObjectManager *Scene::GetLocalObjectManager() const
{
    return m_localObjectManager;
}

void Scene::OnDestroyed(EventEmitter<IDestroyListener> *object)
{
    if (object == GetCamera())
    {
        SetCamera(nullptr);
    }
}

Camera *Scene::GetCamera() const { return p_camera; }
BehaviourManager *Scene::GetBehaviourManager() const { return m_behaviourManager; }

void Scene::ImportXML(const XMLNode &xmlInfo)
{
    GameObject::ImportXML(xmlInfo);
    SetFirstFoundCamera();
}

void Scene::ExportXML(XMLNode *xmlInfo) const
{
    GameObject::ExportXML(xmlInfo);
    xmlInfo->SetTagName("Scene");
}

void Scene::PostImportXML(const XMLNode &xmlInfo)
{
    GameObject::PostImportXML(xmlInfo);
}
