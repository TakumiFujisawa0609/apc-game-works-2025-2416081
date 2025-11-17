#include"ActorBase.h"

#include"../Application/Application.h"

ActorBase::ActorBase() :
	trans_(prevPos_),

	collider_(),

	dynamicFlg_(1),

	prevPos_(trans_.pos)
{
}

void ActorBase::Init(void)
{
	SubInit();
}

void ActorBase::Update(void)
{
	// 動的オブジェクトは１フレーム前の座標を保持
	if (dynamicFlg_ == 1) { prevPos_ = trans_.pos; }

	// 派生先追加更新
	SubUpdate();
}

void ActorBase::Draw(void)
{
	// 派生先追加描画
	SubDraw();

	// モデルの描画
	trans_.Draw();

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