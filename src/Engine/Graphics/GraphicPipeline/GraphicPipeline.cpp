﻿#include "Bang/GraphicPipeline.h"

#include "Bang/GL.h"
#include "Bang/Mesh.h"
#include "Bang/VAO.h"
#include "Bang/VBO.h"
#include "Bang/Scene.h"
#include "Bang/Light.h"
#include "Bang/Input.h"
#include "Bang/Screen.h"
#include "Bang/Camera.h"
#include "Bang/ChronoGL.h"
#include "Bang/Material.h"
#include "Bang/Renderer.h"
#include "Bang/Transform.h"
#include "Bang/GBuffer.h"
#include "Bang/Texture.h"
#include "Bang/GameObject.h"
#include "Bang/MeshFactory.h"
#include "Bang/SceneManager.h"
#include "Bang/ShaderProgram.h"
#include "Bang/RectTransform.h"
#include "Bang/MaterialFactory.h"
#include "Bang/RenderTexture.h"
#include "Bang/TextureUnitManager.h"
#include "Bang/SelectionFramebuffer.h"

USING_NAMESPACE_BANG

GraphicPipeline::GraphicPipeline(Screen *screen)
{
    m_gl = new GL();
    m_texUnitManager = new TextureUnitManager();

    m_gbuffer = new GBuffer(screen->GetWidth(), screen->GetHeight());
    m_selectionFB = new SelectionFramebuffer(screen->GetWidth(),
                                             screen->GetHeight());

    m_renderGBufferToScreenMaterial = MaterialFactory::GetRenderGBufferToScreen();
    m_screenPlaneMesh = MeshFactory::GetUIPlane();
}

GraphicPipeline::~GraphicPipeline()
{
    delete m_gbuffer;
    delete m_selectionFB;
    delete m_texUnitManager;
    delete m_gl;
}

void GraphicPipeline::RenderScene(Scene *scene, bool inGame)
{
    p_scene = scene; ENSURE(p_scene);
    m_renderingInGame = inGame;

    Camera *camera = scene->GetCamera();
    if (camera) { camera->Bind(); }

    RenderGBuffer(scene);
    RenderToScreen(m_gbuffer->GetAttachmentTexture(GBuffer::AttColor));

    RenderSelectionBuffer(scene);
    // RenderToScreen(m_selectionFB->GetColorTexture());
}

void GraphicPipeline::ApplyDeferredLights(Renderer *rend)
{
    // Limit rendering to the renderer visible rect
    Rect renderRect = Rect::ScreenRect;
    Camera *sceneCam = p_scene->GetCamera();
    if (rend)
    {
        renderRect = rend->gameObject->GetBoundingScreenRect(sceneCam, false);
    }
    else
    {
        // Apply deferred lights to the whole scene
        renderRect = p_scene->GetBoundingScreenRect(sceneCam, true);
    }
    renderRect = Rect::ScreenRect;
    ENSURE(renderRect != Rect::Zero);

    // We have marked from before the zone where we want to apply the effect
    GL::SetStencilValue(1);
    GL::SetStencilFunc(GL::Function::Equal);

    // m_gbuffer->ExportStencil(Path("stencil.png"), 64);
    Material *rendMat = rend ? rend->GetMaterial() : nullptr;
    if ( !rend || (rendMat && rendMat->IsReceivesLighting()) )
    {
        List<Light*> lights = p_scene->GetComponentsInChildren<Light>();
        for (Light *light : lights)
        {
            if (!light || !light->IsEnabled(true)) { continue; }
            light->ApplyLight(m_gbuffer, renderRect);
        }
    }

    GL::SetStencilFunc(GL::Function::Always);
}

void GraphicPipeline::RenderGBuffer(Scene *scene)
{
    m_gbuffer->Bind();
    Color bgColor = p_scene->GetCamera()->GetClearColor();
    m_gbuffer->ClearBuffersAndBackground(bgColor);
    m_gbuffer->SetAllColorDrawBuffers();

    // GBuffer Scene rendering
    GL::SetDepthMask(true); // Write depth
    GL::SetDepthFunc(GL::Function::LEqual);

    GL::SetStencilValue(1);
    GL::SetStencilOp(GL::StencilOperation::Replace); // Write to stencil
    scene->Render(RenderPass::Scene_Lighted);
    GL::SetStencilOp(GL::StencilOperation::Keep); // Dont modify stencil
    ApplyDeferredLights();

    scene->Render(RenderPass::Scene_UnLighted);
    scene->Render(RenderPass::Scene_PostProcess);
    GL::ClearDepthBuffer();
    GL::ClearStencilBuffer();

    // GBuffer Canvas rendering
    m_gbuffer->SetAllColorDrawBuffers();
    GL::SetDepthMask(false);
    GL::SetDepthFunc(GL::Function::Always);
    scene->Render(RenderPass::Canvas);
    scene->Render(RenderPass::Canvas_PostProcess);
    GL::SetDepthFunc(GL::Function::LEqual);
    GL::ClearStencilBuffer();

    // GBuffer Gizmos rendering
    GL::SetDepthMask(true);
    GL::SetDepthFunc(GL::Function::LEqual);
    scene->RenderGizmos();

    m_gbuffer->UnBind();
}
void GraphicPipeline::RenderSelectionBuffer(Scene *scene)
{
    m_selectionFB->Bind();
    GL::ClearStencilBuffer();
    m_selectionFB->ClearDepth();
    m_selectionFB->ClearColor(Color::One);

    // Selection rendering
    m_selectionFB->PrepareForRender(scene);
    scene->Render(RenderPass::Scene_Lighted);
    scene->Render(RenderPass::Scene_UnLighted);
    GL::ClearStencilBuffer();
    scene->Render(RenderPass::Canvas);
    scene->RenderGizmos();

    m_selectionFB->UnBind();
}


void GraphicPipeline::ApplyScreenPass(ShaderProgram *sp, const Rect &mask)
{
    sp->Bind();
    m_gl->ApplyToShaderProgram(sp);
    sp->Set("B_rectMinCoord", mask.GetMin());
    sp->Set("B_rectMaxCoord", mask.GetMax());
    sp->Set("B_ScreenSize", Vector2f(Screen::GetSizeS()));
    RenderScreenPlane();
    sp->UnBind();
}

void GraphicPipeline::RenderToScreen(Texture *fullScreenTexture)
{
    m_renderGBufferToScreenMaterial->Bind();

    ShaderProgram *sp = m_renderGBufferToScreenMaterial->GetShaderProgram();
    m_gbuffer->PrepareForRender(sp);
    sp->Set("B_GTex_Color", fullScreenTexture);

    GraphicPipeline::RenderScreenPlane();

    m_renderGBufferToScreenMaterial->UnBind();
}

void GraphicPipeline::RenderScreenPlane()
{
    GL::SetWireframe(false);
    GL::SetDepthFunc(GL::Function::Always);
    GL::SetDepthMask(false);
    GL::SetCullFace(GL::Face::None);
    GL::Render(m_screenPlaneMesh->GetVAO(), GL::Primitives::Triangles,
               m_screenPlaneMesh->GetVertexCount());
    GL::SetDepthMask(true);
    GL::SetDepthFunc(GL::Function::LEqual);
}

GraphicPipeline* GraphicPipeline::GetActive()
{
    Screen *screen = Screen::GetInstance();
    return screen ? screen->GetGraphicPipeline() : nullptr;
}

void GraphicPipeline::OnResize(int newWidth, int newHeight)
{
    m_gbuffer->Resize(newWidth, newHeight);
    m_selectionFB->Resize(newWidth, newHeight);
}

void GraphicPipeline::Render(Renderer *rend)
{
    if (GL::IsBound(m_gbuffer))
    {
        rend->Bind();
        Material *rendMat = rend->GetMaterial();

        if (rend->NeedsReadingColorBuffer())
        {
            Camera *cam = p_scene->GetCamera();
            m_gbuffer->PrepareColorReadBuffer(rend->GetBoundingRect(cam));
        }
        m_gbuffer->PrepareForRender(rendMat->GetShaderProgram());
        rend->OnRender();
        rend->UnBind();
    }
    else if (GL::IsBound(m_selectionFB))
    {
        m_selectionFB->RenderForSelectionBuffer(rend);
    }
}

GL *GraphicPipeline::GetGL() const { return m_gl; }
GBuffer *GraphicPipeline::GetGBuffer() { return m_gbuffer; }
SelectionFramebuffer *GraphicPipeline::GetSelectionFramebuffer()
{
    return m_selectionFB;
}
TextureUnitManager *GraphicPipeline::GetTextureUnitManager() const
{
    return m_texUnitManager;
}
