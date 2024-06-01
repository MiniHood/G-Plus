import { checkAccess, runLuaCommand } from '../../objects.js';
import { FLAG } from '../../flags.js';
import { SlashCommandBuilder } from 'discord.js';

const create = () => {
    const command = new SlashCommandBuilder()
        .setName('data')
        .setDescription('Get data about unprepared, available and connected bots.')
    return command.toJSON();
};

const invoke = async (interaction) => {
    const user = interaction.user;
    const userId = user.id;

    if (userId) {
        try {
            await interaction.deferReply({ ephemeral: true });
            const accessResult = await checkAccess(userId, 0); // Check access with 0 amount of bots
            if (accessResult === FLAG.ACCESS_GRANTED) {
                const luaOutput = await runLuaCommand(['data', userId]);
                const usefulOutput = luaOutput.slice(3).map(item => item.replace(/,/g, ''));
                
                interaction.editReply({ content: `${usefulOutput.join('\n')}`, ephemeral: true });
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