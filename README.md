# CARBON!!!

Carbon is our internal 2D game engine! It provides API support for all 2D game development needs, as well as a basic, extentable runtime editor. Carbon is also geared for developers first - meaning that there is no
isolated / dedicated editor by itself, and is rather an editor integrated into the active runtime! In other words, Carbon is the entrypoint and API packaged all together. To make
a game, you simply *extend* Carbon by injecting your scene definitions and script logic at different points into the runtime. When you compile and run your Carbon project,
you will then see an editor interface in which you can select your scenes, inspect your entities and assets, and play and debug your game. Development for your game is done
completely in-code, and the runtime interface is strictly for debugging, inspecting, and testing your implemented ideas!

Once your game is ready for production, simply flip the production flag, and Carbon will compile your game as a lightweight, standalone executable, stripping away the
editor interface and any non-game related components.

Carbon is an engine intended for developers who enjoy programming, and value the quality of their game from a software perspective!

Lastly, not to worry - you don't have to compile ALL of carbon every time you update your game! If you have a modular build system (like [tiny](https://github.com/JHeflinger/tiny)), then you can easily set up
carbon as a third party library, and simply compile the minimal entrypoint alongside your extension code each time.

## Status

Carbon is currently under alpha development - it is very unfinished and in a early development state. Explore and try at your own risk! While there is no changelog right now, there will be one as of the 0.1 release
to track new developments.

## Roadmap

Currently, Carbon is planned to undergo the following stages:
1. 0.1 Release with all basic features and architectures implemented
2. 0.2 Release targeted to reformat/convert Carbon into modulo submodules (if you don't know what that means, that's because it's a secret!)
3. 0.2.1 Release targeted for testing, bug fixes, and final architecture tweaks
4. 0.3 Release targeted to build fully cross platform across Linux, Windows, and Mac devices
5. 1.0 Release targeted to finish testing suites, bug fixes, and full cross platform support
