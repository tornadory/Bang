#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include "Bang/WinUndef.h"

#include "Bang/IWindow.h"

class G_Screen;
class GameWindow : public IWindow
{
private:
    static GameWindow *s_m_win;
    void SetupUI();

public:
    G_Screen *screen = nullptr;

    GameWindow(QMainWindow *window);

    static void InitFromMainBinary(QMainWindow *window, QApplication *application);

    static GameWindow *GetInstance();
    QMainWindow *GetMainWindow() const override;
    QApplication *GetApplication() const override;
};

#endif // GAMEWINDOW_H
