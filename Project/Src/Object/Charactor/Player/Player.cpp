#include "Player.h"

#include"../../../Manager/Input/KeyManager.h"
#include"../../../Manager/Sound/SoundManager.h"

#include"../../../Application/Application.h"
#include"../../../scene/SceneManager/SceneManager.h"

#include"../../../Scene/Game/GameScene.h"

#include"../../Common/Collider/CapsuleCollider.h"
#include"../../Common/Collider/LineCollider.h"

#include"../Boss/Boss.h"

Player::Player(const Vector3& cameraPos):
	CharactorBase(),

	cameraAngle_(cameraPos),

	hp_(0),

	isJump_(),
	jumpKeyCounter_(),

	punch_(nullptr),
	attackStage_(ATTACK_STAGE::NON),
	isAttack_(),
	attackStageCounter_(0),

	gouge_(nullptr),
	throwing_(nullptr),
	
	knockBackVec_()
{
}

void Player::Load(void)
{
	trans_.Load("Player/Player");
	trans_.scale = SCALE;
	trans_.localAngle = LOCAL_ROT;

	SetDynamicFlg(true);
	SetGravityFlg(true);

	SetPushFlg(true);
	SetPushWeight(50);

#pragma region 関数ポインタ配列へ各関数を格納
	SET_STATE(STATE::NON, &Player::Non);
	SET_STATE(STATE::MOVE, &Player::Move);
	SET_STATE(STATE::ATTACK, &Player::Attack);
	SET_STATE(STATE::GOUGE, &Player::Gouge);
	SET_STATE(STATE::CARRY_OBJ, &Player::CarryObj);
	SET_STATE(STATE::THROWING_OBJ, &Player::ThrowingObj);
	SET_STATE(STATE::EVASION, &Player::Evasion);
	SET_STATE(STATE::DAMAGE, &Player::Damage);
	SET_STATE(STATE::DEATH, &Player::Death);
	SET_STATE(STATE::END, &Player::End);
#pragma endregion

	// モーションの初期設定と初期モーション再生
	CreateAnimationController();
	AnimeLoad();
	AnimePlay((int)ANIME_TYPE::IDLE, true);

	// 音声読み込み
	Smng::GetIns().Load(SOUND::PLAYER_RUN);
	Smng::GetIns().Load(SOUND::PLAYER_PUNCH);
	Smng::GetIns().Load(SOUND::PLAYER_EVASION);
	Smng::GetIns().Load(SOUND::PLAYER_DAMAGE);

	// コライダー生成
	ColliderCreate(new LineCollider(TAG::PLAYER, LINE_COLLIDER_START_POS, LINE_COLLIDER_END_POS, (LINE_COLLIDER_START_POS - LINE_COLLIDER_END_POS).Length()));
	ColliderCreate(new CapsuleCollider(TAG::PLAYER, CAPSULE_COLLIDER_START_POS, CAPSULE_COLLIDER_END_POS, RADIUS,(CAPSULE_COLLIDER_START_POS - CAPSULE_COLLIDER_END_POS).Length() + RADIUS * 2));

	// プレイヤーが抱える下位クラスの読み込み処理
	LowerLoad();
}

void Player::CharactorInit(void)
{
	SetIsDraw(true);
	SetJudge(true);;

	trans_.pos = Vector3(1000.0f, 1000.0f, 200.0f);;
	trans_.centerDiff = CENTER_DIFF;

	trans_.angle = {};
	trans_.localAngle = LOCAL_ROT;

	state_ = (int)STATE::MOVE;

	for (auto& jump : isJump_) { jump = false; }
	for (auto& cou : jumpKeyCounter_) { cou = 0; }

	attackStage_ = ATTACK_STAGE::NON;
	for (auto& at : isAttack_) { at = false; }
	attackStageCounter_ = 0;

	hp_ = HP_MAX;

	// プレイヤーが抱える下位クラスの初期化処理
	LowerInit();
}

void Player::CharactorUpdate(void)
{
	// ステート遷移条件
	StateManager();

	// プレイヤーが抱える下位クラスの更新処理
	LowerUpdate();

	if (trans_.pos.y < -500.0f) {
		trans_.pos = Vector3(1000.0f, 1000.0f, 200.0f);
		AccelSum.y = 0.0f;
		
		SetInviCounter(100);
		HpSharpen(30);
		knockBackVec_ = {};
		stageRevival_();

		for (auto& jump : isJump_) { jump = false; }
		for (auto& cou : jumpKeyCounter_) { cou = 0; }
	}
}

void Player::CharactorDraw(void)
{
	// プレイヤーが抱える下位クラスの描画処理
	LowerDraw();
}

void Player::CharactorAlphaDraw(void)
{
	// プレイヤーが抱える下位クラスのアルファ描画処理
	LowerAlphaDraw();
}

void Player::UiDraw(void)
{
	auto drawHpBar = [&](Vector2 sPos, Vector2 size, int color)->void {
		DrawBoxAA(sPos.x, sPos.y, sPos.x + size.x, sPos.y + size.y, color, true);
		};

	float dif = 20.0f;

	Vector2 size = { Application::SCREEN_SIZE_X * 0.45,50.0f };
	Vector2 sPos = { dif,Application::SCREEN_SIZE_Y - dif - size.y };

	drawHpBar(sPos, size, 0xffffff);

	dif = 10.0f;
	sPos += dif;
	size -= dif * 2;

	drawHpBar(sPos, size, 0x000000);

	size.x *= ((float)hp_ / (float)HP_MAX);
	drawHpBar(sPos, size, 0x00ff00);

}

void Player::CharactorRelease(void)
{
	// プレイヤーが抱えている下位クラスのゲーム終了時処理
	LowerRelease();

	// 音声解放
	Smng::GetIns().Delete(SOUND::PLAYER_RUN);
	Smng::GetIns().Delete(SOUND::PLAYER_PUNCH);
	Smng::GetIns().Delete(SOUND::PLAYER_EVASION);
	Smng::GetIns().Delete(SOUND::PLAYER_DAMAGE);
}

void Player::OnGrounded()
{
	AccelSum.y = 0.0f;
	for (auto& jump : isJump_) { jump = false; }
	for (auto& cou : jumpKeyCounter_) { cou = 0; }
}

void Player::OnCollision(const ColliderBase& collider)
{
	if (GetInviCounter() > 0) { return; }

	auto knockBack = [&](Vector3 pos)->void {
		GameScene::Shake(ShakeKinds::ROUND, ShakeSize::BIG);
		GameScene::Slow(20);

		AccelSum.y = 10.0f;

		Vector3 vec = trans_.pos - pos;
		vec.y = 0.0f;
		knockBackVec_ = vec.Normalized() * 10.0f;

		trans_.angle.y = atan2f(-vec.x, -vec.z);
		};

	switch (collider.GetTag())
	{
	case TAG::ENEMY:
		knockBack(collider.GetPos());
		HpSharpen(10);
		return;
	case TAG::GOLEM_ATTACK_FALL:
		knockBack(collider.GetPos());
		HpSharpen(10);
		return;
	case TAG::GOLEM_ATTACK_PSYCHOROCK:
		knockBack(collider.GetPos());
		HpSharpen(10);
		return;
	case TAG::GOLEM_ATTACK_STONE:
		knockBack(collider.GetPos());
		HpSharpen(10);
		return;
	}
}

void Player::StateManager(void)
{
	//各状態から遷移可能にさせたい状態の、
	//遷移条件関数 ( DoState～～() ) を呼び出す
	switch (state_)
	{
	case (int)STATE::MOVE:
		DoStateAttack();
		DoStateEvasion();
		DoStateGouge();
		if (state_ != (int)STATE::MOVE) { Smng::GetIns().Stop(SOUND::PLAYER_RUN); }
		break;
	case (int)STATE::ATTACK:
		DoStateMove();
		DoStateEvasion();
		if (state_ != (int)STATE::ATTACK) { punch_->Off(); }
		break;
	case (int)STATE::GOUGE:
		break;
	case (int)STATE::CARRY_OBJ:
		DoStateThrowing();
		break;
	case (int)STATE::THROWING_OBJ:
		break;
	case (int)STATE::EVASION:
		break;
	case (int)STATE::DAMAGE:
		break;
	case (int)STATE::DEATH:
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
		state_ = (int)STATE::MOVE;
	}
}
void Player::DoStateAttack(void)
{
	if (!KEY::GetIns().GetInfo(KEY_TYPE::ATTACK).down) { return; }


	state_ = (int)STATE::ATTACK;

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
		AnimePlay((int)ANIME_TYPE::PUNCH_FIRST, false);
		break;
	case Player::ATTACK_STAGE::SECOND:
		AnimePlay((int)ANIME_TYPE::PUNCH_SECOND, false);
		break;
	}

	// SE再生
	Smng::GetIns().Play(SOUND::PLAYER_PUNCH, true, 150);
}
void Player::DoStateGouge(void)
{
	if (isJump_[0]) { return; }

	if (KEY::GetIns().GetInfo(KEY_TYPE::GOUGE).down) {
		state_ = (int)STATE::GOUGE;
		gouge_->On();
		AnimePlay((int)ANIME_TYPE::GOUPE, false);
	}
}
void Player::DoStateThrowing(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::ATTACK).down) {
		state_ = (int)STATE::THROWING_OBJ;
		AnimePlay((int)ANIME_TYPE::THROW, false);
	}
}
void Player::DoStateEvasion(void)
{
	if (!KEY::GetIns().GetInfo(KEY_TYPE::EVASION).down) { return; }

	state_ = (int)STATE::EVASION;

	AnimePlay((int)ANIME_TYPE::EVASION, false);

	// SE再生
	Smng::GetIns().Play(SOUND::PLAYER_EVASION, true, 150);
}

void Player::Move(void)
{
	Run();

	Jump();

	if (attackStageCounter_ <= INPUT_ATTACK_FRAME) { attackStageCounter_++; }
}
void Player::Attack(void)
{	
	float nowAnimeRatio = GetAnimeRatio();

	// 攻撃の判定が発生する前の間、前方に移動させる
	if (nowAnimeRatio <= 0.6f) {
		// 移動方向ベクトル
		Vector3 vec = {};

		// unit_.angle_(角度) から ベクトル(向き) を割り出す
		vec.x = sinf(trans_.angle.y);
		vec.z = cosf(trans_.angle.y);

		// 割り出したベクトルを単位ベクトルに直しスピードを乗算して座標情報に加算する
		trans_.pos += vec.Normalized() * 10.0f;
	}

	// 毎フレーム一旦オフ(攻撃判定)
	punch_->Off();

	// 大体攻撃判定を発生させる時間
	if (0.4f <= nowAnimeRatio && nowAnimeRatio <= 0.6f) { punch_->On(); }

	// 攻撃判定終わったらボタンで次段攻撃に遷移可能にしておく(操作性向上)
	if (nowAnimeRatio > 0.6f) { AttackMove(); DoStateAttack(); }

	// 何も入力なく攻撃アニメーションが終了したら通常状態に自動で遷移
	if (IsAnimeEnd()) { state_ = (int)STATE::MOVE; }
}
void Player::Gouge(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::GOUGE).now) {
		if (GetAnimeRatio() > 0.25f) {
			if (gouge_->Gouge()) {
				state_ = (int)STATE::CARRY_OBJ;
				throwing_->Carry(THROW_TYPE::ROCK);
			}
			else {
				state_ = (int)STATE::MOVE;
				AnimePlay((int)ANIME_TYPE::IDLE);
			}
		}
	}
	else {
		gouge_->Off();
		state_ = (int)STATE::MOVE;
		AnimePlay((int)ANIME_TYPE::IDLE);
	}
}
void Player::CarryObj(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::GOUGE).now) {
		CarryRun();
	}
	else {
		throwing_->Drop();
		state_ = (int)STATE::MOVE;
		AnimePlay((int)ANIME_TYPE::IDLE);
	}
}
void Player::ThrowingObj(void)
{
	float nowAnimeRatio = GetAnimeRatio();
	if (nowAnimeRatio <= 0.05f) {}
	else if (nowAnimeRatio <= 0.3f) { throwing_->Throw(); }
	else { state_ = (int)STATE::MOVE; }
}
void Player::Evasion(void)
{
	// 移動方向ベクトル
	Vector3 vec = {};

	// unit_.angle_(角度) から ベクトル(向き) を割り出す
	vec.x = sinf(trans_.angle.y);
	vec.z = cosf(trans_.angle.y);

	// 割り出したベクトルを単位ベクトルに直しスピードを乗算して座標情報に加算する
	vec.Normalize();
	trans_.pos += vec * 15.0f/*unit_.para_.speed * 1.5f*/;

	// 無敵(無敵カウンターを使って当たり判定を無効にする。この状態を抜けたらすぐに無敵が解除されるように １ を代入し続けておく)
	if (GetAnimeRatio() <= 0.7f) { SetInviCounter(1); }
	// 無敵判定が終わったらボタンで遷移可能にしておく(操作性向上)
	else { DoStateMove(); DoStateAttack(); }

	// 何も入力なく回避アニメーションが終了したら通常状態に自動で遷移
	if (IsAnimeEnd()) { state_ = (int)STATE::MOVE; }
}
void Player::Damage(void)
{
	trans_.pos += knockBackVec_;

	if (IsAnimeEnd()) {
		if (hp_ > 0) {
			state_ = (int)STATE::MOVE;
			AnimePlay((int)ANIME_TYPE::IDLE);
		}
		else {
			hp_ = 0;
			state_ = (int)STATE::DEATH;
			AnimePlay((int)ANIME_TYPE::DEATH, false);
			return;
		}
	}
}
void Player::Death(void)
{
	if (IsAnimeEnd()) {
		// 死亡処理
		state_ = (int)STATE::END;
	}
}

void Player::Run(void)
{
	auto& key = KEY::GetIns();

	Vector3 vec = { key.GetLeftStickVec().x,0.0f,-key.GetLeftStickVec().y };

	if (vec == 0.0f) {
		if (key.GetInfo(KEY_TYPE::MOVE_UP).now) { vec.z++; }
		if (key.GetInfo(KEY_TYPE::MOVE_DOWN).now) { vec.z--; }
		if (key.GetInfo(KEY_TYPE::MOVE_RIGHT).now) { vec.x++; }
		if (key.GetInfo(KEY_TYPE::MOVE_LEFT).now) { vec.x--; }
	}

	if (vec == 0.0f) {
		if (!isJump_[0]) { AnimePlay((int)ANIME_TYPE::IDLE); }
		Smng::GetIns().Stop(SOUND::PLAYER_RUN);
	}
	else {
		MATRIX mat = MGetIdent();
		mat = MMult(mat, MGetRotY(cameraAngle_.y));
		vec.TransMatOwn(mat);
		vec.Normalize();

		trans_.pos += vec * 10.0f;

		if (!isJump_[0]) {
			AnimePlay((int)ANIME_TYPE::RUN);
			Smng::GetIns().Play(SOUND::PLAYER_RUN, false, 150, true);
		}
		else { Smng::GetIns().Stop(SOUND::PLAYER_RUN); }

		trans_.angle.y = atan2(vec.x, vec.z);
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
			AnimePlay((int)ANIME_TYPE::JUMP_POST, false);
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
			AccelSum.y = (MAX_JUMP_POWER / (float)INPUT_JUMPKEY_FRAME);

			// その回のジャンプ処理をしたのでそれ以降のループに入らないようにする
			break;
		}
	}

	// モーション更新
	if (isJump_[0] && IsAnimeEnd()) { AnimePlay((int)ANIME_TYPE::JUMP); }
}

void Player::AttackMove(void)
{
	auto& key = KEY::GetIns();

	Vector3 vec = { key.GetLeftStickVec().x,0.0f,-key.GetLeftStickVec().y };

	if (vec == 0.0f) {
		if (key.GetInfo(KEY_TYPE::MOVE_UP).now) { vec.z++; }
		if (key.GetInfo(KEY_TYPE::MOVE_DOWN).now) { vec.z--; }
		if (key.GetInfo(KEY_TYPE::MOVE_RIGHT).now) { vec.x++; }
		if (key.GetInfo(KEY_TYPE::MOVE_LEFT).now) { vec.x--; }
	}

	if (vec != 0.0f) {
		MATRIX mat = MGetIdent();
		mat = MMult(mat, MGetRotY(cameraAngle_.y));
		vec = VTransform(vec, mat);
		trans_.angle.y = atan2(vec.x, vec.z);
	}
}

void Player::CarryRun(void)
{
	auto& key = KEY::GetIns();

	Vector3 vec = { key.GetLeftStickVec().x,0.0f,-key.GetLeftStickVec().y };

	if (vec == 0.0f) {
		if (key.GetInfo(KEY_TYPE::MOVE_UP).now) { vec.z++; }
		if (key.GetInfo(KEY_TYPE::MOVE_DOWN).now) { vec.z--; }
		if (key.GetInfo(KEY_TYPE::MOVE_RIGHT).now) { vec.x++; }
		if (key.GetInfo(KEY_TYPE::MOVE_LEFT).now) { vec.x--; }
	}


	if (vec == 0.0f) {
		if (!isJump_[0]) { AnimePlay((int)ANIME_TYPE::CARRY_IDLE); }
		Smng::GetIns().Stop(SOUND::PLAYER_RUN);
	}
	else {
		MATRIX mat = MGetIdent();
		mat = MMult(mat, MGetRotY(cameraAngle_.y));
		vec.TransMatOwn(mat);
		vec.Normalize();

		trans_.pos += vec * 5.0f;

		if (!isJump_[0]) {
			AnimePlay((int)ANIME_TYPE::CARRY_RUN);
			Smng::GetIns().Play(SOUND::PLAYER_RUN, false, 150, true);
		}
		else { Smng::GetIns().Stop(SOUND::PLAYER_RUN); }

		trans_.angle.y = atan2(vec.x, vec.z);
	}
}
void Player::CarryJump(void)
{
	auto& key = KEY::GetIns();

	for (int i = 0; i < JUMP_NUM; i++) {
		if (isJump_[i]) { continue; }

		// ダウントリガーでジャンプ開始
		if (key.GetInfo(KEY_TYPE::JUMP).down) {
			isJump_[i] = true;
			AnimePlay((int)ANIME_TYPE::JUMP_POST, false);
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
			AccelSum.y = (MAX_JUMP_POWER / (float)INPUT_JUMPKEY_FRAME);

			// その回のジャンプ処理をしたのでそれ以降のループに入らないようにする
			break;
		}
	}

	// モーション更新
	if (isJump_[0] && IsAnimeEnd()) { AnimePlay((int)ANIME_TYPE::JUMP); }
}

void Player::AnimeLoad(void)
{
	const std::string ANIME_PATH = "Data/Model/Player/Animation/";
	
	AddAnimation((int)ANIME_TYPE::IDLE, 1.0f, (ANIME_PATH + "Idle.mv1").c_str());
	AddAnimation((int)ANIME_TYPE::RUN, 1.0f, (ANIME_PATH + "Run.mv1").c_str());

	AddAnimation((int)ANIME_TYPE::JUMP, 1.5f, (ANIME_PATH + "Jump.mv1").c_str());
	AddAnimation((int)ANIME_TYPE::JUMP_POST, 1.5f, (ANIME_PATH + "JumpPost.mv1").c_str());
	AddAnimation((int)ANIME_TYPE::FALL, 1.5f, (ANIME_PATH + "Fall.mv1").c_str());

	AddAnimation((int)ANIME_TYPE::EVASION, 1.5f, (ANIME_PATH + "Evasion.mv1").c_str());

	AddAnimation((int)ANIME_TYPE::PUNCH_FIRST, 1.7f, (ANIME_PATH + "PunchFirst.mv1").c_str());
	AddAnimation((int)ANIME_TYPE::PUNCH_SECOND, 1.7f, (ANIME_PATH + "PunchSecond.mv1").c_str());
	AddAnimation((int)ANIME_TYPE::PUNCH_THIRD, 2.0f, (ANIME_PATH + "PunchThird.mv1").c_str());

	AddAnimation((int)ANIME_TYPE::GOUPE, 0.6f, (ANIME_PATH + "Goupe.mv1").c_str());

	AddAnimation((int)ANIME_TYPE::CARRY_IDLE, 0.5f, (ANIME_PATH + "CarryIdle.mv1").c_str());
	AddAnimation((int)ANIME_TYPE::CARRY_RUN, 0.5f, (ANIME_PATH + "CarryRun.mv1").c_str());
	AddAnimation((int)ANIME_TYPE::THROW, 0.5f, (ANIME_PATH + "Throw.mv1").c_str());

	AddAnimation((int)ANIME_TYPE::DAMAGE, 0.5f, (ANIME_PATH + "Damage.mv1").c_str());
	AddAnimation((int)ANIME_TYPE::DEATH, 0.5f, (ANIME_PATH + "Death.mv1").c_str());
}

void Player::HpSharpen(int damage)
{
	if (hp_ <= 0) { return; }

	punch_->Off();
	throwing_->Drop();

	hp_ -= damage;

	Smng::GetIns().Stop(SOUND::PLAYER_RUN);
	Smng::GetIns().Stop(SOUND::PLAYER_EVASION);
	Smng::GetIns().Stop(SOUND::PLAYER_PUNCH);
	Smng::GetIns().Play(SOUND::PLAYER_DAMAGE, true, 200);

	state_ = (int)STATE::DAMAGE;
	AnimePlay((int)ANIME_TYPE::DAMAGE, false);
	SetInviCounter(60);
}


void Player::LowerLoad(void)
{
	// 通常攻撃（パンチ）
	punch_ = new PlayerPunch(trans_.pos, trans_.angle);
	punch_->Load();

	// 抉り
	gouge_ = new PlayerGouge(trans_.pos, trans_.angle);
	gouge_->Load();

	// 特殊攻撃（投げ）
	throwing_ = new Throwing(trans_.pos, trans_.angle);
	throwing_->Load();

}
void Player::LowerInit(void)
{
	// 通常攻撃（パンチ）
	punch_->Init();

	// 抉り
	gouge_->Init();

	// 特殊攻撃（投げ）
	throwing_->Init();
}
void Player::LowerUpdate(void)
{
	// 通常攻撃（パンチ）
	punch_->Update();

	// 抉り
	gouge_->Update();

	// 特殊攻撃（投げ）
	throwing_->Update();
}
void Player::LowerDraw(void)
{
	// 通常攻撃（パンチ）
	punch_->Draw();

	// 抉り
	gouge_->Draw();

	// 特殊攻撃（投げ）
	throwing_->Draw();
}
void Player::LowerAlphaDraw(void)
{
	// 通常攻撃（パンチ）
	punch_->AlphaDraw();

	// 抉り
	gouge_->AlphaDraw();

	// 特殊攻撃（投げ）
	throwing_->AlphaDraw();
}
void Player::LowerRelease(void)
{
	// 通常攻撃（パンチ）
	if (punch_) {
		punch_->Release();
		delete punch_;
		punch_ = nullptr;
	}

	// 抉り
	if (gouge_) {
		gouge_->Release();
		delete gouge_;
		gouge_ = nullptr;
	}

	// 特殊攻撃（投げ）
	if (throwing_) {
		throwing_->Release();
		delete throwing_;
		throwing_ = nullptr;
	}
}