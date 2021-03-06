#ifndef FONTSHEETCREATOR_H
#define FONTSHEETCREATOR_H

#include "Bang/Font.h"
#include "Bang/Image.h"

NAMESPACE_BANG_BEGIN

FORWARD class Texture2D;

class FontSheetCreator
{
public:
    static bool LoadAtlasTexture(TTF_Font *fontFace,
                                 Texture2D *resultTexture,
                                 const String &charsToLoad,
                                 Array<Recti> *imagesOutputRects = nullptr,
                                 int extraMargin = 0);

    static Imageb PackImages(const Array<Imageb> &images, int margin,
                             Array<Recti> *imagesOutputRects = nullptr,
                             const Color &bgColor = Color::Zero);

    FontSheetCreator() = delete;
};

NAMESPACE_BANG_END

#endif // FONTSHEETCREATOR_H
