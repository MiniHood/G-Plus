Botnet.Controller.FileSystem.FileAccess = {}
Botnet.Controller.FileSystem.Bots = {}
Botnet.Controller.GPlusQueue = {}
local QueueRunning = true

local clock = os.clock
function sleep(n)  -- seconds
  local t0 = clock()
  while clock() - t0 <= n do end
end

local JSONFallback = function()
    return json.encode(Botnet.Helpers.GetAvailableBots())
end

Botnet.Controller.RegisterBot = function(account, host, userid)
    print('Registering ' .. account[1])
    Botnet.Controller.FileSystem.OpenFile(Botnet.Globals.Paths.BotData, 'r')
    local Cache = {}
    local Table = {}
    Cache['Content'] = Botnet.Controller.FileSystem.ReadFile(Botnet.Globals.Paths.BotData) or JSONFallback()
    Table = json.decode(Cache['Content'])
    if Table[account[1]] then Botnet.Controller.FileSystem.CloseFile(Botnet.Globals.Paths.BotData) return account end
    Botnet.Controller.FileSystem.ChangeIOType(Botnet.Globals.Paths.BotData, 'w')
    Table[account[1]] = {account, host, userid}
    Botnet.Controller.FileSystem.WriteFile(Botnet.Globals.Paths.BotData, json.encode(Table))
    Botnet.Controller.FileSystem.CloseFile(Botnet.Globals.Paths.BotData)
    return account, host
end

Botnet.Controller.UnregisterBot = function(account, disconnect)
    local _, Path = table.unpack(Botnet.Controller.FileSystem.OpenFile(Botnet.Globals.Paths.BotData, 'r'))
    local Cache = {}
    Cache['Content'] = Botnet.Controller.FileSystem.ReadFile(Path) or JSONFallback()
    local Table = json.decode(Cache['Content'])
    if not Table[account[1]] then if disconnect then Botnet.Helpers.DisconnectBot(account) end Botnet.Controller.FileSystem.CloseFile(Path) return true end
    Table[account[1]] = nil
    _, Path = table.unpack(Botnet.Controller.FileSystem.ChangeIOType(Path, 'w'))
    Botnet.Controller.FileSystem.WriteFile(Path, json.encode(Table))
    Botnet.Controller.FileSystem.CloseFile(Path)
    if disconnect then Botnet.Helpers.DisconnectBot(account) end
end

Botnet.Controller.GetBotsByUserID = function (userid)
    local Bot = Botnet.Controller.GetAllRegisteredBots()
    local UserIDBots = {}
    for key, value in pairs(Bot) do
        if Botnet.Controller.DoesBotBelongToo(value, userid) then
            table.insert(UserIDBots, value)
        end
    end
    return UserIDBots or nil
end

Botnet.Controller.GetUserIDByAccount = function (account)
    local Bot = Botnet.Controller.GetAllRegisteredBots()
    for key, value in pairs(Bot) do
        if value[1][1] == account[1] then
            return value[3]  -- Assuming the userid is at index 3 in the value table
        end
    end
    return nil  -- Return nil if the account is not found
end

Botnet.Controller.DoesBotBelongToo = function (account, userid)
    local Bot = Botnet.Controller.GetRegisteredBot(account[1])
    if not Bot then return end
    if Bot[3] == userid then return true end
    return false
end 

Botnet.Controller.GetAmountOfRegisteredBots = function()
    local index = 0
    for _, value in ipairs(Botnet.Accounts.LoginInformation) do
        if Botnet.Controller.GetRegisteredBot(value) then
            index = index + 1
        end
    end
    return index
end

Botnet.Controller.GetAllRegisteredBots = function()
    local UserIDBots = {}
    for _, value in pairs(Botnet.Accounts.LoginInformation) do
        local RegBot = Botnet.Controller.GetRegisteredBot(value)
        if RegBot then
            table.insert(UserIDBots, RegBot)
        end
    end
    return UserIDBots
end

Botnet.Controller.GetRegisteredBot = function (account)
    local _, Path = table.unpack(Botnet.Controller.FileSystem.OpenFile(Botnet.Globals.Paths.BotData, 'r'))
    local Content = Botnet.Controller.FileSystem.ReadFile(Path) or JSONFallback()
    local Table = json.decode(Content)
    local Result = nil
    for Key, Value_Table in pairs(Table) do
        if Value_Table[1][1] == account[1] then
            Botnet.Controller.FileSystem.CloseFile(Path)
            return Value_Table
        end
    end

    Botnet.Controller.FileSystem.CloseFile(Path)
    return nil
end

Botnet.Controller.AddToQueue = function(account, command) 
    table.insert(Botnet.Controller.GPlusQueue, {account, command})
end

Botnet.Controller.ProcessGPlusQueue = function()
    if #Botnet.Controller.GPlusQueue > 0 then
    for _, value in ipairs(Botnet.Controller.GPlusQueue) do
        while (Botnet.Helpers.IsGPlusRunning()) do
            sleep(2)
        end 

        local command = table.remove(Botnet.Controller.GPlusQueue[1], 2)
        local account = table.remove(Botnet.Controller.GPlusQueue[1], 1)
        print(command)
        table.remove(Botnet.Controller.GPlusQueue, 1)
        Botnet.Helpers.RunCommand(account, command)
    end
end
end

--[[
    Table is deserialized into this format
    ['bot_username'] = {
        account = {username, password},
        host,
        userid
    }
]]

Botnet.Controller.FileSystem.IsFileOpen = function (path)
    if table.HasValue(Botnet.Controller.FileSystem.FileAccess, path, 2) then return true end
    return false
end

Botnet.Controller.FileSystem.ChangeIOType = function(path, type)
    if not Botnet.Controller.FileSystem.IsFileOpen(path) then return Botnet.Controller.FileSystem.OpenFile(path, type) end
    Botnet.Controller.FileSystem.CloseFile(path)
    return Botnet.Controller.FileSystem.OpenFile(path, type)
end

Botnet.Controller.FileSystem.OpenFile = function (path, type)
    if table.HasValue(Botnet.Controller.FileSystem.FileAccess, path, 2) then  for _, value in ipairs(Botnet.Controller.FileSystem.FileAccess) do if value[2] == path then return value end end end
    local File = assert(io.open(path, type))
    if not File then error(path .. ' could not be opened') return false end
    table.insert(Botnet.Controller.FileSystem.FileAccess, {File, path})
    return {File, path}
end

Botnet.Controller.FileSystem.CloseFile = function(path)
    if table.HasValue(Botnet.Controller.FileSystem.FileAccess, path, 2) then  for index, value in ipairs(Botnet.Controller.FileSystem.FileAccess) do if value[2] == path then io.close(value[1]) Botnet.Controller.FileSystem.FileAccess[index] = nil return true end end end
    return false
end

Botnet.Controller.FileSystem.ReadFile = function (path)
    local File, _ = table.unpack(Botnet.Controller.FileSystem.OpenFile(path, 'r'))
    if not File then Botnet.Controller.FileSystem.CloseFile(path) return nil end
    local Content = File:read("*all")
    if not Content then Botnet.Controller.FileSystem.CloseFile(path) return nil end
    return Content
end

Botnet.Controller.FileSystem.WriteFile = function (path, data)
    local File, _ = table.unpack(Botnet.Controller.FileSystem.OpenFile(path, 'w'))
    if not File then Botnet.Controller.FileSystem.CloseFile(path) return nil end
    local Result = File:write(data)
    if not Result then Botnet.Controller.FileSystem.CloseFile(path) return nil end
    return Result
end

Botnet.Controller.FileSystem.RunFile = function(path)
    local File, _ = table.unpack(Botnet.Controller.FileSystem.OpenFile(path, 'r'))
    if not File then Botnet.Controller.FileSystem.CloseFile(path) return nil end
    local Result = assert(io.popen(path, 'r'))
    if not Result then Botnet.Controller.FileSystem.CloseFile(path) return nil end
    local DisposableOutput = Result:read('*all') -- I think we need this simply to run it as read
    Botnet.Controller.FileSystem.CloseFile(path)
    return Result
end