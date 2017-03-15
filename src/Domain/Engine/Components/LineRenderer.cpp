#include "LineRenderer.h"

#include "VBO.h"
#include "VAO.h"
#include "Math.h"
#include "Material.h"
#include "Transform.h"
#include "ShaderProgram.h"
#include "AssetsManager.h"

LineRenderer::LineRenderer()
{
    m_vbo = new VBO();
    m_vao = new VAO();
}

LineRenderer::~LineRenderer()
{

}

String LineRenderer::GetName() const
{
    return "LineRenderer";
}

ICloneable *LineRenderer::Clone() const
{
    LineRenderer *lr = new LineRenderer();
    CloneInto(lr);
    return lr;
}


void LineRenderer::BindPointsToVAO() const
{
    if (m_points.Size() >= 2 &&
        m_material  && m_material->GetShaderProgram() )
    {
        m_vbo->Fill(m_points.Data(), m_points.Size() * sizeof(Vector3));
        GLint verticesShaderLocation = m_material->GetShaderProgram()->
                GetAttribLocation("B_In_PositionObject");
        m_vao->UnBindVBO(verticesShaderLocation);
        m_vao->BindVBO(m_vbo, verticesShaderLocation, 3, GL_FLOAT);
    }
}

void LineRenderer::RenderWithoutMaterial() const
{
    GL::Render(m_vao, m_drawLinesMode, m_points.Size());
}

void LineRenderer::SetMaterial(Material *m)
{
    Renderer::SetMaterial(m);
    BindPointsToVAO();
}

AABox LineRenderer::GetAABBox() const
{
    if (m_points.Empty()) { return AABox::Empty; }

    Vector3 minp = m_points.Front();
    Vector3 maxp = m_points.Front();
    for (const Vector3 &p : m_points)
    {
        minp.x = Math::Min(minp.x, p.x);
        minp.y = Math::Min(minp.y, p.y);
        minp.z = Math::Min(minp.z, p.z);
        maxp.x = Math::Max(maxp.x, p.x);
        maxp.y = Math::Max(maxp.y, p.y);
        maxp.z = Math::Max(maxp.z, p.z);
    }

    // Add a bit in every dimensions, to avoid flattened Rects
    const float strokeAdd = 0.01f;
    minp.x -= strokeAdd; maxp.x += strokeAdd;
    minp.y -= strokeAdd; maxp.y += strokeAdd;
    minp.z -= strokeAdd; maxp.z += strokeAdd;
    return AABox(minp, maxp);
}

const Array<Vector3> &LineRenderer::GetPoints() const
{
    return m_points;
}

void LineRenderer::Read(const XMLNode *xmlInfo)
{
    Renderer::Read(xmlInfo);
}

void LineRenderer::Write(XMLNode *xmlInfo) const
{
    Renderer::Write(xmlInfo);
    xmlInfo->SetTagName("LineRenderer");
}
