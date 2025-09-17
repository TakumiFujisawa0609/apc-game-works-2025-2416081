#pragma once
#include <string>
#include <vector>
#include <DxLib.h>
#include "../Common/Vector2.h"

class Utility
{

public:

	// ラジアン(rad)・度(deg)変換用
	static constexpr float RAD2DEG = (180.0f / DX_PI_F);
	static constexpr float DEG2RAD = (DX_PI_F / 180.0f);

	static constexpr float kEpsilonNormalSqrt = 1e-15F;

	// 四捨五入
	static int Round(float v);

	// 文字列の分割
	static std::vector <std::string> Split(std::string& line, char delimiter);

	// ラジアン(rad)から度(deg)
	static double Rad2DegD(double rad);
	static float Rad2DegF(float rad);
	static int Rad2DegI(int rad);

	// 度(deg)からラジアン(rad)
	static double Deg2RadD(double deg);
	static float Deg2RadF(float deg);
	static int Deg2RadI(int deg);

	// 0～360度の範囲に収める
	static double DegIn360(double deg);

	// 0(0)～2π(360度)の範囲に収める
	static double RadIn2PI(double rad);

	// 回転が少ない方の回転向きを取得する(時計回り:1、反時計回り:-1)
	static int DirNearAroundRad(float from, float to);

	// 回転が少ない方の回転向きを取得する(時計回り:1、反時計回り:-1)
	static int DirNearAroundDeg(float from, float to);

	// 線形補間
	static int Lerp(int start, int end, float t);
	static float Lerp(float start, float end, float t);
	static double Lerp(double start, double end, double t);
	static Vector2 Lerp(const Vector2& start, const Vector2& end, float t);

	// 角度の線形補間
	static double LerpDeg(double start, double end, double t);

	// ベジェ曲線
	static Vector2 Bezier(const Vector2& p1, const Vector2& p2, const Vector2& p3, float t);

	// ベクトルの長さ
	static double Magnitude(const Vector2& v);
	static int SqrMagnitude(const Vector2& v);
	static double Distance(const Vector2& v1, const Vector2& v2);


	// 
	static float VLength(const VECTOR& v);

	//
	static VECTOR VDivision(const VECTOR& v, float f);

	// ベクトル正規化
	static VECTOR Normalize(const VECTOR& v);
	static Vector2 Normalize(const Vector2& v);

	// クランプ代入
	static float Clamp(float value, float min, float max) { return ((value <= min) ? min : ((value >= max) ? max : value)); }
	static int Clamp(int value, int min, int max) { return ((value <= min) ? min : ((value >= max) ? max : value)); }

	static VECTOR Clamp(const VECTOR& v, const VECTOR& minV, const VECTOR& maxV);

	// ベクトルが０かどうか
	static bool VZERO(const VECTOR& v);

	// 画像読み込み（エラーチェック付き）
	static void LoadImg(int& handle, std::string path);
	// 画像読み込み（エラーチェック付き）
	static int LoadImg(std::string path);
	// スプライト画像読み込み（エラーチェック付き）
	static void LoadArrayImg(std::string path, int AllNum, int XNum, int YNum, int XSize, int YSize, int* handleArray);
	static void LoadArrayImg(std::string path, int AllNum, int XNum, int YNum, int XSize, int YSize, std::vector<int>& handleArray);




	// 衝突判定関数
	static bool CollisionAABB(const VECTOR& pos1, const VECTOR& size1, const VECTOR& pos2, const VECTOR& size2);


	static bool SphereVsAABB_MTV(const VECTOR& C, float R, const VECTOR& bmin, const VECTOR& bmax,VECTOR& outN, float& outDepth);
};

