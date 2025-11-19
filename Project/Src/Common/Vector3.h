#pragma once

#include"../Utility/Utility.h"

struct Vector3
{
	float x = 0.0f, y = 0.0f, z = 0.0f;

	// Vector3ê∂ê¨
	Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f) :x(x), y(y), z(z) {}

	// Vector3ê∂ê¨
	Vector3(const VECTOR& value) :x(value.x), y(value.y), z(value.z) {}

	// VECTOR(Dxlib)Ç…ïœä∑
	VECTOR ToVECTOR(void)const { return VGet(x, y, z); }

#pragma region ââéZ
	// â¡éZ
	Vector3 operator+(const Vector3& value)const { return Vector3(x + value.x, y + value.y, z + value.z); }
	Vector3 operator+(const VECTOR& value)const { return Vector3(x + value.x, y + value.y, z + value.z); }
	Vector3 operator+(float value)const { return Vector3(x + value, y + value, z + value); }
	void operator+=(const Vector3& value) { x += value.x; y += value.y; z += value.z; }
	void operator+=(const VECTOR& value) { x += value.x; y += value.y; z += value.z; }
	void operator+=(float value) { x += value; y += value; z += value; }

	// å∏éZ
	Vector3 operator-(const Vector3& value)const { return Vector3(x - value.x, y - value.y, z - value.z); }
	Vector3 operator-(const VECTOR& value)const { return Vector3(x - value.x, y - value.y, z - value.z); }
	Vector3 operator-(float value)const { return Vector3(x - value, y - value, z - value); }
	void operator-=(const Vector3& value) { x -= value.x; y -= value.y; z -= value.z; }
	void operator-=(const VECTOR& value) { x -= value.x; y -= value.y; z -= value.z; }
	void operator-=(float value) { x -= value; y -= value; z -= value; }

	// èÊéZ
	Vector3 operator*(const Vector3& value)const { return Vector3(x * value.x, y * value.y, z * value.z); }
	Vector3 operator*(const VECTOR& value)const { return Vector3(x * value.x, y * value.y, z * value.z); }
	Vector3 operator*(float value)const { return Vector3(x * value, y * value, z * value); }
	void operator*=(const Vector3& value) { x *= value.x; y *= value.y; z *= value.z; }
	void operator*=(const VECTOR& value) { x *= value.x; y *= value.y; z *= value.z; }
	void operator*=(float value) { x *= value; y *= value; z *= value; }

	// èúéZ
	Vector3 operator/(const Vector3& value)const { return Vector3(x / value.x, y / value.y, z / value.z); }
	Vector3 operator/(const VECTOR& value)const { return Vector3(x / value.x, y / value.y, z / value.z); }
	Vector3 operator/(float value)const { return Vector3(x / value, y / value, z / value); }
	void operator/=(const Vector3& value) { x /= value.x; y /= value.y; z /= value.z; }
	void operator/=(const VECTOR& value) { x /= value.x; y /= value.y; z /= value.z; }
	void operator/=(float value) { x /= value; y /= value; z /= value; }

	// ì‡êœ
	float Dot(const Vector3& value)const { return x * value.x + y * value.y + z * value.z; }

	// äOêœ

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


	void operator=(float value) { x = value; y = value; z = value; }


	// ÉxÉNÉgÉãÇÃëÂÇ´Ç≥(Å„Ç»Çµ)
	float LengthSq(void)const { return (x * x + y * y + z * z); }

	// ÉxÉNÉgÉãÇÃëÂÇ´Ç≥
	float Length(void)const { return sqrtf(LengthSq()); }

	// ê≥ãKâªÇ≥ÇÍÇΩílÇï‘Ç∑
	Vector3 Normalized(void)const { return (*this / Length()); }

	// ê≥ãKâªÇ∑ÇÈ
	void Normalize(void) {
		float len = Length();
		x /= len; y /= len; z /= len;
	}

	// çsóÒÇ≈ïœä∑
	Vector3 TransMat(const MATRIX& mat)const {
		if (*this == 0.0f) { return Vector3(); }
		return VTransform(*this, mat);
	}

	// çsóÒÇ≈ïœä∑(é©êgÇïœä∑)
	void TransMatOwn(const MATRIX& mat) {
		if (*this == 0.0f) { return; }
		*this = VTransform(*this, mat);
	}

};