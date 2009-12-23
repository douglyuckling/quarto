/**
 * @file Lights.hpp
 */
#pragma once

#include <Light.hpp>

using peek::Point3f;
using peek::Color;
using peek::Light;

static const int numLights = 3;

static const Light lights[numLights] = {Light(Point3f(12.0f,  6.0f, 12.0f),
											  Color(0.1f, 0.1f, 0.1f),
											  Color(0.5f, 0.5f, 0.5f),
											  Color(1.0f, 1.0f, 1.0f)),
										Light(Point3f(0.0f, -6.0f,  3.9f),
											  Color(0.1f, 0.1f, 0.1f),
											  Color(0.5f, 0.5f, 0.5f),
											  Color(1.0f, 1.0f, 1.0f)),
										Light(Point3f(-6.0f,  0.0f,  2.7f),
											  Color(0.1f, 0.1f, 0.1f),
											  Color(0.5f, 0.5f, 0.5f),
											  Color(1.0f, 1.0f, 1.0f))};
