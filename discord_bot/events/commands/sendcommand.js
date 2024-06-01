import { checkAccess, runLuaCommand } from '../../objects.js';
import { FLAG } from '../../flags.js';
import { SlashCommandBuilder } from 'discord.js';

const create = () => {
    const command = new SlashCommandBuilder()
        .setName('sendcommand')
        .setDescription('Send a command to all bots')
        .addStringOption(option =>
            option.setName('command')
                .setDescription('Command to send')
                .setRequired(true)
        )
        .addStringOption(option =>
            option.setName('argument')
                .setDescription('Argument to send')
                .setRequired(true)
        )
    return command.toJSON();
};

const invoke = async (interaction) => {
    const user = interaction.user;
    const userId = user.id;
    const command = interaction.options.getString('command') || false;
    const argument = interaction.options.getString('argument') || false;
    const isAdmin = interaction.member?.permissions.has('ADMINISTRATOR');
    
    // Check access with 0 amount of bots
    const accessResult = await checkAccess(userId, 0);
    await interaction.deferReply({ ephemeral: true });

    if (userId) {
        try {
            if (accessResult === FLAG.ACCESS_GRANTED) {
                const luaOutput = await runLuaCommand(['sendcommand', command, argument, isAdmin]);
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