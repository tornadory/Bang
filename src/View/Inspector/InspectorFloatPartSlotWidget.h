#ifndef INSPECTORFLOATPARTSLOTWIDGET_H
#define INSPECTORFLOATPARTSLOTWIDGET_H

#include <string>
#include <QTimer>
#include <QLabel>
#include <QFocusEvent>
#include <QVBoxLayout>
#include <QDoubleSpinBox>

#include "InspectorPartSlotWidget.h"

class FloatPartSlotSpinBox;

template <>
class InspectorPartSlotWidget<float>  : public InspectorPartSlotWidgetBase //Slot for a float (label + float)
{
    Q_OBJECT

private:
    FloatPartSlotSpinBox *spinbox = nullptr;
    bool editing =false;

public:
    InspectorPartSlotWidget(InspectorPartWidget *parent,
                            const std::string &labelString,
                            float value);

    virtual void SetValue(float f);
    virtual float GetValue();

    virtual void OnSpinBoxFocusIn();
    virtual void OnSpinBoxFocusOut();
    QSize sizeHint() const override;
};

class FloatPartSlotSpinBox : public QDoubleSpinBox //Slot for a float (label + float)
{
    Q_OBJECT

public:
    FloatPartSlotSpinBox() : QDoubleSpinBox()
    {
        connect(this, SIGNAL(valueChanged(double)), this, SLOT(AdjustStep(double)));
        AdjustStep(value());
    }

    virtual void focusInEvent(QFocusEvent * event) override
    {
        QDoubleSpinBox::focusInEvent(event);
        QTimer::singleShot(50, this, SLOT(SelectAll()));
        static_cast<InspectorPartSlotWidget<float>*>(parent())->OnSpinBoxFocusIn();
    }

    virtual void focusOutEvent(QFocusEvent * event) override
    {
        QDoubleSpinBox::focusOutEvent(event);
        static_cast<InspectorPartSlotWidget<float>*>(parent())->OnSpinBoxFocusOut();
    }

    virtual void  keyPressEvent(QKeyEvent *event) override
    {
        QDoubleSpinBox::keyPressEvent(event);
        if(event->key() == QKeyEvent::Enter)
        {
            static_cast<InspectorPartSlotWidget<float>*>(parent())->OnSpinBoxFocusOut();
        }
    }

public slots:
    void AdjustStep(double v)
    {
        if(abs(v) <= 1.0f) setSingleStep(0.1f);
        else setSingleStep( pow(10.0, int(log10(v == 0.0f ? 0.1f : abs(v)))-1) );
    }

    void SelectAll() { selectAll(); }
};

#endif // INSPECTORFLOATPARTSLOTWIDGET_H
