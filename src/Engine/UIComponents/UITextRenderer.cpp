#include "Bang/UITextRenderer.h"

#include "Bang/Math.h"
#include "Bang/Font.h"
#include "Bang/Scene.h"
#include "Bang/Gizmos.h"
#include "Bang/Camera.h"
#include "Bang/XMLNode.h"
#include "Bang/Material.h"
#include "Bang/Transform.h"
#include "Bang/Texture2D.h"
#include "Bang/Resources.h"
#include "Bang/GLUniforms.h"
#include "Bang/SceneManager.h"
#include "Bang/RectTransform.h"
#include "Bang/ShaderProgram.h"
#include "Bang/TextFormatter.h"
#include "Bang/MaterialFactory.h"
#include "Bang/UILayoutManager.h"

USING_NAMESPACE_BANG

UITextRenderer::UITextRenderer() : UIRenderer()
{
    p_mesh = Resources::Create<Mesh>();
    SetMaterial(MaterialFactory::GetUIText().Get());

    RH<Font> font = Resources::Load<Font>(EPATH("Fonts/Roboto.ttf"));
    SetFont(font.Get());
    SetContent("");
    SetTextSize(20.0f);
    SetTextColor(Color::Black);

    SetRenderPrimitive(GL::Primitive::Triangles);
    OnChanged();
}

UITextRenderer::~UITextRenderer()
{
}

void UITextRenderer::OnRender()
{
    UIRenderer::OnRender();
    RegenerateCharQuadsVAO();

    int vertCount = p_mesh.Get()->GetVertexCount();
    if (vertCount >= 3)
    {
        GL::Render(p_mesh.Get()->GetVAO(), GetRenderPrimitive(), vertCount);
    }
}

void UITextRenderer::CalculateLayout(Axis axis)
{
    if (!GetFont()) { SetCalculatedLayout(axis, 0, 0); return; }

    Vector2i minSize = Vector2i::Zero;

    Vector2i prefSize = TextFormatter::GetTextSizeOneLined(GetContent(), GetFont(),
                                                           GetTextSize(),
                                                           GetSpacingMultiplier());
    prefSize.y = Math::Max<int>(prefSize.y,
                                m_numberOfLines *
                                GetFont()->GetFontHeight(GetTextSize()));

    SetCalculatedLayout(axis, minSize.GetAxis(axis), prefSize.GetAxis(axis));
}

void UITextRenderer::RegenerateCharQuadsVAO() const
{
    if (!IInvalidatable<UITextRenderer>::IsInvalid()) { return; }
    IInvalidatable<UITextRenderer>::Validate();

    if (!GetFont())
    {
        p_mesh.Get()->LoadPositions({});
        p_mesh.Get()->LoadUvs({});
        return;
    }

    // Get the quad positions of the rects of each char
    if (!GetGameObject()) { return; }
    RectTransform *rt = GetGameObject()->GetRectTransform();
    if (!rt) { return; }

    Array<TextFormatter::CharRect> textCharRects =
            TextFormatter::GetFormattedTextPositions(
                                        GetContent(),
                                        GetFont(),
                                        GetTextSize(),
                                        Recti( rt->GetViewportRect() ),
                                        GetSpacingMultiplier(),
                                        GetHorizontalAlignment(),
                                        GetVerticalAlignment(),
                                        IsWrapping(),
                                        true,
                                        &m_numberOfLines);

    // Generate quad positions and uvs for the mesh, and load them
    Array<Vector2> textQuadUvs;
    Array<Vector2> textQuadPos2D;
    Array<Vector3> textQuadPos3D;

    m_charRectsLocalNDC.Clear();
    for (const TextFormatter::CharRect &cr : textCharRects)
    {
        if (!GetFont()->HasCharacter(cr.character)) { continue; }

        Vector2 minPxPerf = GL::FromPixelsPointToPixelPerfect(cr.rectPx.GetMin());
        Vector2 maxPxPerf = cr.rectPx.GetMin() + cr.rectPx.GetSize();
        Vector2f minViewportNDC ( GL::FromViewportPointToViewportPointNDC(minPxPerf) );
        Vector2f maxViewportNDC ( GL::FromViewportPointToViewportPointNDC(maxPxPerf) );

        Vector2 minUv, maxUv;
        if (GetFont()->HasDistanceField())
        {
            minUv = GetFont()->GetCharMinUvInDistField(cr.character);
            maxUv = GetFont()->GetCharMaxUvInDistField(cr.character);

            // Scale the character quad and uvs so that the character is as
            // large as if we weren't using SDF. If we dont compensate
            // this size, we would get all the distance field in the same quad,
            // and consequently the character itself would be smaller
            Vector2 spOffsetPx = Vector2(GetFont()->GetDistFieldSpreadOffsetPx(cr.character));
            Vector2 spOffsetUv( Vector2(spOffsetPx) /
                                Vector2(GetFont()->GetDistFieldTexture()->GetSize()) );

            Vector2 uvSize = (maxUv-minUv);
            Vector2 uvScaling = (uvSize + spOffsetUv * 2.0f) / uvSize;
            Vector2 viewportNDCCharSize  = (maxViewportNDC - minViewportNDC);
            Vector2 viewportNDCPosCenter = (maxViewportNDC + minViewportNDC) / 2.0f;
            Vector2 scaledSize = (viewportNDCCharSize * uvScaling);
            minViewportNDC = viewportNDCPosCenter - scaledSize * 0.5f;
            maxViewportNDC = viewportNDCPosCenter + scaledSize * 0.5f;
            minUv -= spOffsetUv;
            maxUv += spOffsetUv;
        }
        else
        {
            GetFont()->GetFontAtlas(GetTextSize()); // Load atlas
            minUv = GetFont()->GetCharMinUv(GetTextSize(), cr.character);
            maxUv = GetFont()->GetCharMaxUv(GetTextSize(), cr.character);
        }

        Rect charRectViewportNDC(minViewportNDC, maxViewportNDC);
        Rect charRectLocalNDC =
                    rt->FromViewportRectNDCToLocalRectNDC(charRectViewportNDC);

        textQuadUvs.PushBack( Vector2(minUv.x, maxUv.y) );
        textQuadPos2D.PushBack(charRectLocalNDC.GetMinXMinY());
        textQuadPos3D.PushBack( Vector3(charRectLocalNDC.GetMinXMinY(), 0) );
        textQuadUvs.PushBack( Vector2(maxUv.x, maxUv.y) );
        textQuadPos2D.PushBack(charRectLocalNDC.GetMaxXMinY());
        textQuadPos3D.PushBack( Vector3(charRectLocalNDC.GetMaxXMinY(), 0) );
        textQuadUvs.PushBack( Vector2(maxUv.x, minUv.y) );
        textQuadPos2D.PushBack(charRectLocalNDC.GetMaxXMaxY());
        textQuadPos3D.PushBack( Vector3(charRectLocalNDC.GetMaxXMaxY(), 0) );

        textQuadUvs.PushBack( Vector2(minUv.x, maxUv.y) );
        textQuadPos2D.PushBack(charRectLocalNDC.GetMinXMinY());
        textQuadPos3D.PushBack( Vector3(charRectLocalNDC.GetMinXMinY(), 0) );
        textQuadUvs.PushBack( Vector2(maxUv.x, minUv.y) );
        textQuadPos2D.PushBack(charRectLocalNDC.GetMaxXMaxY());
        textQuadPos3D.PushBack( Vector3(charRectLocalNDC.GetMaxXMaxY(), 0) );
        textQuadUvs.PushBack( Vector2(minUv.x, minUv.y) );
        textQuadPos2D.PushBack(charRectLocalNDC.GetMinXMaxY());
        textQuadPos3D.PushBack( Vector3(charRectLocalNDC.GetMinXMaxY(), 0) );

        Rect charRectLocalNDCRaw ( rt->FromViewportPointToLocalPointNDC(minPxPerf),
                                   rt->FromViewportPointToLocalPointNDC(maxPxPerf) );
        m_charRectsLocalNDC.PushBack(charRectLocalNDCRaw);
    }

    m_textRectNDC = Rect::GetBoundingRectFromPositions(textQuadPos2D.Begin(),
                                                       textQuadPos2D.End());
    p_mesh.Get()->LoadPositions(textQuadPos3D);
    p_mesh.Get()->LoadUvs(textQuadUvs);
}

void UITextRenderer::Bind() const
{
    // Nullify RectTransform model, since we control its position and size
    // directly from the VBO creation...
    Vector3 translate(0, 0, GetGameObject()->GetTransform()->GetPosition().z);
    GLUniforms::SetModelMatrix( Matrix4::TranslateMatrix(translate) );
    UIRenderer::Bind();

    if (GetFont())
    {
        const int textSize = Math::Max(GetTextSize(), 1);

        bool usingDistField = GetFont()->HasDistanceField();
        if (usingDistField)
        {
            Texture2D *fontDistField = GetFont()->GetDistFieldTexture();
            GetMaterial()->SetTexture(fontDistField);

            float blurriness = GetBlurriness() / textSize;
            blurriness = Math::Clamp(blurriness, 0.0f, 1.0f);
            GL::Uniform("B_textBlurriness", blurriness);

            float alphaThresh = GetAlphaThreshold() + (0.05f / textSize);
            alphaThresh = Math::Clamp(alphaThresh, 0.0f, 1.0f);
            GL::Uniform("B_textAlphaThreshold", alphaThresh);

            GL::Uniform("B_outlineWidth", GetOutlineWidth());
            if (GetOutlineWidth() > 0.0f)
            {
                GL::Uniform("B_outlineColor", GetOutlineColor());
                GL::Uniform("B_outlineBlurriness", GetOutlineBlurriness());
            }
        }
        else
        {
            Texture2D *fontAtlas = GetFont()->GetFontAtlas(GetTextSize());
            GetMaterial()->SetTexture(fontAtlas);
        }
        // GL::Uniform("B_usingDistField", usingDistField,  false);
    }
}

void UITextRenderer::UnBind() const
{
    UIRenderer::UnBind();
}

void UITextRenderer::SetHorizontalAlign(HorizontalAlignment horizontalAlignment)
{
    if (GetHorizontalAlignment() != horizontalAlignment)
    {
        m_horizontalAlignment = horizontalAlignment;
        OnChanged();
    }
}

void UITextRenderer::SetVerticalAlign(VerticalAlignment verticalAlignment)
{
    if (GetVerticalAlignment() != verticalAlignment)
    {
        m_verticalAlignment = verticalAlignment;
        OnChanged();
    }
}

void UITextRenderer::SetFont(Font *font)
{
    if (GetFont() != font)
    {
        p_font.Set(font);
        OnChanged();
    }
}

void UITextRenderer::SetKerning(bool kerning)
{
    if (IsKerning() != kerning)
    {
        m_kerning = kerning;
        OnChanged();
    }
}

void UITextRenderer::SetWrapping(bool wrapping)
{
    if (IsWrapping() != wrapping)
    {
        m_wrapping = wrapping;
        OnChanged();
    }
}

void UITextRenderer::SetAlphaThreshold(float alphaThreshold)
{
    if (alphaThreshold != GetAlphaThreshold())
    {
        m_alphaThreshold = alphaThreshold;
        OnChanged();
    }
}

void UITextRenderer::SetBlurriness(float blurriness)
{
    if (blurriness != GetBlurriness())
    {
        m_blurriness = blurriness;
        OnChanged();
    }
}

void UITextRenderer::SetContent(const String &content)
{
    if (GetContent() != content)
    {
        m_content = content;
        OnChanged();
    }
}

void UITextRenderer::SetTextSize(int size)
{
    if (GetTextSize() != size)
    {
        m_textSize = Math::Max(size, 1);
        OnChanged();
    }
}

void UITextRenderer::SetOutlineWidth(float outlineWidth)
{
    if (outlineWidth != GetOutlineWidth())
    {
        m_outlineWidth = outlineWidth;
        OnChanged();
    }
}

void UITextRenderer::SetOutlineColor(const Color &color)
{
    if (color != GetOutlineColor())
    {
        m_outlineColor = color;
        OnChanged();
    }
}

void UITextRenderer::SetOutlineBlurriness(float outlineBlurriness)
{
    if (outlineBlurriness != GetOutlineBlurriness())
    {
        m_outlineBlurriness = outlineBlurriness;
        OnChanged();
    }
}

void UITextRenderer::SetSpacingMultiplier(const Vector2& spacingMultiplier)
{
    if (GetSpacingMultiplier() != spacingMultiplier)
    {
        m_spacingMultiplier = spacingMultiplier;
        OnChanged();
    }
}

void UITextRenderer::SetTextColor(const Color &textColor)
{
    if (textColor != GetTextColor())
    {
        GetMaterial()->SetDiffuseColor( textColor );
        OnChanged();
    }
}

Font *UITextRenderer::GetFont() const { return p_font.Get(); }
bool UITextRenderer::IsKerning() const { return m_kerning; }
bool UITextRenderer::IsWrapping() const { return m_wrapping; }

float UITextRenderer::GetBlurriness() const { return m_blurriness; }
float UITextRenderer::GetAlphaThreshold() const { return m_alphaThreshold; }
const String &UITextRenderer::GetContent() const { return m_content; }
int UITextRenderer::GetTextSize() const { return m_textSize; }

float UITextRenderer::GetOutlineWidth() const { return m_outlineWidth; }
const Color &UITextRenderer::GetOutlineColor() const { return m_outlineColor; }

float UITextRenderer::GetOutlineBlurriness() const { return m_outlineBlurriness; }
const Vector2& UITextRenderer::GetSpacingMultiplier() const { return m_spacingMultiplier; }
const Array<Rect> &UITextRenderer::GetCharRectsLocalNDC() const
{ return m_charRectsLocalNDC; }
const Rect &UITextRenderer::GetCharRectLocalNDC(uint charIndex) const
{
    return GetCharRectsLocalNDC()[charIndex];
}

Rect UITextRenderer::GetCharRectViewportNDC(uint charIndex) const
{
    return GetGameObject()->GetRectTransform()->
            FromLocalRectNDCToViewportRectNDC(GetCharRectsLocalNDC()[charIndex]);
}
Rect UITextRenderer::GetContentViewportNDCRect() const
{
    return GetGameObject()->GetRectTransform()->
            FromLocalRectNDCToViewportRectNDC(m_textRectNDC);
}

VerticalAlignment UITextRenderer::GetVerticalAlignment() const
{
    return m_verticalAlignment;
}
HorizontalAlignment UITextRenderer::GetHorizontalAlignment() const
{
    return m_horizontalAlignment;
}

Rect UITextRenderer::GetBoundingRect(Camera *camera) const
{
    return GetContentViewportNDCRect();
}

const Color &UITextRenderer::GetTextColor() const
{
    return GetMaterial()->GetDiffuseColor();
}

void UITextRenderer::CloneInto(ICloneable *clone) const
{
    UIRenderer::CloneInto(clone);

    UITextRenderer *text = Cast<UITextRenderer*>(clone);
    text->SetFont ( GetFont() );
    text->SetContent( GetContent() );
    text->SetTextSize( GetTextSize() );
    text->SetTextColor( GetTextColor() );
    text->SetBlurriness( GetBlurriness() );
    text->SetAlphaThreshold( GetAlphaThreshold() );
    text->SetOutlineWidth( GetOutlineWidth() );
    text->SetOutlineColor( GetOutlineColor() );
    text->SetOutlineBlurriness( GetOutlineBlurriness() );
    text->SetSpacingMultiplier( GetSpacingMultiplier() );
    text->SetWrapping( IsWrapping() );
    text->SetHorizontalAlign( GetHorizontalAlignment() );
    text->SetVerticalAlign( GetVerticalAlignment() );
}

void UITextRenderer::ImportXML(const XMLNode &xml)
{
    UIRenderer::ImportXML(xml);

    if (xml.Contains("Font"))
    { SetFont(Resources::Load<Font>(xml.Get<GUID>("Font")).Get()); }

    if (xml.Contains("Content"))
    { SetContent(xml.Get<String>("Content")); }

    if (xml.Contains("TextSize"))
    { SetTextSize(xml.Get<float>("TextSize")); }

    if (xml.Contains("SpacingMultiplier"))
    { SetSpacingMultiplier(xml.Get<Vector2>("SpacingMultiplier")); }

    if (xml.Contains("Kerning"))
    { SetKerning(xml.Get<bool>("Kerning")); }

    if (xml.Contains("TextColor"))
    { SetTextColor( xml.Get<Color>("TextColor") ); }

    if (xml.Contains("AlphaThreshold"))
    { SetWrapping( xml.Get<bool>("AlphaThreshold") ); }

    if (xml.Contains("Wrapping"))
    { SetWrapping( xml.Get<bool>("Wrapping") ); }

    if (xml.Contains("VerticalAlign"))
    { SetVerticalAlign( xml.Get<VerticalAlignment>("VerticalAlign") ); }

    if (xml.Contains("HorizontalAlign"))
    { SetHorizontalAlign( xml.Get<HorizontalAlignment>("HorizontalAlign")); }
}

void UITextRenderer::ExportXML(XMLNode *xmlInfo) const
{
    UIRenderer::ExportXML(xmlInfo);

    xmlInfo->Set("Font", GetFont() ? GetFont()->GetGUID() : GUID::Empty());
    xmlInfo->Set("Content", GetContent());
    xmlInfo->Set("TextSize", GetTextSize());
    xmlInfo->Set("SpacingMultiplier", GetSpacingMultiplier());
    xmlInfo->Set("TextColor", GetTextColor());
    xmlInfo->Set("AlphaThreshold", GetAlphaThreshold());
    xmlInfo->Set("Kerning", IsKerning());
    xmlInfo->Set("Wrapping", IsWrapping());
    xmlInfo->Set("VerticalAlign", GetVerticalAlignment() );
    xmlInfo->Set("HorizontalAlign", GetHorizontalAlignment() );
}

void UITextRenderer::OnChanged()
{
    IInvalidatable<UITextRenderer>::Invalidate();
    IInvalidatable<ILayoutElement>::Invalidate();
    UIRenderer::PropagateRendererChanged();
}

void UITextRenderer::OnTransformChanged()
{
    UIRenderer::OnTransformChanged();
    OnChanged();
}
