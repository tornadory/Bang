#include "Bang/GameBuilder.h"

#include "Bang/Paths.h"
#include "Bang/Debug.h"
#include "Bang/Scene.h"
#include "Bang/Dialog.h"
#include "Bang/Project.h"
#include "Bang/SystemUtils.h"
#include "Bang/EditorWindow.h"
#include "Bang/ProjectManager.h"
#include "Bang/GameBuildDialog.h"
#include "Bang/BehaviourManager.h"

GameBuilder *GameBuilder::s_instance = nullptr;

GameBuilder::GameBuilder()
{
    QObject::connect(this, SIGNAL(DialogError(const QString&, const QString&)),
                     SLOT(OnDialogError(const QString&, const QString&)));
}

GameBuilder::~GameBuilder()
{
}

GameBuilder *GameBuilder::GetInstance()
{
    if (!GameBuilder::s_instance)
    {
        GameBuilder::s_instance = new GameBuilder();
    }
    return GameBuilder::s_instance;
}

Path GameBuilder::AskForExecutableFilepath()
{
    // Get the executable output filepath
    const Path defaultOutputDirectory = Paths::Project();
    const String projectName = ProjectManager::GetCurrentProject()->GetProjectName();
    Path executableFilepath =
        Dialog::GetSaveFilepath(
                "Choose the file where you want to create your game",
                "exe",
                defaultOutputDirectory,
                projectName + ".exe");

    if (!executableFilepath.IsFile())
    {
        executableFilepath = executableFilepath.AppendExtension("exe");
    }
    return executableFilepath;
}

void GameBuilder::BuildGame(bool runGame)
{
    if (m_gameBuilderThread && m_gameBuilderThread->isRunning())
    {
        Debug_Status("Game is already being built", 5.0f);
    }
    else
    {
        m_runGameAfterBuild = runGame;

        // First ask for the executable output file (in the main thread)
        m_latestGameExecutableFilepath = AskForExecutableFilepath();
        ENSURE(!m_latestGameExecutableFilepath.IsEmpty());

        // Create the progress window
        if (m_gameBuildDialog) { delete m_gameBuildDialog; }
        m_gameBuildDialog = new GameBuildDialog();

        // Create the game builder job and connect stuff
        if (m_gameBuilderJob) { delete m_gameBuilderJob; }
        m_gameBuilderJob = new GameBuilderJob();
        m_gameBuilderJob->m_executableFilepath = m_latestGameExecutableFilepath;

        // Create and start the building thread
        if (m_gameBuilderThread)
        {
            m_gameBuilderThread->exit(0);
            delete m_gameBuilderThread;
        }
        m_gameBuilderThread = new QThread();

        // Connect stuff
        QObject::connect(m_gameBuilderJob, SIGNAL(NotifyPercent(float)),
                         m_gameBuildDialog, SLOT(SetPercent(float)));
        QObject::connect(m_gameBuilderJob, SIGNAL(NotifyMessage(const QString&)),
                         m_gameBuildDialog, SLOT(SetMessage(const QString&)));
        QObject::connect(m_gameBuilderThread, SIGNAL(started()),
                         m_gameBuilderJob, SLOT(BuildGame()) );
        QObject::connect(m_gameBuilderJob, SIGNAL(NotifyGameHasBeenBuilt()),
                         this, SLOT(OnGameHasBeenBuilt()));
        QObject::connect(m_gameBuilderJob,
                         SIGNAL(NotifyGameBuildingHasFailed(const QString&)),
                         this, SLOT(OnGameBuildingHasFailed(const QString&)));
        QObject::connect(m_gameBuildDialog, SIGNAL(canceled()),
                         this, SLOT(OnGameBuildingCanceled()));

        // Start the thread
        m_gameBuilderJob->moveToThread(m_gameBuilderThread);
        m_gameBuilderThread->start();
    }
}

void GameBuilder::OnGameHasBeenBuilt()
{
    if (m_gameBuildDialog)
    {
        m_gameBuildDialog->finished(0);
        m_gameBuildDialog->Destroy();
    }
    m_gameBuilderThread->exit(0);

    if (m_runGameAfterBuild)
    {
        SystemUtils::SystemBackground(
                    m_latestGameExecutableFilepath.GetAbsolute(), {});
    }
}

void GameBuilder::OnGameBuildingHasFailed(const QString &errorMsg)
{
    Debug_Error(errorMsg);
    OnDialogError("Error building game",
                  "The game could not be built.\n"
                  "Look the Console errors for more information.");
}

void GameBuilder::OnDialogError(const QString &title, const QString &msg)
{
    Dialog::Error(title, msg);
    if (m_gameBuildDialog)
    {
        m_gameBuildDialog->finished(1);
        m_gameBuildDialog->Destroy();
    }
    m_gameBuilderThread->exit(0);
}

void GameBuilder::OnGameBuildingHasBeenCanceled()
{
    m_gameBuilderJob->OnGameBuildingCanceled();
    if (m_gameBuildDialog)
    {
        m_gameBuildDialog->finished(2);
        m_gameBuildDialog->Destroy();
    }
    m_gameBuilderThread->exit(0);
}

bool GameBuilder::CompileGameExecutable()
{
    List<Path> sceneFiles = Paths::ProjectAssets().GetFiles(
                              true, {"*." + Scene::GetFileExtensionStatic()});
    if (sceneFiles.IsEmpty())
    {
        emit DialogError("Error building game",
                         "Please save at least one scene in the \
                          Assets directory to build the game");
        return false;
    }

    const Path initialOutputDir = Paths::Engine().Append("bin/Game")
                                                 .AppendExtension("exe");
    File::Remove(initialOutputDir);

    bool ok = false;
    String output = "";
    String cmd = Paths::Engine() + "/scripts/compile.sh";
    SystemUtils::System(cmd.ToCString(), {"GAME", "RELEASE_MODE"},
                        &output, &ok);
    ok = ok && initialOutputDir.IsFile();
    if (!ok)
    {
        Debug_Error(output);
        return false;
    }
    return true;
}

bool GameBuilder::CreateDataDirectory(const Path &executableDir)
{
    Path dataDir = executableDir.Append("GameData");
    File::Remove(dataDir);
    if (!File::CreateDirectory(dataDir)) { return false; }

    // Copy the Engine Assets in the GameData directory
    if (!File::DuplicateDir(Paths::EngineAssets(),
                            dataDir.Append("EngineAssets")))
    {
        return false;
    }

    // Copy the Project Assets in the GameData directory
    if (!File::DuplicateDir(Paths::ProjectAssets(), dataDir.Append("Assets")))
    {
        return false;
    }

    return true;
}

Project* GameBuilder::CreateGameProject(const Path &executableDir)
{
    String projectFile = executableDir + "/GameData/Game.bproject";
    return ProjectManager::CreateNewProjectFileOnly( Path(projectFile ) );
}

bool GameBuilder::CompileBehaviours(const Path &executableDir,
                                    Project *gameProject,
                                    bool *cancel)
{
    if (*cancel) { return true; }

    Path libsDir = Path(executableDir).Append("GameData").Append("Libraries");
    BehaviourManager::SetCurrentLibsDir(libsDir);
    File::CreateDirectory(libsDir);
    bool success = BehaviourManager::PrepareBehavioursLibrary(true, cancel);
    return success;
}

void GameBuilder::RemoveLatestGameBuild()
{
    File::Remove(m_latestGameExecutableFilepath);

    Path executableDir = m_latestGameExecutableFilepath.GetDirectory();
    Path gameDataDir = executableDir.Append("GameData");
    File::Remove(gameDataDir);
}

GameBuildDialog *GameBuilder::GetGameBuildDialog()
{
    return m_gameBuildDialog;
}
