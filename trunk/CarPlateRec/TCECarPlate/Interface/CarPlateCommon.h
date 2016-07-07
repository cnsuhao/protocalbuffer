#pragma once

#ifdef TCE_CARPLATE_EXPORT
#define TCE_CARPLATE_API __declspec(dllexport)
#else
#define TCE_CARPLATE_API __declspec(dllimport)
#endif // TCECarPlate_EXPORTS

#include <string>
#include <vector>


struct tRect
{
    int x;
    int y;
    int width;
    int height;
};

struct tPlateInfo
{
    std::string strPlate;
    std::string strColor;
    tRect PlatePos;
};
