#include"PlayerPreview.h"

#include"../../../../../Utility/Utility.h"

#include"../../Player.h"

PlayerPreview::PlayerPreview(const Vector3& playerPos, std::function<void(void)> func) :

	playerPos(playerPos),
	DrawModel(std::move(func)),

	previewCam(nullptr),

	mainScreen(-1),

	frameImg(-1),
	backImg(-1),

	drawPos()
{
}

void PlayerPreview::Load(void)
{
	mainScreen = MakeScreen((int)SIZE, (int)SIZE);

	Utility::LoadImg(frameImg, "Data/Image/Game/UI/PlayerPreviewFrame.png");
	Utility::LoadImg(backImg, "Data/Image/Game/UI/PlayerPreviewBack.png");

	previewCam = new InstantCamera();
}

void PlayerPreview::Init(const Vector2& drawPos)
{
	this->drawPos = drawPos;

	previewCam->Init();
	previewCam->ChangeModeFollow(&playerPos, camOffset);
}

void PlayerPreview::Update(void)
{
	previewCam->Update();
}

void PlayerPreview::Draw(unsigned int defaultScreen)
{
	SetDrawScreen(mainScreen);
	ClearDrawScreen();

	previewCam->Apply();

	DrawGraph(0, 0, backImg, true);

	DrawModel();

	DrawGraph(0, 0, frameImg, true);

	SetDrawScreen(defaultScreen);

	DrawGraph((int)drawPos.x, (int)drawPos.y, mainScreen, true);
}

void PlayerPreview::Release(void)
{
	if (previewCam != nullptr) {
		previewCam->Release();
		delete previewCam;
		previewCam = nullptr;
	}

	DeleteGraph(frameImg);
	DeleteGraph(backImg);
	DeleteGraph(mainScreen);
}
