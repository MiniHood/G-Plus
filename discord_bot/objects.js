import {FLAG} from './flags.js'
import { exec } from 'child_process';
import fs from 'fs/promises'; // Import fs with Promises
const checkAccess = async (userId, requestedBots) => {
    try {
        const data = await fs.readFile('user_data.json', 'utf8');
        let jsonData = JSON.parse(data);

        if (jsonData[userId]) {
            const { maxBots, expirationDate } = jsonData[userId];
            const currentTime = Date.now();

            if (currentTime < expirationDate) {
                const luaOutput = await runLuaCommand(['checkaccess', requestedBots, userId, maxBots]);
                 if (luaOutput[3].includes('ACCESS_GRANTED')) {
                    return FLAG.ACCESS_GRANTED; // User has access and requested bots are allowed
                } else {
                    console.log('Requested bots exceed the maximum allowed.');
                    return FLAG.EXCEEDED_LIMIT; // Requested bots exceed the maximum allowed
                }
            } else {
                // Access has expired, remove user data
                delete jsonData[userId];
                await fs.writeFile('user_data.json', JSON.stringify(jsonData, null, 2));
                return FLAG.ACCESS_EXPIRED;
            }
        }
        return FLAG.ACCESS_NOT_FOUND; // User does not have access or data not found
    } catch (err) {
        console.error(err);
        return FLAG.ACCESS_NOT_FOUND;
    }
};

// Function to periodically check and remove access for users whose time limit has expired
const checkAndRemoveExpiredAccess = () => {
    setInterval(() => {
        console.log('Checking users for time expiration.');
        fs.readFile('user_data.json', 'utf8', (err, data) => {
            if (err) {
                console.error(err);
                return;
            }

            let jsonData = JSON.parse(data);

            // Get the current timestamp
            const currentTime = Date.now();

            // Iterate over users to check and remove expired access
            for (let userId in jsonData) {
                const { expirationDate } = jsonData[userId];

                if (currentTime >= expirationDate) {
                    // Remove access for the user
                    delete jsonData[userId];
                    console.log(`${userId}'s time ran out, removing.`);
                }
            }

            // Save the updated user data back to the file
            fs.writeFile('user_data.json', JSON.stringify(jsonData, null, 2), (err) => {
                if (err) {
                    console.error(err);
                    return;
                }
            });
        });
    }, 1 * 60 * 60 * 1000); // Run every 1 hour
};

const runLuaCommand = (args) => {
    return new Promise((resolve, reject) => {
        const command = `lua52 ../Main.lua ${args.join(' ')}`;

        exec(command, (error, stdout, stderr) => {
            if (error) {
                console.error(`Error executing Lua command: ${error.message}`);
                console.error(`stderr: ${stderr}`); // Log the stderr as well
                reject(error.message);
            }

            // Split the Lua output by newlines to process each line separately
            const outputLines = stdout.split('\n');
            outputLines.pop();
            resolve(outputLines);
        });
    });
};


export { checkAccess, checkAndRemoveExpiredAccess, runLuaCommand };