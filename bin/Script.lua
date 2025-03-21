
function WriteTroops(stringToWrite)
	file=io.open("troops.lua","a")
	file:write(stringToWrite)
	file:close()
end


coins=100
currentRound=2
enemyBuizel ={x=2,y=2,type=3}

