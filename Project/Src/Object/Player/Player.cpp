#include "Player.h"

#include"../../Manager/Input/KeyManager.h"

#include"../../Manager/Sound/SoundManager.h"

#include"../Rock/Rock.h"

Player::Player():
	state_(STATE::NON),
	stateFuncPtr(),

	isJump_(),
	jumpKeyCounter_(),

	punch_(nullptr),
	attackStage_(ATTACK_STAGE::NON),
	isAttack_(),
	attackStageCounter_(0),

	anime_(nullptr)
{
}

void Player::Load(void)
{
	unit_.para_.colliType = CollisionType::ALLY;
	unit_.para_.colliShape = CollisionShape::CAPSULE;
	unit_.para_.size = SIZE;
	unit_.para_.radius = SIZE.z / 2;
	unit_.para_.capsuleHalfLen = (SIZE.y / 2) - (unit_.para_.radius * 2);

	unit_.model_ = MV1LoadModel("Data/Model/Player/Player.mv1");
	if (unit_.model_ != -1) {
		MV1SetScale(unit_.model_, { 1.0f,1.0f,1.0f });
	}

	unit_.para_.speed = 10.0f;

#pragma region 関数ポインタ配列へ各関数を格納

#define SET_STATE(state, func) stateFuncPtr[(int)(state)] = static_cast<STATEFUNC>(func)

	SET_STATE(STATE::NON, &Player::Non);
	SET_STATE(STATE::MOVE, &Player::Move);
	SET_STATE(STATE::ATTACK, &Player::Attack);
	SET_STATE(STATE::EVASION, &Player::Evasion);
	SET_STATE(STATE::DAMAGE, &Player::Damage);
	SET_STATE(STATE::DEATH, &Player::Death);

#pragma endregion


	// モーションの初期設定と初期モーション再生
	AnimeLoad();
	anime_->Play((int)ANIME_TYPE::IDLE);


	// 音声読み込み
	Smng::GetIns().Load(SOUND::PLAYER_RUN);
	Smng::GetIns().Load(SOUND::PLAYER_PUNCH);
	Smng::GetIns().Load(SOUND::PLAYER_EVASION);


	// プレイヤーが抱える下位クラスの読み込み処理
	SubLoad();
}

void Player::Init(void)
{
	unit_.pos_ = { 0.0f,0.0f,-500.0f };
	unit_.angle_ = {};

	unit_.isAlive_ = true;

	state_ = STATE::MOVE;

	for (auto& jump : isJump_) { jump = false; }
	for (auto& cou : jumpKeyCounter_) { cou = 0; }

	attackStage_ = ATTACK_STAGE::NON;
	for (auto& at : isAttack_) { at = false; }
	attackStageCounter_ = 0;

	SubInit();
}

void Player::Update(void)
{
	// 無敵カウンターの更新
	Invi();

	prevPos_ = unit_.pos_;

	// ステート遷移条件
	StateManager();

	// 現在のステートに対応する関数を実行
	(this->*stateFuncPtr[(int)state_])();

	SubUpdate();

	// 重力
	Gravity();

	// 加速度の加算
	AccelUpdate();

	// アニメーションの更新
	anime_->Update();



	if (unit_.pos_.y < SIZE.y / 2) {
		yAccelSum_ = 0.0f;
		unit_.pos_.y = SIZE.y / 2;
		for (auto& jump : isJump_) { jump = false; }
		for (auto& cou : jumpKeyCounter_) { cou = 0; }
	}
}

void Player::Draw(void)
{
	if (!unit_.isAlive_) { return; }

	SubDraw();

	MV1SetPosition(unit_.model_, VAdd(unit_.pos_, CENTER_DIFF));
	MV1SetRotationXYZ(unit_.model_, unit_.angle_);
	MV1DrawModel(unit_.model_);
}

void Player::Release(void)
{
	// プレイヤーが抱えている下位クラスのゲーム終了時処理
	SubRelease();

	// 音声解放
	Smng::GetIns().Delete(SOUND::PLAYER_RUN);
	Smng::GetIns().Delete(SOUND::PLAYER_PUNCH);
	Smng::GetIns().Delete(SOUND::PLAYER_EVASION);

	// アニメーション管理クラスの解放
	if (anime_) {
		anime_->Release();
		delete anime_;
		anime_ = nullptr;
	}

	// モデル解放
	MV1DeleteModel(unit_.model_);
}

void Player::OnCollision(UnitBase* other)
{
	if (VoxelBase* voxel = dynamic_cast<VoxelBase*>(other)) {
		// ボクセル形状に沿ってプレイヤーを押し戻す
		CollisionVoxel(voxel);
		return;
	}
}

void Player::CollisionVoxel(VoxelBase* voxel)
{
}


void Player::StateManager(void)
{
	//各状態から遷移可能にさせたい状態の、
	//遷移条件関数 ( DoState～～() ) を呼び出す
	switch (state_)
	{
	case STATE::MOVE:
		DoStateAttack();
		DoStateEvasion();
		if (state_ != STATE::MOVE) { Smng::GetIns().Stop(SOUND::PLAYER_RUN); }
		break;
	case STATE::ATTACK:
		DoStateMove();
		DoStateEvasion();
		if (state_ != STATE::ATTACK) { punch_->Off(); }
		break;
	case STATE::EVASION:
		break;
	case STATE::DAMAGE:
		break;
	case STATE::DEATH:
		break;
	}
}
void Player::DoStateMove(void)
{
	auto& key = KEY::GetIns();

	if (key.GetInfo(KEY_TYPE::MOVE_UP).down ||
		key.GetInfo(KEY_TYPE::MOVE_DOWN).down ||
		key.GetInfo(KEY_TYPE::MOVE_RIGHT).down ||
		key.GetInfo(KEY_TYPE::MOVE_LEFT).down ||
		key.GetInfo(KEY_TYPE::JUMP).down
		) 
	{
		state_ = STATE::MOVE;
	}
}
void Player::DoStateAttack(void)
{
	if (!KEY::GetIns().GetInfo(KEY_TYPE::ATTACK).down) { return; }


	state_ = STATE::ATTACK;

	// 最終段までいっている または 前の段の攻撃から一定時間過ぎていたら フラグリセット
	if ((isAttack_[(int)ATTACK_STAGE::MAX - 1]) || (attackStageCounter_ > INPUT_ATTACK_FRAME)) {
		for (int i = 0; i < (int)ATTACK_STAGE::MAX; i++) { isAttack_[i] = false; }
	}

	// １段目から探索して適切な段数をattackStage_に代入する
	for (int i = 0; i < (int)ATTACK_STAGE::MAX; i++) {
		if (!isAttack_[i]) {
			isAttack_[i] = true;
			attackStageCounter_ = 0;
			attackStage_ = (ATTACK_STAGE)i;
			break;
		}
	}

	// モーション更新
	switch (attackStage_)
	{
	case Player::ATTACK_STAGE::FIRST:
		anime_->Play((int)ANIME_TYPE::PUNCH_FIRST, false);
		break;
	case Player::ATTACK_STAGE::SECOND:
		anime_->Play((int)ANIME_TYPE::PUNCH_SECOND, false);
		break;
	case Player::ATTACK_STAGE::THIRD:
		anime_->Play((int)ANIME_TYPE::PUNCH_THIRD, false);
		break;
	}

	// SE再生
	Smng::GetIns().Play(SOUND::PLAYER_PUNCH, true, 150);
}
void Player::DoStateEvasion(void)
{
	if (!KEY::GetIns().GetInfo(KEY_TYPE::EVASION).down) { return; }

	state_ = STATE::EVASION;

	anime_->Play((int)ANIME_TYPE::EVASION, false);

	// SE再生
	Smng::GetIns().Play(SOUND::PLAYER_EVASION, true, 150);
}

void Player::Move(void)
{
	Run();

	Jump();

	if (attackStageCounter_ <= INPUT_ATTACK_FRAME) attackStageCounter_++;
}

void Player::Attack(void)
{	
	// 攻撃の判定が発生する前の間、前方に移動させる
	if (anime_->GetAnimeRatio() < 0.4f) {
		// 移動方向ベクトル
		VECTOR vec = {};

		// unit_.angle_(角度) から ベクトル(向き) を割り出す
		vec.x = sinf(unit_.angle_.y + Utility::Deg2RadF(180.0f));
		vec.z = cosf(unit_.angle_.y + Utility::Deg2RadF(180.0f));

		// 割り出したベクトルを単位ベクトルに直しスピードを乗算して座標情報に加算する
		vec = Utility::Normalize(vec);
		unit_.pos_ = VAdd(unit_.pos_, VScale(vec, 2.0f));
	}

	// 毎フレーム一旦オフ(攻撃判定)
	punch_->Off();

	// 大体攻撃判定を発生させる時間
	if (anime_->GetAnimeRatio() >= 0.4f && anime_->GetAnimeRatio() <= 0.6f) { punch_->On(); }

	// 攻撃判定終わったらボタンで次段攻撃に遷移可能にしておく(操作性向上)
	if (anime_->GetAnimeRatio() > 0.6f) { DoStateAttack(); }

	// 何も入力なく攻撃アニメーションが終了したら通常状態に自動で遷移
	if (anime_->GetAnimEnd()) { state_ = STATE::MOVE; }
}

void Player::Evasion(void)
{
	// 移動方向ベクトル
	VECTOR vec = {};

	// unit_.angle_(角度) から ベクトル(向き) を割り出す
	vec.x = sinf(unit_.angle_.y + Utility::Deg2RadF(180.0f));
	vec.z = cosf(unit_.angle_.y + Utility::Deg2RadF(180.0f));

	// 割り出したベクトルを単位ベクトルに直しスピードを乗算して座標情報に加算する
	vec = Utility::Normalize(vec);
	unit_.pos_ = VAdd(unit_.pos_, VScale(vec, unit_.para_.speed / 1.5f));

	// 無敵(無敵カウンターを使って当たり判定を無効にする。この状態を抜けたらすぐに無敵が解除されるように １ を代入し続けておく)
	if (anime_->GetAnimeRatio() <= 0.7f) { unit_.inviciCounter_ = 1; }
	// 無敵判定が終わったらボタンで遷移可能にしておく(操作性向上)
	else { DoStateMove(); DoStateAttack(); }

	// 何も入力なく回避アニメーションが終了したら通常状態に自動で遷移
	if (anime_->GetAnimEnd()) { state_ = STATE::MOVE; }
}

void Player::Damage(void)
{
}

void Player::Death(void)
{
}


void Player::Run(void)
{
	auto& key = KEY::GetIns();

	VECTOR vec = {};

	if (key.GetInfo(KEY_TYPE::MOVE_UP).now) { vec.z++; }
	if (key.GetInfo(KEY_TYPE::MOVE_DOWN).now) { vec.z--; }
	if (key.GetInfo(KEY_TYPE::MOVE_RIGHT).now) { vec.x++; }
	if (key.GetInfo(KEY_TYPE::MOVE_LEFT).now) { vec.x--; }

	vec = Utility::Normalize(vec);
	unit_.pos_ = VAdd(unit_.pos_, VScale(vec, unit_.para_.speed));

	if (Utility::VZERO(vec)) {
		if (!isJump_[0]) { anime_->Play((int)ANIME_TYPE::IDLE); }
		Smng::GetIns().Stop(SOUND::PLAYER_RUN);
	}
	else {
		if (!isJump_[0]) {
			anime_->Play((int)ANIME_TYPE::RUN);
			Smng::GetIns().Play(SOUND::PLAYER_RUN, false, 150, true);
		}
		else { Smng::GetIns().Stop(SOUND::PLAYER_RUN); }

		unit_.angle_.y = atan2(vec.x, vec.z);
		unit_.angle_.y += Utility::Deg2RadF(180.0f);
	}
}

void Player::Jump(void)
{
	auto& key = KEY::GetIns();

	for (int i = 0; i < JUMP_NUM; i++) {
		if (isJump_[i]) { continue; }

		// ダウントリガーでジャンプ開始
		if (key.GetInfo(KEY_TYPE::JUMP).down) {
			isJump_[i] = true; 
			anime_->Play((int)ANIME_TYPE::JUMP_POST, false);
		}

		// ジャンプしていなかったらループから抜ける
		if (!isJump_[i]) { break; }

		//ジャンプキーを離したら、ジャンプキー入力判定を終了
		if (key.GetInfo(KEY_TYPE::JUMP).up) { jumpKeyCounter_[i] = INPUT_JUMPKEY_FRAME; }

		//入力時間に応じてジャンプ量を変更する
		if (isJump_[i] && key.GetInfo(KEY_TYPE::JUMP).now && jumpKeyCounter_[i] < INPUT_JUMPKEY_FRAME) {
			//ジャンプキーの入力カウンターを増やす
			jumpKeyCounter_[i]++;

			//ジャンプ力を分配加算する
			yAccelSum_ = MAX_JUMP_POWER / (float)INPUT_JUMPKEY_FRAME;

			// その回のジャンプ処理をしたのでそれ以降のループに入らないようにする
			break;
		}
	}

	// モーション更新
	if (isJump_[0] && anime_->GetAnimEnd()) { anime_->Play((int)ANIME_TYPE::JUMP); }
}

void Player::AnimeLoad(void)
{
	anime_ = new AnimationController(unit_.model_);

	const std::string ANIME_PATH = "Data/Model/Player/Animation/";
	anime_->Add((int)ANIME_TYPE::IDLE, 60.0f, (ANIME_PATH + "Idle.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::RUN, 90.0f, (ANIME_PATH + "Run.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::JUMP, 90.0f, (ANIME_PATH + "Jump.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::JUMP_POST, 90.0f, (ANIME_PATH + "JumpPost.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::FALL, 90.0f, (ANIME_PATH + "Fall.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::EVASION, 120.0f, (ANIME_PATH + "Evasion.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::PUNCH_FIRST, 120.0f, (ANIME_PATH + "PunchFirst.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::PUNCH_SECOND, 120.0f, (ANIME_PATH + "PunchSecond.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::PUNCH_THIRD, 120.0f, (ANIME_PATH + "PunchThird.mv1").c_str());
}

void Player::SubLoad(void)
{
	// 通常攻撃（パンチ）
	punch_ = new PlayerPunch(unit_.pos_, unit_.angle_);
	punch_->Load();


}
void Player::SubInit(void)
{
	// 通常攻撃（パンチ）
	punch_->Init();

}
void Player::SubUpdate(void)
{
	// 通常攻撃（パンチ）
	punch_->Update();

}
void Player::SubDraw(void)
{
	// 通常攻撃（パンチ）
	punch_->Draw();

}
void Player::SubRelease(void)
{
	// 通常攻撃（パンチ）
	if (punch_) {
		punch_->Release();
		delete punch_;
		punch_ = nullptr;
	}

}



std::vector<UnitBase*> Player::GetSubIns(void)
{
	std::vector<UnitBase*> ret = {};

	ret.emplace_back(punch_);

	return ret;
}