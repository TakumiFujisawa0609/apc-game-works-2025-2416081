#include"ActorBase.h"

#include"../Application/Application.h"

ActorBase::ActorBase() :
	trans_(prevPos_),
	collider_(),

	dynamicFlg_(true),
	isGravity_(false),

	pushFlg_(true),
	pushWeight_(0),

	prevPos_(trans_.pos),

	AccelSum(0.0f, 0.0f, 0.0f),

	isGroundMaster_(false),

	isDraw_(true),
	isAlphaDraw_(false)
{
}

void ActorBase::Init(void)
{
	SubInit();
}

void ActorBase::Update(void)
{
	// 動的オブジェクトは１フレーム前の座標を保持
	if (dynamicFlg_) { prevPos_ = trans_.pos; }

	// 派生先追加更新
	SubUpdate();

	// 重力処理
	if (dynamicFlg_ && isGravity_) { Gravity(); }

	// 加速度更新
	if (dynamicFlg_) { AccelUpdate(); }
}

void ActorBase::Draw(void)
{
	// 派生先追加描画
	SubDraw();

	// 描画判定
	if (!isDraw_) { return; }

	// モデルの描画
	if (!isAlphaDraw_) {
		trans_.Draw(); 
	}
}

void ActorBase::AlphaDraw(void)
{
	// 派生先追加アルファ描画
	SubAlphaDraw();

	// 描画判定
	if (!isDraw_) { return; }

	// モデルの描画（アルファ描画）
	if (isAlphaDraw_) { trans_.Draw(); }

	// 当たり判定のデバッグ描画
	if (App::GetIns().IsDrawDebug()) {
		for (ColliderBase*& c : collider_) { c->DrawDebug(); }
	}
}

void ActorBase::Release(void)
{
	// 派生先追加解放
	SubRelease();

	// 当たり判定情報を解放
	for (ColliderBase*& c : collider_) {
		if (!c) { continue; }
		delete c;
		c = nullptr;
	}
	collider_.clear();

	// モデル制御情報の解放
	trans_.Release();
}

void ActorBase::AccelUpdate(void)
{
	// 引数の数値の符号を絶対値１で返すラムダ関数
	auto Sign = [&](float value)->float {
		if (value > 0.0f) { return +1.0f; }
		else if (value < 0.0f) { return -1.0f; }
		else { return 0.0f; }
		};

	// 横軸(横軸は減衰もする)
	if (AccelSum.x != 0.0f) {
		if (abs(AccelSum.x) > ACCEL_MAX) { AccelSum.x = ACCEL_MAX * Sign(AccelSum.x); }
		trans_.pos.x += AccelSum.x;
		AccelSum.x -= ATTENUATION * Sign(AccelSum.x);
		if (abs(AccelSum.x) <= ATTENUATION / 2) { AccelSum.x = 0.0f; }
	}
	if (AccelSum.z != 0.0f) {
		if (abs(AccelSum.z) > ACCEL_MAX) { AccelSum.z = ACCEL_MAX * Sign(AccelSum.z); }
		trans_.pos.z += AccelSum.z;
		AccelSum.z -= ATTENUATION * Sign(AccelSum.z);
		if (abs(AccelSum.z) <= ATTENUATION / 2) { AccelSum.z = 0.0f; }
	}

	// 縦軸
	if (AccelSum.y != 0.0f) { trans_.pos.y += AccelSum.y; }
}

void ActorBase::Gravity(void)
{
	AccelSum.y += GRAVITY;
	if (AccelSum.y < GRAVITY_MAX) { AccelSum.y = GRAVITY_MAX; }
}
