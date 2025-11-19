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
	void SubInit(void)override;
	// 更新
	void SubUpdate(void)override;
	// 描画
	void SubDraw(void)override;
	// 解放
	void SubRelease(void)override;

	// モデルデフォルトカラー保存用配列
	std::vector<COLOR_F> DEFAULT_COLOR;

	// アニメーションコントローラーのインスタンス
	AnimationController* anime_;

	// 無敵カウンター
	unsigned char inviCounter_;
	
	// 無敵カウンターの更新
	void Invi(void);

	// 無敵カウンターによるダメージ演出を行うかどうか（1 = 「する」、0 = 「しない」）← デフォルトは「する」
	unsigned char isInviEffect_;

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

	// 無敵カウンターのゲット関数
	unsigned char GetInviCounter(void)const { return inviCounter_; }
	// 無敵カウンターのセット関数
	void SetInviCounter(unsigned char counter) { inviCounter_ = (counter < 0) ? 0 : (counter > 255) ? 255 : counter; }

	// 無敵演出フラグのセット関数（true = 「する」、false = 「しない」）← デフォルトは「する」
	void SetInviEffectFlg(bool flg = true) {
		if (isInviEffect_ == (flg) ? 1 : 0) { return; }

		isInviEffect_ = (flg) ? 1 : 0;

		// 無敵演出フラグが変わったとき色をデフォルトに戻す

		// 無敵演出フラグが「する」に変わったときはモデルデフォルトカラーを保存しておく
		int mnum = MV1GetMaterialNum(trans_.model);
		for (int i = 0; i < mnum; ++i) {
			COLOR_F emi = MV1GetMaterialEmiColor(trans_.model, i);

			emi.r = (std::min)(emi.r + 0.4f, 1.0f);
			emi.g = (std::min)(emi.g + 0.4f, 1.0f);
			emi.b = (std::min)(emi.b + 0.4f, 1.0f);

			// モデルデフォルトカラーの保存
			if (isInviEffect_) { DEFAULT_COLOR.emplace_back(emi); }

			MV1SetMaterialEmiColor(trans_.model, i, emi);
		}

		// 「しない」に変わったときは必要ないので情報を破棄する
		if (isInviEffect_ == 0) { DEFAULT_COLOR.clear(); }
	}

};