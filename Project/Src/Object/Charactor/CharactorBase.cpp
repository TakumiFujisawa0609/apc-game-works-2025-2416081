#include"CharactorBase.h"

CharactorBase::CharactorBase():
	ActorBase(),

	state_(0),
	stateFuncPtr(),

	DEFAULT_COLOR(),

	anime_(nullptr),

	inviCounter_(0),
	isInviEffect_(1)
{
}

CharactorBase::~CharactorBase()
{
}

void CharactorBase::SubInit(void)
{
	// モデルのカラーの初期化
	SetInviEffectFlg();

	// キャラクター固有の初期化
	CharactorInit();
}

void CharactorBase::SubUpdate(void)
{
	// 無敵カウンターの更新
	Invi();

	// キャラクター固有の更新
	CharactorUpdate();

	// 派生先で割り振り可能のステート別関数
	(this->*stateFuncPtr[state_])();

}

void CharactorBase::SubDraw(void)
{
	// キャラクター固有の描画
	CharactorDraw();
}

void CharactorBase::SubRelease(void)
{
	// キャラクター固有の解放
	CharactorRelease();

	// デフォルトカラー情報の解放
	if (!DEFAULT_COLOR.empty()) {
		DEFAULT_COLOR.clear();
	}

	// アニメーションコントローラーの解放（使われていたら）
	if (anime_) {
		anime_->Release();
		delete anime_;
		anime_ = nullptr;
	}
}

void CharactorBase::Invi(void)
{
	if (inviCounter_ > 0) { inviCounter_--; }
	else { inviCounter_ = 0; }

	// ダメージ演出
	if (isInviEffect_ == 0) { return; }

	if (inviCounter_ > 1) {
		if (inviCounter_ / 10 % 2 == 0) {
			for (int i = 0; i < DEFAULT_COLOR.size(); i++) {
				MV1SetMaterialEmiColor(trans_.model, i, DEFAULT_COLOR[i]);
			}
		}
		else {
			for (int i = 0; i < DEFAULT_COLOR.size(); i++) {
				COLOR_F emi = DEFAULT_COLOR[i];
				emi.r = (std::min)(DEFAULT_COLOR[i].r + 0.6f, 1.0f);
				MV1SetMaterialEmiColor(trans_.model, i, emi);
			}
		}

	}
	else if (inviCounter_ == 1) {
		for (int i = 0; i < DEFAULT_COLOR.size(); i++) {
			MV1SetMaterialEmiColor(trans_.model, i, DEFAULT_COLOR[i]);
		}
	}
}

void CharactorBase::AddInFbxAnimation(int inFbxMaxIndex, float speed)
{
	for (int index = 0; index < inFbxMaxIndex; index++) {
		anime_->AddInFbx(index, speed, index);
	}
}

void CharactorBase::AddAnimation(int index, float speed, const char* filePath)
{
	anime_->Add(index, speed, filePath);
}
