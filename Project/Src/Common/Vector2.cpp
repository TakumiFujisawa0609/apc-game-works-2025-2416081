#include"Vector2.h"

Vector2 Vector2I::operator/(const float value)const { return Vector2(x / value, y / value); }

void Vector2I::operator=(const Vector2& value) { x = (int)value.x; y = (int)value.y; }

Vector2 Vector2I::ToVector2(void) const { return Vector2((float)x, (float)y); }

Vector2 Vector2I::Normalized(void) const { return ToVector2() / Length(); }