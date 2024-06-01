using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Botnet
{
    public static class Helpers
    {
        public static List<string> SearchSandboxieList(string accountName, string specificID = null)
        {
            specificID = specificID ?? null;
            List<string> cleanedOutput = new List<string>();

            using (var inputOutput = Process.Start(new ProcessStartInfo
            {
                FileName = Botnet.Globals.Paths.SandboxiePath,
                Arguments = $"/box:{accountName} /listpids | more",
                UseShellExecute = false,
                RedirectStandardOutput = true,
                CreateNoWindow = true
            }))
            {
                if (inputOutput == null)
                    return new List<string>();

                string line;
                while ((line = inputOutput.StandardOutput.ReadLine()) != null)
                {
                    if (!string.IsNullOrEmpty(line) && line != "0")
                        cleanedOutput.Add(line);
                }
            }

            return cleanedOutput;
        }

        public static bool IsGarrysModOpen(string[] account)
        {
            var list = SearchSandboxieList(account[0]);
            if (list.Count == 0)
                return false;

            foreach (var v in list)
            {
                var process = Process.GetProcessById(int.Parse(v));
                if (process.ProcessName == "gmod.exe")
                    return true;
            }

            return false;
        }

        public static void FlagError(int errorCode)
        {
            Console.WriteLine("[ERROR] " + errorCode);
        }

        public static void SetAllSandboxieSettings(List<string[]> accounts)
        {
            foreach (var account in accounts)
            {
                string arguments = $" set {account[0]} ConfigLevel 7";
                arguments += $" && set {account[0]} Enabled y";
                arguments += $" && set {account[0]} AutoDelete n";
                arguments += $" && set {account[0]} BorderColor #00FFFF";
                arguments += $" && set {account[0]} BoxNameTitle y";
                arguments += $" && set {account[0]} AutoRecover y";
                arguments += $" && set {account[0]} PromptForFileMigration n";
                arguments += $" && set {account[0]} CopyLimitKb 5000000";
                arguments += $" && set {account[0]} Template AutoRecoverIgnore";

                Process.Start(new ProcessStartInfo
                {
                    FileName = Botnet.Globals.Paths.SandboxieBoxCreator,
                    Arguments = arguments,
                    UseShellExecute = true
                });
            }
        }

        public static int GetTotalBots(List<string[]> loginInformation)
        {
            return loginInformation.Count - GetAmountOfAvailableBots(loginInformation);
        }

        public static int GetAmountOfAvailableBots(List<string[]> loginInformation)
        {
            return GetAvailableBots(loginInformation).Count;
        }

        public static bool IsBotAvailable(string[] account, List<string[]> loginInformation)
        {
            return GetAvailableBots(loginInformation).Any(acc => acc.SequenceEqual(account));
        }

        public static bool IsBotConnected(string[] account, List<string[]> loginInformation)
        {
            return GetConnectedBots(loginInformation).Any(acc => acc.SequenceEqual(account));
        }

        public static bool IsBotUnprepared(string[] account, List<string[]> loginInformation)
        {
            return GetUnpreparedBots(loginInformation).Any(acc => acc.SequenceEqual(account));
        }

        public static List<string[]> GetAvailableBots(List<string[]> loginInformation)
        {
            List<string[]> bots = loginInformation ?? new List<string[]>();
            List<string[]> availableBoxes = new List<string[]>();

            foreach (var v in bots)
            {
                if (!availableBoxes.Any(acc => acc.SequenceEqual(v)))
                    availableBoxes.Add(v);
            }

            foreach (var v in bots)
            {
                if (IsGarrysModOpen(v) && !Botnet.Controller.GetRegisteredBot(v, loginInformation))
                    availableBoxes.Add(v);
            }

            return availableBoxes;
        }

        public static List<string[]> GetConnectedBots(List<string[]> loginInformation)
        {
            List<string[]> bots = loginInformation ?? new List<string[]>();
            List<string[]> availableBots = new List<string[]>();

            foreach (var v in bots)
            {
                if (IsGarrysModOpen(v) && Botnet.Controller.GetRegisteredBot(v, loginInformation))
                    availableBots.Add(v);
            }

            return availableBots;
        }

        public static List<string[]> GetUnpreparedBots(List<string[]> loginInformation)
        {
            List<string[]> bots = loginInformation ?? new List<string[]>();
            List<string[]> availableBots = new List<string[]>();

            foreach (var v in bots)
            {
                if (!IsGarrysModOpen(v) && !Botnet.Controller.GetRegisteredBot(v, loginInformation))
                    availableBots.Add(v);
            }

            return availableBots;
        }

        public static void DisconnectAllBots(bool force, List<string[]> loginInformation)
        {
            foreach (var value in loginInformation)
            {
                DisconnectBot(value, force, loginInformation);
            }
        }

        public static bool DisconnectBot(string[] account, bool force, List<string[]> loginInformation)
        {
            if (account.Length == 1 && account[0] is string)
            {
                for (int i = 0; i < loginInformation.Count; i++)
                {
                    if (loginInformation[i][0] == account[0])
                    {
                        account = loginInformation[i];
                        break;
                    }
                }
            }

            force = force || false;

            if (force)
            {
                StopBox(account);
                Botnet.Controller.UnregisterBot(account);
                return true;
            }

            if (!Botnet.Controller.GetRegisteredBot(account, loginInformation))
            {
                return false;
            }

            Console.WriteLine("Disconnecting: " + account[0]);

            Botnet.Helpers.RunCommand(account, "disconnect"); // Assuming this method exists
            Botnet.Controller.UnregisterBot(account, loginInformation);

            CheckStatus(account, 'close'); // Assuming this method exists
            return true;
        }

        public static void StopBox(string[] account)
        {
            Botnet.Controller.UnregisterBot(account, false);
            ExecuteCommand(Botnet.Globals.Paths.SandboxiePath + " /box:" + account[0] + " /terminate");
        }

        private static void ExecuteCommand(string command)
        {
            System.Diagnostics.Process.Start("cmd", "/c " + command);
        }

        public static void StartBox(string[] account, string initialHost, string Query)
        {
            string LoginArgument = "-login";
            string Username = account[0];
            string Password = account[1];
            string RunGameArgument = null;
            string Whitespace = " ";

            if (IsBotAvailable(account) || IsBotConnected(account)) return;

            if (!string.IsNullOrEmpty(initialHost))
            {
                RunGameArgument = "-applaunch 4000" + Whitespace + Botnet.Globals.Arguments.GMOD + Whitespace +
                                  Botnet.Globals.Arguments.Connect + Whitespace + initialHost + Whitespace +
                                  Botnet.Globals.Arguments.Query + Query;
            }
            else
            {
                RunGameArgument = "-applaunch 4000" + Whitespace + Botnet.Globals.Arguments.GMOD + Whitespace +
                                  Whitespace + Botnet.Globals.Arguments.Query + Query;
            }

            string command = Botnet.Globals.Paths.SandboxiePath + Whitespace + "/box:" + account[0] + Whitespace +
                             Botnet.Globals.Paths.SteamPath + Whitespace + LoginArgument + Whitespace + Username +
                             Whitespace + Password + Whitespace + RunGameArgument;
            Console.WriteLine(command);
            ExecuteCommand(command);
        }

        public static void RunCommand(string[] account, string command)
        {
            var List = SearchSandboxieList(account[0]);
            if (List == null || List.Count == 0) return;

            bool GMODIsOpen = false;

            foreach (var processId in List)
            {
                try
                {
                    var process = Process.GetProcessById(int.Parse(processId));
                    if (process.ProcessName == "gmod")
                    {
                        GMODIsOpen = true;
                        break;
                    }
                }
                catch (ArgumentException)
                {
                    // The process with the specified process ID is not running.
                }
                catch (Exception ex)
                {
                    Console.WriteLine("Error checking process: " + ex.Message);
                }
            }

                if (!GMODIsOpen) return;

            StartGPlusRLE(account, command);
        }

        private static void StartGPlusRLE(string[] account, string args)
        {
            string Whitespace = " ";
            string command = "\"" + Botnet.Globals.Paths.SandboxiePath + Whitespace + "/box:" + account[0] +
                             Whitespace + Botnet.Globals.Paths.GPlusRemoteExecutor + Whitespace + "\"" + args +
                             "\"" + "\"";
            Console.WriteLine(command);
            ExecuteCommand(command);
        }

        public static void ConnectAllAvailableBots(string host)
        {
            var availableBots = GetAvailableBots();

            foreach (var account in availableBots)
            {
                ConnectBot(account, host);
            }
        }

        public static void ConnectBot(string[] account, string host)
        {
            if (account == null) return;
            Botnet.Controller.RegisterBot(account, host);
            RunCommand(account, "connect " + host);
            CheckStatus(account, "open");
        }

        public static void StartBotVM(string[] account, string host = null)
        {
            host = host ?? string.Empty;
            var query = Botnet.Globals.Arguments.QueryStart + Botnet.Helpers.GetTotalBots();
            StartBox(account, host, query);
        }

        public static void PrepareBots(int amount)
        {
            for (int i = 0; i < amount; i++)
            {
                StartBotVM(Botnet.Helpers.GetUnpreparedBots()[i]);
            }
        }

    }
}
