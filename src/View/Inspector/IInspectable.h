#ifndef IINSPECTABLE_H
#define IINSPECTABLE_H

#include "InspectorPartInfo.h"
#include "InspectorPartWidget.h"

class IInspectable
{
protected:
    IInspectable() {}

public:

    InspectorPartInfo inspectorPartInfo;
    virtual InspectorPartInfo* GetPartInfo()
    {
        return &inspectorPartInfo;
    }
};

#endif // IINSPECTABLE_H