#include "Vector2.h"

#include"../Utility/Utility.h"

// コンストラクタ
Vector2I::Vector2I(void)
{
	x = 0;
	y = 0;
}

// コンストラクタ
Vector2I::Vector2I(int vX, int vY)
{
	x = vX;
	y = vY;
}

Vector2I::~Vector2I(void)
{
}

void Vector2I::operator=(const int value)
{
	x = value; y = value;
}

Vector2I Vector2I::operator+(const Vector2I value)const
{
	return Vector2I(x + value.x, y + value.y);
}

void Vector2I::operator+=(const Vector2I value)
{
	x = x + value.x;
	y = y + value.y;
}

Vector2I Vector2I::operator+(const int value) const
{
	return Vector2I(x + value, y + value);
}

void Vector2I::operator+=(const int value)
{
	x += value; y += value;
}

Vector2I Vector2I::operator-(const Vector2I value)const
{
	return Vector2I(x - value.x, y - value.y);
}

void Vector2I::operator-=(const Vector2I value)
{
	x = x - value.x;
	y = y - value.y;
}

Vector2I Vector2I::operator-(const int value) const
{
	return Vector2I(x - value, y - value);
}

void Vector2I::operator-=(const int value)
{
	x -= value; y -= value;
}

Vector2I Vector2I::operator*(const Vector2I value) const
{
	return Vector2I(x * value.x, y * value.y);
}

void Vector2I::operator*=(const Vector2I value)
{
	x *= value.x; y *= value.y;
}

Vector2I Vector2I::operator*(const int value)const
{
	return Vector2I(x * value, y * value);
}

void Vector2I::operator*=(const int value)
{
	x = x * value;
	y = y * value;
}

Vector2I Vector2I::operator/(const Vector2I value) const
{
	return Vector2I(x / value.x, y / value.y);
}

void Vector2I::operator/=(const Vector2I value)
{
	x /= value.x; y /= value.y;
}

Vector2I Vector2I::operator/(const int value)const
{
	return Vector2I(x / value, y / value);
}

Vector2 Vector2I::operator/(const float value) const
{
	return Vector2(x / value, y / value);
}

void Vector2I::operator/=(const int value)
{
	x = x / value;
	y = y / value;
}

Vector2 Vector2I::ToVector2(void) const
{
	return Vector2((float)x, (float)y);
}

float Vector2I::Length(void)const
{
	return sqrtf((float)(x * x + y * y));
}

Vector2 Vector2I::Normalized(void) const
{
	return ToVector2() / Length();
}


Vector2::Vector2(void)
{
	x = 0.0f;
	y = 0.0f;
}

Vector2::Vector2(float vX, float vY)
{
	x = vX;
	y = vY;
}

Vector2::~Vector2(void)
{
}

void Vector2::operator=(const float value)
{
	x = value; y = value;
}

Vector2 Vector2::operator+(const Vector2 value) const
{
	return Vector2(x + value.x, y + value.y);
}

void Vector2::operator+=(const Vector2 value)
{
	x = x + value.x;
	y = y + value.y;
}

Vector2 Vector2::operator+(const float value) const
{
	return Vector2(x + value, y + value);
}

void Vector2::operator+=(const float value)
{
	x += value;
	y += value;
}

Vector2 Vector2::operator-(const Vector2 value) const
{
	return Vector2(x - value.x, y - value.y);
}

void Vector2::operator-=(const Vector2 value)
{
	x = x - value.x;
	y = y - value.y;
}

Vector2 Vector2::operator-(const float value) const
{
	return Vector2(x - value, y - value);
}

void Vector2::operator-=(const float value)
{
	x -= value;
	y -= value;
}

Vector2 Vector2::operator*(const Vector2 value) const
{
	return Vector2(x * value.x, y * value.y);
}

void Vector2::operator*=(const Vector2 value)
{
	x *= value.x;
	y *= value.y;
}

Vector2 Vector2::operator*(const float value) const
{
	return Vector2(x * value, y * value);
}

void Vector2::operator*=(const float value)
{
	x = x * value;
	y = y * value;
}

Vector2 Vector2::operator/(const Vector2 value) const
{
	return Vector2(x / value.x, y / value.y);
}

void Vector2::operator/=(const Vector2 value)
{
	x /= value.x;
	y /= value.y;
}

Vector2 Vector2::operator/(const float value) const
{
	return Vector2(x / value, y / value);
}

void Vector2::operator/=(const float value)
{
	x = x / value;
	y = y / value;
}

bool Vector2::operator==(const Vector2 value) const
{
	return (x == value.x && y == value.y);
}

bool Vector2::operator==(const float value) const
{
	return (x == value && y == value);
}

bool Vector2::operator!=(const Vector2 value) const
{
	return (x != value.x || y != value.y);
}

bool Vector2::operator!=(const float value) const
{
	return  (x != value || y != value);
}

Vector2I Vector2::ToVector2I(void) const
{
	return Vector2I(Utility::Round(x), Utility::Round(y));
}

float Vector2::Length(void) const
{
	return sqrtf(x * x + y * y);
}

Vector2 Vector2::Normalized(void) const
{
	return Vector2(x, y) / Length();
}

void Vector2::Normalize(void)
{
	float len = Length();
	x /= len; y /= len;
}
