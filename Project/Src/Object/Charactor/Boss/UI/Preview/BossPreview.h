#pragma once

#include<functional>

#include"../../../../../Manager/Camera/InstantCamera.h"

class BossPreview
{
public:
	static constexpr unsigned char SIZE = 160;

	BossPreview(const Vector3& bossPos, std::function<void(void)> func);
	~BossPreview() = default;

	void Load(void);
	void Init(const Vector2& drawPos);
	void Update(void);
	void Draw(unsigned int defaultScreen);
	void Release(void);

private:
	// ボス座標
	const Vector3& bossPos;
	// モデル描画関数
	const std::function<void(void)>DrawModel;

	// プレビュー用カメラ
	InstantCamera* previewCam;

	// メインスクリーンID
	int mainScreen;

	// フレーム画像のハンドルID
	int frameImg;

	// 背景画像のハンドルID
	int backImg;

	// 描画先
	Vector2 drawPos;

	const Vector3 camOffset = Vector3::Zonly(-700.0f);
	const Vector3 camAngle = Vector3::Xonly(20.0f * (DX_PI_F / 180.0f));
};