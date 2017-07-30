#include "Bang/SelectionFramebuffer.h"

#include "Bang/Paths.h"
#include "Bang/Scene.h"
#include "Bang/Input.h"
#include "Bang/Screen.h"
#include "Bang/Vector3.h"
#include "Bang/Material.h"
#include "Bang/Renderer.h"
#include "Bang/GameObject.h"
#include "Bang/ShaderProgram.h"
#include "Bang/G_RenderTexture.h"

SelectionFramebuffer::SelectionFramebuffer(int width, int height) :
    G_Framebuffer(width, height)
{
    ShaderProgram *selectionProgram = new ShaderProgram();
    selectionProgram->Load( EPATH("Shaders/G_Default.vert_g"),
                            EPATH("Shaders/SelectionBuffer.frag_sel") );

    m_selectionMaterial = new Material();
    m_selectionMaterial->SetShaderProgram(selectionProgram);

    CreateColorAttachment(AttColor, G_Texture::Format::RGBA_Float16);
    CreateDepthRenderbufferAttachment();

    m_colorTexture = GetAttachmentTexture(AttColor);
}

SelectionFramebuffer::~SelectionFramebuffer()
{
    delete m_selectionMaterial;
}

void SelectionFramebuffer::PrepareForRender(const Scene *scene)
{
    int id = 1;
    m_gameObject_To_Id.Clear();
    m_id_To_GameObject.Clear();

    List<GameObject*> gameObjects = scene->GetChildrenRecursively();
    for (GameObject *go : gameObjects)
    {
        m_gameObject_To_Id[go] = id;
        m_id_To_GameObject[id] = go;
        ++id;
    }
}

void SelectionFramebuffer::RenderForSelectionBuffer(Renderer *rend)
{
    ASSERT(GL::IsBound(this));
    SetAllDrawBuffers();

    GameObject *go = rend->gameObject;

    ShaderProgram *selSP = m_selectionMaterial->GetShaderProgram();
    ShaderProgram *prevSP = rend->GetMaterial()->GetShaderProgram();
    rend->GetMaterial()->SetShaderProgram(selSP);

    rend->Bind();
    selSP->Set("selectionColor", GetSelectionColor(go));
    rend->Render();
    rend->UnBind();

    rend->GetMaterial()->SetShaderProgram(prevSP);
}

GameObject *SelectionFramebuffer::GetGameObjectInPosition(
        const Vector2i &screenCoords)
{
    Color mouseOverColor = ReadColor(screenCoords.x, screenCoords.y, AttColor);
    int id = MapColorToId(mouseOverColor);
    if (mouseOverColor != Color::Zero && m_id_To_GameObject.ContainsKey(id))
    {
        return m_id_To_GameObject[id];
    }
    return nullptr;
}

Color SelectionFramebuffer::GetSelectionColor(GameObject *go) const
{
    return MapIdToColor(m_gameObject_To_Id[go]);
}

Color SelectionFramebuffer::MapIdToColor(long id)
{
    constexpr int C = 256;
    Color color =
            Color(
                    double(   id                % C),
                    double(  (id / C)           % C),
                    double( ((id / C) / C)      % C),
                    double((((id / C) / C) / C) % C)
                   );
   return color / float(C);
}

long SelectionFramebuffer::MapColorToId(const Color &color)
{
    constexpr int C = 256;
    return long(color.r * C) +
           long(color.g * C * C) +
           long(color.b * C * C * C) +
           long(color.a * C * C * C * C);
}

G_RenderTexture *SelectionFramebuffer::GetColorTexture() const
{
    return m_colorTexture;
}