#pragma once
#include"../ActorBase.h"
#include"../Common/AnimationController/AnimationController.h"

#include<map>

class CharactorBase : public ActorBase
{
public:
	CharactorBase();
	~CharactorBase()override;

	virtual void Load(void) = 0;

	// ステートのゲット関数
	int GetState(void)const { return state_; }

private:
	// 初期化
	void LowerInit(void)override;
	// 更新
	void LowerUpdate(void)override;
	// 描画
	void LowerDraw(void)override;
	// 解放
	void LowerRelease(void)override;

	std::vector<COLOR_F> DEFAULT_COLOR;

	AnimationController* anime_;

protected:
	// ステート管理用変数
	int state_;
	using STATEFUNC = void (CharactorBase::*)(void);
	std::map<int, STATEFUNC>stateFuncPtr;
#define SET_STATE(state, func) {stateFuncPtr[(int)(state)] = static_cast<STATEFUNC>(func);}

	// キャラクター固有の処理をここに追加
	virtual void CharactorInit(void) = 0;
	virtual void CharactorUpdate(void) = 0;
	virtual void CharactorDraw(void) = 0;
	virtual void CharactorRelease(void) = 0;

#pragma region アニメーションコントローラー
	// アニメーションコントローラーの作成
	void CreateAnimationController(void) { anime_ = new AnimationController(trans_.model); }
	
	/// <summary>
	/// モデルにくっついてるFBXアニメーションを全部登録する
	/// </summary>
	/// <param name="inFbxMaxIndex">くっついてるアニメーションの数</param>
	/// <param name="speed">再生速度</param>
	void AddInFbxAnimation(int inFbxMaxIndex, float speed);

	/// <summary>
	/// 別ファイルに保存されているFBXアニメーションを登録する
	/// </summary>
	/// <param name="index">参照番号</param>
	/// <param name="speed">再生速度</param>
	/// <param name="filePath">パス</param>
	void AddAnimation(int index, float speed, const char* filePath);

	void AnimePlay(int type, bool loop = true) { anime_->Play(type, loop); }

	bool IsAnimeEnd(void)const { return anime_->GetAnimEnd(); }

	float GetAnimeRatio(void)const { return anime_->GetAnimeRatio();}

	int GetAnimePlayType(void)const { return anime_->GetPlayType(); }
	
#pragma endregion オーバーライド不可(使用のみ)



};