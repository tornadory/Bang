#include "Bang/BehaviourManager.h"

#include "Bang/Debug.h"
#include "Bang/Scene.h"
#include "Bang/String.h"
#include "Bang/Library.h"
#include "Bang/Behaviour.h"
#include "Bang/Application.h"
#include "Bang/SceneManager.h"

USING_NAMESPACE_BANG

BehaviourManager::BehaviourManager()
{
}

BehaviourManager::~BehaviourManager()
{
    if (m_behavioursLibrary) { delete m_behavioursLibrary; }
}

Behaviour *BehaviourManager::CreateBehaviourInstance(const String &behaviourName,
                                                     Library *behavioursLib)
{
    if (!behavioursLib) { return nullptr; }
    if (!behavioursLib->IsLoaded()) { behavioursLib->Load(); }

    String errorString = "";
    if (behavioursLib->IsLoaded())
    {
        // Get the pointer to the CreateDynamically function
        String funcName = "CreateDynamically_" + behaviourName;
        Behaviour* (*createFunction)(Application*) =
            behavioursLib->Get< Behaviour*(*)(Application*) >(funcName.ToCString());

        if (createFunction)
        {
            // Call it and get the pointer to the created Behaviour
            // Create the Behaviour, passing to it the Application
            // of this main binary, so it can link it.
            return createFunction(Application::GetInstance());
        }
        else { errorString = behavioursLib->GetErrorString(); }
    }
    else { errorString = behavioursLib->GetErrorString(); }

    Debug_Error(errorString);
    return nullptr;
}

bool BehaviourManager::DeleteBehaviourInstance(const String &behaviourName,
                                               Behaviour *behaviour,
                                               Library *behavioursLib)
{
    if (!behavioursLib)
    {
        delete behaviour;
        return false;
    }

    // Get the pointer to the DeleteDynamically function
    String funcName = "DeleteDinamically_" + behaviourName;
    void (*deleteFunction)(Behaviour*) =
            (behavioursLib->Get<void (*)(Behaviour*)>(funcName.ToCString()));

    if (deleteFunction)
    {
        deleteFunction(behaviour);
        return true;
    }

    Debug_Error(behavioursLib->GetErrorString());
    return false;
}

bool BehaviourManager::IsInstanceCreationAllowed() const
{
    return true;
}

Library *BehaviourManager::GetBehavioursLibrary() const
{
    return m_behavioursLibrary;
}

BehaviourManager *BehaviourManager::GetActive()
{
    return SceneManager::GetActiveScene()->GetBehaviourManager();
}

void BehaviourManager::SetBehavioursLibrary(const Path &libPath)
{
    Library *behavioursLib = new Library(libPath);
    behavioursLib->Load();
    SetBehavioursLibrary(behavioursLib);
}

void BehaviourManager::SetBehavioursLibrary(Library *behavioursLibrary)
{
    if (GetBehavioursLibrary()) { delete GetBehavioursLibrary(); }
    m_behavioursLibrary = behavioursLibrary;
}
