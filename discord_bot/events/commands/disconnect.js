import { checkAccess, runLuaCommand } from '../../objects.js';
import { FLAG } from '../../flags.js';
import { SlashCommandBuilder } from 'discord.js';

const create = () => {
    const command = new SlashCommandBuilder()
        .setName('disconnect')
        .setDescription('Disconnect bots from a server or forcefully shut down their VM')
        .addStringOption(option =>
            option.setName('account_name')
                .setDescription('Account name of the bot to disconnect')
                .setRequired(true)
        )
        .addBooleanOption(option =>
            option.setName('force')
                .setDescription('Force disconnect')
        )
        .addBooleanOption(option =>
            option.setName('disconnect_all')
                .setDescription('Disconnect all bots')
        )

    return command.toJSON();
};

const invoke = async (interaction) => {
    const user = interaction.user;
    const userId = user.id;
    const accountName = interaction.options.getString('account_name');
    const force = interaction.options.getBoolean('force') || false;
    const disconnectAll = interaction.options.getBoolean('disconnect_all') || false;
    const isAdmin = interaction.member?.permissions.has('ADMINISTRATOR');
    
    // Check access with 0 amount of bots
    const accessResult = await checkAccess(userId, 0);
    await interaction.deferReply({ ephemeral: true });

    if (userId) {
        try {
            if (accessResult === FLAG.ACCESS_GRANTED) {
                const luaOutput = await runLuaCommand(['disconnect', accountName, force, disconnectAll, isAdmin, userId]);
                const usefulOutput = luaOutput.slice(3).map(item => item.replace(/,/g, ''));
                interaction.editReply({ content: `${usefulOutput}`, ephemeral: true });
                return
            } else if (accessResult === FLAG.EXCEEDED_LIMIT) {
                interaction.editReply({ content: 'Requested bots exceed the maximum allowed.', ephemeral: true });
                return
            } else {
                interaction.editReply({ content: 'Access denied or user not found.', ephemeral: true });
                return
            }
        } catch (error) {
            console.error(`Error executing Lua command: ${error}`);
            interaction.editReply({ content: `Error executing Lua command: ${error}`, ephemeral: true });
            return;
        }
    } else {
        interaction.editReply({ content: 'Error: User not specified.', ephemeral: true });
        return;
    }
    interaction.editReply({ content: 'Something went wrong.', ephemeral: true });
};

export { create, invoke };