// C# code with removed functions that are already included in the .NET framework

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using Newtonsoft.Json;

namespace Botnet
{
    public class Controller
    {
        public static List<Tuple<Stream, string>> FileAccess = new List<Tuple<Stream, string>>();
        public static Dictionary<string, object> Bots = new Dictionary<string, object>();

        public static void RegisterBot(string account, string host)
        {
            Console.WriteLine("Registering " + account);
            var path = Botnet.Globals.Paths.BotData;
            var cache = new Dictionary<string, object>();
            var table = new Dictionary<string, object>();
            var content = ReadFile(path) ?? JSONFallback();
            table = JsonConvert.DeserializeObject<Dictionary<string, object>>(content);
            if (table.ContainsKey(account)) CloseFile(Globals.Paths.BotData);
            ChangeIOType(Botnet.Globals.Paths.BotData, 'w');
            table[account] = new List<object> { account, host };
            WriteFile(Botnet.Globals.Paths.BotData, JsonConvert.SerializeObject(table));
            CloseFile(Botnet.Globals.Paths.BotData);
        }

        private static string JSONFallback()
        {
            return JsonConvert.SerializeObject("{[\"fallback\": \"fallback\"]}");
        }

        public static bool UnregisterBot(string account, bool disconnect)
        {
            Console.WriteLine("Unregistering " + account);
            var path = Botnet.Globals.Paths.BotData;
            var cache = new Dictionary<string, object>();
            var content = ReadFile(path) ?? JSONFallback();
            var table = JsonConvert.DeserializeObject<Dictionary<string, object>>(content);
            if (!table.ContainsKey(account))
            {
                if (disconnect) Helpers.DisconnectBot(account);
                CloseFile(path);
                return true;
            }
            table.Remove(account);
            ChangeIOType(path, 'w');
            WriteFile(path, JsonConvert.SerializeObject(table));
            CloseFile(path);
            if (disconnect) Helpers.DisconnectBot(account);
            return true;
        }

        public static int GetAmountOfRegisteredBots()
        {
            var index = 0;
            foreach (var value in Botnet.Accounts.LoginInformation)
            {
                if (GetRegisteredBot(value) != null)
                {
                    index++;
                }
            }
            return index;
        }

        public static object GetRegisteredBot(string account)
        {
            var path = Botnet.Globals.Paths.BotData;
            var content = ReadFile(path) ?? JSONFallback();
            var table = JsonConvert.DeserializeObject<Dictionary<string, object>>(content);
            object result = null;
            foreach (var pair in table)
            {
                var acc = ((List<object>)pair.Value)[0];
                if (acc == account)
                {
                    result = pair.Value;
                }
            }
            CloseFile(path);
            return result;
        }

        public static bool IsFileOpen(string path)
        {
            return FileAccess.Any(x => x.Item2 == path);
        }

        public static void ChangeIOType(string path, char type)
        {
            if (!IsFileOpen(path)) OpenFile(path, type);
            CloseFile(path);
            OpenFile(path, type);
        }

        public static Tuple<Stream, string> OpenFile(string path, char type)
        {
            if (IsFileOpen(path))
            {
                foreach (var value in FileAccess)
                {
                    if (value.Item2 == path)
                    {
                        return value;
                    }
                }
            }
            var file = File.Open(path, type == 'r' ? FileMode.Open : FileMode.Create);
            if (file == null)
            {
                throw new Exception(path + " could not be opened");
            }
            FileAccess.Add(new Tuple<Stream, string>(file, path));
            return new Tuple<Stream, string>(file, path);
        }

        public static void CloseFile(string path)
        {
            if (IsFileOpen(path))
            {
                for (int i = 0; i < FileAccess.Count; i++)
                {
                    if (FileAccess[i].Item2 == path)
                    {
                        FileAccess[i].Item1.Close();
                        FileAccess.RemoveAt(i);
                        return;
                    }
                }
            }
        }

        public static string ReadFile(string path)
        {
            var file = OpenFile(path, 'r');
            if (file == null)
            {
                CloseFile(path);
                return null;
            }
            var content = new StreamReader(file.Item1).ReadToEnd();
            if (content == null)
            {
                CloseFile(path);
                return null;
            }
            return content;
        }

        public static bool WriteFile(string path, string data)
        {
            var file = OpenFile(path, 'w');
            if (file == null)
            {
                CloseFile(path);
                return false;
            }
            var writer = new StreamWriter(file.Item1);
            writer.Write(data);
            writer.Close();
            writer.Write(data);
            writer.Close();
            return true;
        }

        public static object RunFile(string path)
        {
            var file = OpenFile(path, 'r');
            if (file == null)
            {
                CloseFile(path);
                return null;
            }
            var result = File.OpenRead(path);
            if (result == null)
            {
                CloseFile(path);
                return null;
            }
            var disposableOutput = new StreamReader(result).ReadToEnd();
            CloseFile(path);
            return result;
        }
    }
}