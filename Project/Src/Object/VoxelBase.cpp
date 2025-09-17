#include "VoxelBase.h"

#include <algorithm>
#include <queue>
#include <cmath>

#include"../Manager/Camera/Camera.h"

#include"../Application/Application.h"

#include"Player/Player.h"

VoxelBase::VoxelBase() :

    camera_(nullptr),

    textureId_(-1),
    batches_(),
    marked(0), solid(0),
    Nx_(0), Ny_(0), Nz_(0),
    cell_(0.0f),
    gridCenter_(0.0f, 0.0f, 0.0f),
    density_(),
    regeneration_(false)
{
}


void VoxelBase::Load(void)
{
    SubLoad();

    if (unit_.model_ != -1) {

        BuildVoxelMeshFromMV1Handle(
            unit_.model_,                   // モデル
            20.0f,                          // セル(大きさ)
            unit_.para_.center,             // グリッド中心（ワールド）
            VScale(unit_.para_.size, 0.5f), // halfExt（おおよその半サイズ）
            batches_);

        MV1DeleteModel(unit_.model_);
    }
}


void VoxelBase::Init(void)
{
    SubInit();

    regeneration_ = false;
}


void VoxelBase::Update(void)
{
    SubUpdate();

    if (regeneration_) {

        BuildCubicMesh(density_, Nx_, Ny_, Nz_, cell_, gridCenter_, batches_);

        regeneration_ = false; 
    }
}


void VoxelBase::Draw(void)
{
    if (!unit_.isAlive_) { return; }

    SubDraw();

    const VECTOR camPos = camera_->GetPosition();

    for (auto& b : batches_) {
        const VECTOR worldOff = unit_.pos_;

        VECTOR toCam = Utility::Normalize(VSub(camPos, VAdd(b.centerLocal, worldOff)));

        bool drawDir[6];
        for (int f = 0; f < 6; f++) {
            drawDir[f] = (VDot(kDirNrm[f], toCam) > 0.0f) && !b.iDir[f].empty();
        }
        if (!(drawDir[0] || drawDir[1] || drawDir[2] || drawDir[3] || drawDir[4] || drawDir[5])) { continue; }

        std::vector<VERTEX3D> vtx;
        vtx.resize(b.v.size());

        for (size_t k = 0; k < b.v.size(); k++) {
            vtx[k] = b.v[k];
            vtx[k].pos = VAdd(vtx[k].pos, worldOff);
        }

        for (int f = 0; f < 6; f++) {
            if (!drawDir[f]) { continue; }
            DrawPolygonIndexed3D(
                vtx.data(), (int)vtx.size(),
                b.iDir[f].data(), (int)(b.iDir[f].size() / 3),
                textureId_, true);
        }
    }
}


void VoxelBase::Release(void)
{
    SubRelease();

    for (auto& b : batches_) {
        b.i.clear();
        b.v.clear();
    }
    batches_.clear();
    DeleteGraph(textureId_);
}






void VoxelBase::SolidFill(std::vector<uint8_t>& density, int Nx, int Ny, int Nz)
{
    const int total = Nx * Ny * Nz;
    std::vector<uint8_t> ext(total, 0);
    std::queue<int> q;
    auto pushIf = [&](int x, int y, int z) {
        if (!Inb(x, y, z, Nx, Ny, Nz)) return; int i = Idx(x, y, z, Nx, Ny);
        if (ext[i] || density[i] != 0) return; ext[i] = 1; q.push(i);
    };
    for (int x = 0; x < Nx; ++x) { for (int y = 0; y < Ny; ++y) { pushIf(x, y, 0); pushIf(x, y, Nz - 1); } }
    for (int x = 0; x < Nx; ++x) { for (int z = 0; z < Nz; ++z) { pushIf(x, 0, z); pushIf(x, Ny - 1, z); } }
    for (int y = 0; y < Ny; ++y) { for (int z = 0; z < Nz; ++z) { pushIf(0, y, z); pushIf(Nx - 1, y, z); } }
    const int off[6][3] = { {1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1} };
    while (!q.empty()) {
        int i = q.front(); q.pop();
        int z = i / (Nx * Ny), y = (i - z * Nx * Ny) / Nx, x = i % Nx;
        for (int k = 0; k < 6; ++k) { pushIf(x + off[k][0], y + off[k][1], z + off[k][2]); }
    }

    // まずカウント（上書き前に数える）
    for (int i = 0; i < total; ++i) {
        if (density[i] == 200) ++marked;     // 表面フラグ
        if (!ext[i])           ++solid; // 内部領域
    }

    // 外は0のまま、内部と表面を255へ
    for (int i = 0; i < total; ++i) {
        if (!ext[i])                density[i] = 255;  // 内部
        else if (density[i] == 200)   density[i] = 255;  // 外部側の表面
        // それ以外（外部空気）は 0 のまま
    }
}


void VoxelBase::MarkSurfaceByCollisionProbe(int mv1, float cell, const VECTOR& center, const VECTOR& halfExt, int Nx, int Ny, int Nz, std::vector<uint8_t>& density)
{
    VECTOR minW = VGet(center.x - halfExt.x, center.y - halfExt.y, center.z - halfExt.z);
    float r = cell * 0.55f;
    for (int z = 0; z < Nz; ++z)
        for (int y = 0; y < Ny; ++y)
            for (int x = 0; x < Nx; ++x) {
                VECTOR pc = VGet(minW.x + (x + 0.5f) * cell,
                    minW.y + (y + 0.5f) * cell,
                    minW.z + (z + 0.5f) * cell);
                bool hit = false;
                auto res = MV1CollCheck_Sphere(mv1, -1, pc, r);
                hit = (res.HitNum > 0);
                MV1CollResultPolyDimTerminate(res);
                if (hit) { density[Idx(x, y, z, Nx, Ny)] = 200; }
            }
}


void VoxelBase::BuildCubicMesh(const std::vector<uint8_t>& density, int Nx, int Ny, int Nz, float cell, const VECTOR& center, std::vector<MeshBatch>& batches)
{
    static const size_t kMaxVerts = 65000;

    // 近傍サンプル（境界は0でOK）
    auto S = [&](int x, int y, int z)->int {
        if (x < 0 || y < 0 || z < 0 || x >= Nx || y >= Ny || z >= Nz) return 0;
        return density[(z * Ny + y) * Nx + x];
    };

    // 中心差分の勾配 → 正規化
    auto Grad = [&](int x, int y, int z)->VECTOR {

        float gx = float(S(x + 1, y, z) - S(x - 1, y, z));
        float gy = float(S(x, y + 1, z) - S(x, y - 1, z));
        float gz = float(S(x, y, z + 1) - S(x, y, z - 1));

        // ★外向き法線にしたいので符号を反転（密度は外0→中255なので勾配は内向き）
        gx = -gx; gy = -gy; gz = -gz;
        float len2 = gx * gx + gy * gy + gz * gz;
        if (len2 < 1e-6f) return VECTOR{ 0,0,0 };
        float inv = 1.0f / std::sqrt(len2);
        return VECTOR{ gx * inv, gy * inv, gz * inv };
    };

    batches.clear();

    auto Idx = [&](int x, int y, int z) { return (z * Ny + y) * Nx + x; };
    auto Inb = [&](int x, int y, int z) { return 0 <= x && x < Nx && 0 <= y && y < Ny && 0 <= z && z < Nz; };

    MeshBatch cur;
    auto updAabb = [&](const VECTOR& p) {
        cur.bmin.x = (std::min)(cur.bmin.x, p.x);
        cur.bmin.y = (std::min)(cur.bmin.y, p.y);
        cur.bmin.z = (std::min)(cur.bmin.z, p.z);
        cur.bmax.x = (std::max)(cur.bmax.x, p.x);
        cur.bmax.y = (std::max)(cur.bmax.y, p.y);
        cur.bmax.z = (std::max)(cur.bmax.z, p.z);
    };

    auto flush = [&]() {
        if (!cur.v.empty()) {
            cur.centerLocal = {
                (cur.bmin.x + cur.bmax.x) * 0.5f,
                (cur.bmin.y + cur.bmax.y) * 0.5f,
                (cur.bmin.z + cur.bmax.z) * 0.5f
            };
            batches.push_back(std::move(cur));
            cur = MeshBatch{};
        }
    };


    // 6面(+X,-X,+Y,-Y,+Z,-Z)
    const int OFF[6][3] = { {1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1} };

    auto addFace = [&](int x, int y, int z, int f) {
        if (cur.v.size() + 4 > kMaxVerts) flush();

        VECTOR nrm = { (float)OFF[f][0], (float)OFF[f][1], (float)OFF[f][2] };

        // セル中心と面中心
        VECTOR cellC = { (x - Nx / 2) * cell + center.x,
                         (y - Ny / 2) * cell + center.y,
                         (z - Nz / 2) * cell + center.z };

        VECTOR faceC = { cellC.x + nrm.x * cell * 0.5f,
                         cellC.y + nrm.y * cell * 0.5f,
                         cellC.z + nrm.z * cell * 0.5f };

        // 面の2軸（UV用）
        VECTOR t1, t2;
        if (fabsf(nrm.x) > 0.5f) { t1 = { 0,1,0 }; t2 = { 0,0,1 }; }        // X面 → (Y,Z)
        else if (fabsf(nrm.y) > 0.5f) { t1 = { 1,0,0 }; t2 = { 0,0,1 }; }   // Y面 → (X,Z)
        else { t1 = { 1,0,0 }; t2 = { 0,1,0 }; }                            // Z面 → (X,Y)

        float hs = cell * 0.5f;


        auto makeV = [&](float a, float b)->VERTEX3D {
            VERTEX3D v{};
            v.pos = { faceC.x + t1.x * a + t2.x * b,
                       faceC.y + t1.y * a + t2.y * b,
                       faceC.z + t1.z * a + t2.z * b };
            v.norm = nrm;
            v.dif = GetColorU8(255, 255, 255, 255);
            v.spc = GetColorU8(0, 0, 0, 0);
            // 簡易ワールド座標タイルUV（1タイル≒セル2個）
            float inv = 1.0f / (cell * 2.0f);
            if (fabsf(nrm.x) > 0.5f) { v.u = (v.pos.z - center.z) * inv; v.v = (v.pos.y - center.y) * inv; }
            else if (fabsf(nrm.y) > 0.5f) { v.u = (v.pos.x - center.x) * inv; v.v = (v.pos.z - center.z) * inv; }
            else { v.u = (v.pos.x - center.x) * inv; v.v = (v.pos.y - center.y) * inv; }
            return v;
        };

        // ここで勾配法線を取得（取れなければ面法線にフォールバック
        VECTOR g = Grad(x, y, z);
        if (g.x == 0 && g.y == 0 && g.z == 0) g = nrm;

        VERTEX3D q0 = makeV(-hs, -hs);
        VERTEX3D q1 = makeV(+hs, -hs);
        VERTEX3D q2 = makeV(+hs, +hs);
        VERTEX3D q3 = makeV(-hs, +hs);

        // 法線だけ差し替え（まずは4頂点すべて同じ g でOK
        q0.norm = g; q1.norm = g; q2.norm = g; q3.norm = g;

        updAabb(q0.pos); updAabb(q1.pos); updAabb(q2.pos); updAabb(q3.pos);

        unsigned short base = (unsigned short)cur.v.size();
        cur.v.push_back(q0); cur.v.push_back(q1); cur.v.push_back(q2); cur.v.push_back(q3);

        cur.i.push_back(base + 0); cur.i.push_back(base + 2); cur.i.push_back(base + 1);
        cur.i.push_back(base + 0); cur.i.push_back(base + 3); cur.i.push_back(base + 2);

        // ★ 方向別のインデックスへ追加（iDir[f] のみに入れる）
        auto& I = cur.iDir[f];
        I.push_back(base + 0); I.push_back(base + 1); I.push_back(base + 2);
        I.push_back(base + 0); I.push_back(base + 2); I.push_back(base + 3);

    };

    for (int z = 0; z < Nz; ++z) {
        for (int y = 0; y < Ny; ++y) {
            for (int x = 0; x < Nx; ++x) {
                if (density[Idx(x, y, z)] == 0) continue;
                for (int f = 0; f < 6; ++f) {
                    int nx = x + OFF[f][0], ny = y + OFF[f][1], nz = z + OFF[f][2];
                    bool solidN = Inb(nx, ny, nz) ? (density[Idx(nx, ny, nz)] > 0) : false;
                    if (!solidN) addFace(x, y, z, f);
                }
            }

        }
    }
    // 残りを投入
    flush();
}

bool VoxelBase::BuildVoxelMeshFromMV1Handle(int mv1, float cell, const VECTOR& center, const VECTOR& halfExt, std::vector<MeshBatch>& batches)
{
    // 1) グリッド解像度
    int Nx = std::clamp((int)std::ceil((halfExt.x * 2) / cell) + 2, 16, 256);
    int Ny = std::clamp((int)std::ceil((halfExt.y * 2) / cell) + 2, 16, 256);
    int Nz = std::clamp((int)std::ceil((halfExt.z * 2) / cell) + 2, 16, 256);

    // 2) 表面マーキング
    std::vector<uint8_t> density(Nx * Ny * Nz, 0);
    MarkSurfaceByCollisionProbe(mv1, cell, center, halfExt, Nx, Ny, Nz, density);

    // 3) 内部充填
    SolidFill(density, Nx, Ny, Nz);

    // 4) グリッドを保持
    Nx_ = Nx; Ny_ = Ny; Nz_ = Nz;
    cell_ = cell;
    gridCenter_ = center;
    density_ = density;

    // 5) メッシュ化（Cubic）
    BuildCubicMesh(density, Nx, Ny, Nz, cell, center, batches);
    return !(batches.empty());
}

void VoxelBase::ApplyBrush(const Base& other, uint8_t amount)
{
    if (amount == 0) { return; }
    if (density_.empty() || Nx_ == 0) { return; }

    switch (other.para_.colliShape)
    {
    case CollisionShape::NON: { break; }
    case CollisionShape::SPHERE: { ApplyBrushSphere(other, amount); break; }
    case CollisionShape::AABB: { ApplyBrushAABB(other, amount); break; }
    case CollisionShape::CAPSULE: { ApplyBrushCapsule(other, amount); break; }
    }
}

void VoxelBase::ApplyBrushSphere(const Base& other, uint8_t amount)
{
    // ★ローカル→ワールド変換したグリッド中心
    VECTOR centerW = VAdd(unit_.pos_, gridCenter_);

    VECTOR C = VAdd(other.pos_, other.para_.center); // 球の中心（ワールド）
    float  R = other.para_.radius;

    int cx = (int)std::round((C.x - centerW.x) / cell_) + Nx_ / 2;
    int cy = (int)std::round((C.y - centerW.y) / cell_) + Ny_ / 2;
    int cz = (int)std::round((C.z - centerW.z) / cell_) + Nz_ / 2;
    int rr = (int)std::ceil(R / cell_);

    auto idx = [&](int x, int y, int z)->int { return (z * Ny_ + y) * Nx_ + x; };

    float RR = R * R;

    for (int z = cz - rr; z <= cz + rr; ++z)
        for (int y = cy - rr; y <= cy + rr; ++y)
            for (int x = cx - rr; x <= cx + rr; ++x) {
                if (x < 0 || y < 0 || z < 0 || x >= Nx_ || y >= Ny_ || z >= Nz_) continue;

                // セル中心（ワールド）
                VECTOR wp = {
                    (x - Nx_ / 2) * cell_ + centerW.x,
                    (y - Ny_ / 2) * cell_ + centerW.y,
                    (z - Nz_ / 2) * cell_ + centerW.z
                };
                float dx = wp.x - C.x, dy = wp.y - C.y, dz = wp.z - C.z;
                if (dx * dx + dy * dy + dz * dz > RR) continue;

                if (density_[idx(x, y, z)] > 0) {
                    density_[idx(x, y, z)] = 0;
                    regeneration_ = true; // Update() でリメッシュ
                }
            }
}

void VoxelBase::ApplyBrushAABB(const Base& other, uint8_t amount)
{

    // 相手のワールドAABB（中心＋サイズ前提）
    const VECTOR otherCenterW = VAdd(other.pos_, other.para_.center);
    const VECTOR halfSize = VScale(other.para_.size, 0.5f);
    const VECTOR posMin = VSub(otherCenterW, halfSize);
    const VECTOR posMax = VAdd(otherCenterW, halfSize);

    // ★このボクセルオブジェクトの“ワールド中心”
    const VECTOR gridCenterW = VAdd(unit_.pos_, unit_.para_.center);

    // AABB→セル範囲（このボクセル座標系で）
    auto toIdxMin = [&](float w, float c, int N) { return (int)std::floor((w - c) / cell_) + N / 2; };
    auto toIdxMax = [&](float w, float c, int N) { return (int)std::floor((w - c) / cell_) + N / 2; };

    int ix0 = toIdxMin(posMin.x, gridCenterW.x, Nx_);
    int iy0 = toIdxMin(posMin.y, gridCenterW.y, Ny_);
    int iz0 = toIdxMin(posMin.z, gridCenterW.z, Nz_);
    int ix1 = toIdxMax(posMax.x, gridCenterW.x, Nx_);
    int iy1 = toIdxMax(posMax.y, gridCenterW.y, Ny_);
    int iz1 = toIdxMax(posMax.z, gridCenterW.z, Nz_);

    // クランプ＆早期終了
    ix0 = (std::max)(0, ix0); iy0 = (std::max)(0, iy0); iz0 = (std::max)(0, iz0);
    ix1 = (std::min)(Nx_ - 1, ix1); iy1 = (std::min)(Ny_ - 1, iy1); iz1 = (std::min)(Nz_ - 1, iz1);
    if (ix0 > ix1 || iy0 > iy1 || iz0 > iz1) return; // そもそも交差なし

    bool touched = false;

    for (int z = iz0; z <= iz1; ++z)
        for (int y = iy0; y <= iy1; ++y)
            for (int x = ix0; x <= ix1; ++x) {
                // ★セル中心のローカル座標 → ワールド座標（毎回“新しく”計算）
                const VECTOR lp = {
                    (x - Nx_ / 2) * cell_,
                    (y - Ny_ / 2) * cell_,
                    (z - Nz_ / 2) * cell_
                };
                const VECTOR wp = VAdd(gridCenterW, lp);

                // AABB 内判定
                if (wp.x < posMin.x || wp.x > posMax.x) continue;
                if (wp.y < posMin.y || wp.y > posMax.y) continue;
                if (wp.z < posMin.z || wp.z > posMax.z) continue;

                auto& v = density_[Idx(x, y, z)];
                uint8_t nv = (v > amount) ? (uint8_t)(v - amount) : 0;
                if (nv != v) { v = nv; touched = true; }
            }

    if (touched) regeneration_ = true;
}

void VoxelBase::ApplyBrushCapsule(const Base& other, uint8_t amount)
{
}



bool VoxelBase::ResolveAABBGrid(VECTOR& pos, const VECTOR& half, VECTOR& vel, float dt, bool& grounded)
{
    grounded = false;
    if (Nx_ <= 0 || Ny_ <= 0 || Nz_ <= 0) return false;

    const float EPS = 1e-4f;
    const VECTOR gridCenterW = VAdd(unit_.pos_, unit_.para_.center); // このボクセルの“世界中心”
    auto cellMinX = [&](int ix) { return gridCenterW.x + (ix - Nx_ / 2) * cell_; };
    auto cellMinY = [&](int iy) { return gridCenterW.y + (iy - Ny_ / 2) * cell_; };
    auto cellMinZ = [&](int iz) { return gridCenterW.z + (iz - Nz_ / 2) * cell_; };

    auto solidAt = [&](int x, int y, int z)->bool {
        if (x < 0 || y < 0 || z < 0 || x >= Nx_ || y >= Ny_ || z >= Nz_) return false;
        return density_[Idx(x, y, z, Nx_, Ny_)] > 0;
        };

    // その軸で“先頭スラブ”だけ調べるヘルパ
    auto sweepAxis = [&](int axis, float& delta)->bool {
        if (delta == 0.0f) return false;

        // 位置/半サイズ
        float minX = pos.x - half.x, maxX = pos.x + half.x;
        float minY = pos.y - half.y, maxY = pos.y + half.y;
        float minZ = pos.z - half.z, maxZ = pos.z + half.z;

        // いまのスラブ範囲（2軸ぶん）
        int ix0 = (int)std::floor((minX - gridCenterW.x) / cell_) + Nx_ / 2;
        int ix1 = (int)std::floor((maxX - gridCenterW.x) / cell_) + Nx_ / 2;
        int iy0 = (int)std::floor((minY - gridCenterW.y) / cell_) + Ny_ / 2;
        int iy1 = (int)std::floor((maxY - gridCenterW.y) / cell_) + Ny_ / 2;
        int iz0 = (int)std::floor((minZ - gridCenterW.z) / cell_) + Nz_ / 2;
        int iz1 = (int)std::floor((maxZ - gridCenterW.z) / cell_) + Nz_ / 2;

        // 進行方向の“新しい境界インデックス”
        int step = (delta > 0.0f) ? +1 : -1;

        // 軸ごとに、越えるセル境界を1つずつチェック
        if (axis == 0) { // X
            int iOld = (int)std::floor(((delta > 0 ? maxX : minX) - gridCenterW.x) / cell_) + Nx_ / 2;
            int iNew = (int)std::floor(((delta > 0 ? (maxX + delta) : (minX + delta)) - gridCenterW.x) / cell_) + Nx_ / 2;

            if (delta > 0.0f) {
                for (int ix = iOld + 1; ix <= iNew; ++ix) {
                    int y0 = Utility::Clamp(iy0, 0, Ny_ - 1), y1 = Utility::Clamp(iy1, 0, Ny_ - 1);
                    int z0 = Utility::Clamp(iz0, 0, Nz_ - 1), z1 = Utility::Clamp(iz1, 0, Nz_ - 1);
                    bool hit = false;
                    for (int iy = y0; iy <= y1 && !hit; ++iy)
                        for (int iz = z0; iz <= z1 && !hit; ++iz) {
                            if (solidAt(ix, iy, iz)) hit = true;
                        }
                    if (hit) {
                        // ぶつかったセルの“手前の面”でクランプ
                        float face = cellMinX(ix); // そのセルの最小面(左側)
                        pos.x = face - half.x - EPS;
                        vel.x = 0;
                        delta = 0;
                        return true;
                    }
                }
            }
            else { // delta < 0
                for (int ix = iOld - 1; ix >= iNew; --ix) {
                    int y0 = Utility::Clamp(iy0, 0, Ny_ - 1), y1 = Utility::Clamp(iy1, 0, Ny_ - 1);
                    int z0 = Utility::Clamp(iz0, 0, Nz_ - 1), z1 = Utility::Clamp(iz1, 0, Nz_ - 1);
                    bool hit = false;
                    for (int iy = y0; iy <= y1 && !hit; ++iy)
                        for (int iz = z0; iz <= z1 && !hit; ++iz) {
                            if (solidAt(ix, iy, iz)) hit = true;
                        }
                    if (hit) {
                        // ぶつかったセルの“奥の面”(右側)でクランプ
                        float face = cellMinX(ix) + cell_; // そのセルの最大面
                        pos.x = face + half.x + EPS;
                        vel.x = 0;
                        delta = 0;
                        return true;
                    }
                }
            }
            // 何も無ければ移動
            pos.x += delta; delta = 0; return false;
        }
        else if (axis == 1) { // Y
            int iOld = (int)std::floor(((delta > 0 ? maxY : minY) - gridCenterW.y) / cell_) + Ny_ / 2;
            int iNew = (int)std::floor(((delta > 0 ? (maxY + delta) : (minY + delta)) - gridCenterW.y) / cell_) + Ny_ / 2;

            if (delta > 0.0f) {
                for (int iy = iOld + 1; iy <= iNew; ++iy) {
                    int x0 = Utility::Clamp(ix0, 0, Nx_ - 1), x1 = Utility::Clamp(ix1, 0, Nx_ - 1);
                    int z0 = Utility::Clamp(iz0, 0, Nz_ - 1), z1 = Utility::Clamp(iz1, 0, Nz_ - 1);
                    bool hit = false;
                    for (int ix = x0; ix <= x1 && !hit; ++ix)
                        for (int iz = z0; iz <= z1 && !hit; ++iz) {
                            if (solidAt(ix, iy, iz)) hit = true;
                        }
                    if (hit) {
                        float face = cellMinY(iy);
                        pos.y = face - half.y - EPS;
                        vel.y = 0;
                        delta = 0;
                        return true;
                    }
                }
            }
            else { // delta < 0（落下）
                for (int iy = iOld - 1; iy >= iNew; --iy) {
                    int x0 = Utility::Clamp(ix0, 0, Nx_ - 1), x1 = Utility::Clamp(ix1, 0, Nx_ - 1);
                    int z0 = Utility::Clamp(iz0, 0, Nz_ - 1), z1 = Utility::Clamp(iz1, 0, Nz_ - 1);
                    bool hit = false;
                    for (int ix = x0; ix <= x1 && !hit; ++ix)
                        for (int iz = z0; iz <= z1 && !hit; ++iz) {
                            if (solidAt(ix, iy, iz)) hit = true;
                        }
                    if (hit) {
                        float face = cellMinY(iy) + cell_;
                        pos.y = face + half.y + EPS;
                        grounded = true; // ★接地
                        if (vel.y < 0) vel.y = 0;
                        delta = 0;
                        return true;
                    }
                }
            }
            pos.y += delta; delta = 0; return false;
        }
        else { // Z
            int iOld = (int)std::floor(((delta > 0 ? maxZ : minZ) - gridCenterW.z) / cell_) + Nz_ / 2;
            int iNew = (int)std::floor(((delta > 0 ? (maxZ + delta) : (minZ + delta)) - gridCenterW.z) / cell_) + Nz_ / 2;

            if (delta > 0.0f) {
                for (int iz = iOld + 1; iz <= iNew; ++iz) {
                    int x0 = Utility::Clamp(ix0, 0, Nx_ - 1), x1 = Utility::Clamp(ix1, 0, Nx_ - 1);
                    int y0 = Utility::Clamp(iy0, 0, Ny_ - 1), y1 = Utility::Clamp(iy1, 0, Ny_ - 1);
                    bool hit = false;
                    for (int ix = x0; ix <= x1 && !hit; ++ix)
                        for (int iy = y0; iy <= y1 && !hit; ++iy) {
                            if (solidAt(ix, iy, iz)) hit = true;
                        }
                    if (hit) {
                        float face = cellMinZ(iz);
                        pos.z = face - half.z - EPS;
                        vel.z = 0;
                        delta = 0;
                        return true;
                    }
                }
            }
            else {
                for (int iz = iOld - 1; iz >= iNew; --iz) {
                    int x0 = Utility::Clamp(ix0, 0, Nx_ - 1), x1 = Utility::Clamp(ix1, 0, Nx_ - 1);
                    int y0 = Utility::Clamp(iy0, 0, Ny_ - 1), y1 = Utility::Clamp(iy1, 0, Ny_ - 1);
                    bool hit = false;
                    for (int ix = x0; ix <= x1 && !hit; ++ix)
                        for (int iy = y0; iy <= y1 && !hit; ++iy) {
                            if (solidAt(ix, iy, iz)) hit = true;
                        }
                    if (hit) {
                        float face = cellMinZ(iz) + cell_;
                        pos.z = face + half.z + EPS;
                        vel.z = 0;
                        delta = 0;
                        return true;
                    }
                }
            }
            pos.z += delta; delta = 0; return false;
        }
        };

    // 予定移動量
    float dx = vel.x * dt;
    float dy = vel.y * dt;
    float dz = vel.z * dt;

    // 推奨順序：Y→X→Z（重力→水平の自然な解決）
    sweepAxis(1, dy);          // Y
    sweepAxis(0, dx);          // X
    sweepAxis(2, dz);          // Z

    return true;
}