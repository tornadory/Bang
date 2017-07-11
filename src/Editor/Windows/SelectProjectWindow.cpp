#include "Bang/SelectProjectWindow.h"

#include "Bang/Paths.h"
#include "Bang/Debug.h"
#include "Bang/Project.h"
#include "Bang/EngineConfig.h"
#include "Bang/EditorProjectManager.h"

SelectProjectWindow *SelectProjectWindow::s_instance = nullptr;

SelectProjectWindow::SelectProjectWindow()
{
}

Path SelectProjectWindow::ExecAndGetProjectFilepath(QMainWindow *window,
                                                    QApplication *app)
{
    SelectProjectWindow::s_instance = new SelectProjectWindow();
    SelectProjectWindow *inst = SelectProjectWindow::s_instance;
    inst->setupUi( window );

    inst->m_window = window;

    connect(inst->listRecentProjects, SIGNAL(cellClicked(int,int)),
            inst, SLOT(OnRecentProjectListSelectionChanged(int,int)) );

    connect(inst->listRecentProjects, SIGNAL(cellDoubleClicked(int,int)),
            inst, SLOT(OnRecentProjectDoubleClicked(int,int)) );

    connect(inst->buttonCreateNewProject, SIGNAL(clicked()),
            inst, SLOT(OnCreateNewProject()));

    connect(inst->buttonBrowseProject, SIGNAL(clicked()),
            inst, SLOT(OnBrowseProject()));

    connect(inst->buttonLoadRecentProject, SIGNAL(clicked()),
            inst, SLOT(OnLoadRecentProject()));

    inst->buttonLoadRecentProject->setFocus();

    // Create recent project list and select first item
    inst->FillRecentProjectsList();
    inst->listRecentProjects->setCurrentItem(inst->listRecentProjects->item(0,0));
    inst->OnRecentProjectListSelectionChanged(0,0);

    if (inst->listRecentProjects->rowCount() == 0)
    {
        inst->buttonLoadRecentProject->setEnabled(false);
    }

    while ( inst->m_loadedProjectFile.IsEmpty() && !inst->m_directlyClosedByUser)
    {
        inst->m_directlyClosedByUser = true;
        window->show();
        app->exec();
    }

    if (inst->m_directlyClosedByUser) { return Path(); }
    else { return inst->m_loadedProjectFile; }
}

SelectProjectWindow *SelectProjectWindow::GetInstance()
{
    return SelectProjectWindow::s_instance;
}

QMainWindow* SelectProjectWindow::GetMainWindow() const
{
    return m_window;
}

void SelectProjectWindow::FillRecentProjectsList()
{
    listRecentProjects->clear();
    EngineConfig::CleanOutdatedRecentProjectList();

    int i = 0;
    List<Path> recentProjectsFilepath = EngineConfig::GetRecentProjectsList();
    listRecentProjects->setRowCount(recentProjectsFilepath.Size());
    listRecentProjects->setColumnCount(2);
    for (const Path &recentProject : recentProjectsFilepath)
    {
        QTableWidgetItem *projNameItem = new QTableWidgetItem(
                    recentProject.GetName().ToQString());
        QTableWidgetItem *projPathItem = new QTableWidgetItem(
                    recentProject.GetAbsolute().ToQString());
        listRecentProjects->setItem(i, 0, projNameItem);
        listRecentProjects->setItem(i, 1, projPathItem);
        ++i;
    }
}

void SelectProjectWindow::OnRecentProjectListSelectionChanged(int row,
                                                              int column)
{
    QTableWidgetItem *selectedItem = listRecentProjects->item(row, 1);
    ENSURE(selectedItem);

    m_selectedRecentProjectFilepath = Path(selectedItem->text());

    String newButtonText = "Load " + m_selectedRecentProjectFilepath.GetName();

    buttonLoadRecentProject->setText(newButtonText.ToQString());
}

void SelectProjectWindow::OnRecentProjectDoubleClicked(int row, int column)
{
    OnLoadRecentProject();
}

void SelectProjectWindow::OnLoadRecentProject()
{
    if (m_selectedRecentProjectFilepath.IsEmpty()) return;

    m_loadedProjectFile = m_selectedRecentProjectFilepath;
    m_directlyClosedByUser = false;
    m_window->close();
}

void SelectProjectWindow::OnCreateNewProject()
{
    m_loadedProjectFile = EditorProjectManager::DialogCreateNewProject();
    if (!m_loadedProjectFile.IsEmpty())
    {
        m_directlyClosedByUser = false;
        m_window->close();
    }
}

void SelectProjectWindow::OnBrowseProject()
{
    m_loadedProjectFile = EditorProjectManager::DialogOpenProject();
    if (!m_loadedProjectFile.IsEmpty())
    {
        m_directlyClosedByUser = false;
        m_window->close();
    }
}