#include "offsets.h"
#include "Memory.h"

static intptr_t offsets::client::BaseAddress(int pid)
{
	return Memory::get_base_address(pid, "client.dll");
}

static intptr_t offsets::steam_api64::BaseAddress(int pid)
{
	return Memory::get_base_address(pid, "steam_api64.dll");
}