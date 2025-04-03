
function WriteTroops(difficulty,stringToWrite)
	local filename="Difficulty"..difficulty
	local fileEnding=".lua"
	file=io.open(filename..fileEnding,"a")
	file:write(stringToWrite)
	file:close()
end

function CallResize()
	OnResize(1920,1080)
end

function InitWin()
	CDispatcher("InitWin")
end

coins=100
currentRound=2
shopCreatureOffsetX=20
shopCreatureOffsetY=20