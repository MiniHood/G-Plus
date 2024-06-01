import { SlashCommandBuilder, EmbedBuilder } from 'discord.js';
import fs from 'fs';
import { checkAccess } from '../../objects.js';
import { FLAG } from '../../flags.js';

const create = () => {
    const command = new SlashCommandBuilder()
        .setName('review')
        .setDescription('Write a review')
        .addIntegerOption((option) =>
            option.setName('stars').setDescription('Rating out of 8').setRequired(true)
        )
        .addStringOption((option) =>
            option.setName('review').setDescription('Your review').setRequired(true)
        )
        .addBooleanOption((option) =>
            option.setName('anonymous').setDescription('Stay anonymous').setRequired(true)
        );

    return command.toJSON();
};

const starLinks = {
    1: "https://cdn.discordapp.com/attachments/1239914709872934973/1239915974904516668/1-10.png?ex=6644a93c&is=664357bc&hm=ec0376d24648e1d8e9cbef450d28bf85f42f1ad54ef1092a65f1b86edf84b906&",
    2: "https://cdn.discordapp.com/attachments/1239914709872934973/1239916086921789450/2-10.png?ex=6644a957&is=664357d7&hm=9dc361c6a0fa06843213de346750d7d4a947d397b0f7e78fd4559277e0d372ed&",
    3: "https://cdn.discordapp.com/attachments/1239914709872934973/1239916153825136640/New_Project.png?ex=6644a967&is=664357e7&hm=d0d31f097441773b484431281f4740111968558fe4d01cf73a13c030b4d80c0f&",
    4: "https://cdn.discordapp.com/attachments/1239914709872934973/1239916195013070937/New_Project_1.png?ex=6644a971&is=664357f1&hm=0344e0622f20fa4f496fc50377f72f56440208f2e15575b364781acd446120c6&",
    5: "https://cdn.discordapp.com/attachments/1239914709872934973/1239916234225750036/New_Project_2.png?ex=6644a97a&is=664357fa&hm=a5a4b2543c86121b9084cd3037ece369c3cebc675eb507a6c2b4c0328aacfa7a&",
    6: "https://cdn.discordapp.com/attachments/1239914709872934973/1239916284171386980/New_Project_3.png?ex=6644a986&is=66435806&hm=ecae94148177f0bea5b5f8d387581afd3b93fb4fad6321ec1f97bc038b8884cb&",
    7: "https://cdn.discordapp.com/attachments/1239914709872934973/1239916317738401813/New_Project_4.png?ex=6644a98e&is=6643580e&hm=1921c316e01e8495b9271dccb94bbb8a0eae100009c466f38ad1a383411ffa91&",
    8: "https://cdn.discordapp.com/attachments/1239914709872934973/1239916355017244692/New_Project_5.png?ex=6644a997&is=66435817&hm=f17271e957da46dcfc62b824ef78749a2aeb5d759476cc62ef9399e225c91eee&"
}

const invoke = async (interaction) => {
    const stars = interaction.options.getInteger('stars');
    const reviewText = interaction.options.getString('review');
    const isAnonymous = interaction.options.getBoolean('anonymous');
    const user = interaction.user;
    var ReviewEmbed = null;

    if(!isAnonymous) {
        await interaction.deferReply();
    }

    if (stars > 8 || stars < 0) {
        if (!isAnonymous) {
            interaction.editReply({ content: "You must provide a rating from 0 - 8", ephemeral: true });
        } else {
            user.send({ content: "You must provide a rating from 0 - 8" });
        }
        return;
    }
    
    if (checkAccess(user.id, 0) !== FLAG.ACCESS_GRANTED && stars < 5) {
        if (!isAnonymous) {
            interaction.editReply({ content: "Users without access cannot give a negative review.", ephemeral: true });
        } else {
            user.send({ content: "Users without access cannot give a negative review." });
        }
        return;
    }

    if (isAnonymous) {
        ReviewEmbed = new EmbedBuilder()
	        .setColor(0x0BCCF1)
	        .setTitle(`New Anonymous Review`)
	        .setDescription(`${reviewText}`)
	        .setImage(starLinks[stars])
	        .setTimestamp()
    } else {
        ReviewEmbed = new EmbedBuilder()
        .setColor(0x0BCCF1)
        .setTitle(`New Review by ${user.username}`)
        .setDescription(`${reviewText || "No text provided"}`)
        .setThumbnail(`${user.displayAvatarURL()}`)
        .setImage(starLinks[stars])
        .setTimestamp()
    }


    if (isAnonymous) {
        interaction.channel.send({ embeds: [ReviewEmbed]})
    } else {
        interaction.editReply({ embeds: [ReviewEmbed] });
    }
};

export { create, invoke };