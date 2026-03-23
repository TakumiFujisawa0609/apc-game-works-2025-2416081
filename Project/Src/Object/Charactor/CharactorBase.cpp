#include"CharactorBase.h"

CharactorBase::CharactorBase() :
	ActorBase(),

	state(0),
	stateFuncPtr(),

	DEFAULT_COLOR(),

	anime(nullptr),

	inviCounter(0),
	isInviEffect(false)
{
}

CharactorBase::CharactorBase(const std::string& parameterPath) :
	ActorBase(parameterPath),

	state(0),
	stateFuncPtr(),

	DEFAULT_COLOR(),

	anime(nullptr),

	inviCounter(0),
	isInviEffect(false)
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
	(this->*stateFuncPtr[state])();

	// アニメーション更新
	if (anime) { anime->Update(); }
}

void CharactorBase::SubDraw(void)
{
	// キャラクター固有の描画
	CharactorDraw();
}

void CharactorBase::SubAlphaDraw(void)
{
	// キャラクター固有の描画
	CharactorAlphaDraw();
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
	if (anime) {
		anime->Release();
		delete anime;
		anime = nullptr;
	}
}

void CharactorBase::Invi(void)
{
	if (inviCounter > 0) { inviCounter--; }
	else { inviCounter = 0; }

	// ダメージ演出
	if (!isInviEffect) { return; }

	if (inviCounter > 1) {
		if (inviCounter / 10 % 2 == 0) {
			for (int i = 0; i < DEFAULT_COLOR.size(); i++) {
				MV1SetMaterialEmiColor(trans.model, i, DEFAULT_COLOR[i]);
			}
		}
		else {
			for (int i = 0; i < DEFAULT_COLOR.size(); i++) {
				COLOR_F emi = DEFAULT_COLOR[i];
				emi.r = (std::min)(DEFAULT_COLOR[i].r + 0.6f, 1.0f);
				MV1SetMaterialEmiColor(trans.model, i, emi);
			}
		}

	}
	else if (inviCounter == 1) {
		for (int i = 0; i < DEFAULT_COLOR.size(); i++) {
			MV1SetMaterialEmiColor(trans.model, i, DEFAULT_COLOR[i]);
		}
	}
}

void CharactorBase::AddInFbxAnimation(int inFbxMaxIndex, float speed)
{
	for (int index = 0; index < inFbxMaxIndex; index++) {
		anime->AddInFbx(index, speed, index);
	}
}

void CharactorBase::AddInFbxAnimation(int inFbxMaxIndex, const float* speed)
{
	for (int index = 0; index < inFbxMaxIndex; index++) {
		anime->AddInFbx(index, speed[index], index);
	}
}

void CharactorBase::AddAnimation(int index, float speed, const char* filePath)
{
	anime->Add(index, speed, filePath);
}
