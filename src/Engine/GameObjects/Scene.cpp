#include "Bang/Scene.h"

#include "Bang/Debug.h"
#include "Bang/Camera.h"
#include "Bang/Gizmos.h"
#include "Bang/XMLNode.h"
#include "Bang/Transform.h"
#include "Bang/GameObject.h"
#include "Bang/SceneManager.h"
#include "Bang/GameObjectFactory.h"

Scene::Scene() : GameObject("Scene")
{
    m_gizmos = new Gizmos();
}

Scene::~Scene()
{
}

void Scene::RenderGizmos()
{
    GameObject::RenderGizmos();
    // GetGizmos()->m_gizmosGo->RenderGizmos();
}

void Scene::_OnResize(int newWidth, int newHeight)
{
    ParentSizeChanged();
}

Gizmos *Scene::GetGizmos() const { return m_gizmos; }

void Scene::SetCamera(Camera *cam)
{
    if (!cam)
    {
        p_camera = nullptr;
        SetCamera(m_defaultCamera->GetComponent<Camera>());
    }
    else
    {
        p_camera = cam;
    }
}

void Scene::SetFirstFoundCameraOrDefaultOne()
{
    List<Camera*> cameras = GetComponentsInChildren<Camera>();
    bool cameraFound = false;
    for (Camera *cam : cameras)
    {
        SetCamera(cam);
        cameraFound = true;
        break;
    }

    if (!cameraFound) // Create default camera
    {
        Debug_Warn("No camera was found. Creating default camera...");
        m_defaultCamera = GameObjectFactory::CreateGameObject(true);
        m_defaultCamera->SetName("DefaultCamera");
        m_defaultCamera->transform->SetPosition(Vector3(90));
        m_defaultCamera->transform->LookAt(Vector3::Zero);
        m_defaultCamera->SetParent(this);
        m_defaultCamera->GetHideFlags().SetOn(HideFlag::DontSave);

        Camera *cam = m_defaultCamera->AddComponent<Camera>();
        cam->SetFovDegrees(60.0f); cam->SetZNear(0.1f);
        cam->SetZFar(99999.0f);
        SetCamera(cam);
    }
}

void Scene::Destroy(GameObject *gameObject)
{
    m_gameObjectsToBeDestroyed.push(gameObject);
}

void Scene::DestroyImmediate(GameObject *gameObject)
{
    gameObject->Destroy();
    delete gameObject;
}

void Scene::DestroyQueuedGameObjects()
{
    while (!m_gameObjectsToBeDestroyed.empty())
    {
        GameObject *go = m_gameObjectsToBeDestroyed.front();
        DestroyImmediate(go);
        m_gameObjectsToBeDestroyed.pop();
    }
}


Scene *Scene::GetActiveScene() { return SceneManager::GetActiveScene(); }
Camera *Scene::GetCamera() const { return p_camera; }

void Scene::Read(const XMLNode &xmlInfo)
{
    GameObject::Read(xmlInfo);
}

void Scene::Write(XMLNode *xmlInfo) const
{
    GameObject::Write(xmlInfo);
    xmlInfo->SetTagName("Scene");
}

void Scene::PostRead(const XMLNode &xmlInfo)
{
    GameObject::PostRead(xmlInfo);
}
