import { REST, Routes } from 'discord.js';
import {} from 'dotenv/config';
import fs from 'fs';
import path from 'path';

const commandsTable = [];
const commandsPath = "./events/commands/";

const commands = fs
 .readdirSync('./events/commands')
 .filter((file) => file.endsWith('.js'));

// Use Promise.all to handle asynchronous imports
Promise.all(commands.map(async (command) => {
    const commandModule = await import(`./events/commands/${command}`);
    if (commandModule.create) {
        const commandData = commandModule.create();
        console.log(`Adding command: ${commandData.name}`); // Log the command name being added
        commandsTable.push(commandData);
    }
}))
.then(async () => {
    const rest = new REST().setToken(process.env.BOT_TOKEN);

    try {
        await rest.put(Routes.applicationCommands(process.env.CLIENT_ID), { body: [] })
       .then(() => console.log('Successfully deleted all global application commands.'))
       .catch(console.error);

        await rest.put(Routes.applicationCommands(process.env.CLIENT_ID, process.env.GUILD_ID), { body: [] })
       .then(() => console.log('Successfully deleted all guild application commands.'))
       .catch(console.error);

        console.log(`Started refreshing ${commandsTable.length} application (/) commands.`);
    
        const data = await rest.put(
            Routes.applicationGuildCommands(process.env.CLIENT_ID, process.env.GUILD_ID),
            { body: commandsTable },
        );
    
        console.log(`Successfully reloaded ${data.length} application (/) commands.`);
    } catch (error) {
        console.error(error);
    }
})
.catch(console.error);