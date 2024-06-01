Botnet.Commands = Botnet.Commands or {}

function Botnet.Commands.Prepare(Args)
    local amount = tonumber(Args[2])  -- Ensure amount is a number

    if not amount then
        return
    end

    if amount > #Botnet.Helpers.GetUnpreparedBots() then
        print('We only have ' .. #Botnet.Helpers.GetUnpreparedBots() .. ' more unprepared bots left.')
        return
    end

    Botnet.Helpers.PrepareBots(amount)
end

function Botnet.Commands.Unregister(Args)
    local accountName = Args[2]  -- Ensure amount is a number

    for _, value in ipairs(Botnet.Accounts.LoginInformation) do
      if value[1] == accountName then accountName = Botnet.Accounts.LoginInformation[_] end
    end
  
    Botnet.Controller.UnregisterBot(accountName)

end

function Botnet.Commands.Connect(Args)
    local amount = Args[3]
    local host = Args[2]
    amount = tonumber(Args[3]) or amount
    local userid = Args[4]
    local IsAdmin = Args[5]

    if not (host or amount or userid) then
        return
    end
    if (type(amount) == 'number') and amount > #Botnet.Helpers.GetAvailableBots() then print('Not enough available bots for ' .. amount .. ' to be connected. Please use the prepare command to prepare more bots.') return end

    local Cache = {}

    if type(amount) == "string" then
        Botnet.Helpers.ConnectBot(amount, host, userid, IsAdmin)
    else
        for i = 1, amount do
            Cache['cache'] = Botnet.Helpers.GetAvailableBots()[math.random(1, #Botnet.Helpers.GetAvailableBots())]
            if not Botnet.Helpers.IsBotConnected(Cache['cache']) then
                Botnet.Helpers.ConnectBot(Cache['cache'], host, userid, IsAdmin)
            end
        end
    end


end

function Botnet.Commands.CheckAccess(Args)
    local amountRequested = Args[2]
    
    if tonumber(amountRequested) == 0 then
        print('ACCESS_GRANTED')
    end

    if not tonumber(amountRequested) then amountRequested = 1 end
    local userid = Args[3] 
    local maxallowed = tonumber(Args[4]) 

    if not (amountRequested or userid) then
        return
    end
    if #Botnet.Controller.GetBotsByUserID(userid) + amountRequested > maxallowed then print('EXCEEDED_LIMIT') return end
    print('ACCESS_GRANTED')
end

function Botnet.Commands.SendCommandsToAll(Args)
    local Command = Args[2]
    local Arguments = Args[3]
    local IsAdmin = Args[4]

    if toboolean(IsAdmin) ~= true then
        print('You cannot access this command.')
        return
    end

    for index, value in ipairs(Botnet.Controller.GetAllRegisteredBots()) do
      print('Sent command to ' .. value[1][1])
      Botnet.Controller.AddToQueue(value[1], Command .. ' ' .. "\"" .. Arguments .. "\"")
      Botnet.Controller.ProcessGPlusQueue()    
    end
end

function Botnet.Commands.Disconnect(Args)
    local AccountName = Args[2] or nil
    local userid = Args[6]
    local Args1, Args2 = Args[3] or nil, Args[4] or nil 
    local IsAdmin = Args[5]
    local force = false

    if not (AccountName) then
        print('No account name provided, what?')
        return
    end

    if Args1 == 'true' and IsAdmin then
        force = true
    end

    if Args2 == 'true' and IsAdmin then
        Botnet.Helpers.DisconnectAllBots(force, userid)
    else
        Botnet.Helpers.DisconnectBot(AccountName, force, userid)
    end

end

function Botnet.Commands.Data(Args)
    for _, value in ipairs(Botnet.Helpers.GetAvailableBots()) do
        if value[1] then
            print(value[1] .. ' is available to connect.')
        end
    end

    for _, value in ipairs(Botnet.Controller.GetBotsByUserID(Args[2])) do
        print(value[1][1] .. ' is connected to ' .. Botnet.Controller.GetRegisteredBot(value[1])[2])
    end

    for _, value in ipairs(Botnet.Helpers.GetUnpreparedBots()) do
        if value[1] then
            print(value[1] .. ' has not been prepared yet.')
        end
    end

    print('\nOverall Stats\n' .. #Botnet.Controller.GetBotsByUserID(Args[2])..' connected to servers.\n'.. #Botnet.Helpers.GetAvailableBots() .. ' available to connect.\n'..#Botnet.Helpers.GetUnpreparedBots()..' unprepared bots.')
end
