#pragma once

class Vector2I
{
public:

	int x;
	int y;

	// コンストラクタ
	Vector2I(void);

	// コンストラクタ
	Vector2I(int vX, int vY);

	// デストラクタ
	~Vector2I(void);

	// 代入---------------------------------------
	void operator=(const int value);
	//--------------------------------------------

	// 加算---------------------------------------
	Vector2I operator+(const Vector2I value)const;
	void operator+=(const Vector2I value);

	Vector2I operator+(const int value)const;
	void operator+=(const int value);
	//--------------------------------------------
	
	// 減算---------------------------------------
	Vector2I operator-(const Vector2I value)const;
	void operator-=(const Vector2I value);

	Vector2I operator-(const int value)const;
	void operator-=(const int value);
	//--------------------------------------------

	// 乗算---------------------------------------
	Vector2I operator*(const Vector2I value)const;
	void operator*=(const Vector2I value);

	Vector2I operator*(const int value)const;
	void operator*=(const int value);
	//--------------------------------------------

	// 除算---------------------------------------
	Vector2I operator/(const Vector2I value)const;
	void operator/=(const Vector2I value);

	Vector2I operator/(const int value)const;
	void operator/=(const int value);
	//--------------------------------------------
};

class Vector2
{
public:

	float x;
	float y;

	//コンストラクタ
	Vector2(void);

	//コンストラクタ
	Vector2(float vX, float vY);

	//デストラクタ
	~Vector2(void);

	//演算

	// 代入---------------------------------------
	void operator=(const float value);
	//--------------------------------------------

	// 加算---------------------------------------
	Vector2 operator+(const Vector2 value)const;
	void operator+=(const Vector2 value);

	Vector2 operator+(const float value)const;
	void operator+=(const float value);
	//--------------------------------------------

	// 減算---------------------------------------
	Vector2 operator-(const Vector2 value)const;
	void operator-=(const Vector2 value);

	Vector2 operator-(const float value)const;
	void operator-=(const float value);
	//--------------------------------------------

	// 乗算---------------------------------------
	Vector2 operator*(const Vector2 value)const;
	void operator*=(const Vector2 value);

	Vector2 operator*(const float value)const;
	void operator*=(const float value);
	//--------------------------------------------

	// 除算---------------------------------------
	Vector2 operator/(const Vector2 value)const;
	void operator/=(const Vector2 value);

	Vector2 operator/(const float value)const;
	void operator/=(const float value);
	//--------------------------------------------
};