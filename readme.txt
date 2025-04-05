-------------------------------
HOW TO PLAY
-------------------------------
Use coins from winning rounds to build a team of critters that will crush your opponents!
The enemies you will fight are displayed on the right.
The grid on the left side of the field will be used to represent your team.

To make a team:
- Press store to open the shop window
- Drag the critter you want to buy and place on an empty grid square
- Your coins are in the top left corner. You can only buy a critter if you have enough coins
- Press store again to close the shop window when you're ready
- You can drag your creatures around the grid to change their starting positions

To fight:
- Have at least one critter placed on your grid
- Close shop window, if it's open
- Press play!

Your critters will automatically fight, targeting the nearest enemies first. How many rounds can they make it??

(all 2d art is by me, there are some missing animations as they're still in progresss…)

-------------------------------
LUA FUNCTIONALITY
-------------------------------


Press W while a battle is in progress for an instant win
Press D while a battle in progress for an instant defeat

Press L to apply/change:
- shopCreatureOffsetX/Y (changes creature position in shop window)
- coins
- current round
- window size
- if shop is open, reset shop


LEVEL EDITING:
Press G to save your current team as an enemy troop.
The level of your team will be calculated (via Game::CalculateDifficulty) and saved in the appropriate .lua file; Difficulty1.lua, Difficulty2.lua, etc.

When generating enemies for the player to fight, PlayMode::GenerateEnemies() will check if there is a valid saved troop. Only troops of an appropriate difficulty, depending on the round will be spawned. Troops will not be repeated; you cannot fight the same group more than once in the same game.
(difficulty1=round 1-3, difficulty2=round 4-5, difficulty3= round 6-7, difficulty4=8-9,difficulty5=10+)
Of the valid troops, one is randomly selected to be spawned.
If there are no valid troops to spawn, the enemies will be randomly generated instead.

If you want to guarantee that you will fight a troop you have saved, you will have to directly edit the DifficultyX.lua file and remove other groups before saving. Otherwise, there is no guarantee that your troop will be selected.
If you do this, make sure the file contains nothing but 'Troops={}' (not including quotation marks)


-------------------------------
RELEVANT C++ FUNCTIONS
-------------------------------
all of LuaHelper.h and LuaHelper.cpp

playMode.cpp
-------------
PlayMode::GenerateScriptEnemies 	line 205
PlayMode::InitLuaFunctions		line 935
PlayMode::ApplyLuaCheats		line 943

game.cpp
-------------
OnResize				line 12
Game::CountTroops			line 117
Game::ApplyLua				line 167
Game::ApplyLuaCheats			line 196
Game::CreateEnemyGroup			line 205

FileUtils.cpp
-------------
RemoveLastCharFromFile			line 12

