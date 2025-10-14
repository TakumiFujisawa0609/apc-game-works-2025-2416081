#pragma once

#include<map>
#include<string>

class SoundManager
{
private:
	SoundManager();
	~SoundManager();
public:
	static void CreateIns(void) { if (ins_ == nullptr) { ins_ = new SoundManager(); ins_->Init(); } }
	static SoundManager& GetIns(void) { CreateIns(); return *ins_; }
	static void DeleteIns(void) { if (ins_ != nullptr) { ins_->Release(); delete ins_; } }

	enum SOUND
	{
		NON=-1,

		// 環境効果音
		OBJECT_BREAK,

		// プレイヤー
		PLAYER_RUN,
		PLAYER_PUNCH,
		PLAYER_EVASION,


		// システム音声
		SE_SYSTEM_BUTTON,
		SE_SYSTEM_SELECT,

		MAX,
	};

	/// <summary>
	/// 指定したサウンドをロード
	/// </summary>
	/// <param name="s">種類</param>
	void Load(SOUND s);

	/// <summary>
	/// 指定したサウンドを再生
	/// </summary>
	/// <param name="s">種類</param>
	/// <param name="over">再生中のうえから再生するかどうか</param>
	/// <param name="volume">音量</param>
	/// <param name="loop">ループ再生</param>
	/// <param name="topPlay">最初から再生するか</param>
	void Play(SOUND s, bool over = false, int volume = 200, bool loop = false, bool topPlay = true);

	/// <summary>
	/// 指定したサウンドを停止
	/// </summary>
	/// <param name="s">種類</param>
	void Stop(SOUND s);

	/// <summary>
	/// 再生中のサウンドをすべて停止
	/// </summary>
	void AllStop(void);

	/// <summary>
	/// AllStop()で一時停止したサウンドをすべて再生再開
	/// </summary>
	void PausePlay(void);

	/// <summary>
	/// AllStop()で一時停止したサウンドのポーズ情報を破棄する
	/// </summary>
	/// <param name=""></param>
	void PauseInfoDelete(void);

	/// <summary>
	/// 指定したサウンドを消去
	/// </summary>
	/// <param name="s">種類</param>
	void Delete(SOUND s);

private:
	static SoundManager* ins_;

	struct Sinfo
	{
		SOUND type_ = SOUND::NON;
		std::string path_;
		int id_ = -1;
		bool loop_ = false;
		bool paused_ = false;
		int volume_ = 255;
	};

	Sinfo sounds_[SOUND::MAX];


	void Init(void);
	void Release(void);
};

using Smng = SoundManager;
using SOUND = Smng::SOUND;