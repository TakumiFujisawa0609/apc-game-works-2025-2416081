#pragma once
#include"../UnitBase.h"

#include"../../Manager/AnimationController/AnimationController.h"

#include"Punch/PlayerPunch.h"

#include"Throwing/Throwing.h"

class VoxelBase;

class Player : public UnitBase
{
public:
	static constexpr VECTOR SIZE = { 80.0f-20.0f,185.368f-40.0f,45.0f-15.0f };
	static constexpr VECTOR CENTER_DIFF = { 0.0f,72.684f,0.0f };
	//static constexpr VECTOR CENTER_DIFF = { 0.0f,92.684f,0.0f };


	Player(const VECTOR& cameraAngle);
	~Player()override {};

	void Load(void)override;
	void Init(void)override;
	void Update(void)override;
	void Draw(void)override;
	void Release(void)override;

	void OnCollision(UnitBase* other)override;

	enum class STATE
	{
		NON,
		MOVE,
		ATTACK,
		GOUGE,
		HAVE_THROWING_OBJ,
		THROWING_OBJ,
		EVASION,
		DAMAGE,
		DEATH,

		MAX
	};

	int GetState(void)const override { return (int)state_; }


	// ˆÚ“®ó‘Ô‚Åg—p‚·‚é``-------------------------------------------------------------------
	// ’è”
	static constexpr float RUN_SPEED = 10.0f;			//ƒvƒŒƒCƒ„[‚Ì‘–‚é‘¬“x

	static constexpr float MAX_JUMP_POWER = 100.0f;		//Å‘åƒWƒƒƒ“ƒv—Í
	static constexpr int INPUT_JUMPKEY_FRAME = 6;		//ƒWƒƒƒ“ƒvƒL[‚ğó‚¯•t‚¯‚éƒtƒŒ[ƒ€”
	static constexpr int JUMP_NUM = 2;					//ƒWƒƒƒ“ƒv‰Â”\‰ñ”
	//----------------------------------------------------------------------------------------------

	// UŒ‚ó‘Ô‚Åg—p‚·‚é``-------------------------------------------------------------------
	// ’è”
	enum class ATTACK_STAGE { NON = -1, FIRST, SECOND, THIRD, MAX, };		//UŒ‚‚Ì’i”
	static constexpr int INPUT_ATTACK_FRAME = 20;							//Ÿ‚Ì’i‚É‚Â‚È‚ª‚éŠÔ(ƒtƒŒ[ƒ€”)

	// ŠÖ”
	//----------------------------------------------------------------------------------------------

	// ƒvƒŒƒCƒ„[‚ª•ø‚¦‚é‰ºˆÊƒNƒ‰ƒX‚ÌƒCƒ“ƒXƒ^ƒ“ƒX‚ğ”z—ñ‚ÉŠi”[‚µ‚Ä•Ô‚·
	std::vector<UnitBase*>GetSubIns(void);

private:

#pragma region ó‘ÔŠÇ—

	// Œ»İ‚Ìó‘Ô
	STATE state_;


	/// <summary>
	/// ó‘Ô‚²‚Æ‚É‘JˆÚ‰Â”\‚ÌSTATE‚ğU‚è•ª‚¯‚Ä“ü—Í‘€ì‚É‰‚¶‚Ä‚»‚ÌSTATE‚É‘JˆÚ‚³‚¹‚é
	/// </summary>
	void StateManager(void);

	// Šeó‘Ô‚Ì‘JˆÚ‚·‚éğŒ------------------------
	void DoStateMove(void);		// ˆÚ“®ó‘Ô‚É‘JˆÚ‚·‚éğŒ
	void DoStateAttack(void);	// UŒ‚ó‘Ô‚É‘JˆÚ‚·‚éğŒ
	void DoStateGouge(void);	// P‚èó‘Ô‚É‘JˆÚ‚·‚éğŒ
	void DoStateThrowing(void);	// “Š‚°ó‘Ô‚É‘JˆÚ‚·‚éğŒ
	void DoStateEvasion(void);	// ‰ñ”ğó‘Ô‚É‘JˆÚ‚·‚éğŒ
	//---------------------------------------------

	// ŠÖ”ƒ|ƒCƒ“ƒ^”z—ñ
	using STATEFUNC = void (Player::*)(void);
	STATEFUNC stateFuncPtr[(int)STATE::MAX];

	// ó‘Ô•ÊŠÖ”````
	void Non(void) {};
	void Move(void);
	void Attack(void);
	void Gouge(void);
	void HaveThrowingObj(void);
	void ThrowingObj(void);
	void Evasion(void);
	void Damage(void);
	void Death(void);
	//`````````

#pragma endregion

	const VECTOR LOCAL_ROT = { 0.0f,Utility::Deg2RadF(180.0f),0.0f };


	void CollisionVoxel(VoxelBase* voxel);


#pragma region ó‘Ô•ÊŠÖ”‚Ì’†g
	// ˆÚ“®ˆ—ŠÖŒW--------------------------

	//enum class DIRECTION { NON, FRONT, BACK };
	//struct DIR_VEC { DIRECTION x = DIRECTION::NON, y = DIRECTION::NON, z = DIRECTION::NON; };

	// ŠÖ”
	void Run(void);			//‰¡ˆÚ“®ŠÖ”
	void Jump(void);		//ƒWƒƒƒ“ƒvŠÖ”

	// •Ï”
	const VECTOR& cameraAngle_;	//ƒJƒƒ‰‚ÌŠp“x

	bool isJump_[JUMP_NUM];	//ƒWƒƒƒ“ƒv‚µ‚Ä‚¢‚é‚©‚Ì•Ï”
	int jumpKeyCounter_[JUMP_NUM];
	//---------------------------------------

	// UŒ‚ó‘ÔŠÖŒW``````````

	// ƒCƒ“ƒXƒ^ƒ“ƒX
	PlayerPunch* punch_;
	// UŒ‚‚Ì’i”
	ATTACK_STAGE attackStage_;
	bool isAttack_[(int)ATTACK_STAGE::MAX];
	int attackStageCounter_;
	//```````````````

	// “Š‚°``````````````
	Throwing* throwing_;


	//````````````````
#pragma endregion

#pragma region ƒ‚[ƒVƒ‡ƒ“
	// ƒ‚[ƒVƒ‡ƒ“ŠÇ—ƒNƒ‰ƒX‚ÌƒCƒ“ƒXƒ^ƒ“ƒX
	AnimationController* anime_;

	// ƒ‚[ƒVƒ‡ƒ“‚Ì‘S‚Ä
	enum class ANIME_TYPE {
		IDLE,
		RUN,
		JUMP_POST,
		JUMP,
		FALL,
		EVASION,
		PUNCH_FIRST,
		PUNCH_SECOND,
		PUNCH_THIRD,
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
};

