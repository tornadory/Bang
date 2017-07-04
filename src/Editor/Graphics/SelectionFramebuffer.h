#ifndef SELECTIONFRAMEBUFFER_H
#define SELECTIONFRAMEBUFFER_H

#include "Bang/Map.h"
#include "Bang/Vector3.h"
#include "Bang/G_Framebuffer.h"

#ifdef BANG_EDITOR
#include "Bang/IWindowEventManagerListener.h"
#endif

class Scene;
class Renderer;
class GameObject;
class G_ShaderProgram;

/**
 * @brief This is the buffer where each GameObject is drawn into with a unique colour.
 */
class SelectionFramebuffer : public G_Framebuffer
                            #ifdef BANG_EDITOR
                            ,public IWindowEventManagerListener
                            #endif
{
public:
    SelectionFramebuffer(int width, int height);
    virtual ~SelectionFramebuffer();

    void PrepareForRender(const Scene *scene);
    void PrepareNextGameObject(GameObject *go);
    void RenderForSelectionBuffer(Renderer *renderer);
    void ProcessSelection();

    G_RenderTexture *GetColorTexture() const;
    G_RenderTexture *GetWorldPosTexture() const;

    GameObject *GetGameObjectInPosition(int x, int y);
    Vector3 GetWorldPositionAt(int x, int y);
    bool IsPassing() const;

    G_ShaderProgram *GetSelectionShaderProgram() const;

    void OnGameObjectDestroyed(GameObject *destroyed) override;

private:
    static const AttachmentId AttColor    = AttachmentId::ColorAttachment0;
    static const AttachmentId AttWorldPos = AttachmentId::ColorAttachment1;

    G_RenderTexture *m_colorTexture    = nullptr;
    G_RenderTexture *m_worldPosTexture = nullptr;

    /**
     * @brief m_isPassing is true only when SelectionFrambuffer is rendering.
     * It is used by the Renderer's, to know if they have to activate its
     * special states before rendering.
     */
    bool m_isPassing = false;
    GameObject *m_nextGameObjectToBeRendered = nullptr;

    G_ShaderProgram *m_selectionProgram = nullptr;
    /**
     * @brief Keeps track of the last GameObject the mouse was over.
     * This lets us inform of mouseOut events.
     */
    GameObject *m_lastMouseOverGO = nullptr;

    mutable Map<GameObject*, long> m_gameObject_To_Id;
    mutable Map<long, GameObject*> m_id_To_GameObject;

    static Color MapIdToColor(long id);
    static long MapColorToId(const Color &color);

    bool CanRenderGameObject(const GameObject *go);

    Color GetSelectionColor(GameObject *go) const;

    friend class Gizmos;
    friend class G_GraphicPipeline;
};



#endif // SELECTIONFRAMEBUFFER_H
