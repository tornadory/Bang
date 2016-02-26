#ifndef WINDOWEVENTMANAGER_H
#define WINDOWEVENTMANAGER_H

#include "Bang.h"

#include <GL/glew.h>

#include <vector>
#include <string>
#include <chrono>
#include <QTimer>
#include <QGLWidget>
#include <QApplication>
#include <QTreeWidgetItem>

#include "ListInspectorItemInfo.h"
#include "IWindowEventManagerListener.h"

#define PROPAGATE_EVENT_PAR(FUNCTION, ITERABLE) do {\
    for(auto it = (ITERABLE).begin(); it != (ITERABLE).end(); ++it ) \
    {\
        if((*it) != nullptr) (*it)->FUNCTION;\
    }\
} while(0)

class Part;
class Entity;
class WindowEventManager : public QGLWidget
{
    Q_OBJECT
private:
    static WindowEventManager *wem;
    std::vector<IWindowEventManagerListener*> listeners;

public:
    explicit WindowEventManager(QWidget *parent = 0);

    void AddListener(IWindowEventManagerListener *listener);

    static WindowEventManager *GetInstance();


public slots:

    static void NotifyChildAdded(Entity *child);
    static void NotifyChildChangedParent(Entity *child, Entity *previousParent);
    static void NotifyChildRemoved(Entity *child);

    static void NotifyMenuBarItemClicked(int itemClcked);
    void _NotifyMenuBarItemClicked(int itemClicked);

    static void NotifyInspectorSlotChanged(Part *updatedPart);
    static void NotifyHierarchyEntitiesSelected(const std::list<Entity*> &selectedEntities);
};

#endif // WINDOWEVENTMANAGER_H