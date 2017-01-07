#include "ProjectManager.h"

#include "List.h"
#include "Debug.h"
#include "Scene.h"
#include "Dialog.h"
#include "Project.h"
#include "XMLNode.h"
#include "MenuBar.h"
#include "Explorer.h"
#include "XMLParser.h"
#include "FileWriter.h"
#include "Persistence.h"
#include "EditorWindow.h"
#include "SceneManager.h"

Project *ProjectManager::s_currentProject = nullptr;

ProjectManager::ProjectManager()
{
}

Project* ProjectManager::CreateNewProject(const String &projectContainingDir,
                                          const String &projectName)
{
    String projectDir = projectContainingDir + "/" + projectName;
    if (!Persistence::ExistsDirectory(projectDir))
    {
        if (!Persistence::CreateDirectory(projectDir))
        {
            Debug_Error ("Could not create project in directory '" << projectDir << "'.");
            return nullptr;
        }
    }
    else
    {
        // TODO: Add overwrite window warning
        Debug_Warn("Directory '" << projectDir << "' already existed, using it.");
    }

    ProjectManager::CloseCurrentProject();

    ProjectManager::s_currentProject = new Project();
    ProjectManager::s_currentProject->SetProjectRootFilepath(projectDir);

    String projectFileFilepath = projectDir + "/" +
            projectName + "." + Project::GetFileExtensionStatic();
    FileWriter::WriteToFile(projectFileFilepath,
                            ProjectManager::s_currentProject->GetXMLInfoString());

    Persistence::CreateDirectory(projectDir + "/Assets");

    return ProjectManager::s_currentProject;
}

Project* ProjectManager::OpenProject(const String &projectFilepath)
{
    XMLNode *xmlInfo = XMLParser::FromFile(projectFilepath);
    if (!xmlInfo)
    {
        Debug_Error("Could not open project '" << projectFilepath << "'");
        return nullptr;
    }

    ProjectManager::CloseCurrentProject();

    ProjectManager::s_currentProject = new Project();
    ProjectManager::s_currentProject->ReadXMLInfo(xmlInfo);
    String projectDir = Persistence::GetDir(projectFilepath);
    ProjectManager::s_currentProject->SetProjectRootFilepath(projectDir);

    // Set directory of the explorer to the Assets' root of the new project
    Explorer::GetInstance()->SetDir(
                ProjectManager::s_currentProject->GetProjectAssetsRootFilepath());

    // Set window title
    String title = "Bang - " + ProjectManager::s_currentProject->GetProjectName();
    EditorWindow::GetInstance()->GetMainWindow()->setWindowTitle(title.ToQString());

    // Set persistence variables
    Persistence::c_ProjectRootAbsolute = ProjectManager::s_currentProject->GetProjectRootFilepath();
    Persistence::c_ProjectAssetsRootAbsolute =
            Persistence::c_ProjectRootAbsolute + "/Assets";


    // Open the first found scene
    List<String> sceneFilepaths = Persistence::GetFiles(projectDir, true,
                                                        {"*." + Scene::GetFileExtensionStatic()});
    if (!sceneFilepaths.Empty())
    {
        SceneManager::LoadScene(sceneFilepaths.Front());
    }
    else
    {
        // Create new empty scene
        MenuBar::GetInstance()->CreateNewScene();
    }
    //

    return ProjectManager::s_currentProject;
}

void ProjectManager::SaveProject(const Project *project)
{
    ASSERT(project);
    bool ok = FileWriter::WriteToFile(project->GetProjectFileFilepath(),
                                      project->GetXMLInfoString());
    if (ok)
    {
        Debug_Status("Project '" << project->GetProjectName() << "' successfully saved.", 5.0f);
    }
    else
    {
        Debug_Error("Could not save the project...");
    }
}

void ProjectManager::SaveCurrentProject()
{
    ProjectManager::SaveProject( ProjectManager::GetCurrentProject() );
}

void ProjectManager::CloseCurrentProject()
{
    // TODO: Ask for saving current open project and stuff
    if (ProjectManager::s_currentProject)
    {
        delete s_currentProject;
        ProjectManager::s_currentProject = nullptr;
    }

}

String ProjectManager::DialogCreateNewProject()
{
    String dirPath = Dialog::GetOpenDirname("Select the project containing directory");
    if (!dirPath.Empty())
    {
        bool ok;
        String projectName = Dialog::GetInputString("Please specify your new project's name",
                                                    "Project name:",
                                                    "MyBangProject",
                                                    &ok);
        if (ok)
        {
            String projectPath = dirPath + "/" + projectName;
            if (!Persistence::ExistsDirectory(projectPath))
            {
                ProjectManager::CreateNewProject(dirPath, projectName);
                return ProjectManager::GetCurrentProject()->GetProjectFileFilepath();
            }
            else
            {
                Dialog::Error("Error creating the project",
                              "The directory '" + projectPath + "' existed before.\n" +
                              "Please select another name for your project.");
            }
        }
    }
    return "";
}

String ProjectManager::DialogOpenProject()
{
    String projectFilepath =
            Dialog::GetOpenFilename("Select the project file to be opened",
                                    Project::GetFileExtensionStatic(),
                                    String(QDir::homePath()) );
    return projectFilepath;
}

Project *ProjectManager::GetCurrentProject()
{
    return ProjectManager::s_currentProject;
}