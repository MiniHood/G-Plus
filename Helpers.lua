-- Registered means they are connecting or connected to a server
-- In any of the Accounts tables apart from login information means the VM is open but connection status in a server is unknown, hence we use registered accounts to find out.

local Queue = {}

function toboolean(str)
  local bool = false
  if str == "true" then
      bool = true
  end
  return bool
end

local SearchSandboxieList = function(accountName, specificID)
  specificID = specificID or nil
  
  local InputOutput = assert(io.popen(Botnet.Globals.Paths.SandboxiePath .. ' /box:' .. accountName .. '  /listpids | more', 'r'))
  local Output = InputOutput:lines()
  local CleanedOutput = {}

  if not (InputOutput or Output) then
    return 0
  end

  for line in Output do
    if line ~= '0' and line ~= '' then
      table.insert(CleanedOutput, line)
    end
  end

  if (specificID ~= nil) and (table.HasValue(CleanedOutput, specificID)) then
    return specificID
  elseif (specificID ~= nil) and not (table.HasValue(CleanedOutput, specificID)) then
    return false
  end

  return CleanedOutput
end

Botnet.Helpers.IsGarrysModOpen = function (account)
  local List = SearchSandboxieList(account[1])
  if type(List) ~= "table" then return end
  if #List == 0 then return end

  local GMODIsOpen = false

  for _, v in ipairs(List) do
    Process = winapi.process_from_id(v)
    if (Process) and Process:get_process_name(false) == 'gmod.exe' then
      GMODIsOpen = true
    end
  end

  return GMODIsOpen  
end

Botnet.Helpers.IsGPlusRunning = function ()
  for index, account in ipairs(Botnet.Helpers.GetAvailableBots()) do
    local List = SearchSandboxieList(account[1])
    if type(List) ~= "table" then return end
    if #List == 0 then return end
  
    local IsGPlusRunning = false
  
    for _, v in ipairs(List) do
      Process = winapi.process_from_id(v)
      if (Process) and Process:get_process_name(false) == 'GPlus.exe' then
        IsGPlusRunning = true
      end
    end
  
    return IsGPlusRunning  
  end
end

local CheckStatus = function(Account, ExpectedStatus)
  local IsActive = Botnet.Helpers.IsBotAvailable(Account) or Botnet.Helpers.IsBotConnected(Account)
  if not IsActive and ExpectedStatus == 'open' then Botnet.Helpers.FlagError(0x01) Botnet.Helpers.StopBox(Account) return end
  return true
end

Botnet.Helpers.DumpTable = function (o)
    if type(o) == 'table' then
       local s = '{ '
       for k,v in pairs(o) do
          if type(k) ~= 'number' then k = '"'..k..'"' end
          s = s .. '['..k..'] = ' .. Botnet.Helpers.DumpTable(v) .. ','
       end
       return s .. '} '
    else
       return tostring(o)
    end
end

Botnet.Helpers.FlagError = function(
  ErrorCode)
 print("[ERROR] " .. ErrorCode)
end

Botnet.Helpers.SetAllSandboxieSettings = function()
  for _, account in ipairs(Botnet.Accounts.LoginInformation) do
      os.execute("\"" .. Botnet.Globals.Paths.SandboxieBoxCreator .. "\"" .. " set " .. account[1] .. ' ConfigLevel 7')
      os.execute("\"" .. Botnet.Globals.Paths.SandboxieBoxCreator .. "\"" .. " set " .. account[1] .. ' Enabled y')
      os.execute("\"" .. Botnet.Globals.Paths.SandboxieBoxCreator .. "\"" .. " set " .. account[1] .. ' AutoDelete n')
      os.execute("\"" .. Botnet.Globals.Paths.SandboxieBoxCreator .. "\"" .. " set " .. account[1] .. ' BorderColor #00FFFF')
      os.execute("\"" .. Botnet.Globals.Paths.SandboxieBoxCreator .. "\"" .. " set " .. account[1] .. ' BoxNameTitle y')
      os.execute("\"" .. Botnet.Globals.Paths.SandboxieBoxCreator .. "\"" .. " set " .. account[1] .. ' AutoRecover y')
      os.execute("\"" .. Botnet.Globals.Paths.SandboxieBoxCreator .. "\"" .. " set " .. account[1] .. ' PromptForFileMigration n')
      os.execute("\"" .. Botnet.Globals.Paths.SandboxieBoxCreator .. "\"" .. " set " .. account[1] .. ' CopyLimitKb 5000000')
      os.execute("\"" .. Botnet.Globals.Paths.SandboxieBoxCreator .. "\"" .. " set " .. account[1] .. ' Template AutoRecoverIgnore')
  end
end

Botnet.Helpers.GetTotalBots = function ()
  return (#Botnet.Accounts.LoginInformation - Botnet.Helpers.GetAmountOfAvailableBots())
end

Botnet.Helpers.GetAmountOfAvailableBots = function()
  return #Botnet.Helpers.GetAvailableBots()
end

Botnet.Helpers.IsBotAvailable = function (account)
  if table.HasValue(Botnet.Helpers.GetAvailableBots(), account) then return true end
  return false
end

Botnet.Helpers.IsBotConnected = function (account)
  if table.HasValue(Botnet.Helpers.GetConnectedBots(), account) then return true end
  return false
end

Botnet.Helpers.IsBotUnprepared = function (account)
  if table.HasValue(Botnet.Helpers.GetUnpreparedBots(), account) then return true end
  return false
end

Botnet.Helpers.GetAvailableBots = function() -- Bot is not connected to server yet GMOD is running.
  local Bots = Botnet.Accounts.LoginInformation or {}

  local NonAvailableBots = {}
  local AvailableBoxes = {}


  for _, v in pairs(Bots) do
    if not table.HasValue(AvailableBoxes, v) then
      table.insert(NonAvailableBots, v)
    end
  end

  for _, v in pairs(Bots) do
    if Botnet.Helpers.IsGarrysModOpen(v) and not Botnet.Controller.GetRegisteredBot(v) then
      table.insert(AvailableBoxes, v)
    end
  end

  return AvailableBoxes
end

Botnet.Helpers.GetConnectedBots = function() -- GMOD Started, Connected to a server
  local Bots = Botnet.Accounts.LoginInformation or {}

  local AvailableBots = {}


  for _, v in pairs(Bots) do
    if Botnet.Helpers.IsGarrysModOpen(v) and Botnet.Controller.GetRegisteredBot(v) then
      table.insert(AvailableBots, v)
    end
  end

  return AvailableBots
end

Botnet.Helpers.GetUnpreparedBots = function() -- Neither GMOD is started nor are they connected to a server
  local Bots = Botnet.Accounts.LoginInformation or {}

  local AvailableBots = {}

  for _, v in pairs(Bots) do
    if not Botnet.Helpers.IsGarrysModOpen(v) and not Botnet.Controller.GetRegisteredBot(v) then
      table.insert(AvailableBots, v)
    end
  end

  return AvailableBots
end

Botnet.Helpers.DisconnectAllBots = function(force, userid)
  force = force or false
  for index, value in ipairs(Botnet.Accounts.LoginInformation) do
    Botnet.Helpers.DisconnectBot(value, force, userid)
  end
end

Botnet.Helpers.DisconnectBot = function(account, force, userid)

  if type(account) == 'string' then
    for _, value in ipairs(Botnet.Accounts.LoginInformation) do
      if value[1] == account then account = Botnet.Accounts.LoginInformation[_] end
    end
  end

  force = force or false

  if force then
    Botnet.Helpers.StopBox(account)
    Botnet.Controller.UnregisterBot(account)
    print('Forcefully disconnected ' .. account[1])
    return true
  end
  
  if userid ~= Botnet.Controller.GetUserIDByAccount(account) then
      return false
  end

  print('Disconnecting ' .. account[1])

  Botnet.Controller.AddToQueue(account, "disconnect")
  Botnet.Controller.UnregisterBot(account)
  Botnet.Controller.ProcessGPlusQueue()

  CheckStatus(account, 'close')
end

Botnet.Helpers.StopBox = function(account)
  Botnet.Controller.UnregisterBot(account, false, userid)
  os.execute("\"" .. Botnet.Globals.Paths.SandboxiePath .. "\"" .. ' /box:' .. account[1] .. ' /terminate')
end

local StartBox = function(account, initialHost, Query)
  --- "C:\Program Files\Sandboxie-Plus\Start.exe" "C:\Program Files (x86)\Steam\steam.exe" -login Co7Te6Oq7Fl7 Mp4St0Gq6Dn0 -applaunch 4000 -novid -nosound -nojoy -noipx -noshaderapi -nopix -nopreload -nod3d9ex -low -textmode +connect 166.1.173.183 QueryPort=Num
  local LoginArgument = "-login"
  local Username = account[1]
  local Password = account[2]
  local RunGameArgument = nil
  local Whitespace = " "

  if Botnet.Helpers.IsBotAvailable(account) or Botnet.Helpers.IsBotConnected(account) then return end

  if initialHost then
    RunGameArgument = "-applaunch 4000" .. Whitespace .. Botnet.Globals.Arguments.GMOD .. Whitespace .. Botnet.Globals.Arguments.Connect .. Whitespace .. initialHost .. Whitespace .. Botnet.Globals.Arguments.Query .. Query
  else
    RunGameArgument = "-applaunch 4000" .. Whitespace .. Botnet.Globals.Arguments.GMOD .. Whitespace .. Whitespace .. Botnet.Globals.Arguments.Query .. Query
  end

  local command = Botnet.Globals.Paths.SandboxiePath .. Whitespace .. '/box:' .. account[1] .. Whitespace ..  Botnet.Globals.Paths.SteamPath .. Whitespace .. LoginArgument .. Whitespace .. Username .. Whitespace .. Password .. Whitespace .. RunGameArgument
  os.execute(command)
end

local StartGPlusRLE = function (account, args)
  local command = "\"" .. Botnet.Globals.Paths.SandboxiePath .. " /box:" .. account[1] .. " " .. Botnet.Globals.Paths.GPlusRemoteExecutor .. " \"" .. args .. "\""
  os.execute(command)
end

Botnet.Helpers.RunCommand = function(account, command)
  local List = SearchSandboxieList(account[1])
  if type(List) ~= "table" then return end
  if #List == 0 then return end

  local GMODIsOpen = false

  for i, v in ipairs(List) do
    Process = winapi.process_from_id(v)
    if (Process) and Process:get_process_name(false) == 'gmod.exe' then
      GMODIsOpen = true
    end
  end

  if not GMODIsOpen then return end

  StartGPlusRLE(account, command)
end

Botnet.Helpers.ConnectAllAvailableBots = function (host)
  local AvailableBots = Botnet.Helpers.GetAvailableBots()

  for _, account in ipairs(AvailableBots) do
    Botnet.Helpers.ConnectBot(account, host)
  end
end

Botnet.Helpers.ConnectBot = function(account, host, userid, IsAdmin)
  if not account then return end

  if type(account) == 'string' then
    for _, value in ipairs(Botnet.Accounts.LoginInformation) do
      if value[1] == account then account = Botnet.Accounts.LoginInformation[_] end
    end
  end

  if userid ~= Botnet.Controller.GetUserIDByAccount(account) and Botnet.Controller.GetRegisteredBot(account) then
    print('You are attempting to connect a bot that you cannot access right now, it may be connected to another server. Please use /data to check available bots.')
    return false
end

  print('Connecting '..account[1]..' to ' .. host)
  Botnet.Controller.RegisterBot(account, host, userid)
  Botnet.Controller.AddToQueue(account, "connect " .. host .. ";cl_downloadfilter mapsonly")
  Botnet.Controller.ProcessGPlusQueue()
  CheckStatus(account, 'open')
end

Botnet.Helpers.StartBotVM = function(account, host) -- TODO
  host = host or nil
  local Query = Botnet.Globals.Arguments.QueryStart + Botnet.Helpers.GetTotalBots()
  StartBox(account, host, Query)
end

Botnet.Helpers.PrepareBots = function(amount)
  for i = 1, amount, 1 do
    print('Preparing ' .. Botnet.Helpers.GetUnpreparedBots()[i][1])
    Botnet.Helpers.StartBotVM(Botnet.Helpers.GetUnpreparedBots()[i])
  end
end