#pragma once

#include<vector>

#include"BossHpBar.h"

class BossHpBarManager
{
public:
	BossHpBarManager(const unsigned short& HP, const unsigned short HP_MAX, const unsigned char& LIFE);
	~BossHpBarManager() = default;

	void Load(void);
	void Init(const Vector2& position, const std::vector<unsigned int> color);
	void Update(void);
	void Draw(void);
	void Release(void);

#pragma region 定数定義
	// フレームまで含めた全体の大きさ
	static constexpr float HP_BAR_WHOLE_SIZE_X = 700.0f;
	static constexpr float HP_BAR_WHOLE_SIZE_Y = 80.0f;
#pragma endregion

private:

	// フレームの画像ハンドル
	int hpBarFrameImageHandle;

	// HPバー
	std::vector<BossHpBar*>hpBar;

	// 始点座標（左上）
	Vector2 position;


	// ボスの現在のHPの参照
	const unsigned short& HP;
	// ボスの最大HP
	const unsigned short HP_MAX;

	// ボスの現在のライフの参照
	const unsigned char& LIFE;
};