#include"CharactorBase.h"

CharactorBase::CharactorBase():
	ActorBase(),

	state_(0),
	stateFuncPtr()
{
}

CharactorBase::~CharactorBase()
{
}

void CharactorBase::SubInit(void)
{
	// キャラクター固有の初期化
	CharactorInit();
}

void CharactorBase::SubUpdate(void)
{
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
}