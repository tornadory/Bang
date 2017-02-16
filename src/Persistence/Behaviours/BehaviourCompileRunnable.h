#ifndef BEHAVIOURCOMPILERUNNABLE_H
#define BEHAVIOURCOMPILERUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "String.h"

class BehaviourManager;

class BehaviourCompileRunnable : public QObject,
                                 public QRunnable
{
    Q_OBJECT

public:
    BehaviourCompileRunnable();
    BehaviourCompileRunnable(const String &behaviourFilepath);
    void Compile();

protected:
    String m_behaviourFilepath = "";
    void run() override;

signals:
    void NotifySuccessCompiling(const QString &behaviourFilepath,
                                const QString &libFilepath,
                                const QString &warnMessage);
    void NotifyFailedCompiling(const QString &behaviourFilepath,
                               const QString &errorMessage);
};

#endif // BEHAVIOURCOMPILERUNNABLE_H
