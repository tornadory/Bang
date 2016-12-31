#include "InspectorWidget.h"

#include "Debug.h"
#include "Component.h"
#include "GameObject.h"
#include "EditorWindow.h"
#include "AttrWidgetEnum.h"
#include "AttrWidgetFile.h"
#include "AttrWidgetBool.h"
#include "AttrWidgetFloat.h"
#include "AttrWidgetColor.h"
#include "AttributeWidget.h"
#include "AttrWidgetButton.h"
#include "AttrWidgetString.h"
#include "WindowEventManager.h"
#include "AttrWidgetVectorFloat.h"


InspectorWidget::InspectorWidget()
{
}

void InspectorWidget::Init(const String &title, IInspectable *relatedInspectable)
{
    m_relatedInspectable = relatedInspectable;

    XMLNode xmlInfo = GetInspectableXMLInfo();
    ConstructFromWidgetXMLInfo(title, xmlInfo);

    setMinimumWidth(40);
    setAcceptDrops(true);
}

void InspectorWidget::ConstructFromWidgetXMLInfo(
        const String &title, XMLNode &xmlInfo, bool autoUpdate)
{
    m_gridLayout = new QGridLayout();
    m_gridLayout->setSpacing(0);

    setLayout(m_gridLayout);

    String fTitle = StringUtils::FormatInspectorLabel(title);
    m_titleLabel = new QLabel( QString(fTitle.ToCString()) );

    QFont font = m_titleLabel->font();
    font.setPixelSize(13);
    font.setBold(true);
    m_titleLabel->setFont(font);

    m_gridLayout->addWidget(m_titleLabel, 0, 0, 2, 1,
                            Qt::AlignLeft | Qt::AlignVCenter);

    CreateWidgetSlots(xmlInfo);
    RefreshWidgetValues(); // Initial catch of values

    m_created = true;
    //

    if (autoUpdate)
    {
        m_updateTimer = new QTimer(this); //Every X seconds, update all the slots values
        connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(RefreshWidgetValues()));
        m_updateTimer->start(150);
    }
}

InspectorWidget::~InspectorWidget()
{
    if (m_updateTimer)
    {
        m_updateTimer->stop();
        delete m_updateTimer;
    }
}

XMLNode InspectorWidget::GetInspectableXMLInfo() const
{
    XMLNode xmlInfo;
    m_relatedInspectable->OnInspectorXMLNeeded(&xmlInfo);
    return xmlInfo;
}

XMLNode InspectorWidget::GetWidgetXMLInfo() const
{
    // Gather all attributes
    XMLNode xmlInfo;
    xmlInfo.SetTagName(m_tagName);
    for (XMLAttribute attribute : m_attributes)
    {
        String attrName = attribute.GetName();
        XMLAttribute::Type attrType = attribute.GetType();

        if (m_attrName_To_AttrWidget.ContainsKey(attrName))
        {
            AttributeWidget *aw = m_attrName_To_AttrWidget[attrName];
            if (attribute.HasVectoredType())
            {
                if (attrType == XMLAttribute::Type::Float)
                {
                    AttrWidgetFloat *wf = static_cast<AttrWidgetFloat*>(aw);
                    float v = wf->GetValue();
                    attribute.SetFloat(v, attribute.GetProperties());
                }
                else
                {
                    AttrWidgetVectorFloat *awv = static_cast<AttrWidgetVectorFloat*>(aw);
                    Array<float> v = awv->GetValue();
                    if (attrType == XMLAttribute::Type::Vector2)
                    {
                        attribute.SetVector2(Vector2(v[0], v[1]), attribute.GetProperties());
                    }
                    else if (attrType == XMLAttribute::Type::Vector3)
                    {
                        attribute.SetVector3(Vector3(v[0], v[1], v[2]), attribute.GetProperties());
                    }
                    else if (attrType == XMLAttribute::Type::Vector4 ||
                             attrType == XMLAttribute::Type::Quaternion ||
                             attrType == XMLAttribute::Type::Rect)
                    {
                        attribute.SetVector4(Vector4(v[0], v[1], v[2], v[3]), attribute.GetProperties());
                    }
                }
            }
            else if (attrType == XMLAttribute::Type::File)
            {
                AttrWidgetFile *awf = static_cast<AttrWidgetFile*>(aw);
                attribute.SetFilepath(awf->GetValue(),
                                      attribute.GetPropertyValue(XMLProperty::FileExtension.GetName()),
                                      attribute.GetProperties());
            }
            else if (attrType == XMLAttribute::Type::String)
            {
                AttrWidgetString *aws = static_cast<AttrWidgetString*>(aw);
                attribute.SetString(aws->GetValue(), attribute.GetProperties());
            }
            else if (attrType == XMLAttribute::Type::Bool)
            {
                AttrWidgetBool *wb = static_cast<AttrWidgetBool*>(aw);
                attribute.SetBool(wb->GetValue(), attribute.GetProperties());
            }
            else if (attrType == XMLAttribute::Type::Enum)
            {
                AttrWidgetEnum *awe = static_cast<AttrWidgetEnum*>(aw);
                attribute.SetEnum(attribute.GetEnumNames(), awe->GetValue(), // selected index
                                  attribute.GetProperties());
            }
            else if (attrType == XMLAttribute::Type::Color)
            {
                AttrWidgetColor *awc = static_cast<AttrWidgetColor*>(aw);
                attribute.SetColor(awc->GetValue(), attribute.GetProperties());
            }
        }
        xmlInfo.SetAttribute(attribute);
    }

    return xmlInfo;
}

QGridLayout *InspectorWidget::GetGridLayout() const
{
    return m_gridLayout;
}

int InspectorWidget::GetNextRowIndex() const
{
    return m_gridLayout->rowCount();
}

void InspectorWidget::SetTitle(const String &title)
{
    m_titleLabel->setText(QString::fromStdString(title));
}

void InspectorWidget::RefreshWidgetValues()
{
    XMLNode xmlInfo = GetInspectableXMLInfo();
    xmlInfo.SetTagName(m_tagName);
    bool hasToRefreshHard = false;
    for (auto itAttr : xmlInfo.GetAttributes())
    {
        XMLAttribute attribute = itAttr.second;
        String attrName  = attribute.GetName();
        if( m_attrName_To_AttrWidget.ContainsKey(attrName))
        {
            AttributeWidget *ws = m_attrName_To_AttrWidget[attrName];
            hasToRefreshHard = hasToRefreshHard ||
                    (ws->isHidden() ^ attribute.HasProperty(XMLProperty::Hidden));
            ws->Refresh(attribute);
        }
    }

    if (hasToRefreshHard)
    {
        Inspector::GetInstance()->RefreshHard();
    }
}

void InspectorWidget::CreateWidgetSlots(XMLNode &xmlInfo)
{
    m_tagName = xmlInfo.GetTagName();

    for (auto itAttr : xmlInfo.GetAttributes())
    {
        XMLAttribute attribute = itAttr.second;
        m_attributes.PushBack(attribute);
        AttributeWidget *w = AttributeWidget::FromXMLAttribute(attribute, this);
        if (w)
        {
            m_attrName_To_AttrWidget[attribute.GetName()] = w;
            w->adjustSize();
        }
    }
}

void InspectorWidget::_OnSlotValueChanged()
{
    NONULL(m_relatedInspectable);
    if (m_created)
    {
        XMLNode xmlInfo = GetWidgetXMLInfo();
        m_relatedInspectable->OnInspectorXMLChanged(&xmlInfo);
    }
    WindowEventManager::GetInstance()->NotifyInspectorSlotChanged(this);
}

void InspectorWidget::_OnSlotValueChanged(int _)
{
    _OnSlotValueChanged();
}
void InspectorWidget::_OnSlotValueChanged(double _)
{
    _OnSlotValueChanged();
}
void InspectorWidget::_OnSlotValueChanged(QString _)
{
    _OnSlotValueChanged();
}
