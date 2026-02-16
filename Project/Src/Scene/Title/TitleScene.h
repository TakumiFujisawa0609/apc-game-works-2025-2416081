#pragma once
#include"../SceneBase.h"

#include"../../Common/Vector3.h"

class SkyDome;

class TitleScene : public SceneBase
{
public:
	TitleScene();
	~TitleScene()override = default;

	// 読み込み
	void Load(void)override;
	// 初期化処理
	void Init(void)override;
	// 更新ステップ
	void Update(void)override;
	// 描画処理
	void Draw(void)override;
	// 解放処理
	void Release(void)override;

private:

	// タイトルの背景の画像
	int img;

	// 操作誘導の画像
	int pushToImg[2];
	const int& GetPushToImg(void)const;

	// 点滅のカウンター
	unsigned char blinkingCounter;
	signed char blinkingSigned;

	// 点滅のカウンターの最大値
	const unsigned char BLINKING_COUNTER_MAX = 240;
	// 点滅のカウンターの最小値
	const unsigned char BLINKING_COUNTER_MIN = 150;

	// スカイドーム
	SkyDome* skyDome;

	// スカイドームの座標
	const Vector3 SKY_DOME_POS = Vector3();
};