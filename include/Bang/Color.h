#ifndef COLOR_H
#define COLOR_H

#include "Bang/Bang.h"

NAMESPACE_BANG_BEGIN

class Color
{
public:
    float r, g, b, a;

    Color();
    explicit Color(float m);
    explicit Color(const Vector2& v, float b = 0, float a = 1);
    explicit Color(const Vector3& v, float a = 1);
    explicit Color(const Vector4& v);
    explicit Color(float r, float g, float b);
    explicit Color(float r, float g, float b, float a);
    explicit Color(const Color &c, float a);

    /**
     * @brief If progression == 0, returns v1.
     *        If progression == 1, returns v2.
     *        If 0 < progression < 1, returns a linear interpolation between v1 and v2.
     * @param v1 First Color
     * @param v2 Second Color
     * @param v2 A float between 0 and 1 indicating the progression.
     * @return
     */
    static Color Lerp(const Color &v1,
                      const Color &v2,
                      float progression);

    Color WithAlpha(float alpha) const;
    Color WithValue(float value) const;
    Color WithSaturation(float saturation) const;

    String ToStringRgb() const;
    String ToStringRgb255() const;
    String ToStringRgba() const;
    String ToStringRgba255() const;

    String ToString() const;
    Vector3 ToVector3() const;
    Vector4 ToVector4() const;

    static Color FromVector3(const Vector3 &v);
    static Color FromVector4(const Vector4 &v);

    const static Color Red;
    const static Color Orange;
    const static Color Yellow;
    const static Color Green;
    const static Color Turquoise;
    const static Color VeryLightBlue;
    const static Color LightBlue;
    const static Color Blue;
    const static Color DarkBlue;
    const static Color Purple;
    const static Color Pink;
    const static Color Black;
    const static Color Gray;
    const static Color LightGray;
    const static Color DarkGray;
    const static Color White;
    const static Color Zero;
    const static Color One;
};

Color operator+(float m, const Color& v);
Color operator+(const Color& v, float m);
Color operator+(const Color& v1, const Color& v2);

Color operator-(float m, const Color& v);
Color operator-(const Color& v, float m);
Color operator-(const Color& v1, const Color& v2);
Color operator-(const Color& v);

Color operator*(float m, const Color& v);
Color operator*(const Color& v, float m);
Color operator*(const Color& v1, const Color& v2);

Color operator/(float m, const Color& v);
Color operator/(const Color& v, float m);
Color operator/(const Color& v1, const Color& v2);

bool operator==(const Color& lhs, const Color& rhs);
bool operator!=(const Color& lhs, const Color& rhs);
Color& operator+=(Color& lhs, const Color& rhs);
Color& operator-=(Color& lhs, const Color& rhs);
Color& operator*=(Color& lhs, const Color& rhs);
Color& operator/=(Color& lhs, const Color& rhs);
Color& operator+=(Color& lhs, float m);
Color& operator-=(Color& lhs, float m);
Color& operator*=(Color& lhs, float m);
Color& operator/=(Color& lhs, float m);

NAMESPACE_BANG_END

#endif // COLOR_H
