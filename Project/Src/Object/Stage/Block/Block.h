#pragma once

#include"../../VoxelBase.h"

// RGB値をカラーコードに変換する（CSVマップデータの番号に使う）
constexpr int NumberConversion(int r, int g, int b) { return (r << 16) | (g << 8) | b; }

class Block : public VoxelBase
{
public:
    enum class TYPE
    {
        SIMPLE = NumberConversion(170, 170, 170),
    };
    // コンストラクタ
    Block(TYPE type, int baseModelId, int textureId, int mapX, int mapY, int mapZ);
    // デストラクタ
    ~Block(void)override = default;

    void OnCollision(const ColliderBase& collider)override;

private:
    // ブロック種別
    TYPE type;

    // ブロックモデルの大きさ
    const float SCALE = 1.0f;

    // テクスチャサイズ
    const float TEXTURE_SIZE = 100.0f;
    // セルサイズ
	const float CELL_SIZE = 25.0f;

    // 生存に必要なセルの生存割合
    const float ALIVE_NEED_RATE = 0.1f;

    void SubLoad(void)override {}
    void SubInit(void)override { SetJudge(true); SetIsDraw(true); }
};
