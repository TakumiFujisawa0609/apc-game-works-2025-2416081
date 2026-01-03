#pragma once

#include<cmath>

#include"Vector3.h"

struct Vector2;

struct Vector2I
{
	int x, y;

#pragma region コンストラクタ
	// Vector2I生成
	Vector2I(void) :x(0), y(0) {}

	// Vector2I生成
	Vector2I(int x, int y) :x(x), y(y) {}

	// Vector2I生成
	Vector2I(int value) :x(value), y(value) {}

	// Vector2I生成
	Vector2I(float x, float y) :x((int)x), y((int)y) {}

	// Vector2I生成
	Vector2I(float value) :x((int)value), y((int)value) {}
#pragma endregion


#pragma region Vector2I生成(特殊)
	// Xのみ
	static Vector2I Xonly(int value) { return Vector2I(value, 0); }

	// Yのみ
	static Vector2I Yonly(int value) { return Vector2I(0, value); }
#pragma endregion

	// 代入
	void operator=(const int value) { x = value; y = value; }
	void operator=(const Vector2& value);

#pragma region 演算
	// 加算---------------------------------------
	Vector2I operator+(const Vector2I value)const { return Vector2I(x + value.x, y + value.y); }
	void operator+=(const Vector2I value) { x = x + value.x; y = y + value.y; }

	Vector2I operator+(const int value)const { return Vector2I(x + value, y + value); }
	void operator+=(const int value) { x += value; y += value; }
	//--------------------------------------------
	
	// 減算---------------------------------------
	Vector2I operator-(const Vector2I value)const { return Vector2I(x - value.x, y - value.y); }
	void operator-=(const Vector2I value) { x = x - value.x; y = y - value.y; }

	Vector2I operator-(const int value)const { return Vector2I(x - value, y - value); }
	void operator-=(const int value) { x -= value; y -= value; }
	//--------------------------------------------

	// 乗算---------------------------------------
	Vector2I operator*(const Vector2I value)const { return Vector2I(x * value.x, y * value.y); }
	void operator*=(const Vector2I value) { x *= value.x; y *= value.y; }

	Vector2I operator*(const int value)const { return Vector2I(x * value, y * value); }
	void operator*=(const int value) { x = x * value; y = y * value; }
	//--------------------------------------------

	// 除算---------------------------------------
	Vector2I operator/(const Vector2I value)const { return Vector2I(x / value.x, y / value.y); }
	void operator/=(const Vector2I value) { x /= value.x; y /= value.y; }

	Vector2I operator/(const int value)const { return Vector2I(x / value, y / value); }

	Vector2 operator/(const float value)const;
	void operator/=(const int value) { x = x / value; y = y / value; }
	//--------------------------------------------
#pragma endregion

	Vector2 ToVector2(void)const;

	float LengthSq(void)const { return (float)(x * x + y * y); }
	float Length(void)const { return sqrtf((float)(x * x + y * y)); }

	Vector2 Normalized(void)const;
};

struct Vector2
{
	float x, y;

#pragma region コンストラクタ
	// Vector2生成
	Vector2(void) :x(0.0f), y(0.0f) {}

	// Vector2生成
	Vector2(float x, float y) :x(x), y(y) {}

	// Vector2生成
	Vector2(float value) :x(value), y(value) {}

	// Vector2生成
	Vector2(int x, int y) :x((float)x), y((float)y) {}

	// Vector2生成
	Vector2(int value) :x((float)value), y((float)value) {}
#pragma endregion


#pragma region Vector2生成(特殊)
	// Xのみ
	static Vector2 Xonly(float value) { return Vector2(value, 0.0f); }

	// Yのみ
	static Vector2 Yonly(float value) { return Vector2(0.0f, value); }
#pragma endregion

	// 代入
	void operator=(const float value) { x = value; y = value; }

#pragma region 演算
	// 加算---------------------------------------
	Vector2 operator+(const Vector2 value)const { return Vector2(x + value.x, y + value.y); }
	void operator+=(const Vector2 value) { x = x + value.x; y = y + value.y; }

	Vector2 operator+(float value)const { return Vector2(x + value, y + value); }
	void operator+=(float value) { x += value; y += value; }

	Vector2 operator+(int value)const { return Vector2(x + (float)value, y + (float)value); }
	void operator+=(int value) { x += (float)value; y += (float)value; }
	//--------------------------------------------

	// 減算---------------------------------------
	Vector2 operator-(const Vector2 value)const { return Vector2(x - value.x, y - value.y); }
	void operator-=(const Vector2 value) { x = x - value.x; y = y - value.y; }

	Vector2 operator-(float value)const { return Vector2(x - value, y - value); }
	void operator-=(float value) { x -= value; y -= value; }

	Vector2 operator-(int value)const { return Vector2(x - (float)value, y - (float)value); }
	void operator-=(int value) { x -= (float)value; y -= (float)value; }
	//--------------------------------------------

	// 乗算---------------------------------------
	Vector2 operator*(const Vector2 value)const { return Vector2(x * value.x, y * value.y); }
	void operator*=(const Vector2 value) { x *= value.x; y *= value.y; }

	Vector2 operator*(float value)const { return Vector2(x * value, y * value); }
	void operator*=(float value) { x = x * value; y = y * value; }

	Vector2 operator*(int value)const { return Vector2(x * (float)value, y * (float)value); }
	void operator*=(int value) { x *= (float)value; y *= (float)value; }
	//--------------------------------------------

	// 除算---------------------------------------
	Vector2 operator/(const Vector2 value)const { return Vector2(x / value.x, y / value.y); }
	void operator/=(const Vector2 value) { x /= value.x; y /= value.y; }

	Vector2 operator/(const int value)const { return Vector2(x / value, y / value); }

	Vector2 operator/(const float value)const { return Vector2(x / value, y / value); }
	void operator/=(const int value) { x = x / value; y = y / value; }
	//--------------------------------------------
#pragma endregion

	bool operator==(const Vector2 value)const { return (x == value.x && y == value.y); }
	bool operator==(const float value)const { return (x == value && y == value); }
	bool operator!=(const Vector2 value)const { return (x != value.x	 || y != value.y); }
	bool operator!=(const float value)const { return  (x != value || y != value); }

	Vector2I ToVector2I(void)const { return Vector2I(x, y); }

	// XZ平面における移動ベクトルなどに使う変換
	Vector3 ToVector3XZ(void)const { return Vector3::XZonly(x, -y); }

	// YXのカメラ回転などに使う変換
	Vector3 ToVector3YX(void)const { return Vector3::XYonly(y, x); }

	float LengthSq(void)const { return (x * x + y * y); }
	float Length(void)const { return sqrtf(x * x + y * y); }

	Vector2 Normalized(void)const { return Vector2(x, y) / Length(); }
	void Normalize(void) { float len = Length(); x /= len; y /= len; }

	// 行列で変換
	Vector2 TransMat(float rot) const
	{
		if (*this == 0.0f) { return Vector2(); }

		float c = cosf(rot);
		float s = sinf(rot);

		return Vector2(
			x * c - y * s,
			x * s + y * c
		);
	}

	// 行列で変換(自身を変換)
	void TransMatOwn(float rot)
	{
		if (*this == 0.0f) { return; }

		float c = cosf(rot);
		float s = sinf(rot);

		float nx = x * c - y * s;
		float ny = x * s + y * c;

		x = nx;
		y = ny;
	}
};