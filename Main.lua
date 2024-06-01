local Answer = nil
local Args = nil
local RawCommand = nil

table.unpack = table.unpack or unpack

dofile('../json.lua')
dofile('../Variables.lua')
dofile('../Globals.lua')
dofile('../Objects.lua')
dofile('../Accounts.lua')
dofile('../Helpers.lua')
dofile('../Controller.lua')
dofile('../Commands.lua')

Botnet.Helpers.SetAllSandboxieSettings()


io.write(Botnet.Helpers.GetAmountOfAvailableBots() .. "/" .. #Botnet.Accounts.LoginInformation .. ' Available')
io.write('\n'..Botnet.Controller.GetAmountOfRegisteredBots() .. ' Registered')


-- Now Args should be the rest of the Answer excluding the command
Args = {select(1, table.unpack(arg))}
RawCommand = Args[1]
print('\n'..RawCommand)

Botnet.Controller.ProcessGPlusQueue()

local cmd_table = 
{
    ['connect'] = function ()
        Botnet.Commands.Connect(Args)
    end,

    ['prepare'] = function ()
        Botnet.Commands.Prepare(Args)
    end,

    ['unregister'] = function()
        Botnet.Commands.Unregister(Args)
    end,

    ['disconnect'] = function()
        Botnet.Commands.Disconnect(Args)
    end,

    ['data'] = function()
        Botnet.Commands.Data(Args)
    end,

    ['checkaccess'] = function()
        Botnet.Commands.CheckAccess(Args)
    end,

    ['sendcommand'] = function()
        Botnet.Commands.SendCommandsToAll(Args)
    end
}

if not cmd_table[RawCommand] then
    print('\nThat\'s not a command.')
else
    cmd_table[RawCommand]()
end

