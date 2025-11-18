#include"CharactorBase.h"

CharactorBase::CharactorBase():
	ActorBase(),

	state_(0),
	stateFuncPtr(),

	anime_(nullptr)
{
}

CharactorBase::~CharactorBase()
{
}

void CharactorBase::LowerInit(void)
{
	// モデルデフォルトカラーの保存
	int mnum = MV1GetMaterialNum(trans_.model);
	for (int i = 0; i < mnum; ++i) {
		COLOR_F emi = MV1GetMaterialEmiColor(trans_.model, i);
		emi.r = (std::min)(emi.r + 0.4f, 1.0f);
		emi.g = (std::min)(emi.g + 0.4f, 1.0f);
		emi.b = (std::min)(emi.b + 0.4f, 1.0f);
		DEFAULT_COLOR.emplace_back(emi);
		MV1SetMaterialEmiColor(trans_.model, i, emi);
	}

	// キャラクター固有の初期化
	CharactorInit();
}

void CharactorBase::LowerUpdate(void)
{
	// キャラクター固有の更新
	CharactorUpdate();

	// 派生先で割り振り可能のステート別関数
	(this->*stateFuncPtr[state_])();

}

void CharactorBase::LowerDraw(void)
{
	// キャラクター固有の描画
	CharactorDraw();
}

void CharactorBase::LowerRelease(void)
{
	// キャラクター固有の解放
	CharactorRelease();

	// アニメーションコントローラーの解放（使われていたら）
	if (anime_) {
		anime_->Release();
		delete anime_;
		anime_ = nullptr;
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
