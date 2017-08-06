#ifndef ISCENEEVENTLISTENER_H
#define ISCENEEVENTLISTENER_H

#include "Bang/List.h"

class SceneAgent
{
public:
    virtual void Start()
    {
        if (!IsStarted()) { OnStart(); m_started = true; }
    }
    virtual void Update() { OnUpdate(); }
    virtual void ParentSizeChanged() { OnParentSizeChanged(); }
    virtual void DrawGizmos() { OnDrawGizmos(); }
    virtual void Destroy() { OnDestroy(); }

    void SetEnabled(bool enabled) { m_enabled = enabled; }

    bool IsEnabled() const { return m_enabled; }
    bool IsStarted() const { return m_started; }

protected:
    SceneAgent() {}
    virtual ~SceneAgent() {}

    virtual void OnStart() {}
    virtual void OnUpdate() {}
    virtual void OnParentSizeChanged() {}
    virtual void OnDrawGizmos() {}
    virtual void OnDestroy() {}

private:
    bool m_enabled = true;
    bool m_started = false;
};


#define PROPAGATE_EVENT(FUNCTION, ITERABLE) do {\
    for (auto it = (ITERABLE).Begin(); it != (ITERABLE).End(); ++it )  \
        if ((*it)->IsEnabled()) { (*it)->FUNCTION; } \
} while (0)
#define PROPAGATE_EVENT_RAW(FUNCTION, ITERABLE) do {\
    for (auto it = (ITERABLE).Begin(); it != (ITERABLE).End(); ++it ) (*it)->FUNCTION; \
} while (0)

template<class T>
class SceneNode : public SceneAgent
{
public:
    T* const& parent = p_parent;

    virtual void Start() override
    {
        PROPAGATE_EVENT(Start(), GetChildren());
        SceneAgent::Start();
    }

    virtual void Update() override
    {
        PROPAGATE_EVENT(Update(), GetChildren());
        SceneAgent::Update();
    }

    virtual void ParentSizeChanged() override
    {
        PROPAGATE_EVENT(ParentSizeChanged(), GetChildren());
        SceneAgent::ParentSizeChanged();
    }

    virtual void DrawGizmos()  override
    {
        PROPAGATE_EVENT(DrawGizmos(), GetChildren());
        SceneAgent::DrawGizmos();
    }

    virtual void Destroy() override
    {
        PROPAGATE_EVENT(Destroy(), GetChildren());
        SceneAgent::Destroy();
    }

    const List<T*>& GetChildren() const
    {
        return m_children;
    }

    T* GetChild(int index) const
    {
        auto it = GetChildren().Begin(); std::advance(it, index);
        return *it;
    }

    List<T*> GetChildrenRecursively() const
    {
        List<T*> cc;
        for (T *c : GetChildren())
        {
            cc.PushBack(c);
            List<T*> childChildren = c->GetChildrenRecursively();
            cc.Splice(cc.Begin(), childChildren);
        }
        return cc;
    }

    bool IsChildOf(const T *_parent, bool recursive = true) const
    {
        if (!parent) { return false; }

        if (recursive)
        {
            return parent == _parent || parent->IsChildOf(_parent);
        }
        return parent == _parent;
    }

    void SetParent(SceneNode<T> *newParent, int _index = -1)
    {
        if (parent) { parent->m_children.Remove( SCAST<T*>(this) ); }

        p_parent = SCAST<T*>(newParent);
        if (parent)
        {
            int index = (_index != -1 ? _index : parent->GetChildren().Size());
            p_parent->m_children.Insert(index, SCAST<T*>(this));
            SceneNode<T>::ParentSizeChanged();
        }
    }

    T* GetParent() { return p_parent; }
    const T* GetParent() const { return p_parent; }

protected:
    SceneNode () {}
    virtual ~SceneNode() {}

private:
    List<T*> m_children;
    T* p_parent = nullptr;
};

#endif // SCENEEVENTLISTENER_H
