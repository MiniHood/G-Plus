#pragma once

#include "Windows.h"

namespace offsets {
	namespace gmod {
		namespace client {
			static DWORD BaseAddress; // cba
			static DWORD XPositionAddress = 0x009DDDF8;
			static DWORD YPositionAddress = 0x009DDDF8;
			static DWORD ZPositionAddress = 0x009DDDF8;
		}
	}
}