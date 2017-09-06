#include "Bang/Library.h"

#include <dlfcn.h>

USING_NAMESPACE_BANG

Library::Library(const Path &libPath)
{
    m_libPath = libPath;
}

Library::~Library()
{

}

bool Library::Load()
{
    ClearError();
    m_libHandle = dlopen(GetLibraryPath().GetAbsolute().ToCString(),
                         RTLD_NOW | RTLD_GLOBAL);
    FetchError();
    return !TheresError();
}

bool Library::UnLoad()
{
    ClearError();
    if ( IsLoaded() ) { dlclose(m_libHandle); }
    FetchError();
    return !TheresError();
}

bool Library::IsLoaded() const
{
    return (m_libHandle != nullptr);
}

void *Library::GetSymbol(const String &symbolName)
{
    ClearError();
    void *symbolAddress = dlsym(m_libHandle, symbolName.ToCString());
    FetchError();
    return symbolAddress;
}

const Path &Library::GetLibraryPath() const
{
    return m_libPath;
}

const String& Library::GetErrorString() const
{
    return m_errorString;
}

void Library::ClearError()
{
    dlerror();
    m_errorString = "";
}

void Library::FetchError()
{
    const char *error = dlerror();
    if (error) { m_errorString = String(error); }
    else { m_errorString = ""; }
}

bool Library::TheresError() const
{
    return !m_errorString.IsEmpty();
}
