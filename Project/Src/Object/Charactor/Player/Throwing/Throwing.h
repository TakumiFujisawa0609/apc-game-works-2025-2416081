#pragma once

#include"Rock/ThrowRock.h"

#include"../../../Common/DataLoad/ParameterLoad.h"

enum class THROW_TYPE { NON = -1, ROCK, MAX };

// 整数型配列の合計を返す
constexpr unsigned short ArraySum(const unsigned char* table, int num) {
	unsigned short ret = 0;
	for (int i = 0; i < num; i++) { ret += table[i]; }
	return ret;
}

class Throwing
{
public:

	Throwing(const int& playerModel, const Vector3& playerAngle, int PLAYER_RIGHTHAND_FRAME_INDEX);
	~Throwing() = default;

	void Load(void);
	void Init(void);
	void Update(void);
	void Draw(void);
	void AlphaDraw(void);
	void Release(void);

	void Carry(THROW_TYPE type);
	void Drop(void);
	void Throw(void);

	std::vector<ColliderBase*> GetCollider(void)const {
		std::vector<ColliderBase*>ret = {};

		for (const THROW_OBJ_INFO& obj : throwObj) {
			for (ColliderBase*& collider : obj.ins->GetCollider()) { ret.emplace_back(collider); }
		}

		return ret;
	}

private:
	// スローオブジェクト構造体
	struct THROW_OBJ_INFO {
		ThrowObjBase* ins = nullptr;
		THROW_TYPE type = THROW_TYPE::NON;
	};

	// パラメーターの外部読み込み
	const ParameterLoad parameter;

	// 各タイプごとの最大生成個数テーブル
	const unsigned char BY_TYPE_NUM[(int)THROW_TYPE::MAX] = 
	{
		(unsigned char)parameter.GetParameter("RockNum"), // ROCK
	};

	// 全体タイプの最大個数の合計
	const short MAX_OBJ_NUM = ArraySum(BY_TYPE_NUM, (int)THROW_TYPE::MAX);

	// 最大個数の合計の数でインスタンス格納配列を作成
	std::vector<THROW_OBJ_INFO> throwObj;

	// タイプ別モデルハンドル
	int models[(int)THROW_TYPE::MAX];

	const int& playerModel;
	const Vector3& playerAngle;

	// プレイヤーモデルの右手の位置のフレーム番号
	const int PLAYER_RIGHTHAND_FRAME_INDEX;
};