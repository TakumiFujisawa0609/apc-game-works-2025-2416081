#pragma once

#include"../UnitBase.h"

#include"../../Manager/AnimationController/AnimationController.h"

#include"Attack/RockWall/RockWallShooter.h"
#include"Attack/Stone/StoneShooter.h"
#include"Attack/Fall/FallManager.h"

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




	static constexpr int HP_MAX = 100;


private:
#pragma region ó‘ÔŠÇ—

	// Œ»İ‚Ìó‘Ô
	STATE state_;


	/// <summary>
	/// ó‘Ô‚²‚Æ‚É‘JˆÚ‰Â”\‚ÌSTATE‚ğU‚è•ª‚¯‚Ä“ü—Í‘€ì‚É‰‚¶‚Ä‚»‚ÌSTATE‚É‘JˆÚ‚³‚¹‚é
	/// </summary>
	void StateManager(void);

	// ŠÖ”ƒ|ƒCƒ“ƒ^”z—ñ
	using STATEFUNC = void (Boss::*)(void);
	STATEFUNC stateFuncPtr[(int)STATE::MAX];

	// ó‘Ô•ÊŠÖ”````
	void Non(void) {};
	void Idle(void);
	void Attack(void);
	void Damage(void);
	void Stan(void);
	void BigDamage(void);
	void Death(void);
	//`````````

#pragma endregion

#pragma region ó‘Ô•ÊŠÖ”‚Ì’†g


	// UŒ‚ó‘Ô```````````````````
	enum class ATTACK_KINDS
	{
		NON = -1,

		FALL,
		STONE,

		WALL,

		MAX,
	};
	ATTACK_KINDS attackState_;

	// UŒ‚‚Ìí—Ş‚Ì’Š‘I‚ğs‚¤ŠÖ”
	ATTACK_KINDS AttackLottery(void);

	//ŠeUŒ‚‚ÌƒCƒ“ƒXƒ^ƒ“ƒX----
	RockWallShooter* rockWall_;
	StoneShooter* stone_;
	FallManager* fall_;
	//------------------------

	// UŒ‚‚ÌŠÔŠu‚ğŠÇ—‚·‚éƒJƒEƒ“ƒ^[
	int attackInterval_;
	const int ATTACK_INTERVAL = 300;

	// UŒ‚‘JˆÚŒã1‰ñ–Ú‚ğŒ©•ª‚¯‚é•Ï”
	bool attackInit_;
	// UŒ‚ŠJn‚ÌuŠÔ‚ğŒ©•ª‚¯‚é•Ï”
	bool attackStart_;
	// UŒ‚I—¹‚ğŒ©•ª‚¯‚é•Ï”
	bool attackEnd_;

	const int SP_ATTACK_MEASU = 5;
	int spAttackMeasu_;
	// `````````````````````````

	// UŒ‚ó‘Ô`````````````````````
	const int STAN_TIME = 500;
	int stanTimer_;
	// `````````````````````````


#pragma endregion

#pragma region ƒ‚[ƒVƒ‡ƒ“
	// ƒ‚[ƒVƒ‡ƒ“ŠÇ—ƒNƒ‰ƒX‚ÌƒCƒ“ƒXƒ^ƒ“ƒX
	AnimationController* anime_;

	// ƒ‚[ƒVƒ‡ƒ“‚Ì‘S‚Ä
	enum class ANIME_TYPE {
		ATTACK,
		DAMAGE,
		DEATH,
		IDLE,
		JUMP,
		PUNCH,
		RUN,
		SLAP,
		STAN,
		WALK,

		MAX,

		// ƒ‚ƒfƒ‹ŠO
		FALL,
	};

	// ƒ‚[ƒVƒ‡ƒ“‚Ì‰Šúİ’è
	void AnimeLoad(void);
#pragma endregion

#pragma region ƒvƒŒƒCƒ„[‚ª•ø‚¦‚é‰ºˆÊƒNƒ‰ƒX‚ÌƒƒCƒ“ˆ—‚ğ‚Ü‚Æ‚ß‚ÄŒÄ‚Ño‚·
	void SubLoad(void);
	void SubInit(void);
	void SubUpdate(void);
	void SubDraw(void);
	void SubRelease(void);
#pragma endregion

	const int MASTER_LIFE = 3;
	int masterLife_;

	void HpSharpen(int damage);
	void LifeSharpen(void);

	std::vector<COLOR_F> DEFAULT_COLOR;


	const VECTOR LOCAL_ROT = { 0.0f,Utility::Deg2RadF(180.0f),0.0f };

	const VECTOR& playerPos;
};
