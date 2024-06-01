import { checkAccess, runLuaCommand } from '../../objects.js';
import { FLAG } from '../../flags.js';
import { SlashCommandBuilder } from 'discord.js';

const create = () => {
    const command = new SlashCommandBuilder()
        .setName('prepare')
        .setDescription('Prepare a number of bots for connection.')
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
    const amountOfBots = interaction.options.getInteger('amount_of_bots');
    await interaction.deferReply({ ephemeral: true });

    if (userId) {
        try {
            const accessResult = await checkAccess(userId, amountOfBots);
            if (accessResult === FLAG.ACCESS_GRANTED) {
                const luaOutput = await runLuaCommand(['prepare', amountOfBots]);
                const usefulOutput = luaOutput.slice(3).map(item => item.replace(/,/g, ''));
                interaction.editReply({ content: `${usefulOutput}`, ephemeral: true });
            } else if (accessResult === FLAG.EXCEEDED_LIMIT) {
                interaction.editReply({ content: 'Requested bots exceed the maximum allowed.', ephemeral: true });
            } else {
                interaction.editReply({ content: 'Access denied or user not found.', ephemeral: true });
            }
        } catch (error) {
            console.error(`Error executing Lua command: ${error}`);
            interaction.editReply({ content: `Error executing Lua command: ${error}`, ephemeral: true });
        }
    } else {
        interaction.editReply({ content: 'Error: User not specified.', ephemeral: true });
    }
};

export { create, invoke };