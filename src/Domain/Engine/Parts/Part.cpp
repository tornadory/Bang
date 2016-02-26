#include "Part.h"
#include "Entity.h"


Part::Part() : owner(nullptr)
{
}

Part::~Part()
{

}

Entity *Part::GetOwner() const
{
    return owner;
}

const std::string Part::ToString() const
{
    std::ostringstream msg;
    msg << "Part";
    return msg.str();
}

#ifdef BANG_EDITOR
void Part::OnInspectorSlotChanged()
{
    Logger_Log("OnInspectorSlotChanged !!!");
}
#endif