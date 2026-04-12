#include "BossHpBarManager.h"

#include"../../../../../Utility/Utility.h"

BossHpBarManager::BossHpBarManager(const unsigned short& HP, const unsigned short HP_MAX, const unsigned char& LIFE):
	HP(HP), HP_MAX(HP_MAX), LIFE(LIFE),
	hpBarFrameImageHandle(-1),
	hpBar()
{
}

void BossHpBarManager::Load(void)
{
	// HPバーのフレーム画像を読み込む
	LoadImg(hpBarFrameImageHandle, "Data/Image/Game/UI/BossHpBarFrame.png");

	// ライフ数の分だけHPバーを生成する
	for (unsigned char i = 0; i < LIFE; i++) { hpBar.emplace_back(new BossHpBar(HP, HP_MAX, LIFE, i)); }

	// 生成したHPバーの読み込み処理
	for (BossHpBar*& bar : hpBar) { bar->Load(); }
}

void BossHpBarManager::Init(const Vector2& position, const std::vector<unsigned int> color)
{
	// HPバーの位置を設定する
	this->position = position;

	// HPバーの初期化
	for (unsigned char i = 0; i < hpBar.size(); i++) { hpBar[i]->Init(this->position, color[i]); }
}

void BossHpBarManager::Update(void)
{
	// HPバーの更新
	for (BossHpBar*& bar : hpBar) { bar->Update(); }
}

void BossHpBarManager::Draw(void)
{
	// HPバーのフレーム画像を描画する
	DrawGraph((int)position.x, (int)position.y, hpBarFrameImageHandle, true);

	// HPバーの描画
	for (unsigned char i = 0; i < LIFE; i++) { hpBar[i]->Draw(); }
}

void BossHpBarManager::Release(void)
{
	// HPバーの解放
	for (BossHpBar*& bar : hpBar) { bar->Release(); delete bar; bar = nullptr; }

	// HPバーのフレーム画像の解放
	DeleteGraph(hpBarFrameImageHandle);
}