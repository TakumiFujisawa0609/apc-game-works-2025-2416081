#include"BossPreview.h"

#include"../../../../../Utility/Utility.h"

#include"../../Boss.h"

BossPreview::BossPreview(const Vector3& bossPos, std::function<void(void)> func) :

	bossPos(bossPos),
	DrawModel(std::move(func)),

	previewCam(nullptr),

	mainScreen(-1),

	frameImg(-1),
	backImg(-1),

	drawPos()
{
}


void BossPreview::Load(void)
{
	mainScreen = MakeScreen((int)SIZE, (int)SIZE);

	Utility::LoadImg(frameImg, "Data/Image/Game/UI/BossPreviewFrame.png");
	Utility::LoadImg(backImg, "Data/Image/Game/UI/BossPreviewBack.png");

	previewCam = new InstantCamera();
}

void BossPreview::Init(const Vector2& drawPos)
{
	this->drawPos = drawPos;

	previewCam->Init();
	previewCam->ChangeModeFollow(&bossPos, camOffset, camAngle);
}

void BossPreview::Update(void)
{
	previewCam->Update();
}

void BossPreview::Draw(unsigned int defaultScreen)
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

void BossPreview::Release(void)
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
