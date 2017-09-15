#ifndef OBJECT_H
#define OBJECT_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

class Object
{
public:
    virtual ~Object();

    virtual void Start();
    virtual void OnStart();

    void SetEnabled(bool enabled);

    bool IsEnabled() const;
    bool IsStarted() const;

protected:
    Object();

private:
    bool m_enabled = true;
    bool m_started = false;
};

NAMESPACE_BANG_END

#endif // OBJECT_H