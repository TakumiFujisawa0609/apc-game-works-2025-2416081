#include"ActorBase.h"

#include"../Application/Application.h"

ActorBase::ActorBase() :
	trans(prevPos),
	collider(),

	dynamicFlg(true),
	isGravity(false),

	pushFlg(true),
	pushWeight(0),

	prevPos(trans.pos),

	accelSum(0.0f),

	isGroundMaster(false),

	isDraw(true),
	isAlphaDraw(false),

	parameter(nullptr)
{
}

ActorBase::ActorBase(const std::string& parameterPath) :
	trans(prevPos),
	collider(),

	dynamicFlg(true),
	isGravity(false),

	pushFlg(true),
	pushWeight(0),

	prevPos(trans.pos),

	accelSum(0.0f, 0.0f, 0.0f),

	isGroundMaster(false),

	isDraw(true),
	isAlphaDraw(false),

	parameter(new ParameterLoad(parameterPath))
{
}

void ActorBase::Init(void)
{
	SubInit();
}

void ActorBase::Update(void)
{
	// 動的オブジェクトは１フレーム前の座標を保持
	if (dynamicFlg) { prevPos = trans.pos; }

	// 派生先追加更新
	SubUpdate();

	// 重力処理
	if (dynamicFlg && isGravity) { Gravity(); }

	// 加速度更新
	if (dynamicFlg) { AccelUpdate(); }

	// 接地判定のリセット
	if (dynamicFlg) { isGroundMaster = false; }
}

void ActorBase::Draw(void)
{
	// 派生先追加描画
	SubDraw();

	// 描画判定
	if (!isDraw) { return; }

	// モデルの描画
	if (!isAlphaDraw) {
		trans.Draw(); 
	}
}

void ActorBase::AlphaDraw(void)
{
	// 派生先追加アルファ描画
	SubAlphaDraw();

	// 描画判定
	if (!isDraw) { return; }

	// モデルの描画（アルファ描画）
	if (isAlphaDraw) { trans.Draw(); }

	// 当たり判定のデバッグ描画
	if (App::GetIns().IsDrawDebug()) {
		for (ColliderBase*& c : collider) { c->DrawDebug(); }
	}
}

void ActorBase::Release(void)
{
	// 派生先追加解放
	SubRelease();

	// パラメータの解放
	if (parameter != nullptr) {
		parameter->Release();
		delete parameter;
		parameter = nullptr;
	}

	// 当たり判定情報を解放
	for (ColliderBase*& c : collider) {
		if (!c) { continue; }
		delete c;
		c = nullptr;
	}
	collider.clear();

	// モデル制御情報の解放
	trans.Release();
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
	if (accelSum.x != 0.0f) {
		if (abs(accelSum.x) > ACCEL_MAX) { accelSum.x = ACCEL_MAX * Sign(accelSum.x); }
		trans.pos.x += accelSum.x;
		accelSum.x -= ATTENUATION * Sign(accelSum.x);
		if (abs(accelSum.x) <= ATTENUATION / 2) { accelSum.x = 0.0f; }
	}
	if (accelSum.z != 0.0f) {
		if (abs(accelSum.z) > ACCEL_MAX) { accelSum.z = ACCEL_MAX * Sign(accelSum.z); }
		trans.pos.z += accelSum.z;
		accelSum.z -= ATTENUATION * Sign(accelSum.z);
		if (abs(accelSum.z) <= ATTENUATION / 2) { accelSum.z = 0.0f; }
	}

	// 縦軸
	if (accelSum.y != 0.0f) { trans.pos.y += accelSum.y; }
}

void ActorBase::Gravity(void)
{
	accelSum.y += GRAVITY;
	if (accelSum.y < GRAVITY_MAX) { accelSum.y = GRAVITY_MAX; }
}
