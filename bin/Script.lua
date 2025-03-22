
function WriteTroops(difficulty,stringToWrite)
	local filename="Difficulty"..difficulty
	local fileEnding=".lua"
	file=io.open(filename..fileEnding,"a")
	file:write(stringToWrite)
	file:close()

	if (difficulty==1)
	then
		dif1Troops=dif1Troops+1
	elseif(difficulty==2)
	then
		dif2Troops=dif2Troops+1
	elseif(difficulty==3)
	then
		dif3Troops=dif3Troops+1
	elseif(difficulty==4)
	then
		dif4Troops=dif4Troops+1
	elseif(difficulty==5)
	then
		dif5Troops=dif5Troops+1
	end
end

function CountTroops(difficulty)
	local fileToCheck="Difficulty"..difficulty
	fileToCheck=fileToCheck..".lua"
	local lineCount=0
	for line in io.lines(fileToCheck) do
		lineCount=lineCount+1
	end
	return lineCount
end

dif1Troops=0
dif2Troops=0
dif3Troops=0
dif4Troops=0
dif5Troops=0

coins=100
currentRound=2