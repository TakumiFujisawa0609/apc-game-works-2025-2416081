#include "BlockManager.h"

#include <string>
#include<fstream>

#include"../../../Utility/Utility.h"

#include "../../../Application/Application.h"

BlockManager::BlockManager(void):
	textures(),
	models(),
	blocks()
{
}
BlockManager::~BlockManager(void)
{
}

void BlockManager::Load(void)
{
#pragma region 各テクスチャを読み込む
	LoadImg(textures[(int)Block::TYPE::SIMPLE], "Data/Model/StageBlock/Block.png");
#pragma endregion

#pragma region 各モデルを読み込む
	models[(int)Block::TYPE::SIMPLE] = MV1LoadModel("Data/Model/StageBlock/Block.mv1");
#pragma endregion

	// マップデータを読み込んでマップを生成する
	LoadMapCsvData();

	// 生成したブロック全ての読み込み処理
	for (Block*& b : blocks) { b->Load(); }
}
void BlockManager::Init(void)
{
	// 生成したブロック全ての初期化処理
	for (Block*& b : blocks) { b->Init(); }
}
void BlockManager::Update(void)
{
	// 生成したブロック全ての更新処理
	for (Block*& b : blocks) { b->Update(); }

}
void BlockManager::Draw(void)
{
	// 生成したブロック全ての描画処理
	for (Block*& b : blocks) { b->Draw(); }
}

void BlockManager::AlphaDraw(void)
{
	// 生成したブロック全てのアルファ描画処理
	//for (Block*& b : blocks_) { b->AlphaDraw(); }
}

void BlockManager::Release(void)
{
	// 生成したブロック全ての解放
	for (Block*& b : blocks) {
		if (!b) { continue; }
		b->Release(); 
		delete b;
	}
	blocks.clear();

	// モデルの解放
	for (auto& id : models) {
		if (id.second == -1)continue;
		MV1DeleteModel(id.second);
	}
	// テクスチャの解放
	for (auto& id : textures) {
		if (id.second == -1)continue;
		DeleteGraph(id.second);
	}
}

std::vector<ColliderBase*> BlockManager::GetCollider(void)const
{
	std::vector<ColliderBase*> ret = {};
	ret.reserve(blocks.size());

	for (Block* const& b : blocks) {
		for (ColliderBase*& bCollider : b->GetCollider()) {
			ret.emplace_back(bCollider);
		}
	}

	return ret;
}

const std::vector<MeshBatch> BlockManager::GetMesh(void) const
{
	std::vector<MeshBatch>ret = {};
	for (auto& block : blocks) {
		for (auto& mesh : block->GetMeshs()) {
			ret.emplace_back(mesh);
		}
	}
	return ret;
}

void BlockManager::LoadMapCsvData(void)
{
	// ファイルの読込
	std::ifstream ifs = std::ifstream("Data/MapData/MapData1.ply");
	if (!ifs) { return; }

	// 1行の文字情報格納用
	std::string line;

	// ループ回数
	int lineCount = 0;

	while (getline(ifs, line)) {
		// ループ回数記録
		lineCount++;

		// 11行目まではMagicaVoxel特有の無駄な情報なのでスキップ
		if (lineCount <= 11) { continue; }

		// 専用の構造体で1行から必要な情報を抜き出す
		MagicaVoxelCSVRow info = MagicaVoxelCSVRow(Split(line, ' '));

		blocks.emplace_back(
			new Block((Block::TYPE)info.number,
				models[info.number], textures[info.number],
				info.x, info.y, info.z
			)
		);
	}
}

void BlockManager::StageRevival(void)
{
	blocks.at(382)->ReVival();
	blocks.at(383)->ReVival();
	blocks.at(384)->ReVival();
	blocks.at(385)->ReVival();
	blocks.at(399)->ReVival();
	blocks.at(400)->ReVival();
	blocks.at(401)->ReVival();
	blocks.at(402)->ReVival();
	blocks.at(418)->ReVival();
	blocks.at(419)->ReVival();
	blocks.at(420)->ReVival();
	blocks.at(421)->ReVival();
	blocks.at(438)->ReVival();
	blocks.at(439)->ReVival();
	blocks.at(440)->ReVival();
	blocks.at(441)->ReVival();
}
