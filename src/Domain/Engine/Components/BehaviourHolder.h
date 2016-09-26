#ifndef BEHAVIOURHOLDER_H
#define BEHAVIOURHOLDER_H

#include <functional>

#include <QThread>
#include <QLibrary>
#include <QInputDialog>

#include "Bang.h"
#include "Component.h"
#include "IAttrWidgetButtonListener.h"

#include "Behaviour.h"
#include "SystemUtils.h"

/**
 * @brief The BehaviourHolder class is the responsible of holding
 * a UNIQUE dynamically loaded Behaviour, loading them, and passing its events to them,
 * in case they do exist.
 *
 * A GameObject actually contains a Component that is a BehaviourHolder.
 * In case the BehaviourHolder has a Behaviour correctly loaded, it will
 * pass the events to the behaviour. Otherwise, it will just do nothing.
 */
class BehaviourHolder : public Component
                       ,public IAttrWidgetButtonListener
{
private:

    /**
     * @brief The dynamically loaded Behaviour
     */
    Behaviour *m_behaviour = nullptr;

    /**
     * @brief Path to the Behaviour source file.
     * It is indicated using the Inspector
     */
    String m_sourceFilepath = "";

    void ChangeBehaviour(Behaviour *newBehaviour);

protected:
    BehaviourHolder();

public:
    virtual ~BehaviourHolder();

    virtual const String ToString() const override;
    virtual String GetName() const override;

    virtual void CloneInto(ICloneable *clone) const override;
    virtual ICloneable *Clone() const override;

    const String &GetSourceFilepath() const;

    #ifdef BANG_EDITOR
    virtual void OnInspectorXMLNeeded(XMLNode *xmlInfo) const override;
    virtual void OnInspectorXMLChanged(const XMLNode *xmlInfo) override;
    #endif

    void OnButtonClicked(const String &attrName) override;

    virtual void OnAddedToGameObject() override;
    void Refresh();

    virtual void ReadXMLInfo(const XMLNode *xmlInfo) override;
    virtual void FillXMLInfo(XMLNode *xmlInfo) const override;

    virtual void _OnStart () override;
    virtual void _OnUpdate () override;
    virtual void _OnDestroy () override;

    // They will only override if they are in BANG_EDITOR
    virtual void _OnDrawGizmos ();
    virtual void _OnDrawGizmosOverlay ();

    void OnBehaviourLibraryAvailable(QLibrary *lib);

    // TODO: Add windowEventListener events pass to Behaviour too

    friend class GameObject;
};

#endif
