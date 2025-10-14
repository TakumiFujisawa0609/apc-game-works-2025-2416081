#include "Player.h"

#include"../../Manager/Input/KeyManager.h"
#include"../../Manager/Sound/SoundManager.h"
#include"../../Manager/Collision/CollisionUtility.h"

#include"../../Application/Application.h"
#include"../../scene/SceneManager/SceneManager.h"

#include"../../Scene/Game/GameScene.h"

#include"../Boss/Boss.h"

Player::Player(const VECTOR& cameraPos):
	cameraAngle_(cameraPos),
	state_(STATE::NON),
	stateFuncPtr(),

	isJump_(),
	jumpKeyCounter_(),

	punch_(nullptr),
	attackStage_(ATTACK_STAGE::NON),
	isAttack_(),
	attackStageCounter_(0),

	knockBackVec_(),

	throwing_(nullptr),

	anime_(nullptr)
{
}

void Player::Load(void)
{
	unit_.para_.colliType = CollisionType::ALLY;
	unit_.para_.colliShape = CollisionShape::CAPSULE;
	unit_.para_.size = SIZE;
	unit_.para_.radius = SIZE.z;
	unit_.para_.capsuleHalfLen = (SIZE.y - (unit_.para_.radius * 2)) / 2;

	unit_.model_ = MV1LoadModel("Data/Model/Player/Player.mv1");
	if (unit_.model_ != -1) {
		MV1SetScale(unit_.model_, { 1.0f,1.0f,1.0f });
	}

	unit_.para_.speed = 10.0f;
	
	carryModel_ = MV1LoadModel("Data/Model/Player/ThrowingObj/Rock/Rock.mv1");

	int mnum = MV1GetMaterialNum(unit_.model_);
	for (int i = 0; i < mnum; ++i) {
		COLOR_F emi = MV1GetMaterialEmiColor(unit_.model_, i);
		DEFAULT_COLOR.emplace_back(emi);
		emi.r = (std::min)(emi.r + 0.3f, 1.0f);
		emi.g = (std::min)(emi.g + 0.3f, 1.0f);
		emi.b = (std::min)(emi.b + 0.3f, 1.0f);
		MV1SetMaterialEmiColor(unit_.model_, i, emi);
	}


#pragma region 関数ポインタ配列へ各関数を格納

#define SET_STATE(state, func) stateFuncPtr[(int)(state)] = static_cast<STATEFUNC>(func)

	SET_STATE(STATE::NON, &Player::Non);
	SET_STATE(STATE::MOVE, &Player::Move);
	SET_STATE(STATE::ATTACK, &Player::Attack);
	SET_STATE(STATE::GOUGE, &Player::Gouge);
	SET_STATE(STATE::CARRY_OBJ, &Player::CarryObj);
	SET_STATE(STATE::THROWING_OBJ, &Player::ThrowingObj);
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
	Smng::GetIns().Load(SOUND::PLAYER_DAMAGE);


	// プレイヤーが抱える下位クラスの読み込み処理
	SubLoad();
}

void Player::Init(void)
{
	unit_.pos_ = { 1000.0f,1000.0f,600.0f };
	unit_.angle_ = {};

	unit_.isAlive_ = true;

	state_ = STATE::MOVE;

	for (auto& jump : isJump_) { jump = false; }
	for (auto& cou : jumpKeyCounter_) { cou = 0; }

	attackStage_ = ATTACK_STAGE::NON;
	for (auto& at : isAttack_) { at = false; }
	attackStageCounter_ = 0;

	unit_.hp_ = HP_MAX;

	SubInit();
}

void Player::Update(void)
{
	// 無敵カウンターの更新
	Invi();

	// 前フレームの座標をローカル変数に保持(押し戻し処理に使う)
	BeginFrame();

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


	if (unit_.pos_.y < -500.0f) {
		unit_.pos_ = { 1000.0f,1000.0f,600.0f };
		yAccelSum_ = 0.0f;
		
		unit_.inviciCounter_ = 200;
		HpSharpen(30);

		for (auto& jump : isJump_) { jump = false; }
		for (auto& cou : jumpKeyCounter_) { cou = 0; }
	}

	// ダメージ演出
	if (unit_.inviciCounter_ > 1) {
		if (unit_.inviciCounter_ / 10 % 2 == 0) {
			for (int i = 0; i < DEFAULT_COLOR.size(); i++) {
				MV1SetMaterialEmiColor(unit_.model_, i, DEFAULT_COLOR[i]);
			}
		}
		else {
			for (int i = 0; i < DEFAULT_COLOR.size(); i++) {
				COLOR_F emi = DEFAULT_COLOR[i];
				emi.r = (std::min)(DEFAULT_COLOR[i].r + 0.6f, 1.0f);
				MV1SetMaterialEmiColor(unit_.model_, i, emi);
			}
		}

	}
	else if (unit_.inviciCounter_ == 1) {
		for (int i = 0; i < DEFAULT_COLOR.size(); i++) {
			MV1SetMaterialEmiColor(unit_.model_, i, DEFAULT_COLOR[i]);
		}
	}
}

void Player::Draw(void)
{
	if (!unit_.isAlive_) { return; }

	SubDraw();

	if (state_ == STATE::CARRY_OBJ) {
		Utility::MV1ModelMatrix(carryModel_, VAdd(unit_.WorldPos(), VTransform(CARRY_OBJ_LOCAL_POS, Utility::MatrixAllMultY({ unit_.angle_ }))), { unit_.angle_ });
		MV1DrawModel(carryModel_);
	}

	Utility::MV1ModelMatrix(unit_.model_, VSub(unit_.WorldPos(), CENTER_DIFF), { LOCAL_ROT,unit_.angle_ });
	MV1DrawModel(unit_.model_);


	// デバッグ用に当たり判定の表示

	//SetDrawBlendMode(DX_BLENDMODE_ALPHA, 100);

	//VECTOR localPos = { 0.0f,unit_.para_.capsuleHalfLen,0.0f };

	//DrawCapsule3D(
	//	VSub(unit_.WorldPos(), localPos),
	//	VAdd(unit_.WorldPos(), localPos),
	//	unit_.para_.radius, 6, 0xffffff, 0xffffff, true);

	//DrawSphere3D(unit_.WorldPos(), 5.0f, 4, 0xffffff, 0xffffff, true);

	//VECTOR debugPos[8] =
	//{
	//	VAdd(unit_.pos_, VTransform({ -unit_.para_.size.x / 2.0f, -unit_.para_.size.y / 2.0f, -unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
	//	VAdd(unit_.pos_, VTransform({  unit_.para_.size.x / 2.0f, -unit_.para_.size.y / 2.0f, -unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
	//	VAdd(unit_.pos_, VTransform({ -unit_.para_.size.x / 2.0f,  unit_.para_.size.y / 2.0f, -unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
	//	VAdd(unit_.pos_, VTransform({  unit_.para_.size.x / 2.0f,  unit_.para_.size.y / 2.0f, -unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
	//	VAdd(unit_.pos_, VTransform({ -unit_.para_.size.x / 2.0f, -unit_.para_.size.y / 2.0f,  unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
	//	VAdd(unit_.pos_, VTransform({  unit_.para_.size.x / 2.0f, -unit_.para_.size.y / 2.0f,  unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
	//	VAdd(unit_.pos_, VTransform({ -unit_.para_.size.x / 2.0f,  unit_.para_.size.y / 2.0f,  unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_}))),
	//	VAdd(unit_.pos_, VTransform({  unit_.para_.size.x / 2.0f,  unit_.para_.size.y / 2.0f,  unit_.para_.size.z / 2.0f },Utility::MatrixAllMultY({unit_.angle_})))
	//};
	//for (int i = 0; i < 8; i++) {
	//	DrawSphere3D(debugPos[i], 3.0f, 4, GetColor(255, 0, 0), GetColor(255, 0, 0), true);
	//}

	//SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void Player::UiDraw(void)
{
	auto drawHpBar = [&](Vector2 sPos, Vector2 size, int color)->void {
		DrawBoxAA(sPos.x, sPos.y, sPos.x + size.x, sPos.y + size.y, color, true);
		};

	float dif = 20.0f;

	Vector2 size = { 700.0f,60.0f };
	Vector2 sPos = { dif,Application::SCREEN_SIZE_Y - dif - size.y };

	drawHpBar(sPos, size, 0xffffff);

	dif = 3.0f;
	sPos += dif;
	size -= dif * 2;

	drawHpBar(sPos, size, 0x000000);

	size.x *= ((float)unit_.hp_ / (float)HP_MAX);
	drawHpBar(sPos, size, 0x00ff00);
}

void Player::Release(void)
{
	// プレイヤーが抱えている下位クラスのゲーム終了時処理
	SubRelease();

	// 音声解放
	Smng::GetIns().Delete(SOUND::PLAYER_RUN);
	Smng::GetIns().Delete(SOUND::PLAYER_PUNCH);
	Smng::GetIns().Delete(SOUND::PLAYER_EVASION);
	Smng::GetIns().Delete(SOUND::PLAYER_DAMAGE);

	// アニメーション管理クラスの解放
	if (anime_) {
		anime_->Release();
		delete anime_;
		anime_ = nullptr;
	}

	// モデル解放
	MV1DeleteModel(unit_.model_);
}

void Player::OnGrounded()
{
	yAccelSum_ = 0.0f;
	for (auto& jump : isJump_) { jump = false; }
	for (auto& cou : jumpKeyCounter_) { cou = 0; }
}

void Player::OnCollision(UnitBase* other)
{
	auto knockBack = [&](VECTOR pos)->void {
		yAccelSum_ = 10.0f;

		VECTOR vec = VSub(unit_.pos_, pos);
		vec.y = 0.0f;
		knockBackVec_ = VScale(Utility::Normalize(vec), 10.0f);

		unit_.angle_.y = atan2f(-vec.x, -vec.z);
		};

	if (dynamic_cast<Boss*>(other)) {
		GameScene::Shake(ShakeKinds::ROUND, ShakeSize::BIG);
		GameScene::Slow(20);

		knockBack(other->GetUnit().pos_);
		HpSharpen(10);
		return;
	}
	if (dynamic_cast<Stone*>(other)) {
		GameScene::Shake(ShakeKinds::ROUND, ShakeSize::BIG);
		GameScene::Slow(20);

		knockBack(other->GetUnit().pos_);
		HpSharpen(10);
		return;
	}
	if (dynamic_cast<Fall*>(other)) {
		GameScene::Shake(ShakeKinds::ROUND, ShakeSize::BIG);
		GameScene::Slow(20);

		knockBack(other->GetUnit().pos_);
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
	case STATE::MOVE:
		DoStateAttack();
		DoStateEvasion();
		DoStateGouge();
		if (state_ != STATE::MOVE) { Smng::GetIns().Stop(SOUND::PLAYER_RUN); }
		break;
	case STATE::ATTACK:
		DoStateMove();
		DoStateEvasion();
		if (state_ != STATE::ATTACK) { punch_->Off(); }
		break;
	case STATE::GOUGE:
		break;
	case STATE::CARRY_OBJ:
		DoStateThrowing();
		break;
	case STATE::THROWING_OBJ:
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
void Player::DoStateGouge(void)
{
	if (isJump_[0]) { return; }

	if (KEY::GetIns().GetInfo(KEY_TYPE::GOUGE).down) {
		state_ = STATE::GOUGE;
		gouge_->On();
		anime_->Play((int)ANIME_TYPE::GOUPE, false);
	}
}
void Player::DoStateThrowing(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::ATTACK).down) {
		state_ = STATE::THROWING_OBJ;
		anime_->Play((int)ANIME_TYPE::THROW, false);
	}
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

	if (attackStageCounter_ <= INPUT_ATTACK_FRAME) { attackStageCounter_++; }
}
void Player::Attack(void)
{	
	// 攻撃の判定が発生する前の間、前方に移動させる
	if (anime_->GetAnimeRatio() < 0.6f) {
		// 移動方向ベクトル
		VECTOR vec = {};

		// unit_.angle_(角度) から ベクトル(向き) を割り出す
		vec.x = sinf(unit_.angle_.y);
		vec.z = cosf(unit_.angle_.y);

		// 割り出したベクトルを単位ベクトルに直しスピードを乗算して座標情報に加算する
		vec = Utility::Normalize(vec);
		unit_.vel_ = VScale(vec, 5.0f);
	}

	// 毎フレーム一旦オフ(攻撃判定)
	punch_->Off();

	// 大体攻撃判定を発生させる時間
	if (anime_->GetAnimeRatio() >= 0.4f && anime_->GetAnimeRatio() <= 0.6f) {
		punch_->On();
	}

	// 攻撃判定終わったらボタンで次段攻撃に遷移可能にしておく(操作性向上)
	if (anime_->GetAnimeRatio() > 0.6f) { DoStateAttack(); }

	// 何も入力なく攻撃アニメーションが終了したら通常状態に自動で遷移
	if (anime_->GetAnimEnd()) { state_ = STATE::MOVE; }
}
void Player::Gouge(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::GOUGE).now) {
		if (gouge_->ObjectGouge()) {
			state_ = STATE::CARRY_OBJ;
		}
		if (gouge_->GetUnit().isAlive_ == false) {
			state_ = STATE::MOVE;
			anime_->Play((int)ANIME_TYPE::IDLE);
		}
	}
	else {
		gouge_->Off();
		state_ = STATE::MOVE;
		anime_->Play((int)ANIME_TYPE::IDLE);
	}
}
void Player::CarryObj(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::GOUGE).now) {
		CarryRun();
	}
	else {
		state_ = STATE::MOVE;
		anime_->Play((int)ANIME_TYPE::IDLE);
	}
}
void Player::ThrowingObj(void)
{
	if (anime_->GetAnimeRatio() <= 0.1f) {

	}
	else {
		throwing_->Throw(THROW_TYPE::ROCK);
		state_ = STATE::MOVE;
	}
}
void Player::Evasion(void)
{
	// 移動方向ベクトル
	VECTOR vec = {};

	// unit_.angle_(角度) から ベクトル(向き) を割り出す
	vec.x = sinf(unit_.angle_.y);
	vec.z = cosf(unit_.angle_.y);

	// 割り出したベクトルを単位ベクトルに直しスピードを乗算して座標情報に加算する
	vec = Utility::Normalize(vec);
	unit_.vel_ = VScale(vec, unit_.para_.speed * 1.5f);

	// 無敵(無敵カウンターを使って当たり判定を無効にする。この状態を抜けたらすぐに無敵が解除されるように １ を代入し続けておく)
	if (anime_->GetAnimeRatio() <= 0.7f) { unit_.inviciCounter_ = 1; }
	// 無敵判定が終わったらボタンで遷移可能にしておく(操作性向上)
	else { DoStateMove(); DoStateAttack(); }

	// 何も入力なく回避アニメーションが終了したら通常状態に自動で遷移
	if (anime_->GetAnimEnd()) { state_ = STATE::MOVE; }
}
void Player::Damage(void)
{
	unit_.vel_ = VAdd(unit_.vel_, knockBackVec_);

	if (anime_->GetAnimEnd()) {
		state_ = STATE::MOVE;
		anime_->Play((int)ANIME_TYPE::IDLE);
	}
}
void Player::Death(void)
{
	if (anime_->GetAnimEnd()) {
		unit_.isAlive_ = false;
	}
}

void Player::Run(void)
{
	auto& key = KEY::GetIns();

	VECTOR vec = { key.GetLeftStickVec().x,0.0f,-key.GetLeftStickVec().y };

	if (Utility::VZERO(vec)) {
		if (key.GetInfo(KEY_TYPE::MOVE_UP).now) { vec.z++; }
		if (key.GetInfo(KEY_TYPE::MOVE_DOWN).now) { vec.z--; }
		if (key.GetInfo(KEY_TYPE::MOVE_RIGHT).now) { vec.x++; }
		if (key.GetInfo(KEY_TYPE::MOVE_LEFT).now) { vec.x--; }
	}

	if (Utility::VZERO(vec)) {
		if (!isJump_[0]) { anime_->Play((int)ANIME_TYPE::IDLE); }
		Smng::GetIns().Stop(SOUND::PLAYER_RUN);
	}
	else {
		MATRIX mat = MGetIdent();
		mat = MMult(mat, MGetRotY(cameraAngle_.y));
		vec = VTransform(vec, mat);

		vec = Utility::Normalize(vec);
		unit_.vel_ = VScale(vec, unit_.para_.speed);

		if (!isJump_[0]) {
			anime_->Play((int)ANIME_TYPE::RUN);
			Smng::GetIns().Play(SOUND::PLAYER_RUN, false, 150, true);
		}
		else { Smng::GetIns().Stop(SOUND::PLAYER_RUN); }

		unit_.angle_.y = atan2(vec.x, vec.z);
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
			yAccelSum_ = (MAX_JUMP_POWER / (float)INPUT_JUMPKEY_FRAME);

			// その回のジャンプ処理をしたのでそれ以降のループに入らないようにする
			break;
		}
	}

	// モーション更新
	if (isJump_[0] && anime_->GetAnimEnd()) { anime_->Play((int)ANIME_TYPE::JUMP); }
}

void Player::CarryRun(void)
{
	auto& key = KEY::GetIns();

	VECTOR vec = { key.GetLeftStickVec().x,0.0f,-key.GetLeftStickVec().y };

	if (Utility::VZERO(vec)) {
		if (key.GetInfo(KEY_TYPE::MOVE_UP).now) { vec.z++; }
		if (key.GetInfo(KEY_TYPE::MOVE_DOWN).now) { vec.z--; }
		if (key.GetInfo(KEY_TYPE::MOVE_RIGHT).now) { vec.x++; }
		if (key.GetInfo(KEY_TYPE::MOVE_LEFT).now) { vec.x--; }
	}


	if (Utility::VZERO(vec)) {
		if (!isJump_[0]) { anime_->Play((int)ANIME_TYPE::CARRY_IDLE); }
		Smng::GetIns().Stop(SOUND::PLAYER_RUN);
	}
	else {
		MATRIX mat = MGetIdent();
		mat = MMult(mat, MGetRotY(cameraAngle_.y));
		vec = VTransform(vec, mat);

		vec = Utility::Normalize(vec);
		unit_.vel_ = VScale(vec, unit_.para_.speed * 0.5f);

		if (!isJump_[0]) {
			anime_->Play((int)ANIME_TYPE::CARRY_RUN);
			Smng::GetIns().Play(SOUND::PLAYER_RUN, false, 150, true);
		}
		else { Smng::GetIns().Stop(SOUND::PLAYER_RUN); }

		unit_.angle_.y = atan2(vec.x, vec.z);
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
			yAccelSum_ = (MAX_JUMP_POWER / (float)INPUT_JUMPKEY_FRAME);

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
	anime_->Add((int)ANIME_TYPE::EVASION, 150.0f, (ANIME_PATH + "Evasion.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::PUNCH_FIRST, 120.0f, (ANIME_PATH + "PunchFirst.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::PUNCH_SECOND, 120.0f, (ANIME_PATH + "PunchSecond.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::PUNCH_THIRD, 120.0f, (ANIME_PATH + "PunchThird.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::GOUPE, 40.0f, (ANIME_PATH + "Goupe.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::CARRY_IDLE, 30.0f, (ANIME_PATH + "CarryIdle.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::CARRY_RUN, 30.0f, (ANIME_PATH + "CarryRun.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::THROW, 30.0f, (ANIME_PATH + "Throw.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::DAMAGE, 30.0f, (ANIME_PATH + "Damage.mv1").c_str());
	anime_->Add((int)ANIME_TYPE::DEATH, 30.0f, (ANIME_PATH + "Death.mv1").c_str());
}

void Player::HpSharpen(int damage)
{
	if (unit_.hp_ <= 0) { return; }

	unit_.hp_ -= damage;

	if (unit_.hp_ <= 0) {
		unit_.hp_ = 0;
		state_ = STATE::DEATH;
		anime_->Play((int)ANIME_TYPE::DEATH, false);
		return;
	}

	Smng::GetIns().Play(SOUND::PLAYER_DAMAGE, true, 200);

	state_ = STATE::DAMAGE;
	anime_->Play((int)ANIME_TYPE::DAMAGE, false);
	unit_.inviciCounter_ = 60;
}


void Player::SubLoad(void)
{
	// 通常攻撃（パンチ）
	punch_ = new PlayerPunch(unit_.pos_, unit_.angle_);
	punch_->Load();

	// 抉り
	gouge_ = new PlayerGouge(unit_.pos_, unit_.angle_);
	gouge_->Load();

	// 特殊攻撃（投げ）
	throwing_ = new Throwing(unit_.pos_, unit_.angle_);
	throwing_->Load();

}
void Player::SubInit(void)
{
	// 通常攻撃（パンチ）
	punch_->Init();

	// 抉り
	gouge_->Init();
	
	// 特殊攻撃（投げ）
	throwing_->Init();
}
void Player::SubUpdate(void)
{
	// 通常攻撃（パンチ）
	punch_->Update();

	// 抉り
	gouge_->Update();

	// 特殊攻撃（投げ）
	throwing_->Update();
}
void Player::SubDraw(void)
{
	// 通常攻撃（パンチ）
	punch_->Draw();

	// 抉り
	gouge_->Draw();

	// 特殊攻撃（投げ）
	throwing_->Draw();
}
void Player::SubRelease(void)
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

std::vector<UnitBase*> Player::GetSubIns(void)
{
	std::vector<UnitBase*> ret = {};

	ret.emplace_back(punch_);
	ret.emplace_back(gouge_);

	return ret;
}