#include "Brea.h"

#include"../../Utility/Utility.h"

Brea::Brea()
{
}

Brea::~Brea()
{
}

void Brea::Load(void)
{
	unit_.para_.colliType = CollisionType::ALLY;
	unit_.para_.colliShape = CollisionShape::SPHERE;

	unit_.para_.radius = 50.0f;

	unit_.para_.speed = 5.0f;

}

void Brea::Init(void)
{
	unit_.pos_ = {};
	unit_.isAlive_ = false;
}

void Brea::Update(void)
{
	VECTOR moveVec = {};

	if (CheckHitKey(KEY_INPUT_W) == 1) {
		moveVec.z++; 
	}
	if (CheckHitKey(KEY_INPUT_S) == 1) { moveVec.z--; }
	if (CheckHitKey(KEY_INPUT_A) == 1) { moveVec.x--; }
	if (CheckHitKey(KEY_INPUT_D) == 1) { moveVec.x++; }
	if (CheckHitKey(KEY_INPUT_Q) == 1) { moveVec.y--; }
	if (CheckHitKey(KEY_INPUT_E) == 1) { moveVec.y++; }

	moveVec = Utility::Normalize(moveVec);

	unit_.pos_ = VAdd(unit_.pos_, VScale(moveVec, unit_.para_.speed));

	if (CheckHitKey(KEY_INPUT_SPACE) == 1) { unit_.isAlive_ = true; }
	else { unit_.isAlive_ = false; }
}

void Brea::Draw(void)
{
	unsigned int color = 0xffffff;
	if (unit_.isAlive_) { color = 0xff0000; }
	DrawSphere3D(unit_.pos_, unit_.para_.radius, 30, color, color, true);
}

void Brea::Release(void)
{
}