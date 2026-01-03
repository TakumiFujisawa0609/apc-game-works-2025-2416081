#pragma once

#include"../../Common/Vector2.h"
#include"../../Common/Vector3.h"

class Camera
{
private:

	Camera(void);
	~Camera(void) = default;

	// インスタンス
	static Camera* ins;

public:

	static void CreateIns(void) { if (ins == nullptr) { ins = new Camera(); ins->Init(); } }
	static Camera& GetIns(void) { return *ins; }
	static void DeleteIns(void) { if (ins) { ins->Release(); delete ins; ins = nullptr; } }

	// カメラのクリップ範囲
	static constexpr float VIEW_NEAR = 10.0f;
	static constexpr float VIEW_FAR = 30000.0f;

	// カメラモード
	enum class MODE
	{
		// 未設定
		NON,

		// 定点
		FIXED_POINT,

		// フリー
		FREE,

		// 追従（手動操作）
		FOLLOW_REMOTE,

		// 追従（自動操作）
		FOLLOW_AUTO,

		MAX
	};

#pragma region カメラモード切り替え
	// 定点
	void ChangeModeFixedPoint(const Vector3& pos, const Vector3& angle);
	// フリー
	void ChangeModeFree(float ROT_POWER, float MOVE_POWER, const Vector3& pos = Vector3(), const Vector3& angle = Vector3());
	// 追従（手動操作）
	void ChangeModeFollowRemote(const Vector3* lookAt, const Vector3& lookAtDiff = Vector3(0, 0, -400), float ROT_POWER = 3.0f * (DX_PI_F / 180.0f), const Vector3& angle = Vector3());
	// 追従（自動操作）
	void ChangeModeFollowAuto(const Vector3* lookAt, const Vector3* lookTarget, const Vector3& lookAtDiff = Vector3(0, 0, -400), const Vector3& angle = Vector3());
#pragma endregion

	// 更新
	void Update(void);

	// 描画前のカメラ設定
	void Apply(void);

	// デバッグ用描画
	void DrawDebug(void);

#pragma region ゲット関数
	// 座標
	const Vector3& GetPos(void)const { return pos; }

	// 角度
	const Vector3& GetAngle(void)const { return angle; }
#pragma endregion


private:
	// 初期化
	void Init(void);
	// 解放
	void Release(void);

	// モード
	MODE mode;

	using STATEFUNC = void (Camera::*)(void);
	STATEFUNC modeFuncPtr[(int)MODE::MAX];
	STATEFUNC modeApply[(int)MODE::MAX];
#define SET_MODE_FUNC(mode, func) {modeFuncPtr[(int)(mode)] = static_cast<STATEFUNC>(func);}
#define SET_APPLY(mode, func) {modeApply[(int)(mode)] = static_cast<STATEFUNC>(func);}

	// 未設定用
	void NonModeFunc(void) {}

#pragma region 全状態共通
	// カメラの位置
	Vector3 pos;

	// カメラの角度
	Vector3 angle;
#pragma endregion


#pragma region FIXED_POINT
	// 更新処理
	void FixedPointModeFunc(void);

	// 適用
	void FixedPointApply(void);
#pragma endregion


#pragma region FREE
	// 更新処理
	void FreeModeFunc(void);

	// 回転量
	float ROT_POWER;

	// 移動量
	float MOVE_POWER;


	// 適用
	void FreeApply(void);
#pragma endregion


#pragma region FOLLOW_REMOTE
	// 更新処理
	void FollowRemoteModeFunc(void);

	// 追従対象
	const Vector3* lookAt;

	// 追従対象からのローカル座標
	Vector3 lookAtDiff;

	// 回転量
	//float ROT_POWER;
	// ↑FREEのものを流用

	// 適用
	void FollowRemoteApply(void);
#pragma endregion


#pragma region FOLLOW_AUTO
	// 更新処理
	void FollorAutoModeFunc(void);

	// 追従対象
	//const Vector3* lookAt_;
	// ↑FOLLOW_REMOTEのものを流用

	// 視野に入れる対象物
	const Vector3* lookTarget;

	// 回転量
	//float ROT_POWER;
	// ↑FREEのものを流用

	// 適用
	void FollowAutoApply(void);
#pragma endregion
};
