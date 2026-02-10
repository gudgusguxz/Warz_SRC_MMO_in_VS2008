#ifndef GB_STD_INT_H_
#define GB_STD_INT_H_

// vs2008 does not support stdint.h, so explicitly
// declaring the types we need here
#ifdef _MSC_VER
#define GB_DEFINE_STDINT_TYPES
#else
#include <stdint.h>
#endif

namespace GameBlocks
{

#ifdef GB_DEFINE_STDINT_TYPES
	typedef __int32           int32_t;
	typedef unsigned __int32  uint32_t;
	typedef __int64           int64_t;
	typedef unsigned __int64  uint64_t;
#endif

} // namespace GameBlocks

#endif // GB_STD_INT_H_