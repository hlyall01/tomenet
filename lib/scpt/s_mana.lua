-- The mana school

function get_manathrust_dam(Ind, limit_lev)
	--return 3 + get_level(Ind, MANATHRUST, 50), 1 + get_level(Ind, MANATHRUST, 20)
	local lev, lev2

	lev = get_level(Ind, MANATHRUST_I, 50)
	lev2 = get_level(Ind, MANATHRUST_I, 20)
	if limit_lev ~= 0 and lev > limit_lev then lev = limit_lev + (lev - limit_lev) / 3 end

	return 3 + lev, 1 + lev2
end

MANATHRUST_I = add_spell {
	["name"] = 	"Manathrust I",
	["school"] = 	SCHOOL_MANA,
	["level"] = 	1,
	["mana"] = 	2,
	["mana_max"] = 	2,
	["fail"] = 	10,
	["direction"] = TRUE,
	["ftk"] = 1,
	["spell"] = 	function(args)
			fire_bolt(Ind, GF_MANA, args.dir, damroll(get_manathrust_dam(Ind, 1)), " casts a mana bolt for")
	end,
	["info"] = 	function()
			local x, y

			x, y = get_manathrust_dam(Ind, 1)
			return "dam "..x.."d"..y
	end,
	["desc"] = 	{ "Conjures up mana into a nearly irresistible bolt.", }
}
MANATHRUST_II = add_spell {
	["name"] = 	"Manathrust II",
	["school"] = 	SCHOOL_MANA,
	["level"] = 	20,
	["mana"] = 	7,
	["mana_max"] = 	7,
	["fail"] = 	-20,
	["direction"] = TRUE,
	["ftk"] = 1,
	["spell"] = 	function(args)
			fire_bolt(Ind, GF_MANA, args.dir, damroll(get_manathrust_dam(Ind, 20)), " casts a mana bolt for")
	end,
	["info"] = 	function()
			local x, y

			x, y = get_manathrust_dam(Ind, 20)
			return "dam "..x.."d"..y
	end,
	["desc"] = 	{ "Conjures up mana into a nearly irresistible bolt.", }
}
MANATHRUST_III = add_spell {
	["name"] = 	"Manathrust III",
	["school"] = 	SCHOOL_MANA,
	["level"] = 	40,
	["mana"] = 	25,
	["mana_max"] = 	25,
	["fail"] = 	-75,
	["direction"] = TRUE,
	["ftk"] = 1,
	["spell"] = 	function(args)
			fire_bolt(Ind, GF_MANA, args.dir, damroll(get_manathrust_dam(Ind, 0)), " casts a mana bolt for")
	end,
	["info"] = 	function()
			local x, y

			x, y = get_manathrust_dam(Ind, 0)
			return "dam "..x.."d"..y
	end,
	["desc"] = 	{ "Conjures up mana into a nearly irresistible bolt.", }
}

DELCURSES_I = add_spell {
	["name"] = 	"Remove Curses I",
	["school"] = 	SCHOOL_MANA,
	["level"] = 	20,
	["mana"] = 	20,
	["mana_max"] =	20,
	["fail"] = 	20,
	["spell"] = 	function()
			done = remove_curse(Ind)
			if done == TRUE then msg_print(Ind, "The curse is broken!") end
	end,
	["info"] = 	function()
			return ""
	end,
	["desc"] = 	{ "Removes light curses from your items.", }
}
DELCURSES_II = add_spell {
	["name"] = 	"Remove Curses II",
	["school"] = 	SCHOOL_MANA,
	["level"] = 	40,
	["mana"] = 	50,
	["mana_max"] =	50,
	["fail"] = 	-20,
	["spell"] = 	function()
			remove_all_curse(Ind)
			if done == TRUE then msg_print(Ind, "The curse is broken!") end
	end,
	["info"] = 	function()
			return ""
	end,
	["desc"] = 	{ "Removes all light and heavy curses from your items.", }
}

RECHARGE_I = add_spell {
	["name"] = 	"Recharge I",
	["school"] = 	{SCHOOL_MANA},
	["level"] = 	5,
	["mana"] = 	10,
	["mana_max"] = 	10,
	["fail"] = 	20,
	["stat"] = 	A_INT,
	["spell"] = 	function()
			recharge(Ind, 10 + get_level(Ind, RECHARGE_I, 45))
	end,
	["info"] = 	function()
			return "power "..(10 + get_level(Ind, RECHARGE_I, 45))
	end,
	["desc"] = 	{ "Taps on the ambient mana to recharge a wand or staff.", }
}
RECHARGE_II = add_spell {
	["name"] = 	"Recharge II",
	["school"] = 	{SCHOOL_MANA},
	["level"] = 	25,
	["mana"] = 	30,
	["mana_max"] = 	30,
	["fail"] = 	-10,
	["stat"] = 	A_INT,
	["spell"] = 	function()
			recharge(Ind, 10 + 50 + get_level(Ind, RECHARGE_II, 50))
	end,
	["info"] = 	function()
			return "power "..(10 + 50 + get_level(Ind, RECHARGE_II, 50))
	end,
	["desc"] = 	{ "Taps on the ambient mana to recharge a magic device.", }
}
RECHARGE_III = add_spell {
	["name"] = 	"Recharge III",
	["school"] = 	{SCHOOL_MANA},
	["level"] = 	40,
	["mana"] = 	100,
	["mana_max"] = 	100,
	["fail"] = 	-60,
	["stat"] = 	A_INT,
	["spell"] = 	function()
			recharge(Ind, 10 + get_level(Ind, RECHARGE_III, 140))
	end,
	["info"] = 	function()
			return "power "..(10 + get_level(Ind, RECHARGE_III, 140))
	end,
	["desc"] = 	{ "Taps on the ambient mana to recharge a magic device.", }
}

--[[PROJECT_SPELLS = add_spell {
	["name"] = 	"Project Spells",
	["school"] = 	{SCHOOL_MANA},
	["level"] = 	10,
	["mana"] = 	0,
	["mana_max"] = 	0,
	["fail"] = 	101,
	["stat"] = 	A_INT,
	["spell"] = 	function()
			if player.spell_project == 0 then
				player.spell_project = 1 + get_level(Ind, PROJECT_SPELLS, 6, 0)
				msg_print(Ind, "Your utility spells will now affect players in a radius of "..(player.spell_project)..".")
			else
				player.spell_project = 0
				msg_print(Ind, "Your utility spells will now only affect yourself.")
			end
			player.redraw = bor(player.redraw, 1048576)
	end,
	["info"] = 	function()
			return "base rad "..(1 + get_level(Ind, PROJECT_SPELLS, 6, 0))
	end,
	["desc"] = 	{
			"Affects some of your spells(mostly utility ones) to make them",
			"have an effect on your nearby party members.",
	}
}]]--

DISPERSEMAGIC = add_spell {
	["name"] = 	"Disperse Magic",
	["school"] = 	{SCHOOL_MANA},
	["level"] = 	15,
	["mana"] = 	30,
	["mana_max"] = 	30,
	["fail"] = 	10,
	["stat"] = 	A_INT,
	-- Unnafected by blindness
	["blind"] = 	FALSE,
	-- Unnafected by confusion
	["confusion"] = FALSE,
	["spell"] = 	function()
			set_blind(Ind, 0)
			set_confused(Ind, 0)
			if get_level(Ind, DISPERSEMAGIC, 50) >= 10 then
				set_image(Ind, 0)
			end
			if get_level(Ind, DISPERSEMAGIC, 50) >= 15 then
				set_slow(Ind, 0)
				set_fast(Ind, 0, 0)
				set_stun(Ind, 0)
			end
	end,
	["info"] = 	function()
			return ""
	end,
	["desc"] = 	{
			"Dispels a lot of magic that can affect you, be it good or bad.",
			"Level 1: blindness and confusion.",
			"Level 10: hallucination.",
			"Level 15: speed (both bad or good) and stun.",
	}
}

MANASHIELD = add_spell {
	["name"] = 	"Disruption Shield",
	["school"] = 	SCHOOL_MANA,
--	["level"] = 	45,
	["level"] = 	35,
	["mana"] = 	50,
	["mana_max"] = 	50,
--	["fail"] = 	-200,
	["fail"] = 	10,
	["spell"] = 	function()
--			if get_level(Ind, MANASHIELD, 50) >= 5 then
--				if (player.invuln == 0) then
--					set_invuln(Ind, randint(5) + 15 + get_level(Ind, MANASHIELD, 15))
--				end
--			else
--				if (player.tim_manashield == 0) then
					set_tim_manashield(Ind, randint(10) + 20 + get_level(Ind, MANASHIELD, 75))
--				end
--			end
	end,
	["info"] = 	function()
			return "dur "..(20 + get_level(Ind, MANASHIELD, 75)).."+d10"
	end,
	["desc"] = 	{
			"Redirects damage taken to your mana pool instead of reducing your hit points."
--			"At level 5 switches to globe of invulnerability",
--			"The spell breaks as soon as a melee, shooting,",
--			"throwing or magical skill action is attempted"
		}
}
