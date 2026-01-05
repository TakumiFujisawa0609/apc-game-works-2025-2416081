#pragma once

#include"../../Object/Common/Transform.h"

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
	// 定点モードに変更
	void ChangeModeFixedPoint(const Vector3& pos, const Vector3& angle, float fov = (60.0f * (DX_PI_F / 180.0f)));
	// フリーモードに変更
	void ChangeModeFree(float ROT_POWER, float MOVE_POWER, const Vector3& pos = Vector3(), const Vector3& angle = Vector3(), float fov = (60.0f * (DX_PI_F / 180.0f)));
	// 追従（手動操作）モードに変更
	void ChangeModeFollowRemote(const Vector3* lookAt, const Vector3& lookAtDiff = Vector3(0, 0, -400), float ROT_POWER = 3.0f * (DX_PI_F / 180.0f), const Vector3& angle = Vector3(), float fov = (60.0f * (DX_PI_F / 180.0f)));
	// 追従（自動操作）モードに変更（引数省略バージョン）
	void ChangeModeFollowAuto(const Transform& lookAt, const Vector3* lookTarget, float fov = (80.0f * (DX_PI_F / 180.0f)));
	// 追従（自動操作）モードに変更（引数非省略バージョン）
	void ChangeModeFollowAuto(const Vector3* lookAt,const float* lookAtYangle, const Vector3* lookTarget, float fov = (80.0f * (DX_PI_F / 180.0f)));
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

#pragma region セット関数
	// 座標を設定（追従モード時を除く。追従モード = FOLLOW_REMOTE / FOLLOW_AUTO）
	void SetPos(const Vector3& pos) {
		// 追従状態のときは処理をしない
		if (mode == MODE::FOLLOW_REMOTE || mode == MODE::FOLLOW_AUTO) { return; }
		// 指定の座標を代入
		this->pos = pos;
	}

	// 角度を設定（追従(自動操作)モード時を除く。追従(自動操作)モード = FOLLOW_AUTO）
	void SetAngle(const Vector3& angle) {
		// 追従（自動操作）のときは処理をしない
		if (mode == MODE::FOLLOW_AUTO) { return; }
		// 指定の角度を代入
		this->angle = angle;
	}

	// 追従モードのときの追従対象を途中で変更する。追従モード = FOLLOW_REMOTE / FOLLOW_AUTO
	void FollowToLookAtChange(const Transform& trans) {
		if (mode != MODE::FOLLOW_REMOTE && mode != MODE::FOLLOW_AUTO) { return; }
		lookAt = &trans.pos;
		if (mode == MODE::FOLLOW_AUTO) { lookAtYangle = &trans.angle.y; }
	}

	// 追従(自動操作)モードのときの視野に含める対象物を途中で変更する。追従(自動操作)モード = FOLLOW_AUTO
	void FollowAutoToLookTargetChange(const Vector3* lookTarget) { if (mode == MODE::FOLLOW_AUTO) { this->lookTarget = lookTarget; } }

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

	// 視野角
	float fov;

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
	void FollowAutoModeFunc(void);

	// 追従対象
	//const Vector3* lookAt_;
	// ↑FOLLOW_REMOTEのものを流用

	// 追従対象の向き
	const float* lookAtYangle;

	// 視野に入れる対象物
	const Vector3* lookTarget;

	// 適用
	void FollowAutoApply(void);
#pragma endregion
};
