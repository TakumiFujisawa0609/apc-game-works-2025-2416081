#include "UnitBase.h"

#include <DxLib.h>

#include"../Scene/SceneManager/SceneManager.h"

UnitBase::UnitBase() :yAccelSum_(0.0f) {}
UnitBase::~UnitBase() {}

void UnitBase::Invi(void)
{
	if (unit_.inviciCounter_ > 0) { unit_.inviciCounter_--; }
}

void UnitBase::AccelUpdate(void)
{
	unit_.pos_.y += yAccelSum_;
}

void UnitBase::Gravity(void)
{
	yAccelSum_ += GRAVITY;
	if (yAccelSum_ < GRAVITY_MAX) {
		yAccelSum_ = GRAVITY_MAX; 
	}
}
