#pragma once

#include "Windows.h"

namespace offsets {
	namespace client {
		static intptr_t BaseAddress(int pid);
		static intptr_t XPositionAddress = 0x009DDDF8;
		static intptr_t YPositionAddress = 0x009DDDF8;
		static intptr_t ZPositionAddress = 0x009DDDF8;
	}

	namespace steam_api64
	{
		static intptr_t BaseAddress(int pid);
	}
}