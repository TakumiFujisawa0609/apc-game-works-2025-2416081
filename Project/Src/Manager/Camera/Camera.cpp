#include "Camera.h"

#include "../../Utility/Utility.h"
#include "../input/KeyManager.h"

Camera::Camera(void)
{
	// DxLibの初期設定では、
	// カメラの位置が x = 320.0f, y = 240.0f, z = (画面のサイズによって変化)、
	// 注視点の位置は x = 320.0f, y = 240.0f, z = 1.0f
	// カメラの上方向は x = 0.0f, y = 1.0f, z = 0.0f
	// 右上位置からZ軸のプラス方向を見るようなカメラ
}

Camera::~Camera(void)
{
}

void Camera::Init(void)
{
	// カメラの初期位置
	pos_ = DERFAULT_POS;

	// カメラの初期角度
	angles_ = DERFAULT_ANGLES;

	xAngle_ = Utility::Deg2RadF(30.0f);
	yAngle_ = 0.0f;
}

void Camera::Update(void)
{
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		SetBeforeDrawFixedPoint();
		break;
	case Camera::MODE::FREE:
		SetBeforeDrawFree();
		break;
	case Camera::MODE::FOLLOW:
		SetBeforeDrawFollow();
		break;
	}
}

void Camera::Apply(void)
{
	// クリップ距離を設定する(SetDrawScreenでリセットされる)
	SetCameraNearFar(VIEW_NEAR, VIEW_FAR);

	// カメラの設定(位置と角度による制御)
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
	case Camera::MODE::FREE:
		SetCameraPositionAndAngle(
			pos_,
			angles_.x,
			angles_.y,
			angles_.z
		);
		break;
	case Camera::MODE::FOLLOW:
		if (lookAt_ != nullptr) { SetCameraPositionAndTarget_UpVecY(pos_, *lookAt_); }
		break;
	}
}

void Camera::SetBeforeDrawFixedPoint(void)
{
}

void Camera::SetBeforeDrawFree(void)
{
	// 矢印キーでカメラの角度を変える
	float rotPow = 1.0f * DX_PI_F / 180.0f;

	if (CheckHitKey(KEY_INPUT_DOWN)) { angles_.x += rotPow; }
	if (CheckHitKey(KEY_INPUT_UP)) { angles_.x -= rotPow; }
	if (CheckHitKey(KEY_INPUT_RIGHT)) { angles_.y += rotPow; }
	if (CheckHitKey(KEY_INPUT_LEFT)) { angles_.y -= rotPow; }

	// WASDでカメラを移動させる
	const float movePow = 3.0f;
	VECTOR dir = {};

	if (CheckHitKey(KEY_INPUT_W)) { dir.z++; }
	if (CheckHitKey(KEY_INPUT_S)) { dir.z--; }
	if (CheckHitKey(KEY_INPUT_D)) { dir.x++; }
	if (CheckHitKey(KEY_INPUT_A)) { dir.x--; }
	if (CheckHitKey(KEY_INPUT_E)) { dir.y++; }
	if (CheckHitKey(KEY_INPUT_Q)) { dir.y--; }

	if (!Utility::VZERO(dir))
	{
		dir = VScale(Utility::Normalize(dir), movePow);

		// XYZの回転行列
		// XZ平面移動にする場合は、XZの回転を考慮しないようにする
		MATRIX mat = MGetIdent();
		//mat = MMult(mat, MGetRotX(angles_.x));
		mat = MMult(mat, MGetRotY(angles_.y));

		// 回転行列を使用して、ベクトルを回転させる
		VECTOR moveDir = VTransform(dir, mat);

		// 方向×スピードで移動量を作って、座標に足して移動
		pos_ = VAdd(pos_, VScale(moveDir, movePow));
	}
}

void Camera::SetBeforeDrawFollow(void)
{
	if (lookAt_ == nullptr) { return; }

    // 追従対象の座標: lookAt_ (VECTOR型)
    // カメラのY軸回転角度: yAngle_ (float型)

	float rotPow = 1.0f * DX_PI_F / 180.0f;
	if (CheckHitKey(KEY_INPUT_LEFT)) { yAngle_ -= rotPow; }
	if (CheckHitKey(KEY_INPUT_RIGHT)) { yAngle_ += rotPow; }
	if (yAngle_ >= 360.0f) { yAngle_ -= 360.0f; }
	
	if (CheckHitKey(KEY_INPUT_DOWN)) { xAngle_ -= rotPow; }
	if (CheckHitKey(KEY_INPUT_UP)) { xAngle_ += rotPow; }
	if (xAngle_ < Utility::Deg2RadF(0.0f)) { xAngle_ = Utility::Deg2RadF(0.0f); }
	if (xAngle_ > Utility::Deg2RadF(89.0f)) { xAngle_ = Utility::Deg2RadF(89.0f); }

	MATRIX mat = MGetIdent();
	mat = MMult(mat, MGetRotX(xAngle_));
	mat = MMult(mat, MGetRotY(yAngle_));


	pos_ = VAdd(*lookAt_, VTransform(LOOKAT_DIFF, mat));

	VECTOR vec = VSub(*lookAt_, pos_);
	angles_.x = -atan2f(vec.y, sqrtf(vec.x * vec.x + vec.z * vec.z));
	angles_.y = atan2f(vec.x, vec.z);
}

void Camera::DrawDebug(void)
{
	DrawFormatString(
		0, 10, 0xffffff,
		"カメラ座標　 ：(% .1f, % .1f, % .1f)",
		pos_.x, pos_.y, pos_.z
	);
	DrawFormatString(
		0, 30, 0xffffff,
		"カメラ角度　 ：(% .1f, % .1f, % .1f)",
		Utility::Rad2DegF(angles_.x),
		Utility::Rad2DegF(angles_.y),
		Utility::Rad2DegF(angles_.z)
	);

}

void Camera::Release(void)
{
}

void Camera::ChangeMode(MODE mode)
{
	// カメラモードの変更
	mode_ = mode;

	// 変更時の初期化処理
	switch (mode_)
	{
	case Camera::MODE::FIXED_POINT:
		break;
	case Camera::MODE::FREE:
		break;
	case Camera::MODE::FOLLOW:
		break;
	}

}