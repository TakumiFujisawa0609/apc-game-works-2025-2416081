#pragma once

#include"PlayerHpBar.h"

class PlayerHpBarManager
{
public:

	PlayerHpBarManager(const unsigned char& HP,const unsigned char HP_MAX);
	~PlayerHpBarManager();

	void Load(void);
	void Init(const Vector2& position);
	void Update(void);
	void Draw(void);
	void Release(void);

#pragma region 定数定義（public）
	// フレームまで含めた全体の大きさ
	static constexpr float HP_BAR_WHOLE_SIZE_X = 500.0f;
	static constexpr float HP_BAR_WHOLE_SIZE_Y = 50.0f;
#pragma endregion


private:
#pragma region 定数定義（private）

	// HPバーの分割数
	static constexpr unsigned char HP_BAR_DIVISION_NUM_X = 46;
	static constexpr unsigned char HP_BAR_DIVISION_NUM_Y = 3;
	static constexpr unsigned char HP_BAR_DIVISION_NUM = HP_BAR_DIVISION_NUM_X * HP_BAR_DIVISION_NUM_Y;

	// HPバーの1分割あたりの大きさ（縦、横 同一）
	const float HP_BAR_ONE_DIVISION_SIZE = 10.0f;

	// HPバーフレームの描画位置から最初のHPバーの描画位置までのローカル座標
	const Vector2 HP_BAR_FIRST_POS = Vector2(HP_BAR_ONE_DIVISION_SIZE * (HP_BAR_DIVISION_NUM_Y + 0.5f), HP_BAR_ONE_DIVISION_SIZE * (HP_BAR_DIVISION_NUM_Y + 0.5f));

	// 現在のHPバーの１ブロックの描画位置から次のHPバーの１ブロックの描画位置までのローカル座標（縦軸移動の場合）
	const Vector2 HP_BAR_NEXT_POS_USUALLY = Vector2(-HP_BAR_ONE_DIVISION_SIZE, -HP_BAR_ONE_DIVISION_SIZE);
	// 現在のHPバーの１ブロックの描画位置から次のHPバーの１ブロックの描画位置までのローカル座標（横軸移動の場合）
	const Vector2 HP_BAR_NEXT_POS_UNIQUE = Vector2(HP_BAR_ONE_DIVISION_SIZE * HP_BAR_DIVISION_NUM_Y, HP_BAR_ONE_DIVISION_SIZE * (HP_BAR_DIVISION_NUM_Y - 1));
	
	
	// HPバーブロックが１つ落ちてから、次のHPバーブロックが落ちるまでのフレーム数
	const unsigned char HP_BAR_DROP_INTERVAL = 2;
#pragma endregion

	// HPバーオブジェクト
	PlayerHpBar* hpBar[HP_BAR_DIVISION_NUM];

	// フレームの画像ハンドル
	int hpBarFrameImageHandle;

	// 始点座標（左上）
	Vector2 position;

	// プレイヤーの現在のHPの参照
	const unsigned char& HP;
	// プレイヤーの１フレーム前のHP
	unsigned char prevHP;

	// プレイヤーの最大HP
	const unsigned char HP_MAX;

	// 生存している（表示する）HPバーブロックの数
	unsigned char aliveHpBarNum;

	// 死んでいるけどまだ表示しているもの（まだ落としてない、STATE::LOST_IDLE状態）も含めた合計のHPバーブロックの数
	unsigned char totalHpBarNum;

	

	// HPバーブロックが１つ落ちてから、次のHPバーブロックが落ちるまでの間隔時間用カウンター
	unsigned char hpBarDropIntervalCounter;

};