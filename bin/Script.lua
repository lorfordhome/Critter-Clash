
function WriteTroops(difficulty,stringToWrite)
	local filename="Difficulty"..difficulty
	local fileEnding=".lua"
	file=io.open(filename..fileEnding,"a")
	file:write(stringToWrite)
	file:close()
end

function CallResize()
	OnResize(1000,1000)
end


dif1Troops=0
dif2Troops=0
dif3Troops=0
dif4Troops=0
dif5Troops=0

coins=100
currentRound=2