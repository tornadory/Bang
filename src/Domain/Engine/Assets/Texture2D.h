﻿#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "Bang.h"

#include <string>
#include <vector>
#include <stack>

#include "glm/glm.hpp"

#include "FileReader.h"
#include "Texture.h"
#include "Logger.h"
#include "Asset.h"

class Texture2D : public Texture, public Asset
{

public:
    const static std::string GetFileExtension() { return "btex2d"; }

public:
    Texture2D();
    Texture2D(const std::string &filepath);
    virtual ~Texture2D();

    void LoadFromFile(const std::string &filepath);
    void CreateEmpty(int width, int height) override;
    void Resize(int width, int height) override;
    void Fill(unsigned char *newData, int width, int height, int numComponents);

#ifdef BANG_EDITOR
    void Write(std::ostream &f) const;
    void Read(std::istream &f);
#endif
};

#endif // TEXTURE2D_H
