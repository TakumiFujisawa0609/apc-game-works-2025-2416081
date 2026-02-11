#pragma once

#include"../../Utility/Utility.h"

struct Transform
{
	// モデル
	int model;

	// 座標
	Vector3 pos;
	// １フレーム前の座標（参照用）
	const Vector3& prevPos;

	// 現在の移動量
	Vector3 Velocity(void)const { return (pos - prevPos); }

	// 描画する際の座標のズレを補完する用の変数。モデルを中心を軸に描画するため
	Vector3 centerDiff;

	// 角度
	Vector3 angle;
	Vector3 localAngle;

	// スケール
	Vector3 scale;

	/// <summary>
	/// Transform生成
	/// </summary>
	/// <param name="prevPos">１フレーム前の座標の変数を参照渡し</param>
	Transform(const Vector3& prevPos) :
		model(-1),

		pos(),
		prevPos(prevPos),

		centerDiff(),

		angle(),
		localAngle(),

		scale(1)
	{
	}


	// 角度を行列化したもの
	MATRIX AngleMat(void)const { return MatrixAllMultXZY({ localAngle,angle }); }

	// 渡されたVector3構造体を自身の角度情報で回転させたVector3構造体を返す
	Vector3 VTrans(const Vector3& v)const { return (v != 0.0f) ? Vector3(VTransform(v.ToVECTOR(), AngleMat())) : Vector3(); }
	// 渡されたVECTOR構造体を自身の角度情報で回転させたVector3構造体を返す
	Vector3 VTrans(const VECTOR& v)const { return VTrans(Vector3(v)); }

	// 角度を代入
	void SetAngleDeg(const Vector3& deg) { angle = Deg2Rad(deg); }
	void SetAngleXDeg(float deg) { angle.x = Deg2Rad(deg); }
	void SetAngleYDeg(float deg) { angle.y = Deg2Rad(deg); }
	void SetAngleZDeg(float deg) { angle.z = Deg2Rad(deg); }

	// 角度に加算
	void AddAngleDeg(const Vector3& deg) { angle += Deg2Rad(deg); }
	void AddAngleXDeg(float deg) { angle.x += Deg2Rad(deg); }
	void AddAngleYDeg(float deg) { angle.y += Deg2Rad(deg); }
	void AddAngleZDeg(float deg) { angle.z += Deg2Rad(deg); }


	/// <summary>
	/// モデルをロード
	/// </summary>
	/// <param name="path">モデルのパス（Data/Model/～～.mv1）</param>
	void Load(std::string path) { model = MV1LoadModel(("Data/Model/" + path + ".mv1").c_str()); }

	// 制御情報をモデルに適用
	void Attach(void) { MV1ModelMatrix(model, scale, pos + centerDiff, { localAngle,angle }); }

	// モデルを描画（変数情報をモデルに適用してから
	void Draw(void) {
		Attach();
		MV1DrawModel(model);
	}

	// モデルを解放
	void Release(void) { MV1DeleteModel(model); }
};