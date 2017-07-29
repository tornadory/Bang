#ifndef XMLNODE_H
#define XMLNODE_H

#include "Bang/Map.h"
#include "Bang/List.h"
#include "Bang/Debug.h"
#include "Bang/IToString.h"
#include "Bang/XMLAttribute.h"

class XMLNode : public IToString
{
public:
    XMLNode(const String &tagName = "");
    virtual ~XMLNode();

    void CloneInto(XMLNode *xmlNode) const;

    void AddChild(const XMLNode &node);

    void UpdateAttributeValue(const String &attributeName,
                              const String &newAttributeValue);

    void Set(const XMLAttribute &attribute);
    void Set(const String &attributeName,
             const String &attributeValue,
             const Array<XMLProperty>& properties = {});

    template <class T>
    void Set(const String &attributeName, const T& value,
             const Array<XMLProperty>& properties = {})
    {
        Debug_Warn("Set type not supported for '" << attributeName << "'");
    }

    template<class T>
    T Get(const String &attributeName) const
    {
        XMLAttribute *attr = GetAttribute(attributeName);
        return attr ? attr->Get<T>() : T();
    }

    void RemoveAttribute(const String& attributeName);
    XMLAttribute* GetAttribute(const String& attributeName) const;
    String GetAttributeValue(const String& attributeName) const;

    const XMLNode *GetChild(const String &name) const;
    void SetTagName(const String tagName);
    String ToString() const override;
    String ToString(const String& indent) const;

    const String& GetTagName() const;
    const Map<String, XMLAttribute>& GetAttributes() const;
    const List<String>& GetAttributesOrderList() const;
    List< std::pair<String, XMLAttribute*> > GetAttributesListInOrder() const;
    const List<XMLNode>& GetChildren() const;
    List<XMLNode>& GetChildren();

    static XMLNode FromString(const String &xml);

private:
    String m_tagName = "";
    mutable List<String> m_attributeOrder;
    mutable Map<String, XMLAttribute> m_attributes;
    List<XMLNode> m_children;
};

#endif // XMLNODE_H
