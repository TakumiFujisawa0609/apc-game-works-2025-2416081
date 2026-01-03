#include "Camera.h"

#include "../../Utility/Utility.h"
#include "../input/KeyManager.h"
#include "InstantCamera.h"

Camera* Camera::ins = nullptr;

Camera::Camera(void) :

	mode(MODE::NON),

	modeFuncPtr(),
	modeApply(),

	pos(),
	angle(),

	ROT_POWER(0.0f),
	MOVE_POWER(0.0f),

	lookAt(nullptr),
	lookAtDiff(),

	lookTarget(nullptr)
{
}

void Camera::Init(void)
{
#pragma region モード別関数を格納
	SET_MODE_FUNC(MODE::NON, &Camera::NonModeFunc);
	SET_APPLY(MODE::NON, &Camera::NonModeFunc);

	SET_MODE_FUNC(MODE::FIXED_POINT, &Camera::FixedPointModeFunc);
	SET_APPLY(MODE::FIXED_POINT, &Camera::FixedPointApply);

	SET_MODE_FUNC(MODE::FREE, &Camera::FreeModeFunc);
	SET_APPLY(MODE::FREE, &Camera::FreeApply);

	SET_MODE_FUNC(MODE::FOLLOW_REMOTE, &Camera::FollowRemoteModeFunc);
	SET_APPLY(MODE::FOLLOW_REMOTE, &Camera::FollowRemoteApply);

	SET_MODE_FUNC(MODE::FOLLOW_AUTO, &Camera::FollorAutoModeFunc);
	SET_APPLY(MODE::FOLLOW_AUTO, &Camera::FollowRemoteApply);
#pragma endregion
}

void Camera::Update(void)
{
	// モード別関数の呼び出し
	(this->*modeFuncPtr[(int)mode])();
}

void Camera::Apply(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	// モード別関数の呼び出し
	(this->*modeApply[(int)mode])();
}


void Camera::ChangeModeFixedPoint(const Vector3& pos, const Vector3& angle)
{
	// 状態遷移
	mode = MODE::FIXED_POINT;

	// 座標を設定
	this->pos = pos;

	// 角度を設定
	this->angle = angle;
}

void Camera::FixedPointModeFunc(void)
{
}

void Camera::FixedPointApply(void)
{
	// 適用
	SetCameraPositionAndAngle(pos.ToVECTOR(), angle.x, angle.y, angle.z);
}

void Camera::ChangeModeFree(float ROT_POWER, float MOVE_POWER, const Vector3& pos, const Vector3& angle)
{
	// 状態遷移
	mode = MODE::FREE;

	// 回転量
	this->ROT_POWER = ROT_POWER;

	// 移動量
	this->MOVE_POWER = MOVE_POWER;

	// 初期座標
	this->pos = pos;

	// 初期角度
	this->angle = angle;
}

void Camera::FreeModeFunc(void)
{
#pragma region 角度 (コントローラースティック -> マウス -> ボタン の順に確認して入力があったもので回転させる)
	// コントローラーの右スティックベクトルを代入
	Vector3 rot = KEY::GetIns().GetRightStickVec().ToVector3YX();

	// コントローラーの右スティックが入力なしならマウスの移動ベクトルを代入
	if (rot == 0.0f) { rot = KEY::GetIns().GetMouceMove().ToVector3YX(); }

	// マウスが動いてなかったらボタンでの入力を検出してボタンごとに回転方向を 加算/減算 していく
	if (rot == 0.0f) {
		if (KEY::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_FRONT).now) { rot.x += ROT_POWER; }
		if (KEY::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_BACK).now) { rot.x -= ROT_POWER; }
		if (KEY::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_RIGHT).now) { rot.y += ROT_POWER; }
		if (KEY::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_LEFT).now) { rot.y -= ROT_POWER; }
	}

	// 最終的に入力が１つでもあれば回転させる
	if (rot != 0.0f) {

		angle += rot.Normalized() * ROT_POWER;

		// 回転の数値制御
		if (angle.x <= Utility::Deg2RadF(0.0f)) { angle.x += Utility::Deg2RadF(360.0f); }
		if (angle.x >= Utility::Deg2RadF(360.0f)) { angle.x -= Utility::Deg2RadF(360.0f); }

		if (angle.y <= Utility::Deg2RadF(0.0f)) { angle.y += Utility::Deg2RadF(360.0f); }
		if (angle.y >= Utility::Deg2RadF(360.0f)) { angle.y -= Utility::Deg2RadF(360.0f); }

		//if (angle.z >= Utility::Deg2RadF(360.0f)) { angle.z = 0.0f; }
		// ↑Z軸回転はしない
	}
#pragma endregion


#pragma region 移動 (コントローラースティック -> ボタン の順に確認して入力があったもので移動させる)
	// コントローラーの左スティックベクトルを代入
	Vector3 dir = KEY::GetIns().GetLeftStickVec().ToVector3XZ();

	// コントローラーの左スティックの入力が検出されなかった場合、ボタンでの入力を検出してボタンごとに移動方向を 加算/減算 していく
	if (dir == 0.0f) {
		if (KEY::GetIns().GetInfo(KEY_TYPE::CAMERA_MOVE_FRONT).now) { dir.z++; }
		if (KEY::GetIns().GetInfo(KEY_TYPE::CAMERA_MOVE_BACK).now) { dir.z--; }
		if (KEY::GetIns().GetInfo(KEY_TYPE::CAMERA_MOVE_RIGHT).now) { dir.x++; }
		if (KEY::GetIns().GetInfo(KEY_TYPE::CAMERA_MOVE_LEFT).now) { dir.x--; }
	}

	// Y軸はボタンのため共通して検出する
	if (KEY::GetIns().GetInfo(KEY_TYPE::CAMERA_MOVE_UP).now) { dir.y++; }
	if (KEY::GetIns().GetInfo(KEY_TYPE::CAMERA_MOVE_DOWN).now) { dir.y--; }

	// 入力があれば、方向×スピードで移動量を作って、座標に足して移動
	if (dir != 0.0f) { pos += dir.Normalized().TransMat(MGetRotY(angle.y)) * MOVE_POWER; }
#pragma endregion
}

void Camera::FreeApply(void)
{
	// 適用
	SetCameraPositionAndAngle(pos.ToVECTOR(), angle.x, angle.y, angle.z);
}

void Camera::ChangeModeFollowRemote(const Vector3* lookAt, const Vector3& lookAtDiff, float ROT_POWER, const Vector3& angle)
{
	// 状態遷移
	mode = MODE::FOLLOW_REMOTE;

	// 追従対象を設定
	this->lookAt = lookAt;

	// 追従対象とのローカル座標を設定
	this->lookAtDiff = lookAtDiff;

	// 回転量をセット
	this->ROT_POWER = ROT_POWER;

	// 角度をセット
	this->angle = angle;

	// 座標をセット
	pos = *lookAt + lookAtDiff.TransMat(Utility::MatrixAllMultXY({ Vector3::XYonly(angle.x,angle.y) }));
}

void Camera::FollowRemoteModeFunc(void)
{
	// 追従対象が設定されていなかったら処理をしない(安全)
	if (lookAt == nullptr) { return; }

	// 回転処理(コントローラー -> マウス-> ボタン の順に入力を確認していく)

	// コントローラーの右スティックベクトルを代入
	Vector3 vec = KEY::GetIns().GetRightStickVec().ToVector3YX();

	// コントローラーの右スティックが入力なしならマウスの移動ベクトルを代入
	if (vec == 0.0f) { vec = KEY::GetIns().GetMouceMove().ToVector3YX(); }

	// マウスが動いてなかったらボタンでの入力を検出してボタンごとに回転方向を 加算/減算 していく
	if (vec == 0.0f) {
		if (KEY::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_FRONT).now) { vec.x++; }
		if (KEY::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_BACK).now) { vec.x--; }
		if (KEY::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_RIGHT).now) { vec.y++; }
		if (KEY::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_LEFT).now) { vec.y--; }
	}

	// 最終的に入力が１つでもあれば回転させる
	if (vec != 0.0f) {
		angle += vec.Normalized() * ROT_POWER;

		if (angle.y >= Utility::Deg2RadF(360.0f)) { angle.y -= Utility::Deg2RadF(360.0f); }
		if (angle.y <= Utility::Deg2RadF(0.0f)) { angle.y += Utility::Deg2RadF(360.0f); }
		if (angle.x < Utility::Deg2RadF(-30.0f)) { angle.x = Utility::Deg2RadF(-30.0f); }
		if (angle.x > Utility::Deg2RadF(60.0f)) { angle.x = Utility::Deg2RadF(60.0f); }
	}

	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	pos = *lookAt + lookAtDiff.TransMat(Utility::MatrixAllMultXY({ Vector3::XYonly(angle.x,angle.y) }));
}

void Camera::FollowRemoteApply(void)
{
	// 追従対象が設定されていなかったら処理をしない(安全)
	if (lookAt == nullptr) { return; }

	// 適用
	SetCameraPositionAndTarget_UpVecY(pos.ToVECTOR(), lookAt->ToVECTOR());
}

void Camera::ChangeModeFollowAuto(const Vector3* lookAt, const Vector3* lookTarget, const Vector3& lookAtDiff, const Vector3& angle)
{
	// 状態遷移
	mode = MODE::FOLLOW_AUTO;

	// 追従対象を設定
	this->lookAt = lookAt;

	// 視野に入れる対象物
	this->lookTarget = lookTarget;

	// 角度をセット
	this->angle = angle;
}

void Camera::FollorAutoModeFunc(void)
{
	// 未実装
}

void Camera::FollowAutoApply(void)
{
	// 未実装
}

void Camera::DrawDebug(void)
{
	DrawFormatString(
		0, 10, 0xffffff,
		"カメラ座標　 ：(% .1f, % .1f, % .1f)",
		pos.x, pos.y, pos.z
	);
	DrawFormatString(
		0, 30, 0xffffff,
		"カメラ角度　 ：(% .1f, % .1f, % .1f)",
		Utility::Rad2DegF(angle.x),
		Utility::Rad2DegF(angle.y),
		Utility::Rad2DegF(angle.z)
	);

}

void Camera::Release(void)
{
}