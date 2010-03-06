-- Use this script for a solar system

-- Keyboard States:
KEYUP, KEYDOWN, KEYPRESSED, KEYTYPED = 0,1,2,3
-- SDL Key Translations
Key = {
	backspace=8, tab=9, clear=12, pause=19, escape=27, space=32,
	up=273, down=274, right=275, left=276, insert=277, home=278, pageup=280, pagedown=281,
	-- Should we support the function keys?
	f1=282, f2=283, f3=284, f4=285, f5=286, f6=287, f7=288, f8=289, f9=290, f10=291, f11=292, f12=293, f13=294, f14=295, f15=296,
	numlock=300, capslock=301, scrolllock=302, rshift=303, lshift=304, rctrl=305, lctrl=306, ralt=307, lalt=308, rmeta=309, lmeta=310, lsuper=311, rsuper=312,
	-- Special Lua keywords
	["return"]=13, ["end"]=279
}

function sdlkey(k)
	if Key[k] then
		return Key[k]
	else
		return k
	end
end

commands = {
	-- Each command should be a table
	-- { KEY, TITLE, SCRIPT }
	{'up', "Accelerate", "PLAYER:Accelerate()",KEYPRESSED},
	{'left', "Turn Left", "PLAYER:Rotate(30)",KEYPRESSED},
	{'right', "Turn Right", "PLAYER:Rotate(-30)",KEYPRESSED},
	{'down', "Reverse", "PLAYER:Rotate(PLAYER:directionTowards(PLAYER:GetMomentumAngle() + 180 ))",KEYPRESSED},
	{'c', "Center", "PLAYER:Rotate(PLAYER:directionTowards(0,0))",KEYPRESSED},
	{'rshift', "Change Weapon 1", "PLAYER:ChangeWeapon()",KEYTYPED},
	{'lshift', "Change Weapon 2", "PLAYER:ChangeWeapon()",KEYTYPED},
	{'tab', "Target Ship", "targetClosestShip()",KEYTYPED},
	{'I', "Get Info", "showInfo()",KEYTYPED},
	{'l', "Land on Planet", "attemptLanding()",KEYTYPED},
	{'w', "Focus on the Target", "Epiar.focusCamera(HUD.getTarget())",KEYTYPED},
	{'q', "Focus on the Player", "Epiar.focusCamera(PLAYER:GetID())",KEYTYPED},
	{'?', "Game Options", "options()",KEYTYPED},
	{'space', "Fire", "PLAYER:Fire()",KEYPRESSED}
}

function chooseKeys()
	Epiar.pause()
	if keywin ~=nil then return end
	-- 
	off_x,off_y = 30,30
	w,h = 300,(#commands*20 + 80)
	keyinput = {}
	labels = {}
	-- 
	keywin = UI.newWindow( 400, 100, w, h, "Keyboard commands")
	for i=1,#commands do
		keyinput[i] = UI.newTextbox(off_x,off_y,70,1)
		keyinput[i]:setText(commands[i][1])
		labels[i] = UI.newLabel(off_x+100,off_y+15,commands[i][2])
		off_y = off_y +20
		keywin:add(keyinput[i])
		keywin:add(labels[i])
	end
	if keyhelpwin ~=nil then return end
	keywin:add(UI.newButton(w/2-120/2, h-38, 120, 30, "Save Changes", "processKeyWin(commands)"))
	keywin:add(UI.newButton(10, h-38, 30, 30, "?", "keyhelp()"))
end

function processKeyWin(commands)
	for i=1,#commands do
		keyval = keyinput[i]:GetText()
		if keyval ~= commands[i][1] then
			Epiar.UnRegisterKey(sdlkey(commands[i][1]), commands[i][4])
			Epiar.RegisterKey(sdlkey(keyval), commands[i][4], commands[i][3])
			HUD.newAlert(string.format("Registered '%s' to %s", keyval, commands[i][2]))
			commands[i][1] = keyinput[i]:GetText()
		end
	end
	Epiar.unpause()
	keywin:close()
	keywin=nil
	if keyhelpwin then
		keyhelpwin:close()
		keyhelpwin=nil
	end
end

function keyhelp()
	Epiar.pause()
	if keyhelpwin ~=nil then return end
	off_x,off_y = 20,20
	w,h = 130,50
	-- This seems to be the only way to count keys in a dictionary.
	for k,v in pairs(Key) do h=h+20 end 
	-- TODO: Make this a scrollable window
	keyhelpwin = UI.newWindow( 100, 100, w, h, "Possible Keys")
	for k,v in pairs(Key) do
		off_y = off_y + 20
		keyhelpwin:add( UI.newLabel(off_x,off_y,"- "..k) )
	end
end

function targetClosestShip()
	x,y = PLAYER:GetPosition()
	nearby = Epiar.ships(x,y,4096)
	if #nearby==0 then return end
	
	nextTarget = 1
	currentTarget = HUD.getTarget()
	for s =1,#nearby-1 do
		if nearby[s]:GetID() == currentTarget then
			nextTarget = s+1
		end
	end
	
	HUD.newAlert("Targeting "..nearby[nextTarget]:GetModelName().." #"..nearby[nextTarget]:GetID())
	HUD.setTarget(nearby[nextTarget]:GetID()) -- First ID in the list
	TargetName:setStatus(nearby[nextTarget]:GetModelName() )
end

function attemptLanding()
	if landingWin ~= nil then return end
	x,y = PLAYER:GetPosition()
	planet = Epiar.nearestPlanet(PLAYER,4096)
	px,py = planet:Position()
	distance = distfrom( px,py, x,y)
	message=""
	if HUD.getTarget() ~= planet:GetID() then -- Add this text before the first message.
		message = string.format("This is %s Landing Control. ",planet:Name())
	end
	
	-- Check if the ship is close enough and moving slowly enough to land on the planet.
	HUD.setTarget(planet:GetID())
	TargetName:setStatus(planet:Name() )
	-- TODO make this distance check based off of the planet size.
	if distance > 200 then
		if message~="" then
			message=message.."Begin your approach."
		else
			message="Continue your approach."
		end
		HUD.newAlert(message)
	else
		velocity = PLAYER:GetMomentumSpeed()
		if velocity > 2 then
			HUD.newAlert(message.."Please slow your approach.")
		else
			HUD.newAlert(string.format("Welcome to %s.",planet:Name()))
			landOnPlanet( planet:GetID() )
		end
	end
end

function landOnPlanet(id)
	-- Create the Planet Landing Screen
	if landingWin ~= nil then return end
	Epiar.pause()
	planet = Epiar.getSprite(id)
	
	landingWin = UI.newWindow( 200,100,400,300, string.format("%s Landing Screen",planet:Name()))
	landingWin:add(UI.newButton( 40,40,100,30,"Shipyard",string.format("shipyard(%d)",id) ))
	landingWin:add(UI.newButton( 40,80,100,30,"Armory",string.format("armory(%d)",id) ))
	landingWin:add(UI.newButton( 40,120,100,30,"Repair","PLAYER:Repair(10000)" ))
	landingWin:add(UI.newButton( 290,260,100,30,string.format("Leave %s ",planet:Name()), "Epiar.unpause();landingWin:close();landingWin=nil" ))
end

function createWindows()
	Epiar.RegisterKey('p',KEYTYPED,"togglePause()")
	Epiar.RegisterKey('g',KEYTYPED,"ui_demo()")
end
registerInit(createWindows)

-- Register the player functions
function createNavigation()
	for i=1,#commands do
		keyval, name, code = commands[i][1],commands[i][2],commands[i][3]
		Epiar.RegisterKey(sdlkey(keyval), commands[i][4], code)
	end

	for k =1,9 do
		kn = string.byte(k)
		ks = string.format("%d",1000*math.pow(2,k-1))
		Epiar.RegisterKey(kn, KEYPRESSED, "HUD.setVisibity("..ks..")")
	end
end
registerInit(createNavigation)

function coordinateToQuadrant(x,y)
	qsize = 4096
	function c2q(z)
		return math.floor( (z+qsize)/(2*qsize))
	end
	return c2q(x),c2q(y)
end

function createHUD()
	-- Location Status Bars
	x,y = PLAYER:GetPosition()
	qx,qy = coordinateToQuadrant(x,y)
	pos = HUD.newStatus("Coordinate:",130,1,string.format("( %d , %d )",x,y))
	quad = HUD.newStatus("Quadrant:",130,1,string.format("( %d , %d )",qx,qy))

	-- Weapon and Armor Status Bars
	myhull = HUD.newStatus("HULL:",100,0,1.0)
	myweapons = {}
	weaponsAndAmmo = PLAYER:GetWeapons()
	for weapon,ammo in pairs(weaponsAndAmmo) do
		if 0==ammo then ammo="---" end
		myweapons[weapon] = HUD.newStatus(weapon..":",130,0,"[ ".. ammo .." ]")
	end

	-- DEBUG Bars
	TargetName = HUD.newStatus("Target:",130,1,"")
	TargetHULL = HUD.newStatus("Target:",130,1,0)
end
registerInit(createHUD)

updateHUD = function ()
	if PLAYER:GetHull() == 0 then return end
	-- Update Positions
	x,y = PLAYER:GetPosition()
	qx,qy = coordinateToQuadrant(x,y)
	pos:setStatus(string.format("( %d , %d )",x,y))
	quad:setStatus(string.format("( %d , %d )",qx,qy))

	-- Update Weapons and Armor
	myhull:setStatus(PLAYER:GetHull())
	weaponsAndAmmo = PLAYER:GetWeapons()
	cur_weapon = PLAYER:GetCurrentWeapon()
	for weapon,ammo in pairs(weaponsAndAmmo) do
		if cur_weapon == weapon then star=" ARMED" else star="" end
		if 0==ammo then ammo="---" end
		myweapons[weapon]:setStatus("[ ".. ammo .." ]".. star)
	end
	if SHIPS[HUD.getTarget()]~=nil then
		TargetHULL:setStatus( SHIPS[HUD.getTarget()]:GetHull() )
	end
end
registerPostStep(updateHUD)

