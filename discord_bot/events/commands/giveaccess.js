import { ButtonBuilder, ButtonStyle, SlashCommandBuilder, ActionRowBuilder, ComponentType } from 'discord.js';
import fs from 'fs';

const create = () => {
	const command = new SlashCommandBuilder()
		.setName('giveaccess')
		.setDescription('Give access to a user')
		.addUserOption((option) =>
			option.setName('user').setDescription('User to give access to')
			.setRequired(true)
		)
		.addIntegerOption((option) =>
			option.setName('max_bots').setDescription('Maximum amount of bots user can use')
			.setRequired(true)
		)
		.addStringOption((option) =>
			option.setName('time_limit').setDescription('Time limit in days for access removal')
			.setRequired(true)
		);

	return command.toJSON();
};

const parseTimeLimit = (timeLimitInput) => {
    if (timeLimitInput) {
        const timeRegex = /^(\d+)([smhd])$/; // Regex to match digits followed by s/m/h/d
        const match = timeLimitInput.match(timeRegex);

        if (match) {
            const value = parseInt(match[1]);
            const unit = match[2];

            switch (unit) {
                case 's':
                    return value * 1000; // seconds to milliseconds
                case 'm':
                    return value * 60 * 1000; // minutes to milliseconds
                case 'h':
                    return value * 60 * 60 * 1000; // hours to milliseconds
                case 'd':
                    return value * 24 * 60 * 60 * 1000; // days to milliseconds
                default:
                    return null;
            }
        } else {
            return null;
        }
    } else {
        return null;
    }
};

const invoke = async (interaction) => {
    const user = interaction.options.getUser('user');
    const id = user?.id;
    const maxBots = interaction.options.getInteger('max_bots');
    const timeLimitInput = interaction.options.getString('time_limit');
    const timeLimit = parseTimeLimit(timeLimitInput);
    const isAdmin = interaction.member?.permissions.has('ADMINISTRATOR');
    const isReseller = interaction.member?.roles.cache.some(role => role.name === 'Reseller');
    const expirationDate = Date.now() + timeLimit; // Calculate expiration date in milliseconds
    await interaction.deferReply({ ephemeral: true });

    if (isReseller && !isAdmin) {
        // Check if user is a reseller but not an admin
        // Send confirmation message to a specific channel
        const confirmationChannel = interaction.guild.channels.cache.find(channel => channel.name === 'confirmation');
        if (confirmationChannel) {
            const confirm = new ButtonBuilder()
            .setCustomId('confirm_button')
            .setStyle(ButtonStyle.Primary) // Use ButtonStyle constants for setting the style
            .setLabel('Confirm');
        
        const deny = new ButtonBuilder()
            .setCustomId('deny_button')
            .setStyle(ButtonStyle.Danger) // Use ButtonStyle constants for setting the style
            .setLabel('Deny');

        const row = new ActionRowBuilder()
			.addComponents(confirm, deny);

        const response = await confirmationChannel.send({
            content: `User ${id} is requesting to give access to ${user} until ${new Date(expirationDate)}\nIs Reseller: ${isReseller}\nMax Bots: ${maxBots}.\n\nReact with ✅ to confirm or ❌ to deny.`,
            components: [row]
        });            

        const collector = response.createMessageComponentCollector({ componentType: ComponentType.Button, time: 3_600_000 });

        collector.on('collect', async i => {
            if (i.customId == "confirm_button") {
                await fs.readFile('user_data.json', 'utf8', async (err, data) => {
                    if (err) {
                        console.error(err);
                        return;
                    }
                    
                    let jsonData = JSON.parse(data);
            
                    // Check if jsonData is an object, if not initialize it as an object
                    if (typeof jsonData !== 'object') {
                        jsonData = {};
                    }
            
                    // Update user data with max bots, time limit, and expiration date
                    jsonData[id] = { maxBots, expirationDate };
            
                    await fs.writeFile('user_data.json', JSON.stringify(jsonData, null, 2), (err) => {
                        if (err) {
                            console.error(err);
                            return;
                        }
            
                        interaction.editReply({ content: `Access was given to ${user} until ${new Date(expirationDate)}`, ephemeral: true });
                    });
                });
            } else {
                interaction.editReply({ content: `Access for ${user} until ${new Date(expirationDate)} was denied.`, ephemeral: true });
            }
        });
        }
    } else if (user !== null && timeLimit !== null && (isAdmin || isReseller)) {
        fs.readFile('user_data.json', 'utf8', (err, data) => {
            if (err) {
                console.error(err);
                return;
            }
            
            let jsonData = JSON.parse(data);

            // Check if jsonData is an object, if not initialize it as an object
            if (typeof jsonData !== 'object') {
                jsonData = {};
            }

            // Update user data with max bots, time limit, and expiration date
            jsonData[id] = { maxBots, expirationDate };

            fs.writeFile('user_data.json', JSON.stringify(jsonData, null, 2), (err) => {
                if (err) {
                    console.error(err);
                    return;
                }

                interaction.editReply({ content: `Access was given to ${user} until ${new Date(expirationDate)}`, ephemeral: true });
            });
        });
    } else {
        interaction.editReply({ content: `Error: Invalid time limit format, user not specified or you don\'t have permission to do this.`, ephemeral: true });
    }
};

export { create, invoke };

