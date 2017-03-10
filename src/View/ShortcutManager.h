#ifndef SHORTCUTMANAGER_H
#define SHORTCUTMANAGER_H

#include "Set.h"
#include "List.h"
#include "Array.h"
#include "Input.h"

class IShortcutListener;
class ShortcutManager
{
friend class Application;
friend class IShortcutListener;

private:
    List<IShortcutListener*> m_shortcutsListeners;
    Set<Input::Key> m_pressedKeys;

    ShortcutManager();

    void OnKeyPressed(Input::Key key);
    void OnKeyReleased(Input::Key key);

    void RegisterListener(IShortcutListener *list);
    void UnregisterListener(IShortcutListener *list);

public:

    static void InitFromMainBinary();
    static ShortcutManager *GetInstance();

    static bool IsPressed(Input::Key key);
    static bool IsPressed(const Array<Input::Key> &keys);

    void Clear();
};

#endif // SHORTCUTMANAGER_H
