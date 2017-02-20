#ifndef RECTTRANSFORM_H
#define RECTTRANSFORM_H

#include "Rect.h"
#include "Vector2.h"
#include "Matrix4.h"
#include "Transform.h"

class XMLNode;
class RectTransform : public Transform
{
/**
  (-1,-1): left-top.
  ( 1, 1): right-bot.
**/
public:
    RectTransform();
    virtual ~RectTransform();

    void CloneInto(ICloneable *clone) const;
    ICloneable *Clone() const;

    void SetMarginLeft(int marginLeft);
    void SetMarginTop(int marginTop);
    void SetMarginRight(int marginRight);
    void SetMarginBot(int marginBot);
    void SetMargins(int marginAll);
    void SetMargins(int left, int top, int right, int bot);

    void SetPivotPosition(const Vector2 &pivotPosition);
    void SetAnchorMin(const Vector2 &anchorMin);
    void SetAnchorMax(const Vector2 &anchorMax);
    void SetAnchors(const Vector2 &anchorMin, const Vector2 &anchorMax);

    int GetMarginLeft()  const;
    int GetMarginTop()   const;
    int GetMarginRight() const;
    int GetMarginBot()   const;
    Vector2 GetMarginLeftBot() const;
    Vector2 GetMarginRightTop() const;

    Vector2 GetPivotPosition()  const;
    Vector2 GetAnchorMin()  const;
    Vector2 GetAnchorMax() const;

    virtual void ReadXMLInfo(const XMLNode *xmlInfo) override;
    virtual void FillXMLInfo(XMLNode *xmlInfo) const override;

    Rect GetScreenSpaceRect(bool takeMarginsIntoAccount) const;

    Rect GetParentScreenRect(bool takeMarginsIntoAccount) const;
    virtual const Matrix4& GetLocalToParentMatrix() const override;
    Matrix4 GetLocalToParentMatrix(bool takeMarginsIntoAccount) const;

    void OnParentSizeChanged();

private:
    Vector2 m_anchorScale = Vector2(1.0f, -1.0f);

    int m_marginLeft  = 0;
    int m_marginTop   = 0;
    int m_marginRight = 0;
    int m_marginBot   = 0;

    Vector2 m_pivotPosition = Vector2::Zero;
    Vector2 m_anchorMin     = Vector2(-1.0f);
    Vector2 m_anchorMax     = Vector2( 1.0f);

    void OnChanged();

    friend class Screen;
    friend class GameObject;
};

#endif // RECTTRANSFORM_H