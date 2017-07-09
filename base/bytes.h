#ifndef __LIBRA_BYTES_H__
#define __LIBRA_BYTES_H__

#include "libra.h"

namespace Libra
{
	template<class T>
	struct NoSwap
	{
		T operator ()(T v) { return v; }
	};

	///////////////////////////////////////////
	template<class T> struct SwapH2N;
	template<> struct SwapH2N<int8_t>
	{
		int8_t operator ()(int8_t v) { return v; }
	};

	template<> struct SwapH2N<uint8_t>
	{
		uint8_t operator ()(uint8_t v) { return v; }
	};

	template<> struct SwapH2N<int32_t>
	{
		int32_t operator ()(int32_t v) { return htonl(v); }
	};

	template<> struct SwapH2N<uint32_t>
	{
		uint32_t operator ()(uint32_t v) { return htonl(v); }
	};

	template<> struct SwapH2N<int16_t>
	{
		int16_t operator ()(int16_t v) { return htons(v); }
	};

	template<> struct SwapH2N<uint16_t>
	{
		uint16_t operator ()(uint16_t v) { return htons(v); }
	};

	///////////////////////////////////////////
	template<class T> struct SwapN2H
	{
		T operator ()(T v)
		{
			//static_assert(false, "nonsupport");
		}
	};

	template<> struct SwapN2H<int8_t>
	{
		int8_t operator ()(int8_t v) { return v; }
	};

	template<> struct SwapN2H<uint8_t>
	{
		uint8_t operator ()(uint8_t v) { return v; }
	};

	template<> struct SwapN2H<int32_t>
	{
		int32_t operator ()(int32_t v) { return ntohl(v); }
	};

	template<> struct SwapN2H<uint32_t>
	{
		uint32_t operator ()(uint32_t v) { return ntohl(v); }
	};

	template<> struct SwapN2H<int16_t>
	{
		int16_t operator ()(int16_t v) { return ntohs(v); }
	};

	template<> struct SwapN2H<uint16_t>
	{
		uint16_t operator ()(uint16_t v) { return ntohs(v); }
	};
}

#endif


