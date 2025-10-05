#include <string>
#include<fstream>
#include "../../../Application/Application.h"
#include"../../../Utility/Utility.h"
#include "Block.h"
#include "BlockManager.h"

BlockManager::BlockManager(void)
{
}
BlockManager::~BlockManager(void)
{
}

void BlockManager::Load(void)
{
	// 各種ブロックモデルのロード
	std::string PATH = "Data/Model/StageBlocks/";

	models_[0] = MV1LoadModel((PATH + "Block_Grass.mv1").c_str());
	models_[1] = MV1LoadModel((PATH + "Block_Metal.mv1").c_str());

	LoadMapCsvData();
}
void BlockManager::Update(void)
{
	for (int y = 0; y < NUM_BLOCK_Y; y++) {
		for (int z = 0; z < NUM_BLOCK_Z; z++) {
			for (int x = 0; x < NUM_BLOCK_X; x++) {
				if (blocks_[y][z][x] == nullptr)continue;
				blocks_[y][z][x]->Update();
			}
		}
	}
}
void BlockManager::Draw(void)
{
	// マップの描画
	for (int y = 0; y < NUM_BLOCK_Y; y++) {
		for (int z = 0; z < NUM_BLOCK_Z; z++) {
			for (int x = 0; x < NUM_BLOCK_X; x++) {
				if (blocks_[y][z][x] == nullptr)continue;
				blocks_[y][z][x]->Draw();
			}
		}
	}
}

void BlockManager::Release(void)
{
	// モデルのメモリ解放
	for (int y = 0; y < NUM_BLOCK_Y; y++) {
		for (int z = 0; z < NUM_BLOCK_Z; z++) {
			for (int x = 0; x < NUM_BLOCK_X; x++) {
				if (blocks_[y][z][x] == nullptr)continue;
				blocks_[y][z][x]->Release();
				delete blocks_[y][z][x];
				blocks_[y][z][x] = nullptr;
			}
		}
	}

	for (auto& b : models_) {
		if (b.second == -1)continue;
		MV1DeleteModel(b.second);
	}
}

const std::vector<UnitBase*> BlockManager::GetBlocks(void) const
{
	std::vector<UnitBase*> ret = {};

	for (auto& by : blocks_) {
		for (auto& bz : by) {
			for (auto& bx : bz) {
				ret.emplace_back(bx);
			}
		}
	}

	return ret;
}



void BlockManager::LoadMapCsvData(void)
{
	// ファイルの読込
	std::ifstream ifs = std::ifstream("Data/MapData/MapData.csv");
	if (!ifs) { return; }

	// ファイルを１行ずつ読み込む
	std::string line; // 1行の文字情報

	std::vector<std::string> strSplit; // 1行を1文字の動的配列に分割

	int chipNo = 0;

	int lineCount = 0;

	blocks_.resize(NUM_BLOCK_Y);
	for (auto& b1 : blocks_) {
		b1.resize(NUM_BLOCK_Z);
		for (auto& b2 : b1) { b2.resize(NUM_BLOCK_X); }
	}

	for (auto& b1 : blocks_) { for (auto& b2 : b1) { for (auto& b3 : b2) { b3 = nullptr; } } }

	while (getline(ifs, line))
	{
		// １行をカンマ区切りで分割
		strSplit = Utility::Split(line, ',');

		for (int i = 0; i < strSplit.size(); i++) {
			if (strSplit[i] == "- 1") { continue; }

			Block* block = new Block();
			block->Create((Block::TYPE)0, models_[std::stoi(strSplit[i])], i, 0, lineCount);
			block->Load();

			// 配列にブロッククラスのポインタを格納
			blocks_[0][lineCount][i] = block;
		}

		lineCount++;
	}
}

void BlockManager::SetCamera(Camera* c)
{
	for (auto& by : blocks_) {
		for (auto& bz : by) {
			for (auto& bx : bz) {
				bx->SetCamera(c);
			}
		}
	}
}
