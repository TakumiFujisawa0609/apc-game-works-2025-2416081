#pragma once

#include"ColliderBase.h"

class BoxCollider : public ColliderBase
{
public:
	BoxCollider(TAG type, Vector3 size, float enoughDistance = -1.0f, Vector3 pos = { 0.0f, 0.0f, 0.0f }) :
		ColliderBase(type, enoughDistance, pos),
		size_(size)
	{
		SetShape(SHAPE::BOX);
	}
	~BoxCollider()override {}

	Vector3 GetSize(void)const { return size_; }

	void DrawDebug(unsigned int color = 0xffffff)override {
        Vector3 half = size_ * 0.5f;
        Vector3 center = GetPos();

        // 8頂点
        Vector3 p[8] =
        {
            { center.x - half.x, center.y - half.y, center.z - half.z }, // 0
            { center.x + half.x, center.y - half.y, center.z - half.z }, // 1
            { center.x + half.x, center.y + half.y, center.z - half.z }, // 2
            { center.x - half.x, center.y + half.y, center.z - half.z }, // 3

            { center.x - half.x, center.y - half.y, center.z + half.z }, // 4
            { center.x + half.x, center.y - half.y, center.z + half.z }, // 5
            { center.x + half.x, center.y + half.y, center.z + half.z }, // 6
            { center.x - half.x, center.y + half.y, center.z + half.z }, // 7
        };

        // 辺を描く（12本）
        auto L = [&](int a, int b)
            {
                DrawLine3D(p[a].ToVECTOR(), p[b].ToVECTOR(), color);
            };

        // 手前
        L(0, 1); L(1, 2); L(2, 3); L(3, 0);
        // 奥
        L(4, 5); L(5, 6); L(6, 7); L(7, 4);
        // 側面
        L(0, 4); L(1, 5); L(2, 6); L(3, 7);
	}
private:
	Vector3 size_;
};