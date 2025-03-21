dif1_groups={"dummy"}
dif2_groups={}
dif3_groups={}
dif4_groups={}
dif5_groups={}
--get rid of these tables ; replace with dif1.lua etc. save the troops only in the respective difficulty file

function AddGroup(difficulty,troopName)
	if (difficulty==1)
	then
		table.insert(dif1_groups,troopName)
		file=io.open("test.txt","w")
		file:write(dif1_groups[2])
		file:close()
	elseif(difficulty==2)
	then
		table.insert(dif2_groups,troopName)
	elseif(difficulty==3)
	then
		table.insert(dif3_groups,troopName)
	elseif(difficulty==4)
	then
		table.insert(dif4_groups,troopName)
	elseif(difficulty==5)
	then
		table.insert(dif5_groups,troopName)
	end
end


Troop1742569241={
creature0 = {x = 0,y = 0,type = 2}
}
Troop1742569369={
creature0 = {x = 0,y = 0,type = 2}
}
Troop1742569390={
creature0 = {x = 0,y = 1,type = 2}
}
Troop1742569395={
creature0 = {x = 0,y = 1,type = 2}
}
Troop1742569696={
creature0 = {x = 1,y = 1,type = 3}
}
Troop1742569805={
creature0 = {x = 1,y = 1,type = 3}
}
Troop1742569830={
creature0 = {x = 1,y = 1,type = 3}
}
Troop1742569856={
creature0 = {x = 1,y = 1,type = 3}
}
Troop1742569899={
creature0 = {x = 1,y = 1,type = 3}
}
Troop1742569915={
creature0 = {x = 1,y = 1,type = 3}
}
Troop1742569921={
creature0 = {x = 1,y = 1,type = 3}
}
Troop1742569982={
creature0 = {x = 1,y = 1,type = 3}
}
Troop1742569988={
creature0 = {x = 1,y = 1,type = 3}
}
Troop1742570043={
creature0 = {x = 1,y = 1,type = 1}
}
Troop1742570068={
creature0 = {x = 1,y = 1,type = 1}
}
Troop1742570124={
creature0 = {x = 1,y = 1,type = 3}
}
Troop1742570145={
creature0 = {x = 1,y = 1,type = 3}
}