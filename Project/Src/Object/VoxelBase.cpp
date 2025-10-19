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
    densityInit_(),
    regeneration_(false),

    aliveNeedRatio_(0.0f),
    cellCenterPoss_()
{
}


void VoxelBase::Load(void)
{
    SubLoad();

    if (unit_.model_ != -1) {
        const VECTOR worldCenterForImport =
            VAdd(unit_.WorldPos(), gridCenter_);

        BuildVoxelMeshFromMV1Handle(
            unit_.model_,
            cell_,
            worldCenterForImport,
            VScale(unit_.para_.size, 0.5f),
            batches_
        );
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
        BuildGreedyMesh(density_, Nx_, Ny_, Nz_, cell_, unit_.WorldPos(), batches_);

        regeneration_ = false;
    }
}


void VoxelBase::Draw(void)
{
    if (!unit_.isAlive_) return;

    SubDraw();

    MATRIX M = MGetTranslate(unit_.WorldPos());
    SetTransformToWorld(&M);

    for (auto& b : batches_) {
        if (b.i.empty()) { continue; }
        DrawPolygonIndexed3D(
            b.v.empty() ? b.v.data() : b.v.data(),
            (int)(b.v.empty() ? b.v.size() : b.v.size()),
            b.i.data(), (int)(b.i.size() / 3),
            textureId_, true
        );
    }

    M = MGetIdent();
    SetTransformToWorld(&M);
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



#pragma region メッシュ生成
bool VoxelBase::BuildVoxelMeshFromMV1Handle(int mv1, float cell, const VECTOR& center, const VECTOR& halfExt, std::vector<MeshBatch>& batches)
{
    // 1) グリッド解像度
    Nx_ = (int)std::ceil((halfExt.x * 2) / cell);
    Ny_ = (int)std::ceil((halfExt.y * 2) / cell);
    Nz_ = (int)std::ceil((halfExt.z * 2) / cell);

    // 2) 表面マーキング
	density_.resize(Nx_ * Ny_ * Nz_, 0);
    MarkSurfaceByCollisionProbe(mv1, cell, center, halfExt, Nx_, Ny_, Nz_, density_);

    // 3) 内部充填
    SolidFill(density_, Nx_, Ny_, Nz_);

    // 4) メッシュ化
    BuildGreedyMesh(density_, Nx_, Ny_, Nz_, cell_, center, batches);
    densityInit_ = density_;
    return !(batches.empty());
}

void VoxelBase::MarkSurfaceByCollisionProbe(int mv1, float cell, const VECTOR& center, const VECTOR& halfExt, int Nx, int Ny, int Nz, std::vector<uint8_t>& density)
{
    MV1SetupCollInfo(mv1, -1);
    VECTOR minW = VSub(center, halfExt);
    float r = cell * 0.5f;
    for (int z = 0; z < Nz; ++z)
        for (int y = 0; y < Ny; ++y)
            for (int x = 0; x < Nx; ++x) {
                VECTOR pc = VGet(
                    minW.x + (x * cell) + (cell * 0.5f),
                    minW.y + (y * cell) + (cell * 0.5f),
                    minW.z + (z * cell) + (cell * 0.5f));
                auto res = MV1CollCheck_Sphere(mv1, -1, pc, r);
                if ((res.HitNum > 0)) { density[Idx(x, y, z, Nx, Ny)] = 200; }
                MV1CollResultPolyDimTerminate(res);
            }
}

void VoxelBase::SolidFill(std::vector<uint8_t>& density, int Nx, int Ny, int Nz)
{
    const int total = Nx * Ny * Nz;
    std::vector<bool> ext(total, false);
    std::queue<int> q;

    auto pushIf = [&](int x, int y, int z) {
        if (!Inb(x, y, z, Nx, Ny, Nz)) { return; }
        int i = Idx(x, y, z, Nx, Ny);
        if (ext[i] || density[i] != 0) { return; }
        ext[i] = true;
        q.push(i);
    };

	// 外部空気領域を BFS でマーク
    for (int x = 0; x < Nx; ++x) { for (int y = 0; y < Ny; ++y) { pushIf(x, y, 0); pushIf(x, y, Nz - 1); } }
    for (int x = 0; x < Nx; ++x) { for (int z = 0; z < Nz; ++z) { pushIf(x, 0, z); pushIf(x, Ny - 1, z); } }
    for (int y = 0; y < Ny; ++y) { for (int z = 0; z < Nz; ++z) { pushIf(0, y, z); pushIf(Nx - 1, y, z); } }

	// 6方向へ展開
    while (!q.empty()) {
        int i = q.front(); q.pop();

        int z = i / (Nx * Ny);
        int y = (i - z * Nx * Ny) / Nx;
        int x = i % Nx;
        for (int k = 0; k < 6; ++k) { pushIf(x + kDirNrm[k].x, y + kDirNrm[k].y, z + kDirNrm[k].z); }
    }

    // まずカウント（上書き前に数える）
    for (int i = 0; i < total; ++i) {
        if (density[i] == 200) { ++marked; }    // 表面フラグ
        if (!ext[i]) { ++solid; }               // 内部領域
    }

    // 外は0のまま、内部と表面を255へ
    for (int i = 0; i < total; ++i) {
        if (!ext[i]) { density[i] = 255; }                  // 内部
        else if (density[i] == 200) { density[i] = 255; }   // 外部側の表面
    }
}

void VoxelBase::BuildGreedyMesh(
    const std::vector<uint8_t>& density, int Nx, int Ny, int Nz,
    float cell, const VECTOR& center,
    std::vector<MeshBatch>& batches)
{
    auto Solid = [&](int x, int y, int z)->int {
        if (!Inb(x, y, z)) return 0;
        return density[Idx(x, y, z)] > 0 ? 1 : 0;
        };

    // 出力先クリア
    batches.clear();

    // 現在のバッチ と AABB 初期化
    MeshBatch cur;
    const float INF = 1e30f;
    cur.bmin = { +INF,+INF,+INF };
    cur.bmax = { -INF,-INF,-INF };

	// 最小座標・最大座標 更新ラムダ
    auto updAabb = [&](const VECTOR& p) {
        cur.bmin.x = (std::min)(cur.bmin.x, p.x);
        cur.bmin.y = (std::min)(cur.bmin.y, p.y);
        cur.bmin.z = (std::min)(cur.bmin.z, p.z);
        cur.bmax.x = (std::max)(cur.bmax.x, p.x);
        cur.bmax.y = (std::max)(cur.bmax.y, p.y);
        cur.bmax.z = (std::max)(cur.bmax.z, p.z);
        };

    // 65k 頂点制限に合わせてフラッシュ
    static const size_t kMaxVerts = 65000;
    auto flush = [&]() {
        if (!cur.v.empty()) {
            cur.centerLocal = {
                (cur.bmin.x + cur.bmax.x) * 0.5f,
                (cur.bmin.y + cur.bmax.y) * 0.5f,
                (cur.bmin.z + cur.bmax.z) * 0.5f
            };
            batches.push_back(std::move(cur));
            cur = MeshBatch{};
            cur.bmin = { +INF,+INF,+INF };
            cur.bmax = { -INF,-INF,-INF };
        }
        };

    // ---- Greedy Meshing 本体 ----
    for (int d = 0; d < 3; d++) {
        const int u = (d + 1) % 3;
        const int v = (d + 2) % 3;
        int dims[3] = { Nx,Ny,Nz };
        const int Du = dims[u];
        const int Dv = dims[v];
        const int Dw = dims[d];

        struct Mask { unsigned char on; unsigned char back; };
        std::vector<Mask> mask(Du * Dv);

        for (int w = 0; w <= Dw; w++) {
            // 1) スライス差分 → “面が立つ”マスク
            for (int j = 0; j < Dv; ++j)
                for (int i = 0; i < Du; ++i) {
                    int L[3] = { 0,0,0 }, R[3] = { 0,0,0 };
                    L[d] = w - 1; R[d] = w; L[u] = R[u] = i; L[v] = R[v] = j;
                    int aL = (w > 0) ? Solid(L[0], L[1], L[2]) : 0;
                    int aR = (w < Dw) ? Solid(R[0], R[1], R[2]) : 0;
                    Mask m{ 0,0 };
                    if (aL != aR) { m.on = 1; m.back = (aL == 1); }
                    mask[j * Du + i] = m;
                }

            // 2) マスクを長方形にまとめて追加
            int i = 0, j = 0;
            while (j < Dv) {
                while (i < Du) {
                    Mask m0 = mask[j * Du + i];
                    if (!m0.on) { ++i; continue; }

                    // 横
                    int wlen = 1;
                    while (i + wlen < Du) {
                        Mask t = mask[j * Du + (i + wlen)];
                        if (t.on != m0.on || t.back != m0.back) break;
                        ++wlen;
                    }
                    // 縦
                    int hlen = 1; bool grow = true;
                    while (j + hlen < Dv && grow) {
                        for (int k = 0; k < wlen; ++k) {
                            Mask t = mask[(j + hlen) * Du + (i + k)];
                            if (t.on != m0.on || t.back != m0.back) { grow = false; break; }
                        }
                        if (grow) ++hlen;
                    }

                    // 頂点数が溢れそうなら一旦フラッシュ
                    if (cur.v.size() + 4 > kMaxVerts) flush();

                    // 面の4頂点（ローカル原点=中心＋center）
                    auto facePos = [&](int I, int J)->VECTOR {
                        int XYZ[3] = { 0,0,0 };
                        XYZ[d] = m0.back ? (w - 1) : w; // セル境界
                        XYZ[u] = I;
                        XYZ[v] = J;
                        float xf = (XYZ[0] - Nx * 0.5f) * cell /*+ center.x*/;
                        float yf = (XYZ[1] - Ny * 0.5f) * cell /*+ center.y*/;
                        float zf = (XYZ[2] - Nz * 0.5f) * cell /*+ center.z*/;
                        const float half = cell * 0.5f;
                        if (d == 0) xf += (m0.back ? +half : -half);
                        if (d == 1) yf += (m0.back ? +half : -half);
                        if (d == 2) zf += (m0.back ? +half : -half);
                        return VGet(xf, yf, zf);
                        };

                    VECTOR p00 = facePos(i, j);
                    VECTOR p10 = facePos(i + wlen, j);
                    VECTOR p11 = facePos(i + wlen, j + hlen);
                    VECTOR p01 = facePos(i, j + hlen);

                    VECTOR nrm{ 0,0,0 }; (&nrm.x)[d] = (m0.back ? -1.0f : +1.0f);

                    // UV は “セル数ぶんタイル”
                    float u0 = 0, v0 = 0, u1 = (float)wlen, v1 = (float)hlen;

                    auto makeV = [&](const VECTOR& P, float uu, float vv)->VERTEX3D {
                        VERTEX3D v{};
                        v.pos = P; v.norm = nrm;
                        v.dif = GetColorU8(255, 255, 255, 255);
                        v.spc = GetColorU8(0, 0, 0, 0);
                        v.u = uu; v.v = vv;
                        return v;
                        };

                    unsigned short base = (unsigned short)cur.v.size();
                    if (!m0.back) {
                        cur.v.push_back(makeV(p00, u0, v0));
                        cur.v.push_back(makeV(p10, u1, v0));
                        cur.v.push_back(makeV(p11, u1, v1));
                        cur.v.push_back(makeV(p01, u0, v1));
                        cur.i.push_back(base + 0); cur.i.push_back(base + 1); cur.i.push_back(base + 2);
                        cur.i.push_back(base + 0); cur.i.push_back(base + 2); cur.i.push_back(base + 3);
                    }
                    else {
                        cur.v.push_back(makeV(p00, u0, v0));
                        cur.v.push_back(makeV(p01, u0, v1));
                        cur.v.push_back(makeV(p11, u1, v1));
                        cur.v.push_back(makeV(p10, u1, v0));
                        cur.i.push_back(base + 0); cur.i.push_back(base + 1); cur.i.push_back(base + 2);
                        cur.i.push_back(base + 0); cur.i.push_back(base + 2); cur.i.push_back(base + 3);
                    }

                    updAabb(cur.v[base + 0].pos);
                    updAabb(cur.v[base + 1].pos);
                    updAabb(cur.v[base + 2].pos);
                    updAabb(cur.v[base + 3].pos);

                    // 使い切り
                    for (int y = 0; y < hlen; ++y)
                        for (int x = 0; x < wlen; ++x)
                            mask[(j + y) * Du + (i + x)] = { 0,0 };

                    i += wlen;
                }
                i = 0; ++j;
            }
        }
    }

    flush();
}
#pragma endregion


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

    return false;
}

bool VoxelBase::ApplyBrushSphere(const Base& other, uint8_t amount)
{
    // ★ローカル→ワールド変換したグリッド中心
    VECTOR centerW = VAdd(unit_.pos_, unit_.para_.center);

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
    if (unit_.isAlive_ == false) { return false; }

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

bool VoxelBase::ResolveCapsuleCenter(
    VECTOR& center, float R, float halfH,
    VECTOR& vel, bool& grounded, float slopeLimitDeg, int maxIter)
{
    // 既存の foot 版に変換して委譲するだけ：
    // foot は「カプセル最下点」= 中心 - (R + halfH) * +Y
    const VECTOR off = VGet(0.0f, R + halfH, 0.0f);
    VECTOR foot = VSub(center, off);

    // 既存の足元版に丸投げ
    const bool hit = ResolveCapsule(foot, R, halfH, vel, grounded, slopeLimitDeg, maxIter);

    // foot が動いた（or ヒットした）なら center を戻す
    const VECTOR newCenter = VAdd(foot, off);

    // 微小誤差を避けるなら閾値で判定
    const VECTOR d = VSub(newCenter, center);
    const float  d2 = d.x * d.x + d.y * d.y + d.z * d.z;
    if (hit || d2 > 1e-12f) {
        center = newCenter;
        return true;
    }
    return false;
}

void VoxelBase::ReVival(void)
{
    density_ = densityInit_;
    regeneration_ = true;
}
