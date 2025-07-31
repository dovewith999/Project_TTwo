#pragma once
//색상
enum class Color
{
	Blue = 1,
	Green = 2,
	Red = 4,
	White = Red | Green | Blue, // 7
	Intensity = 8,
};