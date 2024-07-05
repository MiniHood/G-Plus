#pragma once
#include <string>

namespace Globals {
	namespace Steam {
		static std::string Path = "C:\\Program Files (x86)\\Steam\\";
		static std::string Version = "1718904662";
	}
	namespace GMOD {
		static std::string Path = "C:\\Program Files (x86)\\Steam\\steamapps\\common\\GarrysMod\\";
		static std::string PatchVersion = "2023.06.28";
		// If your game hasnt started by then, we can safely assume it's not going too and we're hogging resources by searching for it. After it destroys the class and closes steam.
		static int SearchTimeOut = 10000; 
	}
}