
namespace Botnet
{
    public class Globals
    {
        public class Paths
        {
            public static string SandboxiePath = "\"C:\\Program Files\\Sandboxie-Plus\\Start.exe\"";
            public static string SteamPath = "C:/Program Files (x86)/Steam/steam.exe";
            public static string GMODDirectory = "C:/Program Files (x86)/Steam/steamapps/common/GarrysMod/";
            public static string GMODExecutable = "hl2.exe";
            public static string SandboxieBoxCreator = "C:/Program Files/Sandboxie-Plus/SbieIni.exe";
            public static string BotData = "botnet.json";
            public static string GPlusRemoteExecutor = "C:/Users/Adam/Desktop/ARC/Garry's Mod/git/Botnet/GPlus Remote Executor/GPlus.exe";
        }

        public class Arguments
        {
            public static int QueryStart = 27000;
            public static string Query = "QueryPort=";
            public static string GMOD = "-console -32bit -novid -nosound -nojoy -noipx -noshaderapi -nopix -nopreload -nod3d9ex -low -textmode";
            public static string Connect = "+connect";
            public static string[,] Hosts = { { "166.1.173.183", "0" } };
        }
    }
}