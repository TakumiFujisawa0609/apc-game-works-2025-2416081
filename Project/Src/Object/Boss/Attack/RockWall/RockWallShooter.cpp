#include"RockWallShooter.h"

#include"../../../../Manager/Collision/Collision.h"

RockWallShooter::RockWallShooter(const VECTOR& bossPos, const VECTOR& bossAngle) :
	bossPos(bossPos),
	bossAngle(bossAngle)
{
}

RockWallShooter::~RockWallShooter()
{
}

void RockWallShooter::Load(void)
{
	walls_.reserve(CREATE_WALL_NUM * 5);
}

void RockWallShooter::Init(void)
{
}

void RockWallShooter::Update(void)
{
	for (const auto& wall : walls_) { wall->Update(); }
}

void RockWallShooter::Draw(void)
{
	for (const auto& wall : walls_) { wall->Draw(); }
}

void RockWallShooter::Release(void)
{
	for (auto& wall : walls_) {
		if (!wall) { continue; }
		wall->Release();
		delete wall;
		wall = nullptr;
	}
	walls_.clear();
}

void RockWallShooter::On(void)
{
	VECTOR vec = VTransform(CREATE_ST_LOCAL_POS, Utility::MatrixAllMultY({ bossAngle }));
	VECTOR pos = VAdd(bossPos, vec);

	vec = Utility::Normalize(vec);
	vec = VScale(vec, ONE_DISTANCE);

	for (int i = 0; i < CREATE_WALL_NUM; i++) {
		for (const auto& wall : walls_) {
			if (wall->GetUnit().isAlive_ == false) {
				walls_.back()->On(pos);
				pos = VAdd(pos, vec);
				continue;
			}
		}

		walls_.emplace_back(new RockWall());
		walls_.back()->Load();
		walls_.back()->Init();
		walls_.back()->On(pos);
		Collision::AddEnemy(walls_.back());

		pos = VAdd(pos, vec);
	}
}
