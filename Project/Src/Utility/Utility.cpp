#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include <DxLib.h>
#include "Utility.h"

int Utility::Round(float v)
{
    return static_cast<int>(roundf(v));
}

std::vector<std::string> Utility::Split(std::string& line, char delimiter)
{

    std::istringstream stream(line);
    std::string field;
    std::vector<std::string> result;

    while (getline(stream, field, delimiter)) {
        result.push_back(field);
    }

    return result;

}

double Utility::Rad2DegD(double rad)
{
    return rad * (180.0 / DX_PI);
}

float Utility::Rad2DegF(float rad)
{
    return rad * (180.0f / DX_PI_F);
}

int Utility::Rad2DegI(int rad)
{
    return rad * Round(180.0f / DX_PI_F);
}

double Utility::Deg2RadD(double deg)
{
    return deg * (DX_PI / 180.0);
}

float Utility::Deg2RadF(float deg)
{
    return deg * (DX_PI_F / 180.0f);
}

int Utility::Deg2RadI(int deg)
{
    return deg * Round(DX_PI_F / 180.0f);
}

double Utility::DegIn360(double deg)
{
    deg = fmod(deg, 360.0);
    if (deg < 0.0f)
    {
        deg += 360.0;
    }
    return deg;
}

double Utility::RadIn2PI(double rad)
{
    rad = fmod(rad, DX_TWO_PI);
    if (rad < 0.0)
    {
        rad += DX_TWO_PI;
    }
    return rad;
}

int Utility::DirNearAroundRad(float from, float to)
{

    float ret = 1.0f;

    float diff = to - from;

    if (diff >= 0.0f)
    {

        // 比較元よりも時計回りに位置する

        if (diff > DX_PI_F)
        {
            // でも、180度以上離れているので、反時計回りの方が近い
            ret = -1.0f;
        }
        else
        {
            // 時計回り
            ret = 1.0f;
        }

    }
    else
    {

        // 比較元よりも反時計回りに位置する

        if (diff < -DX_PI_F)
        {
            // でも、180度以上離れているので、時計回りの方が近い
            ret = 1.0f;
        }
        else
        {
            // 反時計回り
            ret = -1.0f;
        }

    }

    return static_cast<int>(ret);

}

int Utility::DirNearAroundDeg(float from, float to)
{

    float ret = 1.0f;

    float diff = to - from;

    if (diff >= 0.0f)
    {

        // 比較元よりも時計回りに位置する

        if (diff > 180.0f)
        {
            // でも、180度以上離れているので、反時計回りの方が近い
            ret = -1.0f;
        }
        else
        {
            // 時計回り
            ret = 1.0f;
        }

    }
    else
    {

        // 比較元よりも反時計回りに位置する

        if (diff < -180.0f)
        {
            // でも、180度以上離れているので、時計回りの方が近い
            ret = 1.0f;
        }
        else
        {
            // 反時計回り
            ret = -1.0f;
        }

    }

    return static_cast<int>(ret);

}

int Utility::Lerp(int start, int end, float t)
{
    // 線形補間
    if (t >= 1.0f)
    {
        return end;
    }

    int ret = start;
    ret += Round(t * static_cast<float>(end - start));
    return ret;
}

float Utility::Lerp(float start, float end, float t)
{
    // 線形補間
    if (t >= 1.0f)
    {
        return end;
    }

    float ret = start;
    ret += t * (end - start);
    return ret;
}

double Utility::Lerp(double start, double end, double t)
{
    // 線形補間
    if (t >= 1.0)
    {
        return end;
    }

    double ret = start;
    ret += t * (end - start);
    return ret;
}

Vector2 Utility::Lerp(const Vector2& start, const Vector2& end, float t)
{
    // 線形補間
    if (t >= 1.0f)
    {
        return end;
    }

    Vector2 ret = start;
    ret.x += Round(t * static_cast<float>((end.x - start.x)));
    ret.y += Round(t * static_cast<float>((end.y - start.y)));
    return ret;
}

double Utility::LerpDeg(double start, double end, double t)
{

    double ret;

    double diff = end - start;
    if (diff < -180.0)
    {
        end += 360.0;
        ret = Lerp(start, end, t);
        if (ret >= 360.0)
        {
            ret -= 360.0;
        }
    }
    else if (diff > 180.0)
    {
        end -= 360.0;
        ret = Lerp(start, end, t);
        if (ret < 0.0)
        {
            ret += 360.0;
        }
    }
    else
    {
        ret = Lerp(start, end, t);
    }

    return ret;

}

Vector2 Utility::Bezier(const Vector2& p1, const Vector2& p2, const Vector2& p3, float t)
{
    Vector2 a = Lerp(p1, p2, t);
    Vector2 b = Lerp(p2, p3, t);
    return Lerp(a, b, t);
}

double Utility::Magnitude(const Vector2& v)
{
    return sqrt((v.x * v.x) + (v.y * v.y));
}

int Utility::SqrMagnitude(const Vector2& v)
{
    return static_cast<int>(v.x * v.x + v.y * v.y);
}

double Utility::Distance(const Vector2& v1, const Vector2& v2)
{
    return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2));
}

float Utility::VLength(const VECTOR& v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

VECTOR Utility::VDivision(const VECTOR& v, float f)
{
    if (f == 0.0f) { return {}; }
    return { v.x / f,   v.y / f,    v.z / f };
}

VECTOR Utility::Normalize(const VECTOR& v)
{
    return VDivision(v, VLength(v));
}

Vector2 Utility::Normalize(const Vector2& v)
{
    return v / Magnitude(v);
}

VECTOR Utility::Clamp(const VECTOR& v, const VECTOR& minV, const VECTOR& maxV)
{
    VECTOR r;
    r.x = Utility::Clamp(v.x, minV.x, maxV.x);
    r.y = Utility::Clamp(v.y, minV.y, maxV.y);
    r.z = Utility::Clamp(v.x, minV.x, maxV.x);
    return r;
}

bool Utility::VZERO(const VECTOR& v)
{
    return (v.x == 0.0f && v.y == 0.0f && v.z == 0.0f);
}

void Utility::LoadImg(int& handle, std::string path)
{
    handle = LoadGraph(path.c_str());
    if (handle == -1) {
        printfDx("画像読み込みに失敗しました");
    }
}

int Utility::LoadImg(std::string path)
{
    int ret = 0;

    ret = LoadGraph(path.c_str());
    if (ret == -1) {
        printfDx("画像読み込みに失敗しました");
    }

    return ret;
}

void Utility::LoadArrayImg(std::string path, int AllNum, int XNum, int YNum, int XSize, int YSize, int* handleArray)
{
    int err = 0;

    err = LoadDivGraph(path.c_str(), AllNum, XNum, YNum, XSize, YSize, handleArray);

    if (err == -1) {
        printfDx("画像読み込みに失敗しました");
    }
}

void Utility::LoadArrayImg(std::string path, int AllNum, int XNum, int YNum, int XSize, int YSize, std::vector<int>& handleArray)
{
    handleArray.resize(AllNum);

    int err = LoadDivGraph(path.c_str(), AllNum, XNum, YNum, XSize, YSize, handleArray.data());

    if (err == -1) {
        printfDx("画像読み込みに失敗しました: %s\n", path.c_str());
        return;
    }
}

bool Utility::CollisionAABB(const VECTOR& pos1, const VECTOR& size1, const VECTOR& pos2, const VECTOR& size2)
{
	if (pos1.x + size1.x < pos2.x - size2.x) return false;
	if (pos1.x - size1.x > pos2.x + size2.x) return false;
	if (pos1.y + size1.y < pos2.y - size2.y) return false;
	if (pos1.y - size1.y > pos2.y + size2.y) return false;
	if (pos1.z + size1.z < pos2.z - size2.z) return false;
	if (pos1.z - size1.z > pos2.z + size2.z) return false;
	return true;
}

// 球 vs AABB の最小押し戻し（MTV）を返す
bool Utility::SphereVsAABB_MTV(const VECTOR& C, float R, const VECTOR& bmin, const VECTOR& bmax, VECTOR& outN, float& outDepth)
{
    const VECTOR cp = Clamp(C, bmin, bmax);
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
        // 球中心が箱の“内部にいる”ケース：最短軸で押し出す
        float dx = (std::min)(C.x - bmin.x, bmax.x - C.x);
        float dy = (std::min)(C.y - bmin.y, bmax.y - C.y);
        float dz = (std::min)(C.z - bmin.z, bmax.z - C.z);
        if (dx <= dy && dx <= dz) { outN = { (C.x - (bmin.x + bmax.x) * 0.5f) >= 0 ? +1.f : -1.f, 0, 0 }; outDepth = R + dx; }
        else if (dy <= dz) { outN = { 0, (C.y - (bmin.y + bmax.y) * 0.5f) >= 0 ? +1.f : -1.f, 0 }; outDepth = R + dy; }
        else { outN = { 0, 0, (C.z - (bmin.z + bmax.z) * 0.5f) >= 0 ? +1.f : -1.f }; outDepth = R + dz; }
        return true;
    }
}

// 円（XZ）vs 矩形（XZ）のMTV（カプセル側面用、Yは別途範囲チェック）
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
        // 真上（真下）で重なってる：最近面の法線で
        float rx = (std::min)(std::abs(C.x - bmin.x), std::abs(bmax.x - C.x));
        float rz = (std::min)(std::abs(C.z - bmin.z), std::abs(bmax.z - C.z));
        if (rx <= rz) outN = { (C.x < (bmin.x + bmax.x) * 0.5f) ? -1.f : +1.f, 0, 0 }, outDepth = R + rx;
        else          outN = { 0, 0, (C.z < (bmin.z + bmax.z) * 0.5f) ? -1.f : +1.f }, outDepth = R + rz;
        return true;
    }
}