#pragma once

#include<functional>

#include"../../../../../Manager/Camera/InstantCamera.h"

class PlayerPreview
{
public:
	static constexpr unsigned char SIZE = 100;

	PlayerPreview(const Vector3& playerPos, std::function<void(void)> func);
	~PlayerPreview() = default;

	void Load(void);
	void Init(const Vector2& drawPos);
	void Update(void);
	void Draw(unsigned int defaultScreen);
	void Release(void);

private:
	// プレイヤー座標
	const Vector3& playerPos;
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

	const Vector3 camOffset = Vector3::Zonly(200.0f);
};