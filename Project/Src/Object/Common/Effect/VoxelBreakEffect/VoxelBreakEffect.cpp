#include"VoxelBreakEffect.h"

VoxelBreakEffect::VoxelBreakEffect(const int texture) :
	isActive(false),
	aliveTime(0),

	pos(),
	vel(),
	angle(),

	cellSize(),

	vertex(),
	texture(texture)
{
}

void VoxelBreakEffect::Load(void)
{

}

void VoxelBreakEffect::Init(void)
{
	for (VERTEX3D& v : vertex) { v.dif = GetColorU8(255, 255, 255, 255); v.spc = GetColorU8(0, 0, 0, 0); }
}

void VoxelBreakEffect::Update(void)
{
	if (!isActive) { return; }

	if (--aliveTime <= 0) { isActive = false; }

	pos += vel;

	vel.y -= 0.5f;

	// ’¸“_
	for (unsigned char i = 0; i < VERTEX_NUM; i++)
	{
		Vector3 local = kDir[i] * (cellSize * 0.5f);

		vertex[i].pos = (pos + local).ToVECTOR();

		vertex[i].norm = local.Normalized().ToVECTOR();

		unsigned char idot = i % 4;
		vertex[i].u = (idot == 0 || idot == 3) ? 0.0f : 1.0f;
		vertex[i].v = (idot == 0 || idot == 1) ? 0.0f : 1.0f;
	}
}

void VoxelBreakEffect::Draw(void)
{
	if (!isActive) { return; }
	DrawPolygonIndexed3D(vertex, VERTEX_NUM, INDEX, POLYGON_NUM, texture, true);
}

void VoxelBreakEffect::Release(void)
{

}