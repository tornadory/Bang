#include "Prefab.h"

Prefab::Prefab()
{

}

Prefab::Prefab(const Prefab &p)
{
    this->assetDescription = p.assetDescription;
}

Prefab::Prefab(Entity *e)
{
    if(e != nullptr)
    {
        std::ostringstream oss;
        e->Write(oss);
        assetDescription = oss.str();
    }
}

Prefab::Prefab(const std::string &assetDescription)
{
    this->assetDescription = assetDescription;
}

Entity *Prefab::Instantiate() const
{
    Entity *e = InstantiateWithoutStarting();
    if(e != nullptr)
    {
        e->_OnStart();
    }
    return e;
}

Entity *Prefab::InstantiateWithoutStarting() const
{
    if(assetDescription != "")
    {
        std::istringstream iss (assetDescription);
        Entity *e = new Entity();
        e->Read(iss);
        return e;
    }
    return nullptr;
}

#ifdef BANG_EDITOR
void Prefab::Write(std::ostream &f) const
{
    Entity *e = InstantiateWithoutStarting();
    if(e != nullptr)
    {
        e->Write(f);
        delete e;
    }
}

void Prefab::Read(std::istream &f)
{
    //Copy contents of the read file in assetDescription,
    //to be able to use it from Instantiate()
    std::string line;
    assetDescription = "";
    while( FileReader::ReadNextLine(f, &line) )
    {
        assetDescription += line + "\n";
    }
    assetDescription += line + "\n";
}
#endif
