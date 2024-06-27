# G-Plus

[![forthebadge](https://forthebadge.com/images/badges/0-percent-optimized.svg)](https://forthebadge.com)
[![forthebadge](https://forthebadge.com/images/badges/contains-tasty-spaghetti-code.svg)](https://forthebadge.com)
[![forthebadge](https://forthebadge.com/images/badges/made-with-crayons.svg)](https://forthebadge.com)

Follow the development of everything plus related via our discord [Plus Studios](https://discord.gg/Fx9W9zzpUk)

# What is this?

GPlus was a side project of mine that I originally planned to sell but discontinued due to prices of garry's mod accounts. It's a botnet that allows the user to connect bots to a garry's mod server, it also comes with a discord bot to control them on a mass scale.

It was made in LUA and Node JS. I'm quite new to Node JS so the code is quite messy. Other than that, it works. Just make sure to fill out everything correctly, and make sure to make a .ENV file in discord_bot with this structure:
```env
BOT_TOKEN=""
CLIENT_ID=""
GUILD_ID=""
```

After that fill out the Globals.lua file. 

This project requires Sandboxie Plus.

# Credits

The only edits I made to this was editing the projects name to match GPlus and made it compatiable with cmd line arguments.

[External Command Executor](https://github.com/Earu/GTerm)

# Benchmarking
From my testing (With using a config that does some optimisation magic) each instance will use around 2 GB of ram, I don't believe you can go lower than that, so if you're planning on self hosting large amounts of bots. Don't.
