#pragma once

#include"../../Common/Vector2.h"
#include"../../Common/Vector3.h"

class InstantCamera
{
public:
	InstantCamera();
	~InstantCamera() = default;

	// カメラのクリップ範囲
	static constexpr float VIEW_NEAR = 10.0f;
	static constexpr float VIEW_FAR = 30000.0f;

	// カメラモード
	enum class MODE
	{
		// 未設定
		NON,

		// 定点
		POINT,

		// 追従（角度固定）
		FOLLOW,

		MAX
	};

#pragma region カメラモード切り替え
	// 定点
	void ChangeModePoint(const Vector3& pos, const Vector3& angle);
	// 追従（角度固定）
	void ChangeModeFollow(const Vector3* lookAt, const Vector3& lookAtDiff = Vector3(0, 0, -400), const Vector3& angle = Vector3());
#pragma endregion

	// 初期化
	void Init(void);

	// 更新
	void Update(void);

	// 描画前のカメラ設定
	void Apply(void);

	// 解放
	void Release(void);

#pragma region ゲット関数
	// 座標
	const Vector3& GetPos(void)const { return pos; }
	// 角度
	const Vector3& GetAngle(void)const { return angle; }
#pragma endregion

private:

	// モード
	MODE mode;

	using STATEFUNC = void (InstantCamera::*)(void);
	STATEFUNC modeFuncPtr[(int)MODE::MAX];
	STATEFUNC modeApply[(int)MODE::MAX];
#define INSTANT_CAMERA_SET_MODE_FUNC(mode, func) {modeFuncPtr[(int)(mode)] = static_cast<STATEFUNC>(func);}
#define INSTANT_CAMERA_SET_APPLY(mode, func) {modeApply[(int)(mode)] = static_cast<STATEFUNC>(func);}

	// 未設定用
	void NonModeFunc(void) {}

#pragma region 全状態共通
	// カメラの位置
	Vector3 pos;

	// カメラの角度
	Vector3 angle;
#pragma endregion


#pragma region POINT
	// 更新処理
	void PointModeFunc(void);

	// 適用
	void PointApply(void);
#pragma endregion


#pragma region FOLLOW
	// 更新処理
	void FollowModeFunc(void);

	// 追従対象
	const Vector3* lookAt;

	// 追従対象からのローカル座標
	Vector3 lookAtDiff;

	// 適用
	void FolloweApply(void);
#pragma endregion
};