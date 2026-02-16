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
	fov(),

	ROT_POWER(0.0f),
	MOVE_POWER(0.0f),

	fixedLookAtPos(),
	lookAtDiff(),

	lookAt(nullptr),

	lookAtYangle(nullptr),
	lookTarget(nullptr),
	FOLLOW_AUTO_MIN_DISTANCE(),
	FOLLOW_AUTO_MAX_DISTANCE()
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

	SET_MODE_FUNC(MODE::LOOK_AT_FREE, &Camera::LookAtFreeModeFunc);
	SET_APPLY(MODE::LOOK_AT_FREE, &Camera::LookAtFreeAplly);

	SET_MODE_FUNC(MODE::DISPLAY, &Camera::DisplayModeFunc);
	SET_APPLY(MODE::DISPLAY, &Camera::DisplayAplly);

	SET_MODE_FUNC(MODE::FOLLOW_REMOTE, &Camera::FollowRemoteModeFunc);
	SET_APPLY(MODE::FOLLOW_REMOTE, &Camera::FollowRemoteApply);

	SET_MODE_FUNC(MODE::FOLLOW_AUTO, &Camera::FollowAutoModeFunc);
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
	// クリップ距離を設定する(ClearDrawScreenでリセットされる)
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	// 視野角を設定する(ClearDrawScreenでリセットされる)
	SetupCamera_Perspective(fov);

	// モード別関数の呼び出し
	(this->*modeApply[(int)mode])();
}

#pragma region 定点
void Camera::ChangeModeFixedPoint(const Vector3& pos, const Vector3& angle, float fov)
{
	// 現在の情報を破棄
	Release();

	// 状態遷移
	mode = MODE::FIXED_POINT;

	// 座標を設定
	this->pos = pos;

	// 角度を設定
	this->angle = angle;

	// 視野角を設定
	this->fov = fov;
}

void Camera::FixedPointModeFunc(void)
{
}

void Camera::FixedPointApply(void)
{
	// 適用
	SetCameraPositionAndAngle(pos.ToVECTOR(), angle.x, angle.y, angle.z);
}
#pragma endregion

#pragma region フリー
void Camera::ChangeModeFree(float ROT_POWER, float MOVE_POWER, const Vector3& pos, const Vector3& angle, float fov)
{
	// 現在の情報を破棄
	Release();

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

	// 視野角を設定
	this->fov = fov;
}

void Camera::FreeModeFunc(void)
{
#pragma region 角度 (コントローラースティック -> マウス -> ボタン の順に確認して入力があったもので回転させる)
	// コントローラーの右スティックベクトルを代入
	Vector3 rot = Key::GetIns().GetRightStickVec().ToVector3YX();

	// コントローラーの右スティックが入力なしならマウスの移動ベクトルを代入
	if (rot == 0.0f) { rot = Key::GetIns().GetMouceMove().ToVector3YX(); }

	// マウスが動いてなかったらボタンでの入力を検出してボタンごとに回転方向を 加算/減算 していく
	if (rot == 0.0f) {
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_FRONT).now) { rot.x -= ROT_POWER; }
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_BACK).now) { rot.x += ROT_POWER; }
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_LEFT).now) { rot.y -= ROT_POWER; }
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_RIGHT).now) { rot.y += ROT_POWER; }
	}

	// 最終的に入力が１つでもあれば回転させる
	if (rot != 0.0f) {

		angle += rot.Normalized() * ROT_POWER;

		// 回転の数値制御
		if (angle.x <= Deg2Rad(0.0f)) { angle.x += Deg2Rad(360.0f); }
		if (angle.x >= Deg2Rad(360.0f)) { angle.x -= Deg2Rad(360.0f); }

		if (angle.y <= Deg2Rad(0.0f)) { angle.y += Deg2Rad(360.0f); }
		if (angle.y >= Deg2Rad(360.0f)) { angle.y -= Deg2Rad(360.0f); }

		//if (angle.z >= Deg2Rad(360.0f)) { angle.z = 0.0f; }
		// ↑Z軸回転はしない
	}
#pragma endregion


#pragma region 移動 (コントローラースティック -> ボタン の順に確認して入力があったもので移動させる)
	// コントローラーの左スティックベクトルを代入
	Vector3 dir = Key::GetIns().GetLeftStickVec().ToVector3XZ();

	// コントローラーの左スティックの入力が検出されなかった場合、ボタンでの入力を検出してボタンごとに移動方向を 加算/減算 していく
	if (dir == 0.0f) {
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_MOVE_FRONT).now) { dir.z++; }
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_MOVE_BACK).now) { dir.z--; }
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_MOVE_RIGHT).now) { dir.x++; }
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_MOVE_LEFT).now) { dir.x--; }
	}

	// Y軸はボタンのため共通して検出する
	if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_MOVE_UP).now) { dir.y++; }
	if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_MOVE_DOWN).now) { dir.y--; }

	// 入力があれば、方向×スピードで移動量を作って、座標に足して移動
	if (dir != 0.0f) { pos += dir.Normalized().TransMat(MGetRotY(angle.y)) * MOVE_POWER; }
#pragma endregion
}

void Camera::FreeApply(void)
{
	// 適用
	SetCameraPositionAndAngle(pos.ToVECTOR(), angle.x, angle.y, angle.z);
}
#pragma endregion

#pragma region 回転のみフリー（注視点起点）
void Camera::ChangeModeLookAtFree(const Vector3& fixedLookAtPos, const Vector3& lookAtDiff, float ROT_POWER, const Vector3& angle, float fov)
{
	// 現在の情報を破棄
	Release();

	// 状態遷移
	mode = MODE::LOOK_AT_FREE;

	// 注視点
	this->fixedLookAtPos = fixedLookAtPos;

	// 注視点からの相対座標
	this->lookAtDiff = lookAtDiff;

	// 回転量
	this->ROT_POWER = ROT_POWER;

	// 初期角度
	this->angle = angle;

	// 視野角を設定
	this->fov = fov;
}

void Camera::LookAtFreeModeFunc(void)
{
	// 回転処理(コントローラー -> マウス-> ボタン の順に入力を確認していく)

	// コントローラーの右スティックベクトルを代入
	Vector3 vec = Key::GetIns().GetRightStickVec().ToVector3YX();

	// コントローラーの右スティックが入力なしならマウスの移動ベクトルを代入
	if (vec == 0.0f) { vec = Key::GetIns().GetMouceMove().ToVector3YX(); }

	// マウスが動いてなかったらボタンでの入力を検出してボタンごとに回転方向を 加算/減算 していく
	if (vec == 0.0f) {
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_FRONT).now) { vec.x++; }
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_BACK).now) { vec.x--; }
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_RIGHT).now) { vec.y++; }
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_LEFT).now) { vec.y--; }
	}

	// 最終的に入力が１つでもあれば回転させる
	if (vec != 0.0f) {
		angle += vec.Normalized() * ROT_POWER;

		if (angle.y >= Deg2Rad(360.0f)) { angle.y -= Deg2Rad(360.0f); }
		if (angle.y <= Deg2Rad(0.0f)) { angle.y += Deg2Rad(360.0f); }
		if (angle.x < Deg2Rad(-30.0f)) { angle.x = Deg2Rad(-30.0f); }
		if (angle.x > Deg2Rad(60.0f)) { angle.x = Deg2Rad(60.0f); }
	}

	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	pos = fixedLookAtPos + lookAtDiff.TransMat(MatrixAllMultXY({ Vector3::XYonly(angle.x,angle.y) }));
}

void Camera::LookAtFreeAplly(void)
{
	// 適用
	SetCameraPositionAndTarget_UpVecY(pos.ToVECTOR(), fixedLookAtPos.ToVECTOR());
}
#pragma endregion

#pragma region ディスプレイ（特定の座標を起点に周回しながら注視する）
void Camera::ChangeModeDisplay(const Vector3& fixedLookAtPos, const Vector3& lookAtDiff, float ROT_POWER, const Vector3& angle, float fov)
{
	// 現在の情報を破棄
	Release();

	// 状態遷移
	mode = MODE::DISPLAY;

	// 注視点
	this->fixedLookAtPos = fixedLookAtPos;

	// 注視点からの相対座標
	this->lookAtDiff = lookAtDiff;

	// 回転量
	this->ROT_POWER = ROT_POWER;

	// 初期角度
	this->angle = angle;

	// 視野角を設定
	this->fov = fov;
}

void Camera::DisplayModeFunc(void)
{
	// 回転処理（設定された値横向きに回し続ける）
	angle += Vector3::Yonly(1.0f).Normalized() * ROT_POWER;

	if (angle.y >= Deg2Rad(360.0f)) { angle.y -= Deg2Rad(360.0f); }
	if (angle.y <= Deg2Rad(0.0f)) { angle.y += Deg2Rad(360.0f); }

	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	pos = fixedLookAtPos + lookAtDiff.TransMat(MatrixAllMultXY({ Vector3::XYonly(angle.x,angle.y) }));
}

void Camera::DisplayAplly(void)
{
	// 適用
	SetCameraPositionAndTarget_UpVecY(pos.ToVECTOR(), fixedLookAtPos.ToVECTOR());
}
#pragma endregion

#pragma region 追従（手動操作）
void Camera::ChangeModeFollowRemote(const Vector3* lookAt, const Vector3& lookAtDiff, float ROT_POWER, const Vector3& angle, float fov)
{
	// 現在の情報を破棄
	Release();

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
	pos = *lookAt + lookAtDiff.TransMat(MatrixAllMultXY({ Vector3::XYonly(angle.x,angle.y) }));

	// 視野角を設定
	this->fov = fov;
}

void Camera::FollowRemoteModeFunc(void)
{
	// 追従対象が設定されていなかったら処理をしない(安全)
	if (lookAt == nullptr) { return; }

	// 回転処理(コントローラー -> マウス-> ボタン の順に入力を確認していく)

	// コントローラーの右スティックベクトルを代入
	Vector3 vec = Key::GetIns().GetRightStickVec().ToVector3YX();

	// コントローラーの右スティックが入力なしならマウスの移動ベクトルを代入
	if (vec == 0.0f) { vec = Key::GetIns().GetMouceMove().ToVector3YX(); }

	// マウスが動いてなかったらボタンでの入力を検出してボタンごとに回転方向を 加算/減算 していく
	if (vec == 0.0f) {
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_FRONT).now) { vec.x++; }
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_BACK).now) { vec.x--; }
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_RIGHT).now) { vec.y++; }
		if (Key::GetIns().GetInfo(KEY_TYPE::CAMERA_ROT_LEFT).now) { vec.y--; }
	}

	// 最終的に入力が１つでもあれば回転させる
	if (vec != 0.0f) {
		angle += vec.Normalized() * ROT_POWER;

		if (angle.y >= Deg2Rad(360.0f)) { angle.y -= Deg2Rad(360.0f); }
		if (angle.y <= Deg2Rad(0.0f)) { angle.y += Deg2Rad(360.0f); }
		if (angle.x < Deg2Rad(-30.0f)) { angle.x = Deg2Rad(-30.0f); }
		if (angle.x > Deg2Rad(60.0f)) { angle.x = Deg2Rad(60.0f); }
	}

	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	pos = *lookAt + lookAtDiff.TransMat(MatrixAllMultXY({ Vector3::XYonly(angle.x,angle.y) }));
}

void Camera::FollowRemoteApply(void)
{
	// 追従対象が設定されていなかったら処理をしない(安全)
	if (lookAt == nullptr) { return; }

	// 適用
	SetCameraPositionAndTarget_UpVecY(pos.ToVECTOR(), lookAt->ToVECTOR());
}
#pragma endregion

#pragma region 追従（自動操作）
void Camera::ChangeModeFollowAuto(const Transform& lookAt, const Vector3* lookTarget, float FOLLOW_AUTO_MIN_DISTANCE, float FOLLOW_AUTO_MAX_DISTANCE, float fov)
{
	// 現在の情報を破棄
	Release();

	// 状態遷移
	mode = MODE::FOLLOW_AUTO;

	// 追従対象を設定
	this->lookAt = &lookAt.pos;

	// 追従対象の向き
	this->lookAtYangle = &lookAt.angle.y;

	// 視野に入れる対象物
	this->lookTarget = lookTarget;

	// 最低距離
	this->FOLLOW_AUTO_MIN_DISTANCE = FOLLOW_AUTO_MIN_DISTANCE;

	// 最大距離
	this->FOLLOW_AUTO_MAX_DISTANCE = FOLLOW_AUTO_MAX_DISTANCE;

	// 視野角を設定
	this->fov = fov;

#pragma region 座標
	// ２点間ベクトル
	Vector3 atToTarget = *this->lookAt - *this->lookTarget;

	// fovから必要距離を計算（縦fov基準）
	float needDist = (atToTarget.Length() * 0.5f) / tanf(fov * 0.5f);

	// 位置を算出
	pos = *this->lookAt + atToTarget.Normalized() * needDist;
	pos.y += (*this->lookAt - ((*this->lookAt + *this->lookTarget) * 0.5f)).Length();
#pragma endregion

#pragma region 角度
	angle = ((*this->lookAt + *this->lookTarget) * 0.5f) - pos;
	angle = Vector3::Yonly(atan2f(angle.x, angle.z));
#pragma endregion
}

void Camera::ChangeModeFollowAuto(const Vector3* lookAt, const float* lookAtYangle, const Vector3* lookTarget, float FOLLOW_AUTO_MIN_DISTANCE, float FOLLOW_AUTO_MAX_DISTANCE, float fov)
{
	// 現在の情報を破棄
	Release();

	// 状態遷移
	mode = MODE::FOLLOW_AUTO;

	// 追従対象を設定
	this->lookAt = lookAt;

	// 追従対象の向き
	this->lookAtYangle = lookAtYangle;

	// 視野に入れる対象物
	this->lookTarget = lookTarget;

	// 最低距離
	this->FOLLOW_AUTO_MIN_DISTANCE = FOLLOW_AUTO_MIN_DISTANCE;

	// 最大距離
	this->FOLLOW_AUTO_MAX_DISTANCE = FOLLOW_AUTO_MAX_DISTANCE;

	// 視野角を設定
	this->fov = fov;

#pragma region 座標
	// ２点間ベクトル
	Vector3 atToTarget = *this->lookAt - *this->lookTarget;

	// fovから必要距離を計算（縦fov基準）
	float needDist = (atToTarget.Length() * 0.5f) / tanf(fov * 0.5f);

	// 位置を算出
	pos = *this->lookAt + atToTarget.Normalized() * needDist;
	pos.y += (*this->lookAt - ((*this->lookAt + *this->lookTarget) * 0.5f)).Length();
#pragma endregion

#pragma region 角度
	angle = ((*this->lookAt + *this->lookTarget) * 0.5f) - pos;
	angle = Vector3::Yonly(atan2f(angle.x, angle.z));
#pragma endregion
}

void Camera::FollowAutoModeFunc(void)
{
	// 追従対象が設定されていなかったら処理をしない(安全)
	if (lookAt == nullptr || lookTarget == nullptr) { return; }

	// ２点間ベクトル
	Vector3 atToTarget = *lookAt - *lookTarget;

	// fovから必要距離を計算（縦fov基準）
	float needDist = std::clamp((atToTarget.Length() * 0.5f) / tanf(fov * 0.5f), FOLLOW_AUTO_MIN_DISTANCE, FOLLOW_AUTO_MAX_DISTANCE);

	// lookTargetからみてlookAtのそのさらに先にカメラをおきたいのでその方向を取得する
	Vector3 backDir = atToTarget.Normalized();

	// 目標カメラ位置
	Vector3 desiredPos = *lookAt + backDir * needDist;

	// 高さ補正
	desiredPos.y += std::clamp(((*lookAt - *lookTarget) * 0.5f).Length(), 250.0f, 400.0f);

	// 補間（ガタつき防止）
	const float smooth = 0.1f;
	pos += (desiredPos - pos) * smooth;

	// 角度を算出する
	angle = ((*lookAt + *lookTarget) * 0.5f) - pos;
	angle = Vector3::Yonly(atan2f(angle.x, angle.z));
}

void Camera::FollowAutoApply(void)
{
	// 追従対象が設定されていなかったら処理をしない(安全)
	if (lookAt == nullptr || lookTarget == nullptr) { return; }

	// 適用
	SetCameraPositionAndTarget_UpVecY(pos.ToVECTOR(), ((*lookAt + *lookTarget) * 0.5f).ToVECTOR());
}
#pragma endregion

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
		Rad2Deg(angle.x),
		Rad2Deg(angle.y),
		Rad2Deg(angle.z)
	);

}

void Camera::Release(void)
{
	switch (mode)
	{
	case Camera::MODE::NON: { break; }

	case Camera::MODE::FIXED_POINT:
		break;
	case Camera::MODE::FREE:
		break;
	case Camera::MODE::LOOK_AT_FREE:
		break;
	case Camera::MODE::DISPLAY:
		break;
	case Camera::MODE::FOLLOW_REMOTE:
		lookAt = nullptr;

		lookAtDiff = {};
		ROT_POWER = 0.0f;
		break;
	case Camera::MODE::FOLLOW_AUTO:
		lookAt = nullptr;
		lookAtYangle = nullptr;
		lookTarget = nullptr;

		break;
	}
}