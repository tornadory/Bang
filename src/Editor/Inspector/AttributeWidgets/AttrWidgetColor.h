#ifndef ATTRWIDGETCOLOR_H
#define ATTRWIDGETCOLOR_H

#include <QColorDialog>
#include "Bang/WinUndef.h"

#include "Bang/Color.h"
#include "Bang/XMLAttribute.h"
#include "Bang/InspectorWidget.h"
#include "Bang/AttributeWidget.h"

class ColorLabel;
class ColorDialog;
class AttrWidgetColor : public AttributeWidget
{
    Q_OBJECT

private:

    ColorDialog *m_colorDialog = nullptr;
    ColorLabel *m_colorLabel = nullptr;

    QHBoxLayout *m_alphaHorizontalLayout = nullptr;
    ColorLabel *m_alphaWhiteLabel = nullptr;
    ColorLabel *m_alphaBlackLabel = nullptr;

    Color m_selectedColor = Color::White;

    void RefreshLabelColor();

public:

    AttrWidgetColor(const XMLAttribute &xmlAttribute);
    virtual ~AttrWidgetColor();

    void SetValue(const Color &c);
    const Color& GetValue() const;

    virtual void Refresh(const XMLAttribute &attribute) override;
    virtual XMLAttribute GetXMLAttribute() const override;

private slots:

    void OnColorChanged(const QColor &c);
};

//////////////////////////////////////////////////


class ColorDialog : public QColorDialog
{
    Q_OBJECT

public:
    explicit ColorDialog(QWidget *parent = nullptr);
};

//////////////////////////////////////////////////

class ColorLabel : public QLabel
{
    Q_OBJECT

private:
    ColorDialog *m_colorDialog = nullptr;
    const Color *m_selectedColor = nullptr;

public:
    explicit ColorLabel(QWidget *parent,
                        ColorDialog *colorDialog,
                        const Color *selectedColor);

    void SetColor(const Color &c);

    virtual void mousePressEvent(QMouseEvent *e) override;
};

#endif // ATTRWIDGETCOLOR_H
