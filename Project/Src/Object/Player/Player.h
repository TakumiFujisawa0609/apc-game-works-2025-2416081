#pragma once
#include"../UnitBase.h"

#include"../../Manager/AnimationController/AnimationController.h"

#include"Punch/PlayerPunch.h"

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
		HAVE_THROWING_OBJ,
		THROWING,
		EVASION,
		DAMAGE,
		DEATH,

		MAX
	};

	int GetState(void)const override { return (int)state_; }


	// 移動状態で使用する～～-------------------------------------------------------------------
	// 定数
	static constexpr float RUN_SPEED = 10.0f;			//プレイヤーの走る速度

	static constexpr float MAX_JUMP_POWER = 70.0f;		//最大ジャンプ力
	static constexpr int INPUT_JUMPKEY_FRAME = 6;		//ジャンプキーを受け付けるフレーム数
	static constexpr int JUMP_NUM = 2;					//ジャンプ可能回数
	//----------------------------------------------------------------------------------------------

	// 攻撃状態で使用する～～-------------------------------------------------------------------
	// 定数
	enum class ATTACK_STAGE { NON = -1, FIRST, SECOND, THIRD, MAX, };		//攻撃の段数
	static constexpr int INPUT_ATTACK_FRAME = 20;							//次の段につながる時間(フレーム数)

	// 関数
	//----------------------------------------------------------------------------------------------

	// プレイヤーが抱える下位クラスのインスタンスを配列に格納して返す
	std::vector<UnitBase*>GetSubIns(void);

private:

#pragma region 状態管理

	// 現在の状態
	STATE state_;


	/// <summary>
	/// 状態ごとに遷移可能のSTATEを振り分けて入力操作に応じてそのSTATEに遷移させる
	/// </summary>
	void StateManager(void);

	// 各状態の遷移する条件------------------------
	void DoStateMove(void);		// 移動状態に遷移する条件
	void DoStateAttack(void);	// 攻撃状態に遷移する条件
	void DoStateEvasion(void);	// 回避状態に遷移する条件
	//---------------------------------------------

	// 関数ポインタ配列
	using STATEFUNC = void (Player::*)(void);
	STATEFUNC stateFuncPtr[(int)STATE::MAX];

	// 状態別関数～～～～
	void Non(void) {};
	void Move(void);
	void Attack(void);
	void HaveThrowingObj(void);
	void Throwing(void);
	void Evasion(void);
	void Damage(void);
	void Death(void);
	//～～～～～～～～～

#pragma endregion

	const VECTOR LOCAL_ROT = { 0.0f,Utility::Deg2RadF(180.0f),0.0f };


	void CollisionVoxel(VoxelBase* voxel);


#pragma region 状態別関数の中身
	// 移動処理関係--------------------------

	// 関数
	void Run(void);			//横移動関数
	void Jump(void);		//ジャンプ関数

	// 変数
	const VECTOR& cameraAngle_;	//カメラの角度

	bool isJump_[JUMP_NUM];	//ジャンプしているかの変数
	int jumpKeyCounter_[JUMP_NUM];
	//---------------------------------------

	// 攻撃状態関係～～～～～～～～～～

	// インスタンス
	PlayerPunch* punch_;
	// 攻撃の段数
	ATTACK_STAGE attackStage_;
	bool isAttack_[(int)ATTACK_STAGE::MAX];
	int attackStageCounter_;
	//～～～～～～～～～～～～～～～
#pragma endregion

#pragma region モーション
	// モーション管理クラスのインスタンス
	AnimationController* anime_;

	// モーションの全て
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
};

