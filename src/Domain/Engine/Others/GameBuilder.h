#ifndef GAMEBUILDER_H
#define GAMEBUILDER_H

#include <QThread>

#include "String.h"

class BuildGameThread : public QThread
{
    Q_OBJECT
    public:
        bool runGameAfterBuild = false;

    protected:
        void run() override;
};

class GameBuilder
{
private:
    static BuildGameThread buildThread;

    GameBuilder() {}

public:
    static void BuildGame(bool runGame = false);
};

#endif // GAMEBUILDER_H
