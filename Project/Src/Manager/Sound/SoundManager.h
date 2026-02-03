#pragma once

#include<vector>
#include<map>
#include<string>

class SoundManager
{
private:

	// コンストラクタ/デストラクタ
	SoundManager();
	~SoundManager() = default;

	// インスタンス
	static SoundManager* ins_;

	// コピー/ムーブ 操作を禁止
	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;
	SoundManager(SoundManager&&) = delete;
	SoundManager& operator=(SoundManager&&) = delete;
public:

#pragma region シングルトン定義
	// 生成/初期化処理
	static void CreateIns(void) { if (ins_ == nullptr) { ins_ = new SoundManager(); ins_->Init(); } }
	// 取得
	static SoundManager& GetIns(void) { return *ins_; }
	// 終了処理/削除
	static void DeleteIns(void) { if (ins_ != nullptr) { ins_->Release(); delete ins_; } }
#pragma endregion

	/// <summary>
	/// 現在の音声情報を破棄して指定されたシーンの音声情報を読み込む
	/// </summary>
	/// <param name="scene">シーンの名前</param>
	void ChangeScene(const std::string& scene);

	/// <summary>
	/// 音声を再生する
	/// </summary>
	/// <param name="id">音声ID</param>
	void Play(const std::string& id);

	/// <summary>
	/// 指定したサウンドを停止
	/// </summary>
	/// <param name="id">音声ID</param>
	void Stop(const std::string& id);

	/// <summary>
	/// 再生中のサウンドをすべて停止
	/// </summary>
	void AllStop(void);

	/// <summary>
	/// 再生中のサウンドをすべて一時停止。「PausePlay()」で再生再開
	/// </summary>
	void Pause(void);

	/// <summary>
	/// 「Pause()」で一時停止したサウンドをすべて再生再開
	/// </summary>
	void PausePlay(void);

	// 更新
	void Update(void);

private:

#pragma region 定数定義
	// テーブルの参照番号
	enum TABLE_SUBSCR {
		TABLE_ID,		// ID
		TABLE_SCENE,	// 読み込むシーン
		TABLE_TYPE,		// タイプ
		TABLE_PATH,		// パス
		TABLE_VOLUME,	// ボリューム
		TABLE_LOOP,		// ループ
		TABLE_DUPLI,	// 最大同時再生数
	};
#pragma endregion


	// 初期化処理
	void Init(void);
	// 終了処理
	void Release(void);

	// 音声テーブル読み込み用構造体
	struct SoundTable
	{
		// 読み込みシーン
		std::string scene = {};

		// タイプ（BGM/SE）
		std::string type = {};

		// パス
		std::string path = {};

		// 個別の音量の割合
		float volume = 1.0f;

		// ループ再生できるか
		bool loop = false;

		// 最大同時再生数
		unsigned char maxDupli = 1;

		// 生成
		SoundTable(void) {}

		SoundTable(
			const std::string& scene,
			const std::string& type,
			const std::string& path,
			const std::string& volume,
			const std::string& loop,
			const std::string& maxDupli
		) :
			scene(scene),
			type(type),
			path(path),
			volume(std::stof(volume)),
			loop((loop == "TRUE") ? true : false),
			maxDupli((unsigned char)std::stoi(maxDupli))
		{
		}
	};

	// 音声情報構造体
	struct SoundInfo
	{
		// ハンドル番号
		int id = -1;

		// 再生中か
		bool play(void);

		// 一時停止中か
		bool paused = false;
	};
	
	std::map<std::string, SoundTable>SOUND_TABLE;
	std::map<std::string, std::vector<SoundInfo>>sounds;

#pragma region 音量

	// ベースの音量
	unsigned char masterVolume;

	// ベースの音量に対しての「BGM」の音量の割合
	float bgmVolume;

	// ベースの音量に対しての「SE」の音量の割合
	float seVolume;

#pragma endregion

	// ベースの音量とテーブルを参照して最終的な音量を返す
	int VolumeValue(std::string id);

	// 読み込み関数
	void SoundInfoAdd(const std::pair<const std::string, SoundTable>& data);

};

using Snd = SoundManager;