#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include "Bang.h"
#include "Component.h"

class Behaviour : public Component
{
private:
    std::string filepath = "";

public:
    Behaviour();

    virtual const std::string ToString() const override;
    virtual std::string GetName() const override { return "Behaviour"; }

    #ifdef BANG_EDITOR
        virtual InspectorWidgetInfo* GetComponentInfo() override;
        virtual void OnSlotValueChanged(InspectorWidget *source) override;
        void Write(std::ostream &f) const override;
        void Read(std::istream &f) override;
    #endif

    void SetFilepath(const std::string &filepath);
    std::string GetFilepath() const;
};

#endif