dide_mod
version 1.1.0
================================================================================

ABOUT
--------------------------------------------------------------------------------

dide_mod is a simple pak loader for certain Chrome Engine games. In addition,
it can enable the in-game developer menu.

Chrome Engine 5 Games (CE5, x86):

- Dead Island (DI)
- Dead Island Riptide (DIR)

Chrome Engine 6 Games (CE6, x64):

- Dead Island Definitive Edition (DIDE)
- Dead Island Riptide Definitive Edition (DIRDE)
- Dying Light (DL, partial support)

FEATURES
--------------------------------------------------------------------------------

- [pak] Load custom pak files.
- [dev] Enable the in-game developer menu.

| engine | game  | cpu | dev | pak |
|--------|-------|-----|-----|-----|
| CE5    | DI    | x86 |  Y  |  Y  |
| CE5    | DIR   | x86 |  Y  |  Y  |
| CE6    | DIDE  | x64 |  Y  |  Y  |
| CE6    | DIRDE | x64 |  Y  |  Y  |
| CE6    | DL    | x64 |  N  |  Y  |

Y = supported; N = unsupported

DISCLAIMER
--------------------------------------------------------------------------------

This software tampers with the game's process at runtime. It's inadvisable to
use it in the presence of anti-tamper or anti-cheat systems such as VAC.
Use it at your own risk. The developer is not liable for any damage, data loss
or other harm arising from using this software.

INSTALLATION
--------------------------------------------------------------------------------

Copy the included files into the game's install directory, right next to the
game's executables:

    game_directory/
      +-- dide_mod.ini
      +-- dsound.dll
      `-- [game].exe

Pick the dsound.dll file from one of the architecture-specific directories that
matches your game's target architecture (see the table above).

Open the configuration file "dide_mod.ini" with a text editor (e.g. Notepad) and
customize settings.

DEVELOPER
--------------------------------------------------------------------------------

Name:        Steffen André Langnes
Website:     https://www.steffenl.com
Source code: https://github.com/SteffenL/dide-mod
