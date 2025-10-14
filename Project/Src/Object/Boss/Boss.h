#pragma once

#include"../UnitBase.h"

#include"../../Manager/AnimationController/AnimationController.h"

#include"Attack/RockWall/RockWallShooter.h"

class Boss : public UnitBase
{
public:
	static constexpr VECTOR SCALE = { 2.0f,2.0f,2.0f };
	static constexpr VECTOR SIZE = { 500.0f,850.0f,350.0f };
	static constexpr VECTOR CENTER_DIFF = { 0.0f,SIZE.y / 2,0.0f };

	Boss(const VECTOR& playerPos);
	~Boss();

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void UiDraw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other)override;


	enum class STATE
	{
		NON,
		IDLE,
		ATTACK,
		DAMAGE,
		STAN,
		BIG_DAMAGE,
		DEATH,

		MAX
	};
	int GetState(void)const override { return (int)state_; }


	static constexpr int HP_MAX = 300;


private:
#pragma region 状態管理

	// 現在の状態
	STATE state_;


	/// <summary>
	/// 状態ごとに遷移可能のSTATEを振り分けて入力操作に応じてそのSTATEに遷移させる
	/// </summary>
	void StateManager(void);

	// 関数ポインタ配列
	using STATEFUNC = void (Boss::*)(void);
	STATEFUNC stateFuncPtr[(int)STATE::MAX];

	// 状態別関数～～～～
	void Non(void) {};
	void Idle(void);
	void Attack(void);
	void Damage(void);
	void Stan(void);
	void BigDamage(void);
	void Death(void);
	//～～～～～～～～～

#pragma endregion

#pragma region 状態別関数の中身


#pragma endregion

#pragma region モーション
	// モーション管理クラスのインスタンス
	AnimationController* anime_;

	// モーションの全て
	enum class ANIME_TYPE {
		ATTACK,
		DEATH,
		DAMAGE,
		IDLE,
		JUMP,
		RUN,
		WALK,

		MAX,

		// モデル外
		FALL,
	};

	// モーションの初期設定
	void AnimeLoad(void);
#pragma endregion

#pragma region プレイヤーが抱える下位クラスのメイン処理をまとめて呼び出す
	void SubLoad(void);
	void SubInit(void);
	void SubUpdate(void);
	void SubDraw(void);
	void SubRelease(void);
#pragma endregion

	void HpSharpen(int damage);

	std::vector<COLOR_F> DEFAULT_COLOR;

	RockWallShooter* rockWall_;

	const VECTOR LOCAL_ROT = { 0.0f,Utility::Deg2RadF(180.0f),0.0f };

	const VECTOR& playerPos;
};
