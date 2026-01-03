#pragma once
#include <string>
#include <vector>
#include <DxLib.h>
#include "../Common/Vector2.h"
#include "../Common/Vector3.h"

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

	static VECTOR Deg2RadF(const VECTOR& deg) { return { Deg2RadF(deg.x),Deg2RadF(deg.y),Deg2RadF(deg.z) }; }
	static Vector3 Deg2RadF(const Vector3& deg) { return { Deg2RadF(deg.x),Deg2RadF(deg.y),Deg2RadF(deg.z) }; }

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
	static Vector2 Normalize(const Vector2I& v);

	// クランプ代入
	static float Clamp(float value, float min, float max) { return ((value <= min) ? min : ((value >= max) ? max : value)); }
	static int Clamp(int value, int min, int max) { return ((value <= min) ? min : ((value >= max) ? max : value)); }

	static VECTOR Clamp(const VECTOR& v, const VECTOR& minV, const VECTOR& maxV);

	static float Sgn(float x) { return (x < 0) ? -1.0f : 1.0f; }

	// ベクトルが０かどうか
	static bool VZERO(const VECTOR& v);

	// 画像読み込み（エラーチェック付き）
	static void LoadImg(int& handle, std::string path);
	// 画像読み込み（エラーチェック付き）
	static int LoadImg(std::string path);
	// スプライト画像読み込み（エラーチェック付き）
	static void LoadArrayImg(std::string path, int AllNum, int XNum, int YNum, int XSize, int YSize, int* handleArray);
	static void LoadArrayImg(std::string path, int AllNum, int XNum, int YNum, int XSize, int YSize, std::vector<int>& handleArray);

	
#pragma region 各順番の（VECTOR or Vector3）構造体を行列に変換する関数（引数にいくつでも入れられる仕様 左から順に計算される）
	static MATRIX MatrixAllMultX(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultX(const std::initializer_list<Vector3>& vecs);
	static MATRIX MatrixAllMultY(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultY(const std::initializer_list<Vector3>& vecs);
	static MATRIX MatrixAllMultZ(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultZ(const std::initializer_list<Vector3>& vecs);

	static MATRIX MatrixAllMultXY(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultXY(const std::initializer_list<Vector3>& vecs);
	static MATRIX MatrixAllMultXZ(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultXZ(const std::initializer_list<Vector3>& vecs);
	static MATRIX MatrixAllMultXYZ(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultXYZ(const std::initializer_list<Vector3>& vecs);
	static MATRIX MatrixAllMultXZY(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultXZY(const std::initializer_list<Vector3>& vecs);

	static MATRIX MatrixAllMultYX(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultYX(const std::initializer_list<Vector3>& vecs);
	static MATRIX MatrixAllMultYZ(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultYZ(const std::initializer_list<Vector3>& vecs);
	static MATRIX MatrixAllMultYXZ(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultYXZ(const std::initializer_list<Vector3>& vecs);
	static MATRIX MatrixAllMultYZX(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultYZX(const std::initializer_list<Vector3>& vecs);

	static MATRIX MatrixAllMultZX(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultZX(const std::initializer_list<Vector3>& vecs);
	static MATRIX MatrixAllMultZY(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultZY(const std::initializer_list<Vector3>& vecs);
	static MATRIX MatrixAllMultZXY(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultZXY(const std::initializer_list<Vector3>& vecs);
	static MATRIX MatrixAllMultZYX(const std::initializer_list<VECTOR>& vecs);
	static MATRIX MatrixAllMultZYX(const std::initializer_list<Vector3>& vecs);
#pragma endregion


	static VECTOR Minus(const VECTOR& v) { return { -v.x,-v.y,-v.z }; }

	// 行列をつかったモデルの座標と向きのセット
	static void MV1ModelMatrix(int& model, const VECTOR& pos, const std::initializer_list<VECTOR>& angle);
	static void MV1ModelMatrix(int& model, const Vector3& pos, const std::initializer_list<Vector3>& angle);

	static void MV1ModelMatrix(int& model, const VECTOR& scale, const VECTOR& pos, const std::initializer_list<VECTOR>& angle);
	static void MV1ModelMatrix(int& model, const Vector3& scale, const Vector3& pos, const std::initializer_list<Vector3>& angle);

	static VECTOR FtoV(float f) { return { f,f,f }; }
};

static Vector3 VTransform(const Vector3& v, const MATRIX& m) {
	if (v == 0.0f) { return Vector3(); }
	return Vector3(VTransform(v.ToVECTOR(), m));
}