#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <QTimer>
#include <QObject>
#include "Bang/WinUndef.h"

#include "Bang/Map.h"
#include "Bang/Set.h"
#include "Bang/String.h"
#include "Bang/G_Shader.h"

class G_ShaderManager : public QObject
{
    Q_OBJECT

public:
    G_ShaderManager();
    static G_ShaderManager *GetInstance();

    static G_Shader* Load(G_Shader::Type type, const Path& filepath);

    static void RegisterUsageOfShader(G_ShaderProgram *shaderProgram,
                                      G_Shader *shaderBeingUsed);
    static void UnRegisterUsageOfShader(G_ShaderProgram *shaderProgram,
                                        G_Shader *shaderBeingUsed);

private:
    QTimer m_refreshTimer;
    EpochTime m_lastRefreshTime;

    Map<Path, G_Shader*> m_filepathToShaders;
    Map<G_Shader*, Set<G_ShaderProgram*> > m_shaderUsages;

private slots:
    void Refresh();
};

#endif // SHADERMANAGER_H
