#include "VoxelBase.h"

#include <algorithm>
#include <queue>
#include <cmath>

#include"../Manager/Camera/Camera.h"

#include"../Application/Application.h"

VoxelBase::VoxelBase() :

    texture(-1),
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
	// ”h¶æ‚Ì“Ç‚İ‚İˆ—
    SubLoad();

	// ƒ‚ƒfƒ‹‚É§Œäî•ñ‚ğ“K—p‚µ‚Ä‚¨‚­
    trans_.Attach();

	// ƒ{ƒNƒZƒ‹ƒƒbƒVƒ…¶¬iƒ‚ƒfƒ‹‚ª³í‚É“Ç‚İ‚Ü‚ê‚Ä‚¢‚ê‚Îj
    if (trans_.model != -1) {

		// ƒƒbƒVƒ…¶¬Às
        BuildVoxelMeshFromMV1Handle(
            trans_.model,
            cellSize,
            trans_.pos + gridCenter,
            roughSize,
            batches
        );

        // ƒ‚ƒfƒ‹‚Í‚à‚¤g‚í‚È‚¢‚Ì‚Å‰ğ•ú
        trans_.Release();

        effect = new VoxelBreakEffectManager(texture);
        effect->Load();
    }
}


void VoxelBase::Init(void)
{
	// ƒtƒ‰ƒO‰Šú‰»
    SetJudge(true);
    SetIsDraw(true);
    regeneration = false;

    // ”j‰óƒZƒ‹ƒCƒ“ƒfƒbƒNƒXŒQ‚Ì‰Šú‰»
    BreakCellIdxClear();

    effect->Init();

    ActorBase::Init();
}


void VoxelBase::Update(void)
{
    ActorBase::Update();

    effect->Update();

    // À¿“I‚È¶‘¶”»’è
    if (!GetJudgeFlg()) { return; }

	// “®“IƒIƒuƒWƒFƒNƒg‚Ìê‡AˆÚ“®‚µ‚Ä‚¢‚½‚çƒZƒ‹’†SˆÊ’uŒQ‚ğXV‚·‚é
    if (GetDynamicFlg()) {
        if (trans_.Velocity() != 0.0f) {
            cellCenterPoss.clear();
            for (std::pair<const int, Vector3>p : cellCenterLocalPoss) { cellCenterPoss[p.first] = trans_.pos + p.second; }
        }
    }

    // ‘OƒtƒŒ[ƒ€AŒ`ó•Ï‰»‚ª‹N‚±‚Á‚Ä‚¢‚½‚çiƒtƒ‰ƒO‚ª‚½‚Á‚Ä‚¢‚½‚çjƒƒbƒVƒ…‚ğÄ¶¬
    if (regeneration) {
	    // ƒƒbƒVƒ…Ä¶¬ˆ—
        BuildGreedyMesh(density, Nx, Ny, Nz, cellSize, batches);

		// ƒtƒ‰ƒOƒŠƒZƒbƒgiƒƒbƒVƒ…Ä¶¬ˆ—j
        regeneration = false;
    }
}


void VoxelBase::Draw(void)
{
	// ”h¶æ‚Ì•`‰æˆ—
    SubDraw();

    // ƒGƒtƒFƒNƒg‚Ì•`‰æ
    effect->Draw();

    // •`‰æ”»’è
    if (!GetIsDraw()) { return; }
    
    // ƒƒCƒ“‚Ì•`‰æˆ—
    if (!GetIsAlphaDraw()) {

        // À•W‚ğˆÚ“®‚µ‚Ä•`‰æ
        MATRIX M = MGetTranslate(trans_.pos.ToVECTOR());
        SetTransformToWorld(&M);

        // ƒƒbƒVƒ…•`‰æ
        for (auto& b : batches) {
            if (b.i.empty()) { continue; }
            DrawPolygonIndexed3D(
                b.v.empty() ? b.v.data() : b.v.data(),
                (int)(b.v.empty() ? b.v.size() : b.v.size()),
                b.i.data(), (int)(b.i.size() / 3),
                (texture != -1) ? texture : DX_NONE_GRAPH, true
            );
        }

        // À•W‚ğŒ³‚É–ß‚·
        M = MGetIdent();
        SetTransformToWorld(&M);
    }
}

void VoxelBase::AlphaDraw(void)
{
    // ”h¶æ‚Ì•`‰æˆ—
    SubAlphaDraw();

    // •`‰æ”»’è
    if (!GetIsDraw()) { return; }

	// ƒƒCƒ“‚Ì•`‰æˆ—iƒAƒ‹ƒtƒ@•`‰æj
    if (GetIsAlphaDraw()) {

        // À•W‚ğˆÚ“®‚µ‚Ä•`‰æ
        MATRIX M = MGetTranslate(trans_.pos.ToVECTOR());
        SetTransformToWorld(&M);

        // ƒƒbƒVƒ…•`‰æ
        for (auto& b : batches) {
            if (b.i.empty()) { continue; }
            DrawPolygonIndexed3D(
                b.v.empty() ? b.v.data() : b.v.data(),
                (int)(b.v.empty() ? b.v.size() : b.v.size()),
                b.i.data(), (int)(b.i.size() / 3),
                (texture != -1) ? texture : DX_NONE_GRAPH, true
            );
        }

        // À•W‚ğŒ³‚É–ß‚·
        M = MGetIdent();
        SetTransformToWorld(&M);

    }


    if (App::GetIns().IsDrawDebug()) {
        for (ColliderBase*& c : GetCollider()) { c->DrawDebug(); }
    }
}


void VoxelBase::Release(void)
{
    // ƒƒbƒVƒ…î•ñŒQ‚ğ‘S‚Ä”jŠü
    for (auto& b : batches) {
        b.i.clear();
        b.v.clear();
    }
    batches.clear();

    // ƒeƒNƒXƒ`ƒƒ‚ğ‰ğ•úi“Ç‚İ‚Ü‚ê‚Ä‚¢‚½ê‡j
    if (texture != -1) { DeleteGraph(texture); }

    // ƒGƒtƒFƒNƒgŠÇ—ƒNƒ‰ƒX‚Ì”jŠü
    if (effect) {
        effect->Release();
        delete effect;
        effect = nullptr;
    }

    ActorBase::Release();
}



#pragma region ƒƒbƒVƒ…¶¬
void VoxelBase::BuildVoxelMeshFromMV1Handle(int mv1, float cell, const Vector3& center, const Vector3& roughSize, std::vector<MeshBatch>& batches)
{
    // ‡@ƒZƒ‹”‚ğZo````````````````
    Nx = (int)std::ceil(roughSize.x / cell);
    Ny = (int)std::ceil(roughSize.y / cell);
    Nz = (int)std::ceil(roughSize.z / cell);
    // `````````````````````````

    // ‡A•\–Ê‚ğƒ}[ƒLƒ“ƒO```````````````````````
    MarkSurface(mv1, cell, center, roughSize, Nx, Ny, Nz, density);
    // ````````````````````````````````

    // ‡B“à•”[“U```````````````````
    SolidFill(density, Nx, Ny, Nz);

    // ‰Šú–§“xî•ñ‚ğ•Û‚µ‚ÄŠÈ’P‚É–ß‚¹‚é‚æ‚¤‚É‚µ‚Ä‚¨‚­
    densityInit = density;
    // ````````````````````````

    // ‡CƒƒbƒVƒ…‰»`````````````````````
    BuildGreedyMesh(density, Nx, Ny, Nz, cellSize, batches);

    // ƒƒbƒVƒ…¶¬‚ª³í‚És‚í‚ê‚½‚©”Û‚©i¸”s‚ª‚ ‚ê‚Î’Ê’m‚µ‚Ä‚¨‚­j
    if (batches.empty()) { printfDx("ƒ{ƒNƒZƒ‹ƒƒbƒVƒ…¶¬‚É¸”s‚µ‚Ü‚µ‚½"); }
    // ```````````````````````````
}

void VoxelBase::MarkSurface(int mv1, float cell, const Vector3& center, const Vector3& roughSize, int Nx, int Ny, int Nz, std::vector<unsigned char>& density)
{
    // –§“xî•ñŒQ‚Ì”z—ñ”‚ğƒOƒŠƒbƒh”•ªŠm•Ûi‚O‚Å‰Šú‰»j
    density.resize(Nx * Ny * Nz, 0);

    // ƒ‚ƒfƒ‹‚ÌƒƒbƒVƒ…‚Ì“–‚½‚è”»’è‚ÌƒZƒbƒgƒAƒbƒv
    MV1SetupCollInfo(mv1, -1);

    // 
    Vector3 minW = center - (roughSize / 2);

    // ƒZƒ‹ƒTƒCƒY‚Ì”¼•ª‚ÌƒTƒCƒY‚ğ”¼Œa‚Æ‚µ‚Ä•Û‘¶iˆ—Œø—¦“I‚ÉƒZƒ‹‚ğ‹…‘Ì‚Æ‚µ‚ÄƒƒbƒVƒ…‚Æ‚Ì“–‚½‚è”»’è‚ğs‚¤j
    float r = cell * 0.5f;

    // ƒOƒŠƒbƒh‘S‚Ä‚ÅƒƒbƒVƒ…‚Æ‚Ì“–‚½‚è”»’è‚ğs‚¤
    for (int z = 0; z < Nz; ++z)
        for (int y = 0; y < Ny; ++y)
            for (int x = 0; x < Nx; ++x) {

                // ‚RŸŒ³“I‚ÈƒOƒŠƒbƒh”Ô†‚©‚çÀ•W‚ğZo
                Vector3 pc = Vector3(
                    minW.x + (x * cell) + (cell * 0.5f),
                    minW.y + (y * cell) + (cell * 0.5f),
                    minW.z + (z * cell) + (cell * 0.5f));

                // æ“¾‚µ‚½À•W‚Å“–‚½‚è”»’èiƒƒbƒVƒ…VS‹…‘Ìj
                auto res = MV1CollCheck_Sphere(mv1, -1, pc.ToVECTOR(), r);

                // “–‚½‚Á‚Ä‚¢‚½‚çAŠO‘¤‚Æ‚µ‚Ä‚ ‚Æ‚Å”»•Ê‰Â”\‚È‚æ‚¤‚É‚Q‚O‚O‚ğ“ü‚ê‚Ä‚¨‚­
                if ((res.HitNum > 0)) { density[Idx(x, y, z, Nx, Ny)] = 200; }

                // Õ“Ë”»’èî•ñ‚ğ”jŠü
                MV1CollResultPolyDimTerminate(res);
            }
}

void VoxelBase::SolidFill(std::vector<unsigned char>& density, int Nx, int Ny, int Nz)
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

	// ŠO•”‹ó‹C—Ìˆæ‚ğ BFS ‚Åƒ}[ƒN
    for (int x = 0; x < Nx; ++x) { for (int y = 0; y < Ny; ++y) { pushIf(x, y, 0); pushIf(x, y, Nz - 1); } }
    for (int x = 0; x < Nx; ++x) { for (int z = 0; z < Nz; ++z) { pushIf(x, 0, z); pushIf(x, Ny - 1, z); } }
    for (int y = 0; y < Ny; ++y) { for (int z = 0; z < Nz; ++z) { pushIf(0, y, z); pushIf(Nx - 1, y, z); } }

	// 6•ûŒü‚Ö“WŠJ
    while (!q.empty()) {
        int i = q.front(); q.pop();

        int z = i / (Nx * Ny);
        int y = (i - z * Nx * Ny) / Nx;
        int x = i % Nx;
        for (int k = 0; k < 6; ++k) { pushIf(x + (int)kDirNrm[k].x, y + (int)kDirNrm[k].y, z + (int)kDirNrm[k].z); }
    }

    // ŠO‚Í0‚Ì‚Ü‚ÜA“à•”‚Æ•\–Ê‚ğ255‚Ö
    for (int i = 0; i < total; ++i) {
        if (!ext[i]) { density[i] = 255; }                  // “à•”
        else if (density[i] == 200) { density[i] = 255; }   // ŠO•”‘¤‚Ì•\–Ê
    }
}

void VoxelBase::BuildGreedyMesh(
    const std::vector<unsigned char>& density,
    int Nx, int Ny, int Nz, float cell,
    std::vector<MeshBatch>& batches)
{
    auto Solid = [&](int x, int y, int z)->int {
        return (Inb(x, y, z) && density[Idx(x, y, z)] > 0) ? 1 : 0;
        };

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

    static const size_t kMaxVerts = 65000;
    auto flush = [&]() {
        if (!cur.v.empty()) {
            batches.push_back(std::move(cur));
            cur = MeshBatch{};
            cur.bmin = Vector3(+INF, +INF, +INF);
            cur.bmax = Vector3(-INF, -INF, -INF);
        }
        };

    // d: –@ü²(0:X, 1:Y, 2:Z)
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
            // 1) ƒXƒ‰ƒCƒX·•ª‚Åg–Ê‚ª—§‚ÂhŠ‚ğì‚é
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
                        // –Ê‚Í—§‚ÂB‚Ç‚¿‚ç‚ªƒ\ƒŠƒbƒh‘¤‚©‚ğ‹L˜^
                        // aL=1,aR=0 ¨ ƒ\ƒŠƒbƒh‚Íg¶(L‘¤)hB–@ü‚Í +d •ûŒü‚ÖŒü‚­
                        // aL=0,aR=1 ¨ ƒ\ƒŠƒbƒh‚Íg‰E(R‘¤)hB–@ü‚Í -d •ûŒü‚ÖŒü‚­
                        m.on = 1;
                        m.fromL = (aL == 1) ? 1 : 0;
                    }
                    mask[j * Du + i] = m;
                }
            }

            // 2) Greedy ‚Å’·•ûŒ`‚É‚Ü‚Æ‚ß‚é
            int i = 0, j = 0;
            while (j < Dv) {
                while (i < Du) {
                    Mask m0 = mask[j * Du + i];
                    if (!m0.on) { ++i; continue; }

                    // ‰¡‚ÖŠg’£
                    int wlen = 1;
                    while (i + wlen < Du) {
                        Mask t = mask[j * Du + (i + wlen)];
                        if (t.on != m0.on || t.fromL != m0.fromL) break;
                        ++wlen;
                    }
                    // c‚ÖŠg’£
                    int hlen = 1; bool grow = true;
                    while (j + hlen < Dv && grow) {
                        for (int k = 0; k < wlen; ++k) {
                            Mask t = mask[(j + hlen) * Du + (i + k)];
                            if (t.on != m0.on || t.fromL != m0.fromL) { grow = false; break; }
                        }
                        if (grow) ++hlen;
                    }

                    if (cur.v.size() + 4 > kMaxVerts) flush();

                    // –Ê‚Ì4’¸“_iƒ[ƒJƒ‹‹óŠÔj
                    // nSign: –@ü•„† (+1: +d •ûŒü, -1: -d •ûŒü)
                    const int nSign = m0.fromL ? +1 : -1;
                    const float half = cell * 0.5f;

                    // ƒ\ƒŠƒbƒh‘¤‚ÌƒZƒ‹ƒCƒ“ƒfƒbƒNƒXi–Ê‚ª‘®‚·‚égƒ\ƒŠƒbƒhhƒZƒ‹j
                    // fromL(=aL=1) ¨ ƒ\ƒŠƒbƒh‚Í w-1
                    // ‚»‚êˆÈŠO(=aR=1) ¨ ƒ\ƒŠƒbƒh‚Í w
                    int solidW = m0.fromL ? (w - 1) : w;

                    auto facePos = [&](int I, int J)->VECTOR {
                        int XYZ[3] = { 0,0,0 };
                        XYZ[u] = I;
                        XYZ[v] = J;
                        XYZ[d] = w;

                        float xf = (XYZ[0] - Nx * 0.5f) * cell;
                        float yf = (XYZ[1] - Ny * 0.5f) * cell;
                        float zf = (XYZ[2] - Nz * 0.5f) * cell;

                        return VGet(xf, yf, zf);
                        };

                    Vector3 p00 = facePos(i, j);
                    Vector3 p10 = facePos(i + wlen, j);
                    Vector3 p11 = facePos(i + wlen, j + hlen);
                    Vector3 p01 = facePos(i, j + hlen);

                    // –@üƒxƒNƒgƒ‹
                    Vector3 nrm{ 0,0,0 };
                    (&nrm.x)[d] = (float)nSign; // d²‚É }1

                    // BuildGreedyMesh ‚Ì UV Œˆ’è•t‹ß
                    float u0 = 0.0f, v0 = 0.0f;
                    float u1 = (float)wlen;
                    float v1 = (float)hlen;

                    // ’[‚Ì‚É‚¶‚İ‘Îô
                    const float UV_EPS = 0.001f;
                    u0 += UV_EPS; v0 += UV_EPS;
                    u1 -= UV_EPS; v1 -= UV_EPS;

                    auto makeV = [&](const Vector3& P, float uu, float vv)->VERTEX3D {
                        VERTEX3D v{};
                        v.pos = P.ToVECTOR();
                        v.norm = nrm.ToVECTOR();                    // © Æ–¾Œü‚¯–@ü
                        v.dif = GetColorU8(255, 255, 255, 255);     // © ’¸“_ŠgUF
                        v.spc = GetColorU8(0, 0, 0, 0);             // © ƒXƒyƒLƒ…ƒ‰–¢g—p
                        v.u = uu; v.v = vv;                         // © ‚»‚Ì‚Ü‚Üƒ^ƒCƒ‹
                        return v;
                        };

                    // Šª‚«‡i–@ü‚©‚çŒ©‚Ä”½Œv‰ñ‚èj
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

                    // g‚¢Ø‚è
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

	// ¶‘¶‚µ‚Ä‚¢‚éƒZƒ‹’†SˆÊ’uƒŠƒXƒg‚ğì¬
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

	// ¶‘¶”ä—¦‚ğŒvZ‚µ‚ÄAˆê’èˆÈ‰º‚È‚ç€–Åˆµ‚¢‚É‚·‚é
	const int totalCells = Nx * Ny * Nz;
    if (((float)cellCenterPoss.size() / (float)totalCells) < aliveNeedRatio) { SetJudge(false); SetIsDraw(false); }
}
#pragma endregion


void VoxelBase::ApplyBrush(unsigned char amount)
{
	// ¡ƒtƒŒ[ƒ€‚·‚Å‚ÉƒŠƒƒbƒVƒ…‚µ‚Ä‚¢‚½‚ç‰½‚à‚µ‚È‚¢(Œy—Ê‰» ‘Ã‹¦)
    if (regeneration) { return; }

	// Œ¸­—Ê‚ª0ˆÈ‰º‚È‚ç‰½‚à‚µ‚È‚¢
    if (amount <= 0) { return; }

	// Œ»İƒZƒ‹î•ñŒQ‚ª‹ó‚È‚ç‰½‚à‚µ‚È‚¢
    if (density.empty()) { return; }

    // ”j‰óƒZƒ‹ƒCƒ“ƒfƒbƒNƒXŒQ‚Ì‰Šú‰»
    BreakCellIdxClear();

	// Õ“Ëî•ñŒQ‚ğæ“¾‚µ‚ÄAŠY“–ƒZƒ‹‚Ì–§“x‚ğˆø”w’è•ªŒ¸­‚³‚¹‚é
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
    // ”j‰óƒZƒ‹‚ğæ“¾‚µ‚ÄAƒGƒtƒFƒNƒg‚ğ”­¶‚³‚¹‚é
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
