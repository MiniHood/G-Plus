import { SlashCommandBuilder } from 'discord.js';
import fs from 'fs';

const create = () => {
	const command = new SlashCommandBuilder()
		.setName('removeaccess')
		.setDescription('Remove access from a user')
		.addUserOption((option) =>
			option.setName('user').setDescription('User to remove access from')
			.setRequired(true)
		);

	return command.toJSON();
};

const invoke = async (interaction) => {
    const user = interaction.options.getUser('user');
    const id = user?.id;
    const isAdmin = interaction.member?.permissions.has('ADMINISTRATOR');
    await interaction.deferReply({ ephemeral: true });
    if (user !== null && isAdmin) {
        fs.readFile('user_data.json', 'utf8', (err, data) => {
            if (err) {
                console.error(err);
                return;
            }
            
            let jsonData = JSON.parse(data);

            // Check if jsonData is an object
            if (typeof jsonData === 'object') {
                // Check if the user exists in the data
                if (jsonData[id]) {
                    delete jsonData[id]; // Remove the user's data

                    // Check if jsonData will be empty after removing the user
                    const isEmpty = Object.keys(jsonData).length === 0;

                    fs.writeFile('user_data.json', isEmpty ? '{}' : JSON.stringify(jsonData, null, 2), (err) => {
                        if (err) {
                            console.error(err);
                            return;
                        }
                        interaction.editReply({ content: `Access was removed from ${user}`, ephemeral: true });
                    });
                } else {
                    interaction.editReply({ content: 'User not found in access data.', ephemeral: true });
                }
            } else {
                interaction.editReply({ content: 'No access data found.', ephemeral: true });
            }
        });
    } else {
        interaction.editReply({ content: 'Error: User not specified or you don\'t have permission to do this.', ephemeral: true });
    }
};

export { create, invoke };