#include"Boss.h"

#include"../../../Manager/Input/KeyManager.h"
#include"../../../Manager/Sound/SoundManager.h"
#include"../../../Manager/Score/Score.h"
#include"../../../Manager/Font/FontManager.h"

#include"../../../Application/Application.h"
#include"../../../Scene/Game/GameScene.h"

#include"../Player/Player.h"

Boss::Boss(const Vector3& playerPos):

	fall(nullptr),
	stone(nullptr),
	psycho(nullptr),
	rockWall(nullptr),

	attackState(),
	attackInterval(0),
	attackInit(false),
	attackStart(false),
	attackEnd(false),

	stanTimer(0),

	life(LIFE_MAX),
	hp(HP_MAX),

	preview(nullptr),
	hpBar(nullptr),

	playerPos(playerPos)
{
}

void Boss::Load(void)
{
	// モデルをロード
	trans.Load("Boss/Giant");

	// スケールを設定
	trans.scale = SCALE;

	// 相対　座標/角度
	trans.centerDiff = -CENTER_DIFF;
	trans.localAngle = LOCAL_ANGLE;

	SetDynamicFlg(false);
	SetGravityFlg(false);

	SetPushFlg(true);

	// コライダーを生成
	ColliderCreate(new CapsuleCollider(TAG::BOSS, CAPSULE_COLLIDER_START_POS, CAPSULE_COLLIDER_END_POS, CAPSULE_COLLIDER_RADIUS, CAPSULE_COLLIDER_ENOUGH_DISTANCE));

#pragma region 関数ポインタ配列へ各関数を格納

	CHARACTOR_SET_STATE(STATE::NON, &Boss::Non);
	CHARACTOR_SET_STATE(STATE::IDLE, &Boss::Idle);
	CHARACTOR_SET_STATE(STATE::ATTACK, &Boss::Attack);
	CHARACTOR_SET_STATE(STATE::DAMAGE, &Boss::Damage);
	CHARACTOR_SET_STATE(STATE::STAN, &Boss::Stan);
	CHARACTOR_SET_STATE(STATE::BIG_DAMAGE, &Boss::BigDamage);
	CHARACTOR_SET_STATE(STATE::DEATH, &Boss::Death);
	CHARACTOR_SET_STATE(STATE::END, &Boss::End);
	
#pragma endregion

	// アニメーション
	CreateAnimationController();
	AnimeLoad();
	AnimePlay((int)ANIME_TYPE::WALK);

	// Bossクラスが抱える子クラス達の読み込み処理
	LowerLoad();
}

void Boss::CharactorInit(void)
{
	// 待機状態に設定
	state = (int)STATE::IDLE;

	// 座標を初期化
	trans.pos = INIT_POS;

	// 当たり判定 / 描画 のフラグを初期化しておく(安全)
	SetJudge(true);
	SetIsDraw(true);

	// ライフとHPを初期化
	life = LIFE_MAX;
	hp = HP_MAX;

	// 攻撃のインターバルを初期化
	attackInterval = *ATTACK_INTERVAL;

	// スタン状態のカウンター変数を初期化
	stanTimer = 0;

	// Bossクラスが抱える子クラスの初期化処理
	LowerInit();
}

void Boss::CharactorUpdate(void)
{
	// Bossクラスが抱える子クラスの更新処理
	LowerUpdate();
}

void Boss::CharactorDraw(void)
{
	if (!GetIsDraw()) { return; }

	// Bossクラスが抱える子クラスの描画処理
	LowerDraw();
}

void Boss::CharactorAlphaDraw(void)
{
	if (!GetIsDraw()) { return; }

	// Bossクラスが抱える子クラスの描画処理
	LowerAlphaDraw();
}

void Boss::UiDraw(void)
{
	// プレビュー表示
	preview->Draw(DX_SCREEN_BACK);

	// HPバーの表示
	hpBar->Draw();

	// スタン状態の時、HPバーの上にスタン中であることを知らせるテキストを点滅させて表示する
	if (HpBarStanTextFlg()) {
		DrawStringToHandle(
			HP_BAR_STAN_TEXT_POS.x, HP_BAR_STAN_TEXT_POS.y,
			HP_BAR_STAN_TEXT, HP_BAR_STAN_TEXT_COLOR,
			Font::GetIns().GetFont(FontKinds::DEFAULT_45)
		);
	}
}

void Boss::CharactorRelease(void)
{
	// Bossクラスが抱える子クラスの解放処理
	LowerRelease();
}

void Boss::OnCollision(const ColliderBase& collider)
{
	// 無敵時間中はダメージを受けない
	if (GetInviCounter() > 0) { return; }

	// スタン状態のときは処理を変更
	if (state == (int)STATE::STAN) {

		// パンチを受けたときの処理
		if (collider.GetTag() == TAG::PLAYER_PUNCH) {

			// 効果音を鳴らす
			Snd::GetIns().Play("ObjBreak");

			// ライフを減らす
			LifeSharpen();

			// 無敵時間を設定
			SetInviCounter(LIFE_LOST_INVINCIBLE_TIME);

			// スコアを加算
			Score::GetIns().ScoreAddCombo(LIFE_LOST_SCORE);
		}

		return;
	}

	// 投擲攻撃(岩)を受けたときの処理
	if (collider.GetTag() == TAG::PLAYER_THROWING) {
		// 画面を揺らす
		GameScene::Shake();
		// 効果音を鳴らす
		Snd::GetIns().Play("ObjBreak");

		// HPを減らす
		HpSharpen(playerAttackPowerTable[(int)Player::ATTACK_POWER_TYPE::THROWING_ROCK]);

		// 無敵時間を設定
		SetInviCounter(THROWING_DAMAGE_INVINCIBLE_TIME);

		// スコアを加算
		Score::GetIns().ScoreAddCombo(
			playerAttackPowerTable[(int)Player::ATTACK_POWER_TYPE::THROWING_ROCK] * HP_SHARPEN_SCORE_RATE
		);

		return;
	}

	// パンチを受けたときの処理
	if (collider.GetTag() == TAG::PLAYER_PUNCH) {
		// 画面を揺らす
		GameScene::Shake();
		// 効果音を鳴らす
		Snd::GetIns().Play("ObjBreak");

		// HPを減らす
		HpSharpen(playerAttackPowerTable[(int)Player::ATTACK_POWER_TYPE::PUNCH]);

		// 無敵時間を設定
		SetInviCounter(PUNCH_DAMAGE_INVINCIBLE_TIME);

		// スコアを加算
		Score::GetIns().ScoreAddCombo(
			playerAttackPowerTable[(int)Player::ATTACK_POWER_TYPE::PUNCH] * HP_SHARPEN_SCORE_RATE
		);

		return;
	}
}

void Boss::Idle(void)
{
	// 待機状態のアニメーションを再生する
	AnimePlay((int)ANIME_TYPE::IDLE);

	// プレイヤーの方を向く
	Vector3 vec = playerPos - trans.pos;
	trans.angle.y = atan2f(vec.x, vec.z);

	// 攻撃のインターバルが0以下のときは攻撃状態へ遷移させる 攻撃のインターバルが0より大きいときは攻撃のインターバルを減らす
	if (--attackInterval <= 0) {
		// 攻撃のインターバルカウンターを0にする(安全処理)
		attackInterval = 0;

		// 攻撃開始のフラグを立てる
		attackInit = true;

		// 攻撃状態へ遷移させる
		state = (int)STATE::ATTACK;
	}
}
void Boss::Attack(void)
{
#pragma region 攻撃状態へ遷移後 １回目の処理
	if (attackInit) {
		// 攻撃開始のフラグを初期化して2回目以降初期化処理を行わないようにする
		attackInit = false;

		// 攻撃 開始/終了 のフラグを初期化
		attackStart = true;
		attackEnd = false;

		// 攻撃の抽選
		attackState = AttackLottery();

		// 攻撃の種類に合わせて攻撃のインターバルを設定する（何かしらのバグで攻撃の種類の抽選が上手くいかなかった場合は何もしない）
		if (attackState != ATTACK_KINDS::NON) { attackInterval = ATTACK_INTERVAL[(int)attackState]; }

		// 攻撃の種類に合わせて攻撃開始の処理を行う
		switch (attackState)
		{
		case Boss::ATTACK_KINDS::NON:
			attackEnd = true;
			break;
		case Boss::ATTACK_KINDS::FALL:
			AnimePlay((int)ANIME_TYPE::SLAP, false);
			fall->Set();
			break;
		case Boss::ATTACK_KINDS::STONE:
			AnimePlay((int)ANIME_TYPE::PUNCH, false);
			break;
		case Boss::ATTACK_KINDS::PSYCHO:
			AnimePlay((int)ANIME_TYPE::SLAP, false);
			psycho->Set();
			break;
		case Boss::ATTACK_KINDS::WALL:
			AnimePlay((int)ANIME_TYPE::SLAP, false);
			break;
		}
	}
#pragma endregion
#pragma region 攻撃開始の処理
	if (attackStart) {
		switch (attackState)
		{
		case Boss::ATTACK_KINDS::NON:
			attackStart = false;
			break;
		case Boss::ATTACK_KINDS::FALL:
			if (IsAnimeEnd()) {
				attackStart = false;
				fall->On();
			}
			else { return; }
			break;
		case Boss::ATTACK_KINDS::STONE:
			if (IsAnimeEnd()) {
				attackStart = false;
				stone->On();
			}
			else { return; }
			break;
		case Boss::ATTACK_KINDS::PSYCHO:
			if (IsAnimeEnd()) {
				attackStart = false;
				psycho->On();
			}
			else { return; }
			break;
		case Boss::ATTACK_KINDS::WALL:
			if (IsAnimeEnd()) {
				attackStart = false;
				rockWall->On();
			}
			else { return; }
			break;
		}
	}
#pragma endregion
#pragma region 攻撃状態中のみ行う更新処理 また攻撃終了判断

	// 各攻撃の攻撃中と攻撃終了の判断を行う
	switch (attackState)
	{
	case Boss::ATTACK_KINDS::NON:
		attackEnd = true;
		break;
	case Boss::ATTACK_KINDS::FALL:
		if (IsAnimeEnd()) { attackEnd = true; }
		break;
	case Boss::ATTACK_KINDS::STONE:
		if (IsAnimeEnd()) { attackEnd = true; }
		break;
	case Boss::ATTACK_KINDS::PSYCHO:
		if (IsAnimeEnd()) { attackEnd = true; }
		break;
	case Boss::ATTACK_KINDS::WALL:
		if (IsAnimeEnd()) { attackEnd = true; }
		break;
	}
#pragma endregion
#pragma region 攻撃終了 通常状態へ遷移
	if (attackEnd) {
		// すでに設定してあるはずだが攻撃の種類に合わせてインターバルを設定する(安全処理)
		if (attackState != ATTACK_KINDS::NON) { attackInterval = ATTACK_INTERVAL[(int)attackState]; }

		// 通常状態に遷移させる
		state = (int)STATE::IDLE;
		// 通常状態のアニメーションを再生する
		AnimePlay((int)ANIME_TYPE::IDLE);
	}
#pragma endregion
}
void Boss::Damage(void)
{
	// アニメーションが終わったら通常状態へ遷移
	if (IsAnimeEnd()) { state = (int)STATE::IDLE; }
}
void Boss::Stan(void)
{
	// スタン状態の時間が終わったらHPを規定量回復して通常状態へ遷移
	if (--stanTimer <= 0) {
		// HPを規定量回復
		hp = (int)(HP_MAX * STAN_RECOVERY_RATE);

		// 通常状態へ遷移させる
		state = (int)STATE::IDLE;
	}
}
void Boss::BigDamage(void)
{
	// アニメーションが終わったらHPを戻して通常状態へ遷移
	if (GetAnimeRatio() > BIG_DAMAGE_TIME) {
		// HPを最大値に戻す
		hp = HP_MAX;

		// 通常状態へ遷移させる
		state = (int)STATE::IDLE;
		// 通常状態のアニメーションを再生する
		AnimePlay((int)ANIME_TYPE::IDLE);
	}
}
void Boss::Death(void)
{
	// アニメーションが終わったら当たり判定消して終了状態へ遷移
	if (IsAnimeEnd()) {
		// 当たり判定を消す
		SetJudge(false);

		// 終了状態へ遷移させる
		state = (int)STATE::END;
	}
}

Boss::ATTACK_KINDS Boss::AttackLottery(void)
{
	// 最終結果を格納する変数を宣言、初期化
	ATTACK_KINDS ret = ATTACK_KINDS::NON;

	// 乱数を生成
	unsigned short rand = GetRand(ATTACK_LOTTERY_WORK_VALUE);

	// 乱数と比べる為の数値を宣言、初期化
	unsigned short value = 0;

	// 攻撃の種類の数だけループ
	for (unsigned char i = 0; i < (unsigned char)ATTACK_KINDS::MAX; i++) {

		// 乱数と比べる為の数値に現在の攻撃の種類の確率を加算
		value += (unsigned short)((float)ATTACK_LOTTERY_WORK_VALUE * ATTACK_LOTTERY_RATE[i]);

		// 乱数が比べる為の数値以下ならば最終結果を現在の攻撃の種類にしてループを抜ける
		if (rand <= value) { ret = (ATTACK_KINDS)i; break; }
	}

	// 最終結果を返す
	return ret;
}

void Boss::AnimeLoad(void)
{
	AddInFbxAnimation((int)ANIME_TYPE::MAX, IN_FBX_ANIME_SPEED);
}


void Boss::LowerLoad(void)
{
	fall = new FallManager(playerPos);
	fall->Load();

	stone = new StoneShooter(trans.pos, trans.angle);
	stone->Load();

	psycho = new PsychoRockShooter(trans.pos, playerPos);
	psycho->Load();

	rockWall = new RockWallShooter(trans.pos, trans.angle);
	rockWall->Load();

	// プレビュー
	preview = new BossPreview(trans.pos, [this](void) { trans.Draw(); });
	preview->Load();

	// HPバー
	hpBar = new BossHpBarManager(hp, HP_MAX, life);
	hpBar->Load();
}
void Boss::LowerInit(void)
{
	fall->Init();
	stone->Init();
	psycho->Init();
	rockWall->Init();
	
	preview->Init(PREVIEW_POS);

	hpBar->Init(HP_BAR_POS, HP_BAR_COLOR);
}
void Boss::LowerUpdate(void)
{
	fall->Update();
	stone->Update();
	psycho->Update();
	rockWall->Update();

	preview->Update();
	
	hpBar->Update();
}
void Boss::LowerDraw(void)
{
	fall->Draw();
	rockWall->Draw();
	stone->Draw();
	psycho->Draw();
}
void Boss::LowerAlphaDraw(void)
{
	fall->AlphaDraw();
	rockWall->AlphaDraw();
	stone->AlphaDraw();
	psycho->AlphaDraw();
}
void Boss::LowerRelease(void)
{
	if (rockWall) {
		rockWall->Release();
		delete rockWall;
		rockWall = nullptr;
	}
	if (stone) {
		stone->Release();
		delete stone;
		stone = nullptr;
	}
	if (fall) {
		fall->Release();
		delete fall;
		fall = nullptr;
	}
	if (psycho) {
		psycho->Release();
		delete psycho;
		psycho = nullptr;
	}

	if (preview) {
		preview->Release();
		delete preview;
		preview = nullptr;
	}

	if (hpBar) {
		hpBar->Release();
		delete hpBar;
		hpBar = nullptr;
	}
}

void Boss::HpSharpen(int damage)
{
	// HPが0以下の時はダメージを与えない
	if (hp <= 0) { return; }

	// HPを減らす HPがダメージ以上の時はダメージ分減らし、そうでない時はHPを0にする(HPが0を下回らないように)
	hp -= (hp >= damage) ? damage : hp;

	// HPが0になったらスタン状態へ遷移させる
	if (hp <= 0) {
		// HPを0にする(安全処理)
		hp = 0;

		// スタン状態の時間を設定
		stanTimer = STAN_TIME;

		// スタン状態へ遷移させる
		state = (int)STATE::STAN;

		// スタン状態のアニメーションを再生する
		AnimePlay((int)ANIME_TYPE::STAN);

		return;
	}

	// HPが0になっていない場合はダメージ状態へ遷移させる ただし、攻撃状態の時はダメージ状態へ遷移させない(攻撃状態のアニメーションを途中で止めない為)
	if (state != (int)STATE::ATTACK) {

		// ダメージ状態へ遷移させる
		state = (int)STATE::DAMAGE;

		// ダメージ状態のアニメーションを再生する
		AnimePlay((int)ANIME_TYPE::DAMAGE, false);
	}
}
void Boss::LifeSharpen(void)
{
	// ライフが0以下の時はダメージを与えない
	if (life <= 0) { return; }

	// ライフを減らしつつライフがなくなったか判定を行う
	if (--life <= 0) {
		// マイナスにならないようにライフを0にする(安全処理)
		life = 0;

		// ヒットストップ
		GameScene::HitStop(DEATH_HIT_STOP_TIME);
		// スロー
		GameScene::Slow(DEATH_SLOW_TIME);
		// 画面を大きく揺らす
		GameScene::Shake(ShakeKinds::WID, ShakeSize::BIG, DEATH_SCREEN_SHAKE_TIME);

		// 死亡状態へ遷移させる
		state = (int)STATE::DEATH;
		// 死亡状態のアニメーションを再生する
		AnimePlay((int)ANIME_TYPE::DEATH, false);
		return;
	}

	// HPを最大値に戻す
	hp = HP_MAX;

	// スロー
	GameScene::Slow(BIG_DAMAGE_SLOW_TIME);
	// 画面を揺らす
	GameScene::Shake();

	// プレイヤーの方を向く
	Vector3 vec = playerPos - trans.pos;
	trans.angle.y = atan2f(vec.x, vec.z);

	// 大ダメージ状態へ遷移させる
	state = (int)STATE::BIG_DAMAGE;
	// 大ダメージ状態のアニメーションを再生する
	AnimePlay((int)ANIME_TYPE::DEATH, false);
}
