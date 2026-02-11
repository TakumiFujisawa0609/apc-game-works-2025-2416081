#include"InstantCamera.h"

#include"../../Utility/Utility.h"

InstantCamera::InstantCamera() : 

	mode(MODE::NON),

	modeFuncPtr(),
	modeApply(),

	pos(),
	angle(),

	lookAt(nullptr),
	lookAtDiff()
{
}

void InstantCamera::Init(void)
{
#pragma region モード別関数を格納
	INSTANT_CAMERA_SET_MODE_FUNC(MODE::NON, &InstantCamera::NonModeFunc);
	INSTANT_CAMERA_SET_APPLY(MODE::NON, &InstantCamera::NonModeFunc);

	INSTANT_CAMERA_SET_MODE_FUNC(MODE::POINT, &InstantCamera::PointModeFunc);
	INSTANT_CAMERA_SET_APPLY(MODE::POINT, &InstantCamera::PointApply);

	INSTANT_CAMERA_SET_MODE_FUNC(MODE::FOLLOW, &InstantCamera::FollowModeFunc);
	INSTANT_CAMERA_SET_APPLY(MODE::FOLLOW, &InstantCamera::FolloweApply);
#pragma endregion
}

void InstantCamera::Update(void)
{
	// モード別関数の呼び出し
	(this->*modeFuncPtr[(int)mode])();
}

void InstantCamera::Apply(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	// モード別関数の呼び出し
	(this->*modeApply[(int)mode])();
}

#pragma region POINTモード

void InstantCamera::ChangeModePoint(const Vector3& pos, const Vector3& angle)
{
	// 状態遷移
	mode = MODE::POINT;

	// 座標を設定
	this->pos = pos;

	// 角度を設定
	this->angle = angle;
}

void InstantCamera::PointModeFunc(void)
{
}

void InstantCamera::PointApply(void)
{
	// 適用
	SetCameraPositionAndAngle(pos.ToVECTOR(), angle.x, angle.y, angle.z);
}

#pragma endregion


#pragma region FOLLOWモード

void InstantCamera::ChangeModeFollow(const Vector3* lookAt, const Vector3& lookAtDiff , const Vector3& angle)
{
	// 状態遷移
	mode = MODE::FOLLOW;

	// 追従対象を設定
	this->lookAt = lookAt;

	// 追従対象とのローカル座標を設定
	this->lookAtDiff = lookAtDiff;

	// 角度をセット
	this->angle = angle;

	// 座標をセット
	pos = *lookAt + lookAtDiff.TransMat(MatrixAllMultXY({ Vector3::XYonly(angle.x,angle.y) }));
}

void InstantCamera::FollowModeFunc(void)
{
	// 追従対象が設定されていなかったら処理をしない(安全)
	if (lookAt == nullptr) { return; }

	// 現在の追従対象の座標と角度情報から自身(カメラ)の座標を算出する
	pos = *lookAt + lookAtDiff.TransMat(MatrixAllMultXY({ Vector3::XYonly(angle.x,angle.y) }));
}

void InstantCamera::FolloweApply(void)
{
	// 追従対象が設定されていなかったら処理をしない(安全)
	if (lookAt == nullptr) { return; }

	// 適用
	SetCameraPositionAndTarget_UpVecY(pos.ToVECTOR(), lookAt->ToVECTOR());
}

#pragma endregion

void InstantCamera::Release(void)
{

}