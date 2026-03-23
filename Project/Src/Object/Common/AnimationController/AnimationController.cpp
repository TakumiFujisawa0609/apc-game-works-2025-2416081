#include "AnimationController.h"
#include <DxLib.h>

AnimationController::AnimationController(int modelId) : 
	modelId(modelId),
	playType(-1),

	playAnim(),

	loopflg(false)
{
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
	if (playType == type) {
		if (!loop)playAnim.step = 0.0f;
		return;
	}
	if (playType != -1) {
		// モデルからアニメーションを外す
		MV1DetachAnim(modelId, playAnim.attachNo);
	}

	// アニメーション種別を変更
	playType = type;
	playAnim = animations[type];

	// 初期化
	loopflg = loop;
	playAnim.step = 0.0f;

	if (playAnim.model == -1) {
		// モデルと同じファイルからアニメーションをアタッチする
		playAnim.attachNo = MV1AttachAnim(modelId, playAnim.animIndex);
	}
	else {
		int animIndex = 0;
		playAnim.attachNo = MV1AttachAnim(modelId, animIndex, playAnim.model);
	}


	// アニメーション総時間の取得
	playAnim.totalTime = MV1GetAttachAnimTotalTime(modelId, playAnim.attachNo);

}


void AnimationController::Update(void)
{
	// 再生
	playAnim.step += playAnim.speed;

	if (loopflg) {
		if (playAnim.step >= playAnim.totalTime)playAnim.step = 0.0f;
	}
	else {
		if (playAnim.step >= playAnim.totalTime)playAnim.step = playAnim.totalTime;

	}

	// アニメーション設定
	MV1SetAttachAnimTime(modelId, playAnim.attachNo, playAnim.step);

}


void AnimationController::Release(void)
{
	MV1DetachAnim(modelId, playAnim.attachNo);

	//ロードした外部FBXのモデルのメモリを解放する
	for (auto& pair : animations) {
		if (pair.second.model == -1)continue;
		MV1DeleteModel(pair.second.model);
	}

	//可変長配列をクリア
	animations.clear();
}

const bool AnimationController::IsEnd(void) const
{
	bool ret = false;

	if (loopflg)return ret;

	if (playAnim.step >= playAnim.totalTime)ret = true;


	return ret;
}

void AnimationController::Add(int type, float speed, Animation animation)
{
	if (animations.count(type) == 0) {
		//追加
		animation.speed = speed;
		animations.emplace(type, animation);
	}
}


