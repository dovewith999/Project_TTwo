#include "Vector2.h"
#include <iostream>

// 정적 멤버 변수 초기화
Vector2 Vector2::Zero = Vector2(0, 0);
Vector2 Vector2::One = Vector2(1, 0);
Vector2 Vector2::Up = Vector2(0, 1);
Vector2 Vector2::Right = Vector2(1, 0);

Vector2::Vector2(int x, int y)
    : x(x), y(y)
{
}

Vector2::~Vector2()
{
    Free();
}

Vector2 Vector2::operator+(const Vector2& other) const
{
    return Vector2(x + other.x, y + other.y);
}

Vector2 Vector2::operator-(const Vector2& other) const
{
    return Vector2(x - other.x, y - other.y);
}

bool Vector2::operator==(const Vector2& other) const
{
    return (this->x == other.x) && (this->y == other.y);
}

const char* Vector2::ToString()
{
    // 좌표를 문자열로 출력하고 싶음 "(20, 30)"
    // 문자열 조합을 해야됨
    // C스타일로 진행

    Free();

    value = new char[1024];
    sprintf_s(value, 1024, "(%d, %d)", x, y);

    return value;
}

void Vector2::Free()
{
    if (value == nullptr)
    {
        return;
    }

    delete[] value;
    value = nullptr;
}
