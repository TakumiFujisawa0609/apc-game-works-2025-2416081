#pragma once
#include"../ActorBase.h"

#include<map>

class CharactorBase : public ActorBase
{
public:
	CharactorBase();
	~CharactorBase()override;

	virtual void Load(void) = 0;

private:
	// 初期化
	void SubInit(void)override;
	// 更新
	void SubUpdate(void)override;
	// 描画
	void SubDraw(void)override;
	// 解放
	void SubRelease(void)override;

	int state_;
	using STATEFUNC = void (CharactorBase::*)(void);
	std::map<int, STATEFUNC>stateFuncPtr;
#define SET_STATE(state, func) {stateFuncPtr[(int)(state)] = static_cast<STATEFUNC>(func);}

protected:

	// キャラクター固有の処理をここに追加
	virtual void CharactorInit(void) = 0;
	virtual void CharactorUpdate(void) = 0;
	virtual void CharactorDraw(void) = 0;
	virtual void CharactorRelease(void) = 0;

};