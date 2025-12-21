#pragma once

#include<DxLib.h>
#include<algorithm>
#include<cmath>

struct Vector3
{
	float x = 0.0f, y = 0.0f, z = 0.0f;

	// Vector3¶¬
	Vector3(void) :x(0.0f), y(0.0f), z(0.0f) {}

	// Vector3¶¬
	Vector3(float x, float y, float z) :x(x), y(y), z(z) {}

	// Vector3¶¬
	Vector3(float value) :x(value), y(value), z(value) {}

	// Vector3¶¬
	Vector3(const VECTOR& value) :x(value.x), y(value.y), z(value.z) {}

	// Vector3¶¬
	Vector3(int x, int y, int z) :x((float)x), y((float)y), z((float)z) {}

	// Vector3¶¬
	Vector3(int value) :x((float)value), y((float)value), z((float)value) {}

	// VECTOR(Dxlib)‚É•ÏŠ·
	VECTOR ToVECTOR(void)const { return VGet(x, y, z); }

#pragma region ‰‰Z

	// ‰ÁZ``````````````````````````````````````````````````
	
	// “¯‚¶\‘¢‘Ì“¯m‚Æ“¯‚¶—v‘f”“¯m‚Å‰ÁZ
	Vector3 operator+(const Vector3& value)const { return Vector3(x + value.x, y + value.y, z + value.z); }

	// Dxlib‚ÌVECTOR\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚Å‰ÁZ
	Vector3 operator+(const VECTOR& value)const { return Vector3(x + value.x, y + value.y, z + value.z); }

	// ‚P‚Â‚Ì’l‚Æ‘S‚Ä‚Ì—v‘f”‚Å‰ÁZ
	Vector3 operator+(float value)const { return Vector3(x + value, y + value, z + value); }

	// “¯‚¶\‘¢‘Ì“¯m‚Æ“¯‚¶—v‘f”“¯m‚Å‰ÁZ‘ã“ü
	void operator+=(const Vector3& value) { x += value.x; y += value.y; z += value.z; }

	// Dxlib‚ÌVECTOR\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚Å‰ÁZ‘ã“ü
	void operator+=(const VECTOR& value) { x += value.x; y += value.y; z += value.z; }

	// ‚P‚Â‚Ì’l‚Æ‘S‚Ä‚Ì—v‘f”‚Å‰ÁZ‘ã“ü
	void operator+=(float value) { x += value; y += value; z += value; }

	// ````````````````````````````````````````````````````
	
	// Œ¸Z``````````````````````````````````````````````````

	// “¯‚¶\‘¢‘Ì“¯m‚Æ“¯‚¶—v‘f”“¯m‚ÅŒ¸Z
	Vector3 operator-(const Vector3& value)const { return Vector3(x - value.x, y - value.y, z - value.z); }

	// Dxlib‚ÌVECTOR\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ÅŒ¸Z
	Vector3 operator-(const VECTOR& value)const { return Vector3(x - value.x, y - value.y, z - value.z); }

	// ‚P‚Â‚Ì’l‚Æ‘S‚Ä‚Ì—v‘f”‚ÅŒ¸Z
	Vector3 operator-(float value)const { return Vector3(x - value, y - value, z - value); }

	// “¯‚¶\‘¢‘Ì“¯m‚Æ“¯‚¶—v‘f”“¯m‚ÅŒ¸Z‘ã“ü
	void operator-=(const Vector3& value) { x -= value.x; y -= value.y; z -= value.z; }

	// Dxlib‚ÌVECTOR\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ÅŒ¸Z‘ã“ü
	void operator-=(const VECTOR& value) { x -= value.x; y -= value.y; z -= value.z; }

	// ‚P‚Â‚Ì’l‚Æ‘S‚Ä‚Ì—v‘f”‚ÅŒ¸Z‘ã“ü
	void operator-=(float value) { x -= value; y -= value; z -= value; }
	// ````````````````````````````````````````````````````

	// æZ``````````````````````````````````````````````````

	// “¯‚¶\‘¢‘Ì“¯m‚Æ“¯‚¶—v‘f”“¯m‚ÅæZ
	Vector3 operator*(const Vector3& value)const { return Vector3(x * value.x, y * value.y, z * value.z); }

	// Dxlib‚ÌVECTOR\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ÅæZ
	Vector3 operator*(const VECTOR& value)const { return Vector3(x * value.x, y * value.y, z * value.z); }

	// ‚P‚Â‚Ì’l‚Æ‘S‚Ä‚Ì—v‘f”‚ÅæZ
	Vector3 operator*(float value)const { return Vector3(x * value, y * value, z * value); }

	// “¯‚¶\‘¢‘Ì“¯m‚Æ“¯‚¶—v‘f”“¯m‚ÅæZ‘ã“ü
	void operator*=(const Vector3& value) { x *= value.x; y *= value.y; z *= value.z; }

	// Dxlib‚ÌVECTOR\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ÅæZ‘ã“ü
	void operator*=(const VECTOR& value) { x *= value.x; y *= value.y; z *= value.z; }

	// ‚P‚Â‚Ì’l‚Æ‘S‚Ä‚Ì—v‘f”‚ÅæZ‘ã“ü
	void operator*=(float value) { x *= value; y *= value; z *= value; }

	// ````````````````````````````````````````````````````

	// œZ````````````````````````````````````````````````````````````````````````````````````````

	// “¯‚¶\‘¢‘Ì“¯m‚Æ“¯‚¶—v‘f”“¯m‚ÅœZ
	Vector3 operator/(const Vector3& value)const { return Vector3(x / ((value.x != 0.0f) ? value.x : 1), y / ((value.y != 0.0f) ? value.y : 1), z / ((value.z != 0.0f) ? value.z : 1)); }

	// Dxlib‚ÌVECTOR\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ÅœZ
	Vector3 operator/(const VECTOR& value)const { return Vector3(x / ((value.x != 0.0f) ? value.x : 1), y / ((value.y != 0.0f) ? value.y : 1), z / ((value.z != 0.0f) ? value.z : 1)); }

	// ‚P‚Â‚Ì’l‚Æ‘S‚Ä‚Ì—v‘f”‚ÅœZ
	Vector3 operator/(float value)const { return Vector3(x / ((value != 0.0f) ? value : 1), y / ((value != 0.0f) ? value : 1), z / ((value != 0.0f) ? value : 1)); }

	// “¯‚¶\‘¢‘Ì“¯m‚Æ“¯‚¶—v‘f”“¯m‚ÅœZ‘ã“ü
	void operator/=(const Vector3& value) { x /= ((value.x != 0.0f) ? value.x : 1); y /= ((value.y != 0.0f) ? value.y : 1); z /= ((value.z != 0.0f) ? value.z : 1); }

	// Dxlib‚ÌVECTOR\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ÅœZ‘ã“ü
	void operator/=(const VECTOR& value) { x /= ((value.x != 0.0f) ? value.x : 1); y /= ((value.y != 0.0f) ? value.y : 1); z /= ((value.z != 0.0f) ? value.z : 1); }

	// ‚P‚Â‚Ì’l‚Æ‘S‚Ä‚Ì—v‘f”‚ÅœZ‘ã“ü
	void operator/=(float value) { x /= ((value != 0.0f) ? value : 1); y /= ((value != 0.0f) ? value : 1); z /= ((value != 0.0f) ? value : 1); }

	// ``````````````````````````````````````````````````````````````````````````````````````````

	// “àÏ
	float Dot(const Vector3& value)const { return x * value.x + y * value.y + z * value.z; }

	// ŠOÏ

#pragma endregion

#pragma region ”äŠr

	// “¯‚¶\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ğ”äŠr‚µA‘S‚Ä“¯‚¶‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator==(const Vector3& value)const { return (x == value.x && y == value.y && z == value.z); }

	// Dxlib‚ÌVECTOR\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ğ”äŠr‚µA‘S‚Ä“¯‚¶‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator==(const VECTOR& value)const { return (x == value.x && y == value.y && z == value.z); }

	// ‚P‚Â‚Ì’l‚Æ‘S‚Ä‚Ì—v‘f”‚ğ”äŠr‚µA‘S‚Ä“¯‚¶‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator==(float value)const { return (x == value && y == value && z == value); }


	// “¯‚¶\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ğ”äŠr‚µA‘S‚Ä“¯‚¶‚È‚çufalsev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îutruev‚ğ•Ô‚·
	bool operator!=(const Vector3& value)const { return (x != value.x || y != value.y || z != value.z); }

	// Dxlib‚ÌVECTOR\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ğ”äŠr‚µA‘S‚Ä“¯‚¶‚È‚çufalsev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îutruev‚ğ•Ô‚·
	bool operator!=(const VECTOR& value)const { return (x != value.x || y != value.y || z != value.z); }

	// ‚P‚Â‚Ì’l‚Æ‘S‚Ä‚Ì—v‘f”‚ğ”äŠr‚µA‘S‚Ä“¯‚¶‚È‚çufalsev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îutruev‚ğ•Ô‚·
	bool operator!=(float value)const { return (x != value || y != value || z != value); }


	// “¯‚¶\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ğ”äŠr‚µA‘S‚Ä¬‚³‚¢‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator<(const Vector3& value)const { return (x < value.x && y < value.y && z < value.z); }

	// Dxlib‚ÌVECTOR\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ğ”äŠr‚µA‘S‚Ä¬‚³‚¢‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator<(const VECTOR& value)const { return (x < value.x && y < value.y && z < value.z); }

	// ‚P‚Â‚Ì’l‚Æ‘S‚Ä‚Ì—v‘f”‚ğ”äŠr‚µA‘S‚Ä¬‚³‚¢‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator<(float value)const { return (x < value && y < value && z < value); }


	// “¯‚¶\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ğ”äŠr‚µA‘S‚Ä¬‚³‚¢‚©“™‚µ‚¢‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator<=(const Vector3& value)const { return (x <= value.x && y <= value.y && z <= value.z); }

	// Dxlib‚ÌVECTOR\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ğ”äŠr‚µA‘S‚Ä¬‚³‚¢‚©“™‚µ‚¢‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator<=(const VECTOR& value)const { return (x <= value.x && y <= value.y && z <= value.z); }

	// ‚P‚Â‚Ì’l‚Æ‘S‚Ä‚Ì—v‘f”‚ğ”äŠr‚µA‘S‚Ä¬‚³‚¢‚©“™‚µ‚¢‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator<=(float value)const { return (x <= value && y <= value && z <= value); }


	// “¯‚¶\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ğ”äŠr‚µA‘S‚Ä‘å‚«‚¢‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator>(const Vector3& value)const { return (x > value.x && y > value.y && z > value.z); }

	// Dxlib‚ÌVECTOR\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ğ”äŠr‚µA‘S‚Ä‘å‚«‚¢‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator>(const VECTOR& value)const { return (x > value.x && y > value.y && z > value.z); }

	// ‚P‚Â‚Ì’l‚Æ‘S‚Ä‚Ì—v‘f”‚ğ”äŠr‚µA‘S‚Ä‘å‚«‚¢‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator>(float value)const { return (x > value && y > value && z > value); }


	// “¯‚¶\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ğ”äŠr‚µA‘S‚Ä‘å‚«‚¢‚©“™‚µ‚¢‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator>=(const Vector3& value)const { return (x >= value.x && y >= value.y && z >= value.z); }

	// Dxlib‚ÌVECTOR\‘¢‘Ì‚Æ“¯‚¶—v‘f”“¯m‚ğ”äŠr‚µA‘S‚Ä‘å‚«‚¢‚©“™‚µ‚¢‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator>=(const VECTOR& value)const { return (x >= value.x && y >= value.y && z >= value.z); }

	// ‚P‚Â‚Ì’l‚Æ‘S‚Ä‚Ì—v‘f”‚ğ”äŠr‚µA‘S‚Ä‘å‚«‚¢‚©“™‚µ‚¢‚È‚çutruev‚ğA‚»‚¤‚Å‚È‚¯‚ê‚Îufalsev‚ğ•Ô‚·
	bool operator>=(float value)const { return (x >= value && y >= value && z >= value); }

#pragma endregion

#pragma region ‘ã“ü

	// —v‘f”‘S‚Ä‚Éw’è‚³‚ê‚½’l‚Å‘ã“ü
	void operator=(float value) { x = value; y = value; z = value; }

	// —v‘f”‚ğ‡‚í‚¹‚Ä‘ã“ü
	void operator=(const Vector3& value) { x = value.x; y = value.y; z = value.z; }

	// DxLib‚ÌVECTOR\‘¢‘Ì‚ğ—v‘f”‚ğ‡‚í‚¹‚Ä‘ã“ü
	void operator=(const VECTOR& value) { x = value.x; y = value.y; z = value.z; }

#pragma endregion
	
	// ‘S‚Ä‚Ì—v‘f”‚ğ”½“]‚³‚¹‚½Vector3\‘¢‘Ì‚ğ•Ô‚·
	Vector3 operator-(void)const { return Vector3(-x, -y, -z); }

	// ƒxƒNƒgƒ‹‚Ì‘å‚«‚³(ã‚È‚µ)
	float LengthSq(void)const { return (x * x + y * y + z * z); }

	// ƒxƒNƒgƒ‹‚Ì‘å‚«‚³
	float Length(void)const { return sqrtf(LengthSq()); }

	// ³‹K‰»‚³‚ê‚½’l‚ğ•Ô‚·
	Vector3 Normalized(void)const { return (*this / Length()); }

	// ³‹K‰»‚·‚é
	void Normalize(void) { *this /= Length(); }

	// s—ñ‚Å•ÏŠ·
	Vector3 TransMat(const MATRIX& mat)const {
		if (*this == 0.0f) { return Vector3(); }
		return Vector3(VTransform(ToVECTOR(), mat));
	}

	// s—ñ‚Å•ÏŠ·(©g‚ğ•ÏŠ·)
	void TransMatOwn(const MATRIX& mat) {
		if (*this == 0.0f) { return; }
		*this = VTransform(ToVECTOR(), mat);
	}

	// â‘Î’l‚ğæ“¾‚·‚é
	Vector3 Abs(void)const { return Vector3(abs(x), abs(y), abs(z)); }

	// ˆê”Ô‘å‚«‚¢—v‘f”‚ğæ“¾
	float MaxElementF(void)const {
		float max = x;
		if (max < y) { max = y; }
		if (max < z) { max = z; }
		return max;
	}

	// ˆê”Ô¬‚³‚¢—v‘f”‚ğæ“¾
	float MinElementF(void)const {
		float min = x;
		if (min > y) { min = y; }
		if (min > z) { min = z; }
		return min;
	}

	// ˆê”Ô‘å‚«‚¢—v‘f”‚¾‚¯‚É‚µ‚½Vector3\‘¢‘Ì‚ğ•Ô‚·
	Vector3 MaxElement(void)const {
		if (x >= y && x >= z) { return Vector3(x, 0.0f, 0.0f); }
		else if (y >= z) { return Vector3(0.0f, y, 0.0f); }
		else { return Vector3(0.0f, 0.0f, z); }
	}

	// ˆê”Ô¬‚³‚¢—v‘f”‚¾‚¯‚É‚µ‚½Vector3\‘¢‘Ì‚ğ•Ô‚·
	Vector3 MinElement(void)const {
		if (x <= y && x <= z) { return Vector3(x, 0.0f, 0.0f); }
		else if (y <= z) { return Vector3(0.0f, y, 0.0f); }
		else { return Vector3(0.0f, 0.0f, z); }
	}
};