#pragma once
//색상
enum class Color
{
	Blue = 1,
	Green = 2,
	Cyan = 3,           // Blue | Green
	Red = 4,
	Purple = 5,         // Red | Blue  
	Yellow = 6,         // Red | Green
	White = 7,          // Red | Green | Blue
	Intensity = 8,
	DarkGray = 8,       // Intensity만 (어두운 회색)
	LightBlue = 9,      // Blue | Intensity
	LightGreen = 10,    // Green | Intensity
	LightCyan = 11,     // Blue | Green | Intensity
	LightRed = 12,      // Red | Intensity
	LightPurple = 13,   // Red | Blue | Intensity
	Orange = 14,        // Red | Green | Intensity (밝은 노란색/오렌지)
	BrightWhite = 15,   // Red | Green | Blue | Intensity
};