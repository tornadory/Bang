#ifndef NAMEDENUM_H
#define NAMEDENUM_H

#include "Bang/Map.h"
#include "Bang/Array.h"
#include "Bang/String.h"

static Array<String> VariadicStringToNamesArray(const String &_va_args_)
{
    // We receive something like "Wololo, Apple = 49, Pear=29, Lololo=2193, Banana,Sandwich, Monkey=32"
    // We want this vector: ["Wololo", "Apple", "Pear", "Lololo", "Banana", "Sandwich", "Monkey"]
    Array<String> result = _va_args_.Split(',', true);
    for (int i = 0; i < result.Size(); ++i)
    {
        const Array<String> parts = result[i].Split('=', true);
        if (!parts.IsEmpty()) { result[i] = parts[0]; }
    }
    return result;
}

template <class EnumName>
static Array<EnumName> VariadicStringToValuesArray(const String &_va_args_)
{
    // We receive something like "Wololo, Apple = 49, Pear=29, Lololo=2193, Banana,Sandwich, Monkey=32"
    // We want this vector: [0, 49, 29, 2193, 2194, 2195, 32]
    Array<EnumName> result;
    Array<String> splitted = _va_args_.Split(',', true);
    int lastValue = -1;
    for (String str : splitted)
    {
        str = str.Trim();
        Array<String> equalSplitted = str.Split('=', true);
        int val;
        if (equalSplitted.Size() == 2) // Has value
        {
            val = std::atoi(equalSplitted[1].ToCString());
        }
        else // Has no value, one more than the last one
        {
            val = lastValue + 1;
        }

        lastValue = val;
        result.PushBack(static_cast<EnumName>(val));
    }
    return result;
}

/*
 * NOT SUPPORTING ENUMS WITH #define VALUES SUCH AS:
 *
    NamedEnum(FilterMode,
        Nearest = GL_NEAREST,
        Linear = GL_LINEAR
    );
 *
**/

#define NamedEnum(EnumName, ...) \
\
enum class EnumName { __VA_ARGS__ }; \
\
/* Returns the name of the enum itself. */ \
static const String EnumName##_GetEnumName() { \
    return #EnumName; \
} \
\
/* Returns the array of names of the enum. */ \
static const Array<String>& EnumName##_GetNamesVector() { \
    static const Array<String> names = VariadicStringToNamesArray(#__VA_ARGS__); \
    return names; \
} \
\
/* Returns the array of values of the enum. */ \
static const Array<EnumName>& EnumName##_GetValuesVector() { \
    static const Array<EnumName> values = /* Holds the user values of each Enum entry */ \
                VariadicStringToValuesArray<EnumName>(#__VA_ARGS__); \
    return values; \
} \
\
/* For every enum value, it maps to the index inside the Enum. */ \
static Map<EnumName,int>& EnumName##_GetEnumValueToIndexMap() { \
    static bool mapInit = false; \
    static Map<EnumName,int> enumValueToIndex; \
    if (!mapInit) \
    { \
        const Array<String>& names = EnumName##_GetNamesVector(); \
        const Array<EnumName>& values = EnumName##_GetValuesVector(); \
        for (int i = 0; i < names.Size(); ++i) \
        { \
            enumValueToIndex[values[i]] = i;  \
        } \
        mapInit = true; \
    } \
    return enumValueToIndex; \
} \
\
/* Returns the number of fields of the enum. */ \
static inline int EnumName##_GetSize() { \
    return EnumName##_GetNamesVector().Size(); \
} \
\
/* Given an index return its value in the enum. */ \
static String EnumName##_GetValueByIndex(int index) { \
    return EnumName##_GetNamesVector()[index]; \
} \
\
/* Given an index return its name in the enum. */ \
static String EnumName##_GetNameByIndex(int index) { \
    return EnumName##_GetNamesVector()[index]; \
} \
\
/* Given a value, return its index in the enum. */ \
static int EnumName##_GetIndexFromValue(EnumName value) { \
    Map<EnumName, int>& enumValueToIndex = EnumName##_GetEnumValueToIndexMap(); \
    return enumValueToIndex[value]; \
} \
\
/* Given a value, return its name in the enum. */ \
static String EnumName##_GetNameByValue(EnumName enumValue) { \
    const Array<String>& names = EnumName##_GetNamesVector(); \
    return names[EnumName##_GetIndexFromValue(enumValue)]; \
} \
\
/* Given a value as an int, return its name in the enum. */ \
static String EnumName##_ToString(int v) { \
    return  EnumName##_ToString(static_cast<EnumName>(v)); \
} \
\
/* Given a value, return its name in the enum. */ \
static String EnumName##_ToString(EnumName v) { \
    return EnumName##_GetNameByValue(v); \
} \
\
/* Given a string, return its value in the enum. */ \
static EnumName EnumName##_FromString(const String &name) { \
    const Array<EnumName>& values = EnumName##_GetValuesVector(); \
    for (int i = 0; i < EnumName##_GetSize(); ++i) { \
        if (name == EnumName##_GetNameByIndex(i)) { return static_cast<EnumName>(values[i]); } \
    } \
    return static_cast<EnumName>(0); \
}

#endif // NAMEDENUM_H
