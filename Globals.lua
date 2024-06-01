-- Global Paths
Botnet.Globals.Paths.SandboxiePath = "\"C:\\Program Files\\Sandboxie-Plus\\Start.exe\""
Botnet.Globals.Paths.SteamPath = "C:/Program Files (x86)/Steam/steam.exe"
Botnet.Globals.Paths.GMODDirectory = "C:/Program Files (x86)/Steam/steamapps/common/GarrysMod/"
Botnet.Globals.Paths.GMODExecutable = "hl2.exe"
Botnet.Globals.Paths.SandboxieBoxCreator = "C:/Program Files/Sandboxie-Plus/SbieIni.exe"
Botnet.Globals.Paths.BotData = "../botnet.json"
Botnet.Globals.Paths.GPlusRemoteExecutor = "C:/Users/Administrator/Desktop/Botnet/GPlus Remote Executor/GPlus.exe"
-- Global Arguments
Botnet.Globals.Arguments.QueryStart = 27000
Botnet.Globals.Arguments.Query = "QueryPort="
Botnet.Globals.Arguments.GMOD = "-console -novid -nohltv -nosteamcontroller -nosound -nojoy -noipx -noshaderapi -particles 1 -nopix -nopreload -nod3d9ex -low -textmode -heapsize 262144 -disallowhwmorph -high -reflectionTextureSize 0 -soft -softparticlesdefaultoff +exec autoexec.cfg"
Botnet.Globals.Arguments.Connect = "+connect"

-- Idea use winapi to run an exe with arguments such as the gmod process and the host that injects a dll that forces the connection to another server so we don't have to keep restarting the games.
-- DLL also reconnects if it wasn't given the command to disconnect yet disconnected anyways.