import { checkAccess, runLuaCommand } from '../../objects.js';
import { FLAG } from '../../flags.js';
import { SlashCommandBuilder } from 'discord.js';

const create = () => {
    const command = new SlashCommandBuilder()
        .setName('connect')
        .setDescription('Connect bots to a server')
        .addStringOption(option =>
            option.setName('server_ip')
                .setDescription('Server IP to connect bots to')
                .setRequired(true)
        )
        .addIntegerOption(option =>
            option.setName('amount_of_bots')
                .setDescription('Number of bots to connect')
                .setRequired(true)
        );

    return command.toJSON();
};

const invoke = async (interaction) => {
    const user = interaction.user;
    const userId = user.id;
    const serverIp = interaction.options.getString('server_ip');
    const amountOfBots = interaction.options.getInteger('amount_of_bots');
    await interaction.deferReply();

    if (userId) {
        try {
            const accessResult = await checkAccess(userId, amountOfBots);
            if (accessResult === FLAG.ACCESS_GRANTED) {
                const luaOutput = await runLuaCommand(['connect', serverIp, amountOfBots, userId]);
                const usefulOutput = luaOutput.slice(3).map(item => item.replace(/,/g, ''));
                interaction.editReply({ content: `${usefulOutput}` });
            } else if (accessResult === FLAG.EXCEEDED_LIMIT) {
                interaction.editReply({ content: 'Requested bots exceed the maximum allowed.' });
            } else {
                interaction.editReply({ content: 'Access denied or user not found.' });
            }
        } catch (error) {
            console.error(`Error executing Lua command: ${error}`);
            interaction.editReply({ content: `Error executing Lua command: ${error}` });
        }
    } else {
        interaction.editReply({ content: 'Error: User not specified.' });
    }
};

export { create, invoke };