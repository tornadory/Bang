#ifndef FONT_H
#define FONT_H

#include "Bang/Asset.h"
#include "Bang/G_Font.h"

class Font : public G_Font,
             public Asset
{
	OBJECT(Font)

public:
    Font();
    virtual ~Font();

    static String GetFileExtensionStatic();
    virtual String GetFileExtension() const override ;

    virtual void Read(const XMLNode &xmlInfo) override;
    virtual void Write(XMLNode *xmlInfo) const override;

private:
    Path m_fontFilepath;
};

#endif // FONT_H