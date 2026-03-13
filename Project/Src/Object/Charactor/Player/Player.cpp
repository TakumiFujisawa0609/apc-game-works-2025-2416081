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

	hp(0),

	isJump(),
	jumpKeyCounter(),

	punch(nullptr),
	attackStage(ATTACK_STAGE::NON),
	isAttack(),
	attackStageCounter(0),

	gouge(nullptr),
	isGouge(false),

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

	hp = HP_MAX;

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

	// ノックバック処理 & 演出(画面揺れ & スロー)
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
	case TAG::GOLEM_ATTACK_FALL:

		// ノックバック
		knockBack(collider.GetPos());

		// HP減少
		HpSharpen(Boss::ATTACK_DAMAGE_TABLE[(int)Boss::ATTACK_DAMAGE_TYPE::FALL]);

		return;
	case TAG::GOLEM_ATTACK_PSYCHOROCK:

		// ノックバック
		knockBack(collider.GetPos());

		// HP減少
		HpSharpen(Boss::ATTACK_DAMAGE_TABLE[(int)Boss::ATTACK_DAMAGE_TYPE::PSYCHO]);

		return;
	case TAG::GOLEM_ATTACK_STONE:

		// ノックバック
		knockBack(collider.GetPos());

		// HP減少
		HpSharpen(Boss::ATTACK_DAMAGE_TABLE[(int)Boss::ATTACK_DAMAGE_TYPE::STONE]);

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
		DoStateEvasion();
		DoStateThrowing();
		if (state == (int)STATE::EVASION) { throwing->Drop(); }
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
	auto& key = Key::GetIns();

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
	if (!Key::GetIns().GetInfo(KEY_TYPE::PLAYER_ATTACK).down) { return; }


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

	if (Key::GetIns().GetInfo(KEY_TYPE::PLAYER_GOUGE).down) {
		state = (int)STATE::GOUGE;
		gouge->SearchOn();
		AnimePlay((int)ANIME_TYPE::CATCH, false);
	}
}
void Player::DoStateThrowing(void)
{
	if (Key::GetIns().GetInfo(KEY_TYPE::PLAYER_GOUGE).up) {
		state = (int)STATE::THROWING_OBJ;
		AnimePlay((int)ANIME_TYPE::THROW, false);
	}
}
void Player::DoStateEvasion(void)
{
	if (!Key::GetIns().GetInfo(KEY_TYPE::PLAYER_EVASION).down) { return; }

	state = (int)STATE::EVASION;

	AnimePlay((int)ANIME_TYPE::EVASION, false);

	// SE再生
	Snd::GetIns().Play("PlayerEvasion");
}

void Player::Move(void)
{
	// 移動処理
	Run();

	// ジャンプ処理
	Jump();

	// 攻撃の段数を管理するカウンターを増やす
	if (attackStageCounter <= INPUT_ATTACK_FRAME) { attackStageCounter++; }
}
void Player::Attack(void)
{	
	// 現在のアニメーションの再生率を取得する(0.0f～1.0f)
	float nowAnimeRatio = GetAnimeRatio();

	// 攻撃の判定が発生する前の間、前方に移動させる
	if (nowAnimeRatio <= PUNCH_COLLIDER_END_RATE) {

		// 角度から割り出したベクトルを単位ベクトルに直しスピードを乗算して座標情報に加算する
		trans.pos += Vector3::XZonly(sinf(trans.angle.y), cosf(trans.angle.y)).Normalized() * ATTACK_MOVE_SPEED;
	}

	// 毎フレーム一旦オフ(攻撃判定)
	punch->Off();

	// 大体攻撃判定を発生させる時間
	if (PUNCH_COLLIDER_START_RATE <= nowAnimeRatio && nowAnimeRatio <= PUNCH_COLLIDER_END_RATE) { punch->On(); }

	// 攻撃判定終わったらボタンで次段攻撃に遷移可能にしておく(操作性向上)
	if (nowAnimeRatio > PUNCH_COLLIDER_END_RATE) {
		// 移動入力によって向きを変えられるようにする
		AttackRotate();

		// 次段攻撃に遷移可能にする
		DoStateAttack();
	}

	// 何も入力なく攻撃アニメーションが終了したら通常状態に自動で遷移
	if (IsAnimeEnd()) { state = (int)STATE::MOVE; }
}
void Player::Gouge(void)
{
	if (Key::GetIns().GetInfo(KEY_TYPE::PLAYER_GOUGE).now) {
		// ボタンが押され続けている間処理を行う

		// 現在のモーションを取得
		const int animeType = GetAnimePlayType();

		// まだ破壊箇所探索中(掴みアニメーション再生中)の処理
		if (animeType == (int)ANIME_TYPE::CATCH) {
			// 破壊箇所が見つかったら掴み成功のアニメーションに遷移させる
			if (gouge->SearchHit()) {
				// 掴み成功のアニメーションに遷移
				AnimePlay((int)ANIME_TYPE::GOUGE, false);

				// 掴み成功のフラグをリセット(掴み成功のアニメーションで一度だけ処理を行う為のフラグ)
				isGouge = false;
			}
			// 破壊箇所が見つからないままモーション終了で通常状態に遷移
			if (IsAnimeEnd()) {
				// 掴み処理管理クラスのリセット処理
				gouge->Reset();

				// 掴み失敗の為、持てるオブジェクトがない状態にする(安全処理)
				throwing->Drop();

				// 通常状態に遷移
				state = (int)STATE::MOVE;

				// アニメーションを通常状態のものにする
				AnimePlay((int)ANIME_TYPE::IDLE);
			}
		}
		// 掴み成功のアニメーションの処理
		else if (animeType == (int)ANIME_TYPE::GOUGE) {
			// 掴み成功アニメーション待ち
			if (!isGouge && GetAnimeRatio() > GOUGE_COLLIDER_START_RATE) {
				// 掴み成功のフラグを立てる(掴み成功のアニメーションで一度だけ処理を行う為のフラグ)
				isGouge = true;

				// 掴み成功の処理(破壊箇所を中心にえぐり取る)
				gouge->GougeOn();

				// 掴み成功の処理(掴んだオブジェクトを持つ)
				throwing->Carry(THROW_TYPE::ROCK);
			}

			// モーション終了で状態遷移
			if (IsAnimeEnd()) {
				// 掴み処理管理クラスのリセット処理
				gouge->Reset();

				// 状態を掴んでいる状態にする
				state = (int)STATE::CARRY_OBJ;
			}
		}

	}
	// ボタンが離されたら掴み失敗の処理 or 掴んでいる状態から通常状態への遷移
	else {
		// 掴み処理管理クラスのリセット処理
		gouge->Reset();

		// もし何かすでに掴んでいたら離す
		throwing->Drop();

		// 通常状態に遷移
		state = (int)STATE::MOVE;

		// アニメーションを通常状態のものにする
		AnimePlay((int)ANIME_TYPE::IDLE);
	}
}
void Player::CarryObj(void)
{
	// 掴みボタンが押されている間だけ 掴み続ける+移動が可能な状態にする
	if (Key::GetIns().GetInfo(KEY_TYPE::PLAYER_GOUGE).now) {
		// 移動処理
		CarryRun();
	}
	// 掴みボタンが離されたら掴んでいる状態から通常状態への遷移
	else {
		// 掴んでいるものを離す
		throwing->Drop();

		// 通常状態に遷移
		state = (int)STATE::MOVE;

		// アニメーションを通常状態のものにする
		AnimePlay((int)ANIME_TYPE::IDLE);
	}
}
void Player::ThrowingObj(void)
{
	// 投げるアニメーションの再生率が一定以上になったら抱えているオブジェクトを投げる処理を行う
	if (GetAnimeRatio() > THROWING_RELEASE_ANIME_RATE) {
		// 抱えているオブジェクトを投げる
		throwing->Throw();

		// オブジェクトを投げた後は、入力によって各状態に遷移可能にする(操作性向上の為、投げるアニメーションが終わる前から遷移可能にしておく)
		DoStateMove();
		DoStateAttack();
		DoStateEvasion();
		DoStateGouge();
	}

	// 何も入力なく投げるアニメーションが終了したら通常状態に自動で遷移
	if (IsAnimeEnd()) { state = (int)STATE::MOVE; }
}
void Player::Evasion(void)
{
	// 移動方向ベクトル
	Vector3 vec = {};

	// 角度(trans.angle) から 向き(ベクトル) を割り出す
	vec.x = sinf(trans.angle.y);
	vec.z = cosf(trans.angle.y);

	// 割り出したベクトルを単位ベクトルに直しスピードを乗算して座標情報に加算する
	trans.pos += vec.Normalized() * EVASION_SPEED;

	// 回避アニメーションのローリング中無敵にしておく
	// (無敵カウンターを使って当たり判定を無効にする。この状態を抜けたらすぐに無敵が解除されるように 1 を代入し続けておく)
	if (GetAnimeRatio() <= EVASION_INVINCIBLE_ANIME_RATE) { SetInviCounter(); }

	// 何も入力なく回避アニメーションが終了したら通常状態に自動で遷移
	if (IsAnimeEnd()) { state = (int)STATE::MOVE; }
}
void Player::Damage(void)
{
	// ノックバック処理
	trans.pos += knockBackVec;

	// ダメージアニメーションが終わったら状態遷移
	if (IsAnimeEnd()) {
		// HPが残っていたら通常状態に遷移
		if (hp > 0) {
			// 通常状態に遷移
			state = (int)STATE::MOVE;

			// アニメーションを通常状態のものにする
			AnimePlay((int)ANIME_TYPE::IDLE);
		}
		// HPが0以下なら死亡状態に遷移
		else {
			// HPがマイナスにならないように0で止める(安全処理)
			hp = 0;

			// 死亡状態に遷移
			state = (int)STATE::DEATH;

			// 死亡アニメーション再生
			AnimePlay((int)ANIME_TYPE::DEATH, false);

			return;
		}
	}
}
void Player::Death(void)
{
	// 死亡アニメーションが終了したら終了状態に遷移
	if (IsAnimeEnd()) {
		// 終了状態に遷移
		state = (int)STATE::END;
	}
}

void Player::Run(void)
{
	// 移動方向ベクトル
	Vector3 vec = GetInputVec();

	// 入力がなかったときの処理
	if (vec == 0.0f) {
		// 地面に接地していたらアニメーションを通常状態にする(空中にいるときはそのままのアニメーションでいる為)
		if (isGround) { AnimePlay((int)ANIME_TYPE::IDLE); }

		// 走るSE停止
		Snd::GetIns().Stop("PlayerRun");
	}
	// 入力があったときの処理
	else {
		// カメラの角度に合わせて回転させる行列を作成
		MATRIX mat = MGetRotY(Camera::GetIns().GetAngle().y);

		// 入力情報から割り出した移動方向ベクトルをカメラの角度に合わせて行列回転させる
		vec.TransMatOwn(mat);

		// 移動方向ベクトルを単位ベクトルに直しスピードを乗算して座標情報に加算する
		trans.pos += vec.Normalized() * RUN_SPEED;

		// 地面に接地していたら走るアニメーションにする(空中にいるときはそのままのアニメーションでいる為)
		if (isGround) {
			// 走るアニメーションを再生する
			AnimePlay((int)ANIME_TYPE::RUN);

			// 走るSE再生
			Snd::GetIns().Play("PlayerRun");
		}
		// 地面に接地していなかったら走るSE停止
		else { Snd::GetIns().Stop("PlayerRun"); }

		// 移動方向ベクトルを角度に変換してプレイヤーの向きを変える
		trans.angle.y = atan2(vec.x, vec.z);
	}
}
void Player::Jump(void)
{
	// ジャンプ可能回数分だけループ
	for (int i = 0; i < JUMP_NUM; i++) {
		// フラグがすでに立っていたら次のループへ(多段ジャンプの為の処理)
		if (isJump[i]) { continue; }

		// ダウントリガーでジャンプ開始
		if (Key::GetIns().GetInfo(KEY_TYPE::PLAYER_JUMP).down) {

			// その段数のジャンプフラグを立てる
			isJump[i] = true; 

			// ジャンプキーの入力カウンターを増やす
			jumpKeyCounter[i]++;

			// ジャンプ力を分配加算する
			accelSum.y = (std::max)(accelSum.y, (MAX_JUMP_POWER / (float)INPUT_JUMPKEY_FRAME));

			// ジャンプアニメーションを再生する
			AnimePlay((int)ANIME_TYPE::JUMP, false);

			// ジャンプSE再生
			Snd::GetIns().Play("PlayerJump");
		}

		// ループから抜ける
		break;
	}

	// ジャンプフラグを後ろから探索していって、ジャンプフラグが立っている段数のジャンプ入力処理を行う(多段ジャンプの為の処理)
	for (int i = JUMP_NUM - 1; i >= 0; i--) {
		// ジャンプフラグが立っていなかったら次のループへ
		if (!isJump[i]) { continue; }

		//ジャンプキーを離したら、ジャンプキー入力判定を終了
		if (Key::GetIns().GetInfo(KEY_TYPE::PLAYER_JUMP).up) { jumpKeyCounter[i] = INPUT_JUMPKEY_FRAME; }

		//入力時間に応じてジャンプ量を変更する
		if (Key::GetIns().GetInfo(KEY_TYPE::PLAYER_JUMP).now && jumpKeyCounter[i] < INPUT_JUMPKEY_FRAME) {
			//ジャンプキーの入力カウンターを増やす
			jumpKeyCounter[i]++;

			//ジャンプ力を分配加算する
			accelSum.y += (MAX_JUMP_POWER / (float)INPUT_JUMPKEY_FRAME);
		}

		// 一番後ろのジャンプフラグが立っている段数の処理を行ったらループから抜ける(多段ジャンプの為の処理)
		break;
	}

	// モーション更新
	if (isJump && IsAnimeEnd() && accelSum.y <= 0.0f) { AnimePlay((int)ANIME_TYPE::FALL); }
}

void Player::AttackRotate(void)
{
	// 移動方向ベクトル
	Vector3 vec = GetInputVec();

	// 入力があった場合、ベクトルをワールド座標に変換してプレイヤーの向きを変える
	if (vec != 0.0f) {

		// カメラの角度だけ回転させる為の行列を作成
		MATRIX mat = MGetRotY(Camera::GetIns().GetAngle().y);

		// 入力情報から割り出した移動方向ベクトルをカメラの角度に合わせて行列回転
		vec = VTransform(vec, mat);

		// 移動方向ベクトルを角度に変換
		trans.angle.y = atan2(vec.x, vec.z);
	}
}

void Player::CarryRun(void)
{
	// 移動方向ベクトル
	Vector3 vec = GetInputVec();

	// 入力がなかったときの処理
	if (vec == 0.0f) {
		if (isGround) { AnimePlay((int)ANIME_TYPE::CARRY_IDLE); }
		Snd::GetIns().Stop("PlayerRun");
	}
	// 入力があったときの処理
	else {
		// カメラの角度に合わせて回転させる行列を作成
		MATRIX mat = MGetRotY(Camera::GetIns().GetAngle().y);

		// 入力情報から割り出した移動方向ベクトルをカメラの角度に合わせて行列回転させる
		vec.TransMatOwn(mat);

		// 移動方向ベクトルを単位ベクトルに直しスピードを乗算して座標情報に加算する
		trans.pos += vec.Normalized() * CARRY_MOVE_SPEED;

		// 地面に接地していたら走るアニメーションにする(空中にいるときはそのままのアニメーションでいる為)
		if (isGround) {
			// 走るアニメーションを再生する
			AnimePlay((int)ANIME_TYPE::CARRY_RUN);

			// 走るSE再生
			Snd::GetIns().Play("PlayerRun");
		}
		// 地面に接地していなかったら走るSE停止
		else { Snd::GetIns().Stop("PlayerRun"); }

		// 移動方向ベクトルを角度に変換してプレイヤーの向きを変える
		trans.angle.y = atan2(vec.x, vec.z);
	}
}

void Player::HpSharpen(int damage)
{
	// HPが0以下のときはダメージ処理を行わない(安全処理)
	if (hp <= 0) { return; }

	// パンチの判定が切る(当たり判定が残り続けるバグが発生しないように安全処理)
	punch->Off();
	// 掴み処理管理クラスのリセット処理(掴み成功のフラグもリセットされる)
	gouge->Reset();
	// もし何か掴んでいたら離す
	throwing->Drop();

	// HPを減らす HPがダメージ以上の時はダメージ分減らし、そうでない時はHPを0にする(HPが0を下回らないように)
	hp -= (hp >= damage) ? damage : hp;

	// 各種SE停止(走るSEや回避SEが残り続けるバグが発生しないように安全処理)
	Snd::GetIns().Stop("PlayerRun");
	Snd::GetIns().Stop("PlayerEvasion");
	Snd::GetIns().Stop("PlayerPunch");

	// ダメージSE再生
	Snd::GetIns().Play("PlayerDamage");

	// ダメージ状態に遷移
	state = (int)STATE::DAMAGE;
	// ダメージアニメーションを再生
	AnimePlay((int)ANIME_TYPE::DAMAGE, false);

	// 無敵時間を設定
	SetInviCounter(DAMAGE_INVINCIBLE_FRAME);
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
	hpBar = new PlayerHpBarManager(hp, HP_MAX);
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

Vector3 Player::GetInputVec(void) const
{
	// 移動方向ベクトル
	Vector3 vec = {};

	// 左スティックの入力をXZ平面のベクトルに変換する
	vec = Key::GetIns().GetLeftStickVec().ToVector3XZ();

	// 左スティックの入力がない場合、個別登録キーの入力をベクトルに変換する
	if (vec == 0.0f) {
		if (Key::GetIns().GetInfo(KEY_TYPE::PLAYER_MOVE_FRONT).now) { vec.z++; }
		if (Key::GetIns().GetInfo(KEY_TYPE::PLAYER_MOVE_BACK).now) { vec.z--; }
		if (Key::GetIns().GetInfo(KEY_TYPE::PLAYER_MOVE_RIGHT).now) { vec.x++; }
		if (Key::GetIns().GetInfo(KEY_TYPE::PLAYER_MOVE_LEFT).now) { vec.x--; }
	}

	return vec;
}