#ifndef GAME_SIM_TYPES_H_
#define GAME_SIM_TYPES_H_

#include <string>

// vs2008 doesn't suport stdint.h
#ifdef _MSC_VER
typedef __int32           int32_t;
typedef unsigned __int32  uint32_t;
typedef __int64           int64_t;
typedef unsigned __int64  uint64_t;
#else
#include <stdint.h>
#endif

namespace FirstPersonShooter {

typedef std::string SimString;
typedef uint64_t SimPlayerId;

struct SimVect
{
	SimVect(float newX, float newY, float newZ)
		: x(newX)
		, y(newY)
		, z(newZ)
	{}

	SimVect()
		: x(0.f)
		, y(0.f)
		, z(0.f)
	{}

	float x;
	float y;
	float z;
};

} // namespace FirstPersonShooter

#endif // GAME_SIM_TYPES_H_