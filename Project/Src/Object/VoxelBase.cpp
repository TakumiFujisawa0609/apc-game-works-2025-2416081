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

    const VECTOR camPos = camera_->GetPos();

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

bool VoxelBase::ApplyBrush(const Base& other, uint8_t amount)
{
    if (amount == 0) { return false; }
    if (density_.empty() || Nx_ == 0) { return false; }

    switch (other.para_.colliShape)
    {
    case CollisionShape::NON: { break; }
    case CollisionShape::SPHERE: { return ApplyBrushSphere(other, amount); }
    case CollisionShape::OBB: { return ApplyBrushAABB(other, amount); }
    case CollisionShape::CAPSULE: { return ApplyBrushCapsule(other, amount); }
    }
}

bool VoxelBase::ApplyBrushSphere(const Base& other, uint8_t amount)
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
                    density_[idx(x, y, z)] = (std::max)(0, density_[idx(x, y, z)] - amount);
                    regeneration_ = true; // Update() でリメッシュ
                }
            }

	return regeneration_;
}

bool VoxelBase::ApplyBrushAABB(const Base& other, uint8_t amount)
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
    if (ix0 > ix1 || iy0 > iy1 || iz0 > iz1) { return false; } // そもそも交差なし

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

    if (touched) { regeneration_ = true; }

	return regeneration_;
}

bool VoxelBase::ApplyBrushCapsule(const Base& other, uint8_t amount)
{
    return regeneration_;
}



// 球 vs AABB の最小押し戻し
static bool SphereVsAABB_MTV(const VECTOR& C, float R, const VECTOR& bmin, const VECTOR& bmax,
    VECTOR& outN, float& outDepth)
{
    VECTOR cp = Utility::Clamp(C, bmin, bmax);
    VECTOR d = VSub(C, cp);
    float L2 = VDot(d, d);
    if (L2 > R * R) return false;

    if (L2 > 1e-12f) {
        float L = std::sqrt(L2);
        outN = VScale(d, 1.0f / L);
        outDepth = R - L;
        return true;
    }
    else {
        float dx = (std::min)(C.x - bmin.x, bmax.x - C.x);
        float dy = (std::min)(C.y - bmin.y, bmax.y - C.y);
        float dz = (std::min)(C.z - bmin.z, bmax.z - C.z);
        if (dx <= dy && dx <= dz) { outN = { (C.x < (bmin.x + bmax.x) * 0.5f) ? -1.f : +1.f, 0, 0 }; outDepth = R + dx; }
        else if (dy <= dz) { outN = { 0, (C.y < (bmin.y + bmax.y) * 0.5f) ? -1.f : +1.f, 0 }; outDepth = R + dy; }
        else { outN = { 0, 0, (C.z < (bmin.z + bmax.z) * 0.5f) ? -1.f : +1.f }; outDepth = R + dz; }
        return true;
    }
}

// 円(XZ) vs 矩形(XZ) の最小押し戻し（カプセル側面用）
static bool CircleXZ_vs_RectXZ_MTV(const VECTOR& C, float R, const VECTOR& bmin, const VECTOR& bmax,
    VECTOR& outN, float& outDepth)
{
    float qx = Utility::Clamp(C.x, bmin.x, bmax.x);
    float qz = Utility::Clamp(C.z, bmin.z, bmax.z);
    float dx = C.x - qx, dz = C.z - qz;
    float L2 = dx * dx + dz * dz;
    if (L2 > R * R) return false;
    if (L2 > 1e-12f) {
        float L = std::sqrt(L2);
        outN = { dx / L, 0.0f, dz / L };
        outDepth = R - L;
        return true;
    }
    else {
        float rx = (std::min)(std::abs(C.x - bmin.x), std::abs(bmax.x - C.x));
        float rz = (std::min)(std::abs(C.z - bmin.z), std::abs(bmax.z - C.z));
        if (rx <= rz) { outN = { (C.x < (bmin.x + bmax.x) * 0.5f) ? -1.f : +1.f, 0, 0 }; outDepth = R + rx; }
        else { outN = { 0, 0, (C.z < (bmin.z + bmax.z) * 0.5f) ? -1.f : +1.f }; outDepth = R + rz; }
        return true;
    }
}

bool VoxelBase::ResolveCapsule(
    VECTOR& footPos, float R, float halfH,
    VECTOR& vel, bool& grounded,
    float slopeLimitDeg, int maxIters)
{
    grounded = false;
    if (Nx_ <= 0 || Ny_ <= 0 || Nz_ <= 0) return false;

    const float EPS = 1e-4f;
    const float cosSlope = std::cos(slopeLimitDeg * (DX_PI_F / 180.0f));

    // ★このボクセルの“世界中心”（描画と同じオフセット）
    const VECTOR gridCenterW = VAdd(unit_.pos_, unit_.para_.center);

    auto cellMinX = [&](int ix) { return gridCenterW.x + (ix - Nx_ / 2) * cell_; };
    auto cellMinY = [&](int iy) { return gridCenterW.y + (iy - Ny_ / 2) * cell_; };
    auto cellMinZ = [&](int iz) { return gridCenterW.z + (iz - Nz_ / 2) * cell_; };

    auto solidAt = [&](int x, int y, int z)->bool {
        if (x < 0 || y < 0 || z < 0 || x >= Nx_ || y >= Ny_ || z >= Nz_) return false;
        return density_[Idx(x, y, z, Nx_, Ny_)] > 0;
        };

    bool any = false;

    for (int it = 0; it < maxIters; ++it) {
        // 足元→カプセル中心
        VECTOR C = VAdd(footPos, VGet(0, R + halfH, 0));
        float halfY = halfH + R;

        // カプセルの外接AABB → セル範囲
        VECTOR bbmin = { C.x - R, C.y - halfY, C.z - R };
        VECTOR bbmax = { C.x + R, C.y + halfY, C.z + R };

        int ix0 = (int)std::floor((bbmin.x - gridCenterW.x) / cell_) + Nx_ / 2;
        int iy0 = (int)std::floor((bbmin.y - gridCenterW.y) / cell_) + Ny_ / 2;
        int iz0 = (int)std::floor((bbmin.z - gridCenterW.z) / cell_) + Nz_ / 2;
        int ix1 = (int)std::floor((bbmax.x - gridCenterW.x) / cell_) + Nx_ / 2;
        int iy1 = (int)std::floor((bbmax.y - gridCenterW.y) / cell_) + Ny_ / 2;
        int iz1 = (int)std::floor((bbmax.z - gridCenterW.z) / cell_) + Nz_ / 2;

        ix0 = (std::max)(0, ix0); iy0 = (std::max)(0, iy0); iz0 = (std::max)(0, iz0);
        ix1 = (std::min)(Nx_ - 1, ix1); iy1 = (std::min)(Ny_ - 1, iy1); iz1 = (std::min)(Nz_ - 1, iz1);

        VECTOR bestN{ 0,0,0 }; float bestDepth = 0.0f; bool hit = false;

        for (int z = iz0; z <= iz1; ++z)
            for (int y = iy0; y <= iy1; ++y)
                for (int x = ix0; x <= ix1; ++x) {
                    if (!solidAt(x, y, z)) continue;

                    VECTOR bmin = { cellMinX(x), cellMinY(y), cellMinZ(z) };
                    VECTOR bmax = { bmin.x + cell_, bmin.y + cell_, bmin.z + cell_ };

                    // 上球・下球
                    VECTOR N1; float D1;
                    if (SphereVsAABB_MTV(VAdd(C, VGet(0, +halfH, 0)), R, bmin, bmax, N1, D1) && D1 > bestDepth) {
                        bestDepth = D1; bestN = N1; hit = true;
                    }
                    VECTOR N2; float D2;
                    if (SphereVsAABB_MTV(VAdd(C, VGet(0, -halfH, 0)), R, bmin, bmax, N2, D2) && D2 > bestDepth) {
                        bestDepth = D2; bestN = N2; hit = true;
                    }

                    // 側面（Yが重なっている時のみ XZの円で当てる）
                    float yOverlap = (std::min)(bmax.y, C.y + halfH) - (std::max)(bmin.y, C.y - halfH);
                    if (yOverlap > 0.0f) {
                        VECTOR N3; float D3;
                        if (CircleXZ_vs_RectXZ_MTV(C, R, bmin, bmax, N3, D3) && D3 > bestDepth) {
                            bestDepth = D3; bestN = N3; hit = true;
                        }
                    }
                }

        if (!hit) break;

        // 押し戻し（中心→足元へ戻す）
        C = VAdd(C, VScale(bestN, bestDepth + EPS));
        footPos = VAdd(C, VGet(0, -(R + halfH), 0));

        // 速度の法線成分を殺す（スライド）
        float vn = VDot(vel, bestN);
        if (vn < 0.0f) vel = VSub(vel, VScale(bestN, vn));

        // 接地判定
        if (bestN.y > cosSlope && vel.y <= 0.0f) {
            grounded = true;
            if (vel.y < 0) vel.y = 0;
        }
        any = true;
    }
    return any;
}

std::vector<VoxelBase::AABB> VoxelBase::GetVoxelAABBs(void) const
{
    std::vector<AABB> ret;
    if (density_.empty() || Nx_ <= 0 || Ny_ <= 0 || Nz_ <= 0) { return ret; }
    ret.reserve(1024);

    const VECTOR gridCenterW = VAdd(unit_.pos_, unit_.para_.center);

    for (int z = 0; z < Nz_; ++z)
        for (int y = 0; y < Ny_; ++y)
            for (int x = 0; x < Nx_; ++x) {
                if (density_[Idx(x, y, z)] == 0) { continue; }

                VECTOR bmin = {
                    gridCenterW.x + (x - Nx_ / 2) * cell_,
                    gridCenterW.y + (y - Ny_ / 2) * cell_,
                    gridCenterW.z + (z - Nz_ / 2) * cell_
                };
                VECTOR bmax = { bmin.x + cell_, bmin.y + cell_, bmin.z + cell_ };

                ret.push_back(AABB{ bmin, bmax });
            }

    return ret;
}