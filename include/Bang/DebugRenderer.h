#ifndef DEBUGRENDERER_H
#define DEBUGRENDERER_H

#include "Bang/Set.h"
#include "Bang/Rect.h"
#include "Bang/Time.h"
#include "Bang/Color.h"
#include "Bang/Vector3.h"
#include "Bang/GameObject.h"

NAMESPACE_BANG_BEGIN

FORWARD class LineRenderer;

class DebugRenderer : public GameObject
{
    GAMEOBJECT(DebugRenderer);

public:
    static void Clear();

    static void RenderLine(const Vector3 &origin,
                           const Vector3 &end,
                           const Color &color = Color::Green,
                           float time = 1.0f,
                           float thickness = 1.0f,
                           bool depthTest = false);

    static void RenderPoint(const Vector3 &point,
                            const Color &color = Color::Green,
                            float time = 1.0f,
                            float thickness = 1.0f,
                            bool depthTest = false);

    static void RenderRectPx(const Rect &rect,
                             const Color &color = Color::Green,
                             float time = 1.0f,
                             float thickness = 1.0f,
                             bool depthTest = false);

private:
    enum class DebugRendererPrimitiveType { Point, Line, RectPx };
    struct DebugRenderPrimitive
    {
        DebugRendererPrimitiveType primitive;
        Vector3 origin;
        Vector3 end;
        Rect rectNDC;
        Color color;
        float thickness;
        double destroyTimestamp;
        bool depthTest;
        bool renderedOnce;
    };

    List<DebugRenderPrimitive> m_primitivesToRender;

	DebugRenderer();
	virtual ~DebugRenderer();

    void Render(bool withDepth);

    static DebugRenderPrimitive*
           CreateDebugRenderPrimitive(DebugRendererPrimitiveType primitive,
                                      const Array<Vector3> &points,
                                      const Color &color,
                                      float time,
                                      float thickness,
                                      bool depthTest);

    static DebugRenderer *GetActive();

    friend class Scene;
};

NAMESPACE_BANG_END

#endif // DEBUGRENDERER_H

