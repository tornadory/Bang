#ifndef COMPONENT_H
#define COMPONENT_H

#include "Bang.h"

#include "Logger.h"
#include "IFileable.h"
#include "IToString.h"
#include "ICloneable.h"
#include "FileWriter.h"
#include "ISceneEventListener.h"

#ifdef BANG_EDITOR
#include "IInspectable.h"
#include "InspectorWidget.h"
#include "IWindowEventManagerListener.h"
#endif

#define CAN_USE_COMPONENT(comp)  ( comp  && comp->IsEnabled() )

class GameObject;

class Component :
              public ISceneEventListener
             ,public IToString
             ,public IFileable
             ,public ICloneable
            #ifdef BANG_EDITOR
             ,public IWindowEventManagerListener
             ,public IInspectable
            #endif
{
friend class GameObject;
protected:
    bool m_enabled = true;

    Component();
    virtual ~Component();

public:

    GameObject *p_gameObject = nullptr;
    GameObject* const& gameObject = p_gameObject;

    virtual void CloneInto(ICloneable *clone) const override;
    virtual ICloneable *Clone() const override = 0;

    virtual const std::string ToString() const override;

    virtual std::string GetName() const { return "Component"; }

    void SetEnabled(bool enabled) { this->m_enabled = enabled; }
    bool IsEnabled() { return m_enabled; }

    virtual void Write(std::ostream &f) const override {}
    virtual void Read(std::istream &f) override {}
};

#endif // COMPONENT_H
