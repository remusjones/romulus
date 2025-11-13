//
// Created by Remus on 27/01/2024.
//

#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>


class Color
{
public:
	Color(const float r, const float g, const float b, const float a) : R(r), G(g), B(b), A(a)
	{
	}

	static Color RGBA(const float r, const float g, const float b, const float a)
	{
		return Color(r, g, b, a);
	}

	static Color RGB(const float r, const float g, const float b)
	{
		return Color(r, g, b, 1.0f);
	}

	static glm::vec3 RGB(const Color& aColor)
	{
		return glm::vec3(aColor.R, aColor.G, aColor.B);
	}

	static glm::vec4 RGBA(const Color& aColor)
	{
		return {aColor.R, aColor.G, aColor.B, aColor.A};
	}

	glm::vec3 RGB() const
	{
		return {R, G, B};
	}

	glm::vec4 RGBA() const
	{
		return {R, G, B, A};
	}

	static Color White()
	{
		return Color(1.0f, 1.0f, 1.0f, 1.0f);
	}

	static Color Black()
	{
		return Color(0.0f, 0.0f, 0.0f, 1.0f);
	}

	static Color Red()
	{
		return Color(1.0f, 0.0f, 0.0f, 1.0f);
	}

	static Color Green()
	{
		return Color(0.0f, 1.0f, 0.0f, 1.0f);
	}

	static Color Blue()
	{
		return Color(0.0f, 0.0f, 1.0f, 1.0f);
	}

	static Color Yellow()
	{
		return Color(1.0f, 1.0f, 0.0f, 1.0f);
	}

	static Color Cyan()
	{
		return Color(0.0f, 1.0f, 1.0f, 1.0f);
	}

	static Color Magenta()
	{
		return Color(1.0f, 0.0f, 1.0f, 1.0f);
	}

	static Color Gray()
	{
		return Color(0.5f, 0.5f, 0.5f, 1.0f);
	}

	static Color LightGray()
	{
		return Color(0.75f, 0.75f, 0.75f, 1.0f);
	}

	static Color DarkGray()
	{
		return Color(0.25f, 0.25f, 0.25f, 1.0f);
	}

	static Color Orange()
	{
		return Color(1.0f, 0.5f, 0.0f, 1.0f);
	}

	static Color Brown()
	{
		return Color(0.6f, 0.4f, 0.2f, 1.0f);
	}

	static Color Pink()
	{
		return Color(1.0f, 0.6f, 0.6f, 1.0f);
	}

	static Color Purple()
	{
		return Color(0.5f, 0.0f, 1.0f, 1.0f);
	}

	float R;
	float G;
	float B;
	float A;
};
