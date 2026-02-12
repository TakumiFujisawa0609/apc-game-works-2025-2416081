#include "Player.h"

#include"../../../Manager/Input/KeyManager.h"
#include"../../../Manager/Sound/SoundManager.h"
#include"../../../Manager/Camera/Camera.h"

#include"../../../scene/SceneManager/SceneManager.h"

#include"../../../Scene/Game/GameScene.h"

#include"../../Common/Collider/CapsuleCollider.h"
#include"../../Common/Collider/LineCollider.h"

#include"../Boss/Boss.h"

Player::Player() :
	CharactorBase(),

	hp_(0),

	isJump(),
	jumpKeyCounter(),

	punch(nullptr),
	attackStage(ATTACK_STAGE::NON),
	isAttack(),
	attackStageCounter(0),

	gouge(nullptr),
	throwing(nullptr),

	knockBackVec(),

	preview(nullptr),
	hpBar(nullptr),
	operationUi(nullptr)
{
}

void Player::Load(void)
{
	trans.Load("Player/Player");
	trans.scale = SCALE;
	trans.localAngle = LOCAL_ROT;

	SetDynamicFlg(true);
	SetGravityFlg(true);

	SetPushFlg(true);
	SetPushWeight(50);

#pragma region 関数ポインタ配列へ各関数を格納
	CHARACTOR_SET_STATE(STATE::NON, &Player::Non);
	CHARACTOR_SET_STATE(STATE::MOVE, &Player::Move);
	CHARACTOR_SET_STATE(STATE::ATTACK, &Player::Attack);
	CHARACTOR_SET_STATE(STATE::GOUGE, &Player::Gouge);
	CHARACTOR_SET_STATE(STATE::CARRY_OBJ, &Player::CarryObj);
	CHARACTOR_SET_STATE(STATE::THROWING_OBJ, &Player::ThrowingObj);
	CHARACTOR_SET_STATE(STATE::EVASION, &Player::Evasion);
	CHARACTOR_SET_STATE(STATE::DAMAGE, &Player::Damage);
	CHARACTOR_SET_STATE(STATE::DEATH, &Player::Death);
	CHARACTOR_SET_STATE(STATE::END, &Player::End);
#pragma endregion

	// モーションの初期設定と初期モーション再生
	CreateAnimationController();
	AddInFbxAnimation((int)ANIME_TYPE::MAX, INFBX_ANIME_SPEED);
	AnimePlay((int)ANIME_TYPE::IDLE, true);

	// コライダー生成
	ColliderCreate(new LineCollider(TAG::PLAYER, LINE_COLLIDER_START_POS, LINE_COLLIDER_END_POS, (LINE_COLLIDER_START_POS - LINE_COLLIDER_END_POS).Length()));
	ColliderCreate(new CapsuleCollider(TAG::PLAYER, CAPSULE_COLLIDER_START_POS, CAPSULE_COLLIDER_END_POS, RADIUS,(CAPSULE_COLLIDER_START_POS - CAPSULE_COLLIDER_END_POS).Length() + RADIUS * 2));

	// プレイヤーが抱える下位クラスの読み込み処理
	LowerLoad();
}

void Player::CharactorInit(void)
{
	SetIsDraw(true);
	SetJudge(true);

	trans.pos = Vector3(1000.0f, 1000.0f, 200.0f);
	trans.centerDiff = CENTER_DIFF;

	accelSum.x = 10.0f; accelSum.z = 10.0f;

	trans.angle = {};
	trans.localAngle = LOCAL_ROT;

	state = (int)STATE::MOVE;

	for (auto& jump : isJump) { jump = false; }
	for (auto& cou : jumpKeyCounter) { cou = 0; }

	attackStage = ATTACK_STAGE::NON;
	for (auto& at : isAttack) { at = false; }
	attackStageCounter = 0;

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

	if (trans.pos.y < -500.0f) {
		trans.pos = Vector3(1000.0f, 1000.0f, 200.0f);
		accelSum.y = 0.0f;
		accelSum.x = 10.0f; accelSum.z = 10.0f;

		SetInviCounter(100);
		HpSharpen(30);
		knockBackVec = {};
		stageRevival();

		for (auto& jump : isJump) { jump = false; }
		for (auto& cou : jumpKeyCounter) { cou = 0; }
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
	// プレビュー
	preview->Draw(DX_SCREEN_BACK);

	// HPバー描画
	hpBar->Draw();

	// 操作説明
	operationUi->Draw();
}

void Player::CharactorRelease(void)
{
	// プレイヤーが抱えている下位クラスのゲーム終了時処理
	LowerRelease();
}

void Player::OnGrounded()
{
	ActorBase::OnGrounded();

	for (auto& jump : isJump) { jump = false; }
	for (auto& cou : jumpKeyCounter) { cou = 0; }
}

void Player::OnCollision(const ColliderBase& collider)
{
	if (GetInviCounter() > 0) { return; }
	if (state == (int)STATE::DEATH) { return; }

	auto knockBack = [&](Vector3 pos)->void {
		GameScene::Shake(ShakeKinds::ROUND, ShakeSize::BIG);
		GameScene::Slow(20);

		accelSum.y = 10.0f;

		Vector3 vec = trans.pos - pos;
		vec.y = 0.0f;
		knockBackVec = vec.Normalized() * 10.0f;

		trans.angle.y = atan2f(-vec.x, -vec.z);
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
	switch (state)
	{
	case (int)STATE::MOVE:
		DoStateAttack();
		DoStateEvasion();
		DoStateGouge();
		if (state != (int)STATE::MOVE) { Snd::GetIns().Stop("PlayerRun"); }
		break;
	case (int)STATE::ATTACK:
		DoStateMove();
		DoStateEvasion();
		if (state != (int)STATE::ATTACK) { punch->Off(); }
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

	if (key.GetInfo(KEY_TYPE::PLAYER_MOVE_FRONT).down ||
		key.GetInfo(KEY_TYPE::PLAYER_MOVE_BACK).down ||
		key.GetInfo(KEY_TYPE::PLAYER_MOVE_RIGHT).down ||
		key.GetInfo(KEY_TYPE::PLAYER_MOVE_LEFT).down ||
		key.GetInfo(KEY_TYPE::PLAYER_JUMP).down
		)
	{
		state = (int)STATE::MOVE;
	}
}
void Player::DoStateAttack(void)
{
	if (!KEY::GetIns().GetInfo(KEY_TYPE::PLAYER_ATTACK).down) { return; }


	state = (int)STATE::ATTACK;

	// 最終段までいっている または 前の段の攻撃から一定時間過ぎていたら フラグリセット
	if ((isAttack[(int)ATTACK_STAGE::MAX - 1]) || (attackStageCounter > INPUT_ATTACK_FRAME)) {
		for (int i = 0; i < (int)ATTACK_STAGE::MAX; i++) { isAttack[i] = false; }
	}

	// １段目から探索して適切な段数をattackStage_に代入する
	for (int i = 0; i < (int)ATTACK_STAGE::MAX; i++) {
		if (!isAttack[i]) {
			isAttack[i] = true;
			attackStageCounter = 0;
			attackStage = (ATTACK_STAGE)i;
			break;
		}
	}

	// モーション更新
	switch (attackStage)
	{
	case Player::ATTACK_STAGE::FIRST:
		AnimePlay((int)ANIME_TYPE::PUNCH_FIRST, false);
		break;
	case Player::ATTACK_STAGE::SECOND:
		AnimePlay((int)ANIME_TYPE::PUNCH_SECOND, false);
		break;
	}

	// SE再生
	Snd::GetIns().Play("PlayerPunch");
}
void Player::DoStateGouge(void)
{
	if (isJump[0]) { return; }

	if (KEY::GetIns().GetInfo(KEY_TYPE::PLAYER_GOUGE).down) {
		state = (int)STATE::GOUGE;
		gouge->SearchOn();
		AnimePlay((int)ANIME_TYPE::CATCH, false);
	}
}
void Player::DoStateThrowing(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::PLAYER_GOUGE).up) {
		state = (int)STATE::THROWING_OBJ;
		AnimePlay((int)ANIME_TYPE::THROW, false);
	}
}
void Player::DoStateEvasion(void)
{
	if (!KEY::GetIns().GetInfo(KEY_TYPE::PLAYER_EVASION).down) { return; }

	state = (int)STATE::EVASION;

	AnimePlay((int)ANIME_TYPE::EVASION, false);

	// SE再生
	Snd::GetIns().Play("PlayerEvasion");
}

void Player::Move(void)
{
	Run();

	Jump();

	if (attackStageCounter <= INPUT_ATTACK_FRAME) { attackStageCounter++; }
}
void Player::Attack(void)
{	
	float nowAnimeRatio = GetAnimeRatio();

	// 攻撃の判定が発生する前の間、前方に移動させる
	if (nowAnimeRatio <= 0.6f) {
		// 移動方向ベクトル
		Vector3 vec = {};

		// unit_.angle_(角度) から ベクトル(向き) を割り出す
		vec.x = sinf(trans.angle.y);
		vec.z = cosf(trans.angle.y);

		// 割り出したベクトルを単位ベクトルに直しスピードを乗算して座標情報に加算する
		trans.pos += vec.Normalized() * 13.0f;
	}

	// 毎フレーム一旦オフ(攻撃判定)
	punch->Off();

	// 大体攻撃判定を発生させる時間
	if (0.5f <= nowAnimeRatio && nowAnimeRatio <= 0.6f) { punch->On(); }

	// 攻撃判定終わったらボタンで次段攻撃に遷移可能にしておく(操作性向上)
	if (nowAnimeRatio > 0.6f) { AttackMove(); DoStateAttack(); }

	// 何も入力なく攻撃アニメーションが終了したら通常状態に自動で遷移
	if (IsAnimeEnd()) { state = (int)STATE::MOVE; }
}
void Player::Gouge(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::PLAYER_GOUGE).now) {
		// ボタンが押され続けている間処理を行う

		// 現在のモーションを取得
		const int& animeType = GetAnimePlayType();

		// まだ破壊箇所探索中の処理
		if (animeType == (int)ANIME_TYPE::CATCH) {
			if (gouge->SearchHit()) {
				AnimePlay((int)ANIME_TYPE::GOUGE, false);
				isGouge = false;
			}
			// モーション終了で状態遷移
			if (IsAnimeEnd()) {
				gouge->Reset();
				throwing->Drop();
				state = (int)STATE::MOVE;
				AnimePlay((int)ANIME_TYPE::IDLE);
			}
		}
		// 破壊モーション待ち
		else if (animeType == (int)ANIME_TYPE::GOUGE) {
			float animeRatio = GetAnimeRatio();
			if (!isGouge && animeRatio > 0.33f) {
				isGouge = true;
				gouge->GougeOn();
				throwing->Carry(THROW_TYPE::ROCK);
			}

			// モーション終了で状態遷移
			if (IsAnimeEnd()) {
				gouge->Reset();
				state = (int)STATE::CARRY_OBJ;
			}
		}

	}
	else {
		gouge->Reset();
		throwing->Drop();
		state = (int)STATE::MOVE;
		AnimePlay((int)ANIME_TYPE::IDLE);
	}
}
void Player::CarryObj(void)
{
	if (KEY::GetIns().GetInfo(KEY_TYPE::PLAYER_GOUGE).now) {
		CarryRun();
	}
	else {
		throwing->Drop();
		state = (int)STATE::MOVE;
		AnimePlay((int)ANIME_TYPE::IDLE);
	}
}
void Player::ThrowingObj(void)
{
	float nowAnimeRatio = GetAnimeRatio();
	if (nowAnimeRatio <= 0.25f) {}
	else {
		throwing->Throw();
		DoStateMove();
		DoStateAttack();
		DoStateEvasion();
		DoStateGouge();
	}
	if (IsAnimeEnd()) { state = (int)STATE::MOVE; }
}
void Player::Evasion(void)
{
	// 移動方向ベクトル
	Vector3 vec = {};

	// unit_.angle_(角度) から ベクトル(向き) を割り出す
	vec.x = sinf(trans.angle.y);
	vec.z = cosf(trans.angle.y);

	// 割り出したベクトルを単位ベクトルに直しスピードを乗算して座標情報に加算する
	vec.Normalize();
	trans.pos += vec * 15.0f/*unit_.para_.speed * 1.5f*/;

	// 無敵(無敵カウンターを使って当たり判定を無効にする。この状態を抜けたらすぐに無敵が解除されるように １ を代入し続けておく)
	if (GetAnimeRatio() <= 0.7f) { SetInviCounter(1); }
	// 無敵判定が終わったらボタンで遷移可能にしておく(操作性向上)
	//else { DoStateMove(); DoStateAttack(); }

	// 何も入力なく回避アニメーションが終了したら通常状態に自動で遷移
	if (IsAnimeEnd()) { state = (int)STATE::MOVE; }
}
void Player::Damage(void)
{
	trans.pos += knockBackVec;

	if (IsAnimeEnd()) {
		if (hp_ > 0) {
			state = (int)STATE::MOVE;
			AnimePlay((int)ANIME_TYPE::IDLE);
		}
		else {
			hp_ = 0;
			state = (int)STATE::DEATH;
			AnimePlay((int)ANIME_TYPE::DEATH, false);
			return;
		}
	}
}
void Player::Death(void)
{
	if (IsAnimeEnd()) {
		// 死亡処理
		state = (int)STATE::END;
	}
}

void Player::Run(void)
{
	auto& key = KEY::GetIns();

	Vector3 vec = { key.GetLeftStickVec().x,0.0f,-key.GetLeftStickVec().y };

	if (vec == 0.0f) {
		if (key.GetInfo(KEY_TYPE::PLAYER_MOVE_FRONT).now) { vec.z++; }
		if (key.GetInfo(KEY_TYPE::PLAYER_MOVE_BACK).now) { vec.z--; }
		if (key.GetInfo(KEY_TYPE::PLAYER_MOVE_RIGHT).now) { vec.x++; }
		if (key.GetInfo(KEY_TYPE::PLAYER_MOVE_LEFT).now) { vec.x--; }
	}

	if (vec == 0.0f) {
		if (isGround) { AnimePlay((int)ANIME_TYPE::IDLE); }
		Snd::GetIns().Stop("PlayerRun");
	}
	else {
		MATRIX mat = MGetIdent();
		mat = MMult(mat, MGetRotY(Camera::GetIns().GetAngle().y));
		vec.TransMatOwn(mat);
		vec.Normalize();

		trans.pos += vec * 10.0f;

		if (isGround) {
			AnimePlay((int)ANIME_TYPE::RUN);
			Snd::GetIns().Play("PlayerRun");
		}
		else { Snd::GetIns().Stop("PlayerRun"); }

		trans.angle.y = atan2(vec.x, vec.z);
	}
}
void Player::Jump(void)
{
	auto& key = KEY::GetIns();

	for (int i = 0; i < JUMP_NUM; i++) {
		if (isJump[i]) { continue; }

		// ダウントリガーでジャンプ開始
		if (key.GetInfo(KEY_TYPE::PLAYER_JUMP).down) {
			isJump[i] = true; 
			AnimePlay((int)ANIME_TYPE::JUMP, false);

			jumpKeyCounter[i]++;

			accelSum.y = (std::max)(accelSum.y, (MAX_JUMP_POWER / (float)INPUT_JUMPKEY_FRAME));
		}

		// ループから抜ける
		break;
	}

	for (int i = JUMP_NUM - 1; i >= 0; i--) {
		if (!isJump[i]) { continue; }

		//ジャンプキーを離したら、ジャンプキー入力判定を終了
		if (key.GetInfo(KEY_TYPE::PLAYER_JUMP).up) { jumpKeyCounter[i] = INPUT_JUMPKEY_FRAME; }

		//入力時間に応じてジャンプ量を変更する
		if (key.GetInfo(KEY_TYPE::PLAYER_JUMP).now && jumpKeyCounter[i] < INPUT_JUMPKEY_FRAME) {
			//ジャンプキーの入力カウンターを増やす
			jumpKeyCounter[i]++;

			//ジャンプ力を分配加算する
			accelSum.y += (MAX_JUMP_POWER / (float)INPUT_JUMPKEY_FRAME);
		}

		break;
	}

	// モーション更新
	if (isJump[0] && IsAnimeEnd() && accelSum.y <= 0.0f) { AnimePlay((int)ANIME_TYPE::FALL); }
}

void Player::AttackMove(void)
{
	auto& key = KEY::GetIns();

	Vector3 vec = { key.GetLeftStickVec().x,0.0f,-key.GetLeftStickVec().y };

	if (vec == 0.0f) {
		if (key.GetInfo(KEY_TYPE::PLAYER_MOVE_FRONT).now) { vec.z++; }
		if (key.GetInfo(KEY_TYPE::PLAYER_MOVE_BACK).now) { vec.z--; }
		if (key.GetInfo(KEY_TYPE::PLAYER_MOVE_RIGHT).now) { vec.x++; }
		if (key.GetInfo(KEY_TYPE::PLAYER_MOVE_LEFT).now) { vec.x--; }
	}

	if (vec != 0.0f) {
		MATRIX mat = MGetIdent();
		mat = MMult(mat, MGetRotY(Camera::GetIns().GetAngle().y));
		vec = VTransform(vec, mat);
		trans.angle.y = atan2(vec.x, vec.z);
	}
}

void Player::CarryRun(void)
{
	auto& key = KEY::GetIns();

	Vector3 vec = { key.GetLeftStickVec().x,0.0f,-key.GetLeftStickVec().y };

	if (vec == 0.0f) {
		if (key.GetInfo(KEY_TYPE::PLAYER_MOVE_FRONT).now) { vec.z++; }
		if (key.GetInfo(KEY_TYPE::PLAYER_MOVE_BACK).now) { vec.z--; }
		if (key.GetInfo(KEY_TYPE::PLAYER_MOVE_RIGHT).now) { vec.x++; }
		if (key.GetInfo(KEY_TYPE::PLAYER_MOVE_LEFT).now) { vec.x--; }
	}


	if (vec == 0.0f) {
		if (!isJump[0]) { AnimePlay((int)ANIME_TYPE::CARRY_IDLE); }
		Snd::GetIns().Stop("PlayerRun");
	}
	else {
		MATRIX mat = MGetIdent();
		mat = MMult(mat, MGetRotY(Camera::GetIns().GetAngle().y));
		vec.TransMatOwn(mat);
		vec.Normalize();

		trans.pos += vec * 5.0f;

		if (!isJump[0]) {
			AnimePlay((int)ANIME_TYPE::CARRY_RUN);
			Snd::GetIns().Play("PlayerRun");
		}
		else { Snd::GetIns().Stop("PlayerRun"); }

		trans.angle.y = atan2(vec.x, vec.z);
	}
}
void Player::CarryJump(void)
{
	auto& key = KEY::GetIns();

	for (int i = 0; i < JUMP_NUM; i++) {
		if (isJump[i]) { continue; }

		// ダウントリガーでジャンプ開始
		if (key.GetInfo(KEY_TYPE::PLAYER_JUMP).down) {
			isJump[i] = true;
			AnimePlay((int)ANIME_TYPE::JUMP, false);
		}

		// ジャンプしていなかったらループから抜ける
		if (!isJump[i]) { break; }

		//ジャンプキーを離したら、ジャンプキー入力判定を終了
		if (key.GetInfo(KEY_TYPE::PLAYER_JUMP).up) { jumpKeyCounter[i] = INPUT_JUMPKEY_FRAME; }

		//入力時間に応じてジャンプ量を変更する
		if (isJump[i] && key.GetInfo(KEY_TYPE::PLAYER_JUMP).now && jumpKeyCounter[i] < INPUT_JUMPKEY_FRAME) {
			//ジャンプキーの入力カウンターを増やす
			jumpKeyCounter[i]++;

			//ジャンプ力を分配加算する
			accelSum.y = (MAX_JUMP_POWER / (float)INPUT_JUMPKEY_FRAME);

			// その回のジャンプ処理をしたのでそれ以降のループに入らないようにする
			break;
		}
	}

	// モーション更新
	if (isJump[0] && IsAnimeEnd() && accelSum.y < 0.0f) { AnimePlay((int)ANIME_TYPE::FALL); }
}

void Player::HpSharpen(int damage)
{
	if (hp_ <= 0) { return; }

	punch->Off();
	gouge->Reset();
	throwing->Drop();

	hp_ -= (hp_ >= damage) ? damage : hp_;

	Snd::GetIns().Stop("PlayerRun");
	Snd::GetIns().Stop("PlayerEvasion");
	Snd::GetIns().Stop("PlayerPunch");

	Snd::GetIns().Play("PlayerDamage");

	state = (int)STATE::DAMAGE;
	AnimePlay((int)ANIME_TYPE::DAMAGE, false);
	SetInviCounter(60);
}


void Player::LowerLoad(void)
{
	// 通常攻撃（パンチ）
	punch = new PlayerPunch(trans.pos, trans.angle);
	punch->Load();

	// 抉り
	gouge = new PlayerGouge(trans.model);
	gouge->Load();

	// 特殊攻撃（投げ）
	throwing = new Throwing(trans.model, trans.angle);
	throwing->Load();

#pragma region UI
	// プレビュー
	preview = new PlayerPreview(trans.pos, [this](void) { trans.Draw(); });
	preview->Load();

	// HPバー
	hpBar = new PlayerHpBarManager(hp_, HP_MAX);
	hpBar->Load();

	// 操作説明
	operationUi = new PlayerOperationUI(state);
	operationUi->Load();
#pragma endregion

}
void Player::LowerInit(void)
{
	// 通常攻撃（パンチ）
	punch->Init();

	// 抉り
	gouge->Init();

	// 特殊攻撃（投げ）
	throwing->Init();

#pragma region UI
	// プレビュー
	preview->Init(PREVIEW_POS);

	// HPバー
	hpBar->Init(HP_BAR_POS);

	// 操作説明
	operationUi->Init(OPERATION_UI_POS);
#pragma endregion
}
void Player::LowerUpdate(void)
{
	// 通常攻撃（パンチ）
	punch->Update();

	// 抉り
	gouge->Update();

	// 特殊攻撃（投げ）
	throwing->Update();

#pragma region UI
	// プレビュー
	preview->Update();

	// HPバー
	hpBar->Update();

	// 操作説明
	operationUi->Update();
#pragma endregion
}
void Player::LowerDraw(void)
{
	// 通常攻撃（パンチ）
	punch->Draw();

	// 抉り
	gouge->Draw();

	// 特殊攻撃（投げ）
	throwing->Draw();
}
void Player::LowerAlphaDraw(void)
{
	// 通常攻撃（パンチ）
	punch->AlphaDraw();

	// 抉り
	gouge->AlphaDraw();

	// 特殊攻撃（投げ）
	throwing->AlphaDraw();
}
void Player::LowerRelease(void)
{
	// 通常攻撃（パンチ）
	if (punch) {
		punch->Release();
		delete punch;
		punch = nullptr;
	}

	// 抉り
	if (gouge) {
		gouge->Release();
		delete gouge;
		gouge = nullptr;
	}

	// 特殊攻撃（投げ）
	if (throwing) {
		throwing->Release();
		delete throwing;
		throwing = nullptr;
	}

#pragma region UI
	// プレビュー
	if (preview) {
		preview->Release();
		delete preview;
		preview = nullptr;
	}

	// HPバー
	if (hpBar) {
		hpBar->Release();
		delete hpBar;
		hpBar = nullptr;
	}

	// 操作説明
	if (operationUi) {
		operationUi->Release();
		delete operationUi;
		operationUi = nullptr;
	}
#pragma endregion
}