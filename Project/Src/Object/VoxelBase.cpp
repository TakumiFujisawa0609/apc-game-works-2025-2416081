#include "VoxelBase.h"

#include <algorithm>
#include <queue>
#include <cmath>

#include"../Manager/Camera/Camera.h"

#include"../Application/Application.h"

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
    cellCenterPoss_(),

    nowFrameRemesh_(false)
{
}


void VoxelBase::Load(void)
{
	// 派生先の読み込み処理
    SubLoad();

	// モデルに制御情報を適用しておく
    trans_.Attach();

	// ボクセルメッシュ生成（モデルが正常に読み込まれていれば）
    if (trans_.model != -1) {

		// メッシュ生成実行
        BuildVoxelMeshFromMV1Handle(
            trans_.model,
            cell_,
            trans_.WorldPos() + gridCenter_,
            roughSize_,
            batches_
        );

        // モデルはもう使わないので解放
        trans_.Release();
    }

	// ボクセルオブジェクト専用のコライダーを生成
    ColliderCreate(new VoxelCollider(TAG::STAGE, cell_, cellCenterPoss_, (roughSize_ / 2).Length()));
}


void VoxelBase::Init(void)
{
	// フラグ初期化
    SetJudge(true);
    SetIsDraw(true);
    regeneration_ = false;
    nowFrameRemesh_ = false;

    // 派生先の初期化処理
    SubInit();
}


void VoxelBase::Update(void)
{
	// 派生先の更新処理
    SubUpdate();

    // 実質的な生存判定
    if (!GetJudgeFlg()) { return; }

    // フラグリセット（形状変化）
    nowFrameRemesh_ = false;

    if (regeneration_) {
	    // メッシュ再生成処理
        BuildGreedyMesh(density_, Nx_, Ny_, Nz_, cell_, batches_);

		// フラグリセット（メッシュ再生成処理）
        regeneration_ = false;
    }
}


void VoxelBase::Draw(void)
{
	// 派生先の描画処理
    SubDraw();

	// 描画判定
	if (!GetIsDraw()) { return; }

	// 座標を移動して描画
    MATRIX M = MGetTranslate(trans_.WorldPos().ToVECTOR());
    SetTransformToWorld(&M);

	// メッシュ描画
    for (auto& b : batches_) {
        if (b.i.empty()) { continue; }
        DrawPolygonIndexed3D(
            b.v.empty() ? b.v.data() : b.v.data(),
            (int)(b.v.empty() ? b.v.size() : b.v.size()),
            b.i.data(), (int)(b.i.size() / 3),
            (textureId_ != -1) ? textureId_ : textureId_, true
        );
    }

	// 座標を元に戻す
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
bool VoxelBase::BuildVoxelMeshFromMV1Handle(int mv1, float cell, const Vector3& center, const Vector3& roughSize, std::vector<MeshBatch>& batches)
{
    // 1) グリッド解像度
    Nx_ = (int)std::ceil(roughSize.x / cell);
    Ny_ = (int)std::ceil(roughSize.y / cell);
    Nz_ = (int)std::ceil(roughSize.z / cell);

    // 2) 表面マーキング
	density_.resize(Nx_ * Ny_ * Nz_, 0);
    MarkSurfaceByCollisionProbe(mv1, cell, center, roughSize, Nx_, Ny_, Nz_, density_);

    // 3) 内部充填
    SolidFill(density_, Nx_, Ny_, Nz_);

    // 4) メッシュ化
    BuildGreedyMesh(density_, Nx_, Ny_, Nz_, cell_, batches);
    densityInit_ = density_;
    return !(batches.empty());
}

void VoxelBase::MarkSurfaceByCollisionProbe(int mv1, float cell, const Vector3& center, const Vector3& roughSize, int Nx, int Ny, int Nz, std::vector<uint8_t>& density)
{
    MV1SetupCollInfo(mv1, -1);
    Vector3 minW = center - (roughSize / 2);
    float r = cell * 0.5f;
    for (int z = 0; z < Nz; ++z)
        for (int y = 0; y < Ny; ++y)
            for (int x = 0; x < Nx; ++x) {
                Vector3 pc = Vector3(
                    minW.x + (x * cell) + (cell * 0.5f),
                    minW.y + (y * cell) + (cell * 0.5f),
                    minW.z + (z * cell) + (cell * 0.5f));
                auto res = MV1CollCheck_Sphere(mv1, -1, pc.ToVECTOR(), r);
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
    const std::vector<uint8_t>& density,
    int Nx, int Ny, int Nz, float cell,
    std::vector<MeshBatch>& batches)
{
    auto Idx = [&](int x, int y, int z) { return (z * Ny + y) * Nx + x; };
    auto Inb = [&](int x, int y, int z) {
        return (0 <= x && x < Nx && 0 <= y && y < Ny && 0 <= z && z < Nz);
        };
    auto Solid = [&](int x, int y, int z)->int {
        return (Inb(x, y, z) && density[Idx(x, y, z)] > 0) ? 1 : 0;
        };

    batches.clear();

    MeshBatch cur;
    const float INF = 1e30f;
    cur.bmin = { +INF, +INF, +INF };
    cur.bmax = { -INF, -INF, -INF };

    auto updAabb = [&](const Vector3& p) {
        cur.bmin.x = (std::min)(cur.bmin.x, p.x);
        cur.bmin.y = (std::min)(cur.bmin.y, p.y);
        cur.bmin.z = (std::min)(cur.bmin.z, p.z);
        cur.bmax.x = (std::max)(cur.bmax.x, p.x);
        cur.bmax.y = (std::max)(cur.bmax.y, p.y);
        cur.bmax.z = (std::max)(cur.bmax.z, p.z);
        };

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
            cur.bmin = { +INF, +INF, +INF };
            cur.bmax = { -INF, -INF, -INF };
        }
        };

    // d: 法線軸(0:X, 1:Y, 2:Z)
    for (int d = 0; d < 3; ++d) {
        const int u = (d + 1) % 3;
        const int v = (d + 2) % 3;
        int dims[3] = { Nx, Ny, Nz };
        const int Du = dims[u];
        const int Dv = dims[v];
        const int Dw = dims[d];

        struct Mask { unsigned char on; unsigned char fromL; }; // fromL=1: (aL=1,aR=0)
        std::vector<Mask> mask(Du * Dv);

        for (int w = 0; w <= Dw; ++w) {
            // 1) スライス差分で“面が立つ”所を作る
            for (int j = 0; j < Dv; ++j) {
                for (int i = 0; i < Du; ++i) {
                    int L[3] = { 0,0,0 }, R[3] = { 0,0,0 };
                    L[d] = w - 1; R[d] = w;
                    L[u] = R[u] = i;
                    L[v] = R[v] = j;

                    int aL = (w > 0) ? Solid(L[0], L[1], L[2]) : 0;
                    int aR = (w < Dw) ? Solid(R[0], R[1], R[2]) : 0;

                    Mask m{ 0,0 };
                    if (aL != aR) {
                        // 面は立つ。どちらがソリッド側かを記録
                        // aL=1,aR=0 → ソリッドは“左(L側)”。法線は +d 方向へ向く
                        // aL=0,aR=1 → ソリッドは“右(R側)”。法線は -d 方向へ向く
                        m.on = 1;
                        m.fromL = (aL == 1) ? 1 : 0;
                    }
                    mask[j * Du + i] = m;
                }
            }

            // 2) Greedy で長方形にまとめる
            int i = 0, j = 0;
            while (j < Dv) {
                while (i < Du) {
                    Mask m0 = mask[j * Du + i];
                    if (!m0.on) { ++i; continue; }

                    // 横へ拡張
                    int wlen = 1;
                    while (i + wlen < Du) {
                        Mask t = mask[j * Du + (i + wlen)];
                        if (t.on != m0.on || t.fromL != m0.fromL) break;
                        ++wlen;
                    }
                    // 縦へ拡張
                    int hlen = 1; bool grow = true;
                    while (j + hlen < Dv && grow) {
                        for (int k = 0; k < wlen; ++k) {
                            Mask t = mask[(j + hlen) * Du + (i + k)];
                            if (t.on != m0.on || t.fromL != m0.fromL) { grow = false; break; }
                        }
                        if (grow) ++hlen;
                    }

                    if (cur.v.size() + 4 > kMaxVerts) flush();

                    // 面の4頂点（ローカル空間）
                    // nSign: 法線符号 (+1: +d 方向, -1: -d 方向)
                    const int nSign = m0.fromL ? +1 : -1;
                    const float half = cell * 0.5f;

                    // ソリッド側のセルインデックス（面が属する“ソリッド”セル）
                    // fromL(=aL=1) → ソリッドは w-1
                    // それ以外(=aR=1) → ソリッドは w
                    int solidW = m0.fromL ? (w - 1) : w;

                    auto facePos = [&](int I, int J)->VECTOR {
                        int XYZ[3] = { 0,0,0 };
                        XYZ[u] = I;
                        XYZ[v] = J;
                        XYZ[d] = solidW;

                        float xf = (XYZ[0] - Nx * 0.5f) * cell;
                        float yf = (XYZ[1] - Ny * 0.5f) * cell;
                        float zf = (XYZ[2] - Nz * 0.5f) * cell;

                        // ソリッド側セルの中心から、法線方向へ half ずらした位置が面
                        //if (d == 0) xf += nSign * half;
                        //if (d == 1) yf += nSign * cell;
                        //if (d == 2) zf += nSign * half;
                        if (d == 0) xf += nSign * (nSign == 1) ? cell : 0;
                        if (d == 1) yf += nSign * (nSign == 1) ? cell : half;
                        if (d == 2) zf += nSign * (nSign == 1) ? cell : 0;
                        return VGet(xf, yf, zf);
                        };

                    Vector3 p00 = facePos(i, j);
                    Vector3 p10 = facePos(i + wlen, j);
                    Vector3 p11 = facePos(i + wlen, j + hlen);
                    Vector3 p01 = facePos(i, j + hlen);

                    // 法線ベクトル
                    Vector3 nrm{ 0,0,0 };
                    (&nrm.x)[d] = (float)nSign; // d軸に ±1

                    // BuildGreedyMesh の UV 決定付近
                    float u0 = 0.0f, v0 = 0.0f;
                    float u1 = (float)wlen;
                    float v1 = (float)hlen;

                    // 端のにじみ対策
                    const float UV_EPS = 0.001f;
                    u0 += UV_EPS; v0 += UV_EPS;
                    u1 -= UV_EPS; v1 -= UV_EPS;

                    auto makeV = [&](const Vector3& P, float uu, float vv)->VERTEX3D {
                        VERTEX3D v{};
                        v.pos = P.ToVECTOR();
                        v.norm = nrm.ToVECTOR();                    // ← 照明向け法線
                        v.dif = GetColorU8(255, 255, 255, 255);     // ← 頂点拡散色
                        v.spc = GetColorU8(0, 0, 0, 0);             // ← スペキュラ未使用
                        v.u = uu; v.v = vv;                         // ← そのままタイル
                        return v;
                        };

                    // 巻き順（法線から見て反時計回り）
                    unsigned short base = (unsigned short)cur.v.size();
                    if (nSign > 0) {
                        cur.v.push_back(makeV(p00, u0, v0));
                        cur.v.push_back(makeV(p10, u1, v0));
                        cur.v.push_back(makeV(p11, u1, v1));
                        cur.v.push_back(makeV(p01, u0, v1));
                    }
                    else {
                        cur.v.push_back(makeV(p00, u0, v0));
                        cur.v.push_back(makeV(p01, u0, v1));
                        cur.v.push_back(makeV(p11, u1, v1));
                        cur.v.push_back(makeV(p10, u1, v0));
                    }

                    cur.i.push_back(base + 0); cur.i.push_back(base + 1); cur.i.push_back(base + 2);
                    cur.i.push_back(base + 0); cur.i.push_back(base + 2); cur.i.push_back(base + 3);

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

	// 生存しているセル中心位置リストを作成
    cellCenterPoss_.clear();
    cellCenterWorldPoss_.clear();
    for (int z = 0; z < Nz; ++z)
        for (int y = 0; y < Ny; ++y)
            for (int x = 0; x < Nx; ++x) {
                if (density[Idx(x, y, z)] == 0) continue;
                Vector3 lp = {
                    (x - Nx / 2) * cell,
                    (y - Ny / 2) * cell,
                    (z - Nz / 2) * cell
                };
                cellCenterPoss_[Idx(x, y, z)] = lp;
                cellCenterWorldPoss_[Idx(x, y, z)] = trans_.pos + lp;
            }

	// 生存比率を計算して、一定以下なら死滅扱いにする
	const int totalCells = Nx * Ny * Nz;
    if (((float)cellCenterPoss_.size() / (float)totalCells) < aliveNeedRatio_) { SetJudge(false); SetIsDraw(false); }
}
#pragma endregion


void VoxelBase::ApplyBrush(unsigned char amount)
{
	// 今フレームすでにリメッシュしていたら何もしない(軽量化 妥協)
    if (nowFrameRemesh_) { return; }
    else { nowFrameRemesh_ = true; }

	// 減少量が0以下なら何もしない
    if (amount <= 0) { return; }

	// 現在セル情報群が空なら何もしない
    if (density_.empty()) { return; }

	// 衝突情報群を取得して、該当セルの密度を、引数指定分減少させる
    for (int idx : ColliderSerch<VoxelCollider>().back()->GetHitCellIdxs()) {
        density_.at(idx) = (std::max)(density_.at(idx) - amount, 0);
    }
}

void VoxelBase::ReVival(void)
{
    density_ = densityInit_;
    regeneration_ = true;

    SetJudge(true);
    SetIsDraw(true);
}
