#include "VoxelBase.h"

#include <algorithm>
#include <queue>
#include <cmath>

#include"../Manager/Camera/Camera.h"

#include"../Application/Application.h"

VoxelBase::VoxelBase() :

    texture(-1),
    TEX_SCALE(),

    batches(),
    Nx(0), Ny(0), Nz(0),
    cellSize(0.0f),
    gridCenter(0.0f, 0.0f, 0.0f),
    density(),
    densityInit(),
    regeneration(false),

    aliveNeedRatio(0.0f),
    cellCenterLocalPoss(),
    cellCenterPoss(),

    breakCellIdx(),

    effect(nullptr)
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
        BuildVoxelMeshFromMV1Handle();

        // モデルはもう使わないので解放
        trans_.Release();

        // 破壊エフェクト管理クラスを生成
        effect = new VoxelBreakEffectManager(texture);
        effect->Load();
    }
}


void VoxelBase::Init(void)
{
	// フラグ初期化
    SetJudge(true);
    SetIsDraw(true);
    regeneration = false;

    // 破壊セルインデックス群の初期化
    BreakCellIdxClear();

    // 破壊エフェクト管理クラスの初期化
    effect->Init();

    // 基底クラスの初期化の呼び出し直し
    ActorBase::Init();
}


void VoxelBase::Update(void)
{
    // 基底クラスの更新処理の呼び出し直し
    ActorBase::Update();

    // 破壊エフェクト管理クラスの更新
    effect->Update();

    // 実質的な生存判定
    if (!GetJudgeFlg()) { return; }

	// 動的オブジェクトの場合、移動していたらセル中心位置群(ワールド)を更新する
    if (GetDynamicFlg()) {
        // 移動量を見る
        if (trans_.Velocity() != 0.0f) {

            // 一旦もともと格納されていた中心座標群を初期化する
            cellCenterPoss.clear();

            // セル中心座標群(ローカル)からセル中心座標群(ワールド)を算出する
            for (std::pair<const int, Vector3>p : cellCenterLocalPoss) { cellCenterPoss[p.first] = trans_.pos + p.second; }
        }
    }

    // 前フレーム、形状変化が起こっていたら（フラグがたっていたら）メッシュを再生成
    if (regeneration) {
	    // メッシュ再生成処理
        BuildGreedyMesh();

		// メッシュ再生成完了
        regeneration = false;
    }
}


void VoxelBase::Draw(void)
{
	// 派生先の描画処理
    SubDraw();

    // エフェクトの描画
    effect->Draw();

    // 描画判定
    if (!GetIsDraw()) { return; }
    
    // メインの描画処理
    if (!GetIsAlphaDraw()) {

        // 座標を移動して描画
        MATRIX M = MGetTranslate(trans_.pos.ToVECTOR());
        SetTransformToWorld(&M);

        // メッシュ描画
        for (auto& b : batches) {
            if (b.i.empty()) { continue; }
            DrawPolygonIndexed3D(
                b.v.empty() ? b.v.data() : b.v.data(),
                (int)(b.v.empty() ? b.v.size() : b.v.size()),
                b.i.data(), (int)(b.i.size() / 3),
                (texture != -1) ? texture : DX_NONE_GRAPH, true
            );
        }

        // 座標を元に戻す
        M = MGetIdent();
        SetTransformToWorld(&M);
    }
}

void VoxelBase::AlphaDraw(void)
{
    // 派生先の描画処理
    SubAlphaDraw();

    // 描画判定
    if (!GetIsDraw()) { return; }

	// メインの描画処理（アルファ描画）
    if (GetIsAlphaDraw()) {

        // 座標を移動して描画
        MATRIX M = MGetTranslate(trans_.pos.ToVECTOR());
        SetTransformToWorld(&M);

        // メッシュ描画
        for (auto& b : batches) {
            if (b.i.empty()) { continue; }
            DrawPolygonIndexed3D(
                b.v.empty() ? b.v.data() : b.v.data(),
                (int)(b.v.empty() ? b.v.size() : b.v.size()),
                b.i.data(), (int)(b.i.size() / 3),
                (texture != -1) ? texture : DX_NONE_GRAPH, true
            );
        }

        // 座標を元に戻す
        M = MGetIdent();
        SetTransformToWorld(&M);
    }


    if (App::GetIns().IsDrawDebug()) {
        for (ColliderBase*& c : GetCollider()) { c->DrawDebug(); }
    }
}


void VoxelBase::Release(void)
{
    // メッシュ情報群を全て破棄
    for (auto& b : batches) {
        b.i.clear();
        b.v.clear();
    }
    batches.clear();

    // テクスチャを解放（読み込まれていた場合）
    if (texture != -1) { DeleteGraph(texture); }

    // エフェクト管理クラスの破棄
    if (effect) {
        effect->Release();
        delete effect;
        effect = nullptr;
    }

    ActorBase::Release();
}



#pragma region メッシュ生成
void VoxelBase::BuildVoxelMeshFromMV1Handle(void)
{
    // ①セル数を算出～～～～～～～～～～～～～～～～
    Nx = (int)std::ceil(roughSize.x / cellSize);
    Ny = (int)std::ceil(roughSize.y / cellSize);
    Nz = (int)std::ceil(roughSize.z / cellSize);
    // ～～～～～～～～～～～～～～～～～～～～～～～～～

    // ②表面をマーキング～～～～～～～～～～～～～～～～～～～～～～～
    MarkSurface();
    // ～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～～

    // ③内部充填～～～～～～～～～～～～～～～～～～～
    SolidFill();

    // 初期密度情報を保持して簡単に戻せるようにしておく
    densityInit = density;
    // ～～～～～～～～～～～～～～～～～～～～～～～～

    // ④メッシュ化～～～～～～～～～～～～～～～～～～～～～
    BuildGreedyMesh();

    // メッシュ生成が正常に行われたか否か（失敗があれば通知しておく）
    if (batches.empty()) { printfDx("ボクセルメッシュ生成に失敗しました"); }
    // ～～～～～～～～～～～～～～～～～～～～～～～～～～～
}

void VoxelBase::MarkSurface(void)
{
    // 密度情報群の配列数をグリッド数分確保（０で初期化）
    density.resize(Nx * Ny * Nz, 0);

    // モデルのメッシュの当たり判定のセットアップ
    MV1SetupCollInfo(trans_.model, -1);

    // グリッド最小座標を算出
    Vector3 minW = (trans_.pos + gridCenter) - (roughSize / 2);

    // セルサイズの半分のサイズを半径として保存（処理効率的にセルを球体としてメッシュとの当たり判定を行う）
    float r = cellSize * 0.5f;

    // グリッド全てでメッシュとの当たり判定を行う
    for (int z = 0; z < Nz; ++z)
        for (int y = 0; y < Ny; ++y)
            for (int x = 0; x < Nx; ++x) {

                // ３次元的なグリッド番号から座標を算出
                Vector3 pc = Vector3(
                    minW.x + (x * cellSize) + (cellSize * 0.5f),
                    minW.y + (y * cellSize) + (cellSize * 0.5f),
                    minW.z + (z * cellSize) + (cellSize * 0.5f));

                // 取得した座標で当たり判定（メッシュVS球体）
                auto res = MV1CollCheck_Sphere(trans_.model, -1, pc.ToVECTOR(), r);

                // 当たっていたら、そのセルを埋める
                if ((res.HitNum > 0)) { density[Idx(x, y, z, Nx, Ny)] = 255; }

                // 衝突判定情報を破棄
                MV1CollResultPolyDimTerminate(res);
            }
}

void VoxelBase::SolidFill(void)
{
    // グリッド総数を計算
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
        for (int k = 0; k < 6; ++k) { pushIf(x + (int)kDirNrm[k].x, y + (int)kDirNrm[k].y, z + (int)kDirNrm[k].z); }
    }

    // 外は0のまま、内部と表面を255へ
    for (int i = 0; i < total; ++i) {
        // フラグで内部を埋める
        if (!ext[i]) { density[i] = 255; }
    }
}

void VoxelBase::BuildGreedyMesh(void)
{
    // 生存しているセル中心位置リストを作成
    cellCenterLocalPoss.clear();
    cellCenterPoss.clear();
    for (int z = 0; z < Nz; ++z)
        for (int y = 0; y < Ny; ++y)
            for (int x = 0; x < Nx; ++x) {
                if (density[Idx(x, y, z)] == 0) { continue; }
                Vector3 lp = IdxToLocalPos(x, y, z);
                cellCenterLocalPoss[Idx(x, y, z)] = lp;
                cellCenterPoss[Idx(x, y, z)] = trans_.pos + lp;
            }

    // 生存比率を計算して、一定以下なら死滅扱いにする
    const int totalCells = Nx * Ny * Nz;
    if (((float)cellCenterPoss.size() / (float)totalCells) < aliveNeedRatio) { SetJudge(false); SetIsDraw(false); return; }

    auto Solid = [&](int x, int y, int z)->int { return (Inb(x, y, z) && density[Idx(x, y, z)] > 0) ? 1 : 0; };

    batches.clear();

    MeshBatch cur;
    const float INF = 1e30f;
    cur.bmin = Vector3(+INF, +INF, +INF);
    cur.bmax = Vector3(-INF, -INF, -INF);

    auto updAabb = [&](const Vector3& p) {
        cur.bmin.x = (std::min)(cur.bmin.x, p.x);
        cur.bmin.y = (std::min)(cur.bmin.y, p.y);
        cur.bmin.z = (std::min)(cur.bmin.z, p.z);
        cur.bmax.x = (std::max)(cur.bmax.x, p.x);
        cur.bmax.y = (std::max)(cur.bmax.y, p.y);
        cur.bmax.z = (std::max)(cur.bmax.z, p.z);
        };

    const size_t kMaxVerts = 65000;
    auto flush = [&]() {
        if (!cur.v.empty()) {
            batches.push_back(std::move(cur));
            cur = MeshBatch{};
            cur.bmin = Vector3(+INF, +INF, +INF);
            cur.bmax = Vector3(-INF, -INF, -INF);
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
            // スライス差分で“面が立つ”所を作る
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

            // 長方形にまとめる
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
                    const float half = cellSize * 0.5f;

                    // ソリッド側のセルインデックス（面が属する“ソリッド”セル）
                    // fromL(=aL=1) → ソリッドは w-1
                    // それ以外(=aR=1) → ソリッドは w
                    int solidW = m0.fromL ? (w - 1) : w;

                    auto facePos = [&](int I, int J)->VECTOR {
                        int XYZ[3] = { 0,0,0 };
                        XYZ[u] = I;
                        XYZ[v] = J;
                        XYZ[d] = w;

                        float xf = (XYZ[0] - Nx * 0.5f) * cellSize;
                        float yf = (XYZ[1] - Ny * 0.5f) * cellSize;
                        float zf = (XYZ[2] - Nz * 0.5f) * cellSize;

                        return VGet(xf, yf, zf);
                        };

                    Vector3 p00 = facePos(i, j);
                    Vector3 p10 = facePos(i + wlen, j);
                    Vector3 p11 = facePos(i + wlen, j + hlen);
                    Vector3 p01 = facePos(i, j + hlen);

                    // 法線ベクトル
                    Vector3 nrm{ 0,0,0 };
                    (&nrm.x)[d] = (float)nSign; // d軸に ±1

                    auto makeV = [&](const Vector3& P)->VERTEX3D {
                        VERTEX3D v{};
                        v.pos = P.ToVECTOR();
                        v.norm = nrm.ToVECTOR();
                        v.dif = GetColorU8(255, 255, 255, 255);
                        v.spc = GetColorU8(0, 0, 0, 0);

                        // どっち向きの面かによってUV座標を設定する
                        if (d == 0) {
                            v.u = P.z * TEX_SCALE;
                            v.v = P.y * TEX_SCALE;
                        }
                        else if (d == 1) {
                            v.u = P.x * TEX_SCALE;
                            v.v = P.z * TEX_SCALE;
                        }
                        else if (d == 2) {
                            v.u = P.x * TEX_SCALE;
                            v.v = P.y * TEX_SCALE;
                        }

                        return v;
                        };

                    // 巻き順（法線から見て反時計回り）
                    unsigned short base = (unsigned short)cur.v.size();
                    if (nSign > 0) {
                        cur.v.push_back(makeV(p00));
                        cur.v.push_back(makeV(p10));
                        cur.v.push_back(makeV(p11));
                        cur.v.push_back(makeV(p01));
                    }
                    else {
                        cur.v.push_back(makeV(p00));
                        cur.v.push_back(makeV(p01));
                        cur.v.push_back(makeV(p11));
                        cur.v.push_back(makeV(p10));
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
}
#pragma endregion


void VoxelBase::ApplyBrush(unsigned char amount)
{
	// 今フレームすでにリメッシュしていたら何もしない(軽量化 妥協)
    if (regeneration) { return; }

	// 減少量が0以下なら何もしない
    if (amount <= 0) { return; }

	// 現在セル情報群が空なら何もしない
    if (density.empty()) { return; }

    // 破壊セルインデックス群の初期化
    BreakCellIdxClear();

	// 衝突情報群を取得して、該当セルの密度を引数指定分減少させる
    for (int idx : ColliderSerch<VoxelCollider>().back()->GetHitCellIdxs()) {
        density.at(idx) = (std::max)(density.at(idx) - amount, 0);
        if (density.at(idx) <= 0) {
            breakCellIdx.emplace_back(idx);
            regeneration = true; 
        }
    }
}

void VoxelBase::BreakEffect(const Vector3& breakerPos)
{
    // 破壊セルを取得して、エフェクトを発生させる
    for (int idx : breakCellIdx) {
        Vector3 p = IdxToPos(idx);

        float velPower = 5.0f;
        Vector3 velocity = (p - breakerPos).Normalized() * velPower;
        velocity.y = velPower;
        effect->Spawn(p, cellSize, velocity);
    }
}

void VoxelBase::ReVival(void)
{
    density = densityInit;
    regeneration = true;

    SetJudge(true);
    SetIsDraw(true);
}
