#include <vector>

#include <iostream>

#include <QTextStream>

#include "glm/glm.hpp"

#include "Bang.h"

#include "Timer.h"
#include "Debug.h"
#include "Camera.h"
#include "Screen.h"
#include "Shader.h"
#include "XMLNode.h"
#include "Behaviour.h"
#include "Texture2D.h"
#include "XMLParser.h"
#include "GameObject.h"
#include "FileReader.h"
#include "Application.h"
#include "Framebuffer.h"
#include "SceneManager.h"
#include "MeshRenderer.h"
#include "ShaderProgram.h"
#include "SingletonManager.h"

#ifdef BANG_EDITOR

#include "WindowMain.h"
#include "EditorScene.h"
#include "ShortcutManager.h"
#include "WindowMainAutoGenerated.h"

#else

#include "GameWindow.h"

#endif

#include "String.h"

int main(int argc, char *argv[])
{
    String str = "vector: " + Vector2(1,2) + ", " + Vector3(1,2,3) + ", " + Vector4(1,2,3,4);
    Debug_Log(str);
    str = "quat: " + Quaternion(1,2,3,4);
    Debug_Warn(str);
    Matrix4 m;
    str = "mat: " + m;
    Debug_Error(str);
    Color c = Color::blue;
    Debug_Log("wololo: " << c << ", " << Vector2(0,1) << "kjashdas");

    SingletonManager::InitSingletonManagerFromMainBinary();
    ShortcutManager::InitFromMainBinary(); // It must go before Application init

    Application app(argc, argv);
    QMainWindow *window = new QMainWindow();

    #ifdef BANG_EDITOR

    QFont font;
    font.setFamily(font.defaultFamily());
    font.setPixelSize(10);
    app.setFont(font);

    WindowMain::InitFromMainBinary(window, &app);

    #else

    GameWindow::InitFromMainBinary(window, &app);

    #endif

    Screen::InitFromMainBinary();
    Time::InitFromMainBinary();
    Input::InitFromMainBinary();

    #ifdef BANG_EDITOR

    // Load dark Window theme
    QFile f("Assets/Engine/qdarkstyle/style.qss");
    if (!f.exists())
    {
        Debug_Error("Unable to set dark stylesheet, stylesheet not found.");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        app.setStyleSheet(ts.readAll());
    }
    //

    Scene *scene = new EditorScene();
    String filename = "./Assets/Scenes/test.bscene";
    FileReader::ReadScene(filename, scene);
    if (scene)
    {
        SceneManager::AddScene(scene);
        SceneManager::SetActiveScene(scene);
        Persistence::SetActiveSceneFilepath(filename);
    }

    #else

    Scene *scene = new Scene();
    String filename = "./Assets/Scenes/test.bscene";
    FileReader::ReadScene(filename, scene);
    if (scene)
    {
        SceneManager::AddScene(scene);
        SceneManager::SetActiveScene(scene);
        Persistence::SetActiveSceneFilepath(filename);
    }
    #endif

    return app.exec();
}
