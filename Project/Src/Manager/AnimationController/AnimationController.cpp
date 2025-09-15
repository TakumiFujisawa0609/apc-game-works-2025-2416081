#include "AnimationController.h"
#include"../../Scene/SceneManager/SceneManager.h"
#include <DxLib.h>

AnimationController::AnimationController(int modelId)
{
	modelId_ = modelId;
	playType_ = -1;
}

AnimationController::~AnimationController(void)
{
}

void AnimationController::Add(int type, float speed, const std::string path)
{
	Animation animation;
	animation.model = MV1LoadModel(path.c_str());
	animation.animIndex = -1;
	Add(type, speed, animation);
}

void AnimationController::AddInFbx(int type, float speed, int animIndex)
{
	Animation animation;

	animation.model = -1;

	animation.animIndex = animIndex;

	Add(type, speed, animation);

}
void AnimationController::Play(int type,bool loop)
{
	if (playType_ == type) {
		if (!loop)playAnim_.step = 0.0f;
		return;
	}
	if (playType_ != -1) {
		// モデルからアニメーションを外す
		MV1DetachAnim(modelId_, playAnim_.attachNo);
	}

	// アニメーション種別を変更
	playType_ = type;
	playAnim_ = animations_[type];

	// 初期化
	loopflg_ = loop;
	playAnim_.step = 0.0f;

	if (playAnim_.model == -1) {
		// モデルと同じファイルからアニメーションをアタッチする
		playAnim_.attachNo = MV1AttachAnim(modelId_, playAnim_.animIndex);
	}
	else {
		int animIndex = 0;
		playAnim_.attachNo = MV1AttachAnim(modelId_, animIndex, playAnim_.model);
	}


	// アニメーション総時間の取得
	playAnim_.totalTime = MV1GetAttachAnimTotalTime(modelId_, playAnim_.attachNo);

}


void AnimationController::Update(void)
{
	// 経過時間の取得
	float deltaTime = SceneManager::GetIns().GetDeltaTime();

	// 再生
	playAnim_.step += (deltaTime * playAnim_.speed);

	if (loopflg_) {
		if (playAnim_.step > playAnim_.totalTime)playAnim_.step = 0.0f;
	}
	else {
		if (playAnim_.step > playAnim_.totalTime)playAnim_.step = playAnim_.totalTime;

	}

	// アニメーション設定
	MV1SetAttachAnimTime(modelId_, playAnim_.attachNo, playAnim_.step);

}


void AnimationController::Release(void)
{
	MV1DetachAnim(modelId_, playAnim_.attachNo);

	//ロードした外部FBXのモデルのメモリを解放する
	for (auto& pair : animations_) {
		if (pair.second.model == -1)continue;
		MV1DeleteModel(pair.second.model);
	}

	//可変長配列をクリア
	animations_.clear();
}

const bool AnimationController::IsEnd(void) const
{
	bool ret = false;

	if (loopflg_)return ret;

	if (playAnim_.step >= playAnim_.totalTime)ret = true;


	return ret;
}

void AnimationController::Add(int type, float speed, Animation animation)
{
	if (animations_.count(type) == 0) {
		//追加
		animation.speed = speed;
		animations_.emplace(type, animation);
	}
}


