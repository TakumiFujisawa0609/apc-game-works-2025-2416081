#pragma once
#include<map>
#include<string>


class AnimationController
{
public:

	// アニメーションデータ
	struct Animation
	{
		int model = -1;
		int attachNo = -1;
		int animIndex = 0;
		float speed = 0.0f;
		float totalTime = 0.0f;
		float step = 0.0f;
	};

	// コンストラクタ
	AnimationController(int modelId);

	// デストラクタ
	~AnimationController(void);

	//外部FBXからアニメーションを追加
	void Add(int type, float speed, const std::string path);

	// 同じFBX内のアニメーションを準備
	void AddInFbx(int type, float speed, int animIndex);

	// アニメーション再生
	void Play(int type,bool loop=true);
	void Update(void);
	void Release(void);

	const bool IsEnd(void)const;
	const int GetPlayType(void)const { return playType_; }
	const bool GetAnimEnd(void)const { return playAnim_.step >= playAnim_.totalTime; }

	const float GetAnimeRatio(void)const { return (playAnim_.step / playAnim_.totalTime); }

private:
	// アニメーションするモデルのハンドルID
	int modelId_;

	// 種類別のアニメーションデータ
	std::map<int, Animation> animations_;

	// 再生中のアニメーション
	int playType_;
	Animation playAnim_;
	bool loopflg_;

	void Add(int type, float speed, Animation animation);

};

