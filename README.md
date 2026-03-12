# Minecraft Pocket Edition 1.0.0 Partial Source Code
When the Minecraft Dungeons source code leaked, contained in it was a lot of the pocket edition 1.0.0 source code half merged into the unreal game engine. The earliest commit in that repo contains the most, but a lot is still missing or commented out(Just to get it to compile?). I have uncommented a lot, removed unreal and fixed a lot, but there is a LOT more code missing+broken, so you can't compile ordinary minecraft from it yet. It still could be used to make SDKs, backport features to LCE/0.6.2 and it might even be possible to compile MCPE 1.0.0 from it eventually, after a lot of work.

# TO BE CLEAR, THIS ISN'T A NEW LEAK, IT WAS IN THE EARLIEST COMMIT OF THE LEAKED DUNGEONS GIT REPO spice.tar.bz2

## Missing/Recreated code
- **lib-deps/Renderer**
 Was removed because it conflicted with unreal's renderer?
- **src/platform** 
 Was removed because unreal had its own platform stuff?
- **src-client** 
 Was removed because unreal was the launcher exe?
- **network** 
 Was removed because unreal has its own networking?
- **world/entity** 
 Was mostly removed because they wanted to use unreal Actors?
- **world/item**
 Unknown
- **world/level/block/entity**
 Was removed because entity code is heavily removed?
- **Most redstone related code**
 Redstone wasnt needed in dungeons

## Interesting code
- **src/common/world/level/levelgen/v1/FarlandsFeature.cpp**
Yep, intentional farlands, though this kind is quite different to the Java Farlands. RandomLevelSource mentions it next to something called the 'underworlds', and both are chunk post processing steps.

## Contributing
The only contributions I want in this repo (currently) are fixes and decompilation/remakes of the missing parts. If you want to add new features, make a fork. Some code seems to of been ported from other versions of minecraft (4J legacy console edition especially since it runs on C++ too) so some stuff can be restored using that, and of course there is the 0.6.1/0.6.2 leak as well. I have added a comment to every file I have reviewed and found MOSTLY matches (tac complete). If there is no comment up the top saying that, or it says "tac todo", then that file needs work.

## How to compile.
Good luck, to generate the CMake soltution use the build_win32.bat file or if you are on another OS, run CMake directly.
