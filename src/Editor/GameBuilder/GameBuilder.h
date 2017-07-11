#ifndef GAMEBUILDER_H
#define GAMEBUILDER_H

#include <QThread>
#include <QProgressDialog>
#include "Bang/WinUndef.h"

#include "Bang/Path.h"
#include "Bang/String.h"
#include "Bang/GameBuilderJob.h"

class Project;
class GameBuildDialog;
class GameBuilder : public QObject
{
    Q_OBJECT

public:
    static GameBuilder* GetInstance();

    void BuildGame(bool runGame = false);

public slots:
    void OnGameHasBeenBuilt();
    void OnGameBuildingHasBeenCanceled();
    void OnGameBuildingHasFailed(const QString &errorMsg);
    void OnDialogError(const QString &title, const QString &msg);

signals:
    void NotifyGameBuildingCanceled();
    void DialogError(const QString &title, const QString &msg);

private:
    GameBuilder();
    virtual ~GameBuilder();

    static GameBuilder *s_instance;

    bool m_runGameAfterBuild = false;
    Path m_latestGameExecutableFilepath;
    QThread *m_gameBuilderThread = nullptr;
    GameBuilderJob *m_gameBuilderJob = nullptr;
    GameBuildDialog *m_gameBuildDialog = nullptr;

    Path AskForExecutableFilepath();
    bool     CompileGameExecutable();
    bool     CreateDataDirectory(const Path &executableDir);
    Project* CreateGameProject(const Path &executableDir);
    bool     CompileBehaviours(const Path &executableDir,
                               bool *cancel);
    void RemoveLatestGameBuild();

    GameBuildDialog *GetGameBuildDialog();


    friend class GameBuilderJob;
};

#endif // GAMEBUILDER_H