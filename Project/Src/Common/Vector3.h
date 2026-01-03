#pragma once

#include<DxLib.h>
#include<algorithm>
#include<cmath>

struct Vector3
{
	float x = 0.0f, y = 0.0f, z = 0.0f;

#pragma region コンストラクタ
	// Vector3生成
	Vector3(void) :x(0.0f), y(0.0f), z(0.0f) {}

	// Vector3生成
	Vector3(float x, float y, float z) :x(x), y(y), z(z) {}

	// Vector3生成
	Vector3(float value) :x(value), y(value), z(value) {}

	// Vector3生成
	Vector3(const VECTOR& value) :x(value.x), y(value.y), z(value.z) {}

	// Vector3生成
	Vector3(int x, int y, int z) :x((float)x), y((float)y), z((float)z) {}

	// Vector3生成
	Vector3(int value) :x((float)value), y((float)value), z((float)value) {}
#pragma endregion

#pragma region Vector3生成(特殊)
	// Xのみ
	static Vector3 Xonly(float value) { return Vector3(value, 0.0f, 0.0f); }

	// Yのみ
	static Vector3 Yonly(float value) { return Vector3(0.0f, value, 0.0f); }

	// Zのみ
	static Vector3 Zonly(float value) { return Vector3(0.0f, 0.0f, value); }

	// XYのみ
	static Vector3 XYonly(float x, float y) { return Vector3(x, y, 0.0f); }

	// XZのみ
	static Vector3 XZonly(float x, float z) { return Vector3(x, 0.0f, z); }

	// YZのみ
	static Vector3 YZonly(float y, float z) { return Vector3(0.0f, y, z); }
#pragma endregion

	// VECTOR(Dxlib)に変換
	VECTOR ToVECTOR(void)const { return VGet(x, y, z); }

#pragma region 演算
	// 加算
	Vector3 operator+(const Vector3& value)const { return Vector3(x + value.x, y + value.y, z + value.z); }
	Vector3 operator+(const VECTOR& value)const { return Vector3(x + value.x, y + value.y, z + value.z); }
	Vector3 operator+(float value)const { return Vector3(x + value, y + value, z + value); }
	void operator+=(const Vector3& value) { x += value.x; y += value.y; z += value.z; }
	void operator+=(const VECTOR& value) { x += value.x; y += value.y; z += value.z; }
	void operator+=(float value) { x += value; y += value; z += value; }

	// 減算
	Vector3 operator-(const Vector3& value)const { return Vector3(x - value.x, y - value.y, z - value.z); }
	Vector3 operator-(const VECTOR& value)const { return Vector3(x - value.x, y - value.y, z - value.z); }
	Vector3 operator-(float value)const { return Vector3(x - value, y - value, z - value); }
	void operator-=(const Vector3& value) { x -= value.x; y -= value.y; z -= value.z; }
	void operator-=(const VECTOR& value) { x -= value.x; y -= value.y; z -= value.z; }
	void operator-=(float value) { x -= value; y -= value; z -= value; }

	// 乗算
	Vector3 operator*(const Vector3& value)const { return Vector3(x * value.x, y * value.y, z * value.z); }
	Vector3 operator*(const VECTOR& value)const { return Vector3(x * value.x, y * value.y, z * value.z); }
	Vector3 operator*(float value)const { return Vector3(x * value, y * value, z * value); }
	void operator*=(const Vector3& value) { x *= value.x; y *= value.y; z *= value.z; }
	void operator*=(const VECTOR& value) { x *= value.x; y *= value.y; z *= value.z; }
	void operator*=(float value) { x *= value; y *= value; z *= value; }

	// 除算
	Vector3 operator/(const Vector3& value)const { return Vector3(x / ((value.x != 0.0f) ? value.x : 1), y / ((value.y != 0.0f) ? value.y : 1), z / ((value.z != 0.0f) ? value.z : 1)); }
	Vector3 operator/(const VECTOR& value)const { return Vector3(x / ((value.x != 0.0f) ? value.x : 1), y / ((value.y != 0.0f) ? value.y : 1), z / ((value.z != 0.0f) ? value.z : 1)); }
	Vector3 operator/(float value)const { return Vector3(x / ((value != 0.0f) ? value : 1), y / ((value != 0.0f) ? value : 1), z / ((value != 0.0f) ? value : 1)); }
	void operator/=(const Vector3& value) { x /= ((value.x != 0.0f) ? value.x : 1); y /= ((value.y != 0.0f) ? value.y : 1); z /= ((value.z != 0.0f) ? value.z : 1); }
	void operator/=(const VECTOR& value) { x /= ((value.x != 0.0f) ? value.x : 1); y /= ((value.y != 0.0f) ? value.y : 1); z /= ((value.z != 0.0f) ? value.z : 1); }
	void operator/=(float value) { x /= ((value != 0.0f) ? value : 1); y /= ((value != 0.0f) ? value : 1); z /= ((value != 0.0f) ? value : 1); }

	// 内積
	float Dot(const Vector3& value)const { return x * value.x + y * value.y + z * value.z; }

	// 外積

#pragma endregion

	bool operator==(const Vector3& value)const { return (x == value.x && y == value.y && z == value.z); }
	bool operator==(const VECTOR& value)const { return (x == value.x && y == value.y && z == value.z); }
	bool operator==(float value)const { return (x == value && y == value && z == value); }

	bool operator!=(const Vector3& value)const { return (x != value.x || y != value.y || z != value.z); }
	bool operator!=(const VECTOR& value)const { return (x != value.x || y != value.y || z != value.z); }
	bool operator!=(float value)const { return (x != value || y != value || z != value); }

	bool operator<(const Vector3& value)const { return (x < value.x && y < value.y && z < value.z); }
	bool operator<(const VECTOR& value)const { return (x < value.x && y < value.y && z < value.z); }
	bool operator<(float value)const { return (x < value && y < value && z < value); }

	bool operator<=(const Vector3& value)const { return (x <= value.x && y <= value.y && z <= value.z); }
	bool operator<=(const VECTOR& value)const { return (x <= value.x && y <= value.y && z <= value.z); }
	bool operator<=(float value)const { return (x <= value && y <= value && z <= value); }

	bool operator>(const Vector3& value)const { return (x > value.x && y > value.y && z > value.z); }
	bool operator>(const VECTOR& value)const { return (x > value.x && y > value.y && z > value.z); }
	bool operator>(float value)const { return (x > value && y > value && z > value); }

	bool operator>=(const Vector3& value)const { return (x >= value.x && y >= value.y && z >= value.z); }
	bool operator>=(const VECTOR& value)const { return (x >= value.x && y >= value.y && z >= value.z); }
	bool operator>=(float value)const { return (x >= value && y >= value && z >= value); }

	// 代入
	void operator=(float value) { x = value; y = value; z = value; }
	void operator=(const Vector3& value) { x = value.x; y = value.y; z = value.z; }
	void operator=(const VECTOR& value) { x = value.x; y = value.y; z = value.z; }
	
	Vector3 operator-(void)const { return Vector3(-x, -y, -z); }

	// ベクトルの大きさ(√なし)
	float LengthSq(void)const { return (x * x + y * y + z * z); }

	// ベクトルの大きさ
	float Length(void)const { return sqrtf(LengthSq()); }

	// 正規化された値を返す
	Vector3 Normalized(void)const { return (*this / Length()); }

	// 正規化する
	void Normalize(void) { *this /= Length(); }

	// 行列で変換
	Vector3 TransMat(const MATRIX& mat)const {
		if (*this == 0.0f) { return Vector3(); }
		return Vector3(VTransform(ToVECTOR(), mat));
	}

	// 行列で変換(自身を変換)
	void TransMatOwn(const MATRIX& mat) {
		if (*this == 0.0f) { return; }
		*this = VTransform(ToVECTOR(), mat);
	}

	// 絶対値を取得する
	Vector3 Abs(void)const { return Vector3(abs(x), abs(y), abs(z)); }

	// 一番大きい要素数を取得
	float MaxElementF(void)const {
		float max = x;
		if (max < y) { max = y; }
		if (max < z) { max = z; }
		return max;
	}

	// 一番小さい要素数を取得
	float MinElementF(void)const {
		float min = x;
		if (min > y) { min = y; }
		if (min > z) { min = z; }
		return min;
	}

	// 一番大きい要素数だけにしたVector3構造体にして返す
	Vector3 MaxElement(void)const {
		if (x >= y && x >= z) { return Vector3(x, 0.0f, 0.0f); }
		else if (y >= z) { return Vector3(0.0f, y, 0.0f); }
		else { return Vector3(0.0f, 0.0f, z); }
	}

	// 一番小さい要素数だけにしたVector3構造体にして返す
	Vector3 MinElement(void)const {
		if (x <= y && x <= z) { return Vector3(x, 0.0f, 0.0f); }
		else if (y <= z) { return Vector3(0.0f, y, 0.0f); }
		else { return Vector3(0.0f, 0.0f, z); }
	}
};