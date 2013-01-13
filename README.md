

This is the heavily cluttered project folder for my 32x12 RGB Display consisting of 10 meters of Adressable RGB Stripe.

The Main Highlight can be found in the subdir snake_standalone. While it's an awesome snake game, the code is... well... don't ask. It started off with the stripes wrapped around a trash can @29c3 and the implementation was quick and dirty. Back home again, I just kept adding feature after Feature.

Requirements:

The Code was completely developed via ssh on a RaspberryPi and is meant to run with a Wii-Nunchuck connected via I2C and the RGB-Stripes connected via SPI.

It uses WiringPi for the SPI connection.

Current Features:

- Standby Screen showing the Time and the words "PLAY" and "SNAKE" in turn.
- Menu for selecting the gamemode and the scoreboard.
- A scrollable scoreboard.
- A simple bot for demo/standby mode.
- Inverted gamemode where you place the target/salad/apple/whatever for the bot, called "TARGET".
- Nunchuck as (only) input device.
- ncurses interface for rudimentary keyboard input and debug output (e.g. via ssh).
- logging of scores and certain game variables at end of round.
- very colourful output.
- tasty spaghetti code.


contact:
armageddon421 at googlemail dot com
