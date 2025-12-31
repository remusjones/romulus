#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct Color
{
    float R{0.0f};
    float G{0.0f};
    float B{0.0f};
    float A{1.0f};

    constexpr Color() = default;

    constexpr Color(float r, float g, float b, float a = 1.0f) noexcept
        : R(r), G(g), B(b), A(a) {}

    constexpr explicit Color(float gray, float a = 1.0f) noexcept
        : R(gray), G(gray), B(gray), A(a) {}

    explicit Color(const glm::vec3& vec, float a = 1.0f) noexcept
        : R(vec.x), G(vec.y), B(vec.z), A(a) {}

    explicit Color(const glm::vec4& vec) noexcept
        : R(vec.x), G(vec.y), B(vec.z), A(vec.w) {}


    [[nodiscard]] glm::vec3 ToVec3() const noexcept { return {R, G, B}; }

    [[nodiscard]] glm::vec4 ToVec4() const noexcept { return {R, G, B, A}; }

    operator glm::vec4() const noexcept { return {R, G, B, A}; }
    operator glm::vec3() const noexcept { return {R, G, B}; }

    constexpr Color operator+(const Color& other) const noexcept
    {
        return {R + other.R, G + other.G, B + other.B, A + other.A};
    }

    constexpr Color operator-(const Color& other) const noexcept
    {
        return {R - other.R, G - other.G, B - other.B, A - other.A};
    }

    constexpr Color operator*(float scalar) const noexcept
    {
        return {R * scalar, G * scalar, B * scalar, A * scalar};
    }

    constexpr Color operator*(const Color& other) const noexcept
    {
        return {R * other.R, G * other.G, B * other.B, A * other.A};
    }

    bool operator==(const Color& other) const noexcept
    {
        return R == other.R && G == other.G && B == other.B && A == other.A;
    }

    bool operator!=(const Color& other) const noexcept
    {
        return !(*this == other);
    }

    static constexpr Color White()      { return {1.0f, 1.0f, 1.0f}; }
    static constexpr Color Black()      { return {0.0f, 0.0f, 0.0f}; }
    static constexpr Color Red()        { return {1.0f, 0.0f, 0.0f}; }
    static constexpr Color Green()      { return {0.0f, 1.0f, 0.0f}; }
    static constexpr Color Blue()       { return {0.0f, 0.0f, 1.0f}; }
    static constexpr Color Yellow()     { return {1.0f, 1.0f, 0.0f}; }
    static constexpr Color Cyan()       { return {0.0f, 1.0f, 1.0f}; }
    static constexpr Color Magenta()    { return {1.0f, 0.0f, 1.0f}; }
    static constexpr Color Gray()       { return {0.5f, 0.5f, 0.5f}; }
    static constexpr Color LightGray()  { return {0.75f, 0.75f, 0.75f}; }
    static constexpr Color DarkGray()   { return {0.25f, 0.25f, 0.25f}; }
    static constexpr Color Orange()     { return {1.0f, 0.5f, 0.0f}; }
    static constexpr Color Brown()      { return {0.6f, 0.4f, 0.2f}; }
    static constexpr Color Pink()       { return {1.0f, 0.6f, 0.6f}; }
    static constexpr Color Purple()     { return {0.5f, 0.0f, 1.0f}; }
    static constexpr Color Transparent(){ return {0.0f, 0.0f, 0.0f, 0.0f}; }
};