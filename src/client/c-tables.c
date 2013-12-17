/* $Id$ */
/* File: tables.c */

/* Purpose: Angband Tables */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"



/*
 * Global array for looping through the "keypad directions"
 */
s16b ddd[9] =
{ 2, 8, 6, 4, 3, 1, 9, 7, 5 };

/*
 * Global arrays for converting "keypad direction" into offsets
 */
s16b ddx[10] =
{ 0, -1, 0, 1, -1, 0, 1, -1, 0, 1 };

s16b ddy[10] =
{ 0, 1, 1, 1, 0, 0, 0, -1, -1, -1 };

/*
 * Global arrays for optimizing "ddx[ddd[i]]" and "ddy[ddd[i]]"
 */
s16b ddx_ddd[9] =
{ 0, 0, 1, -1, 1, -1, 1, -1, 0 };

s16b ddy_ddd[9] =
{ 1, -1, 0, 0, 1, 1, -1, -1, 0 };


/*
 * Global array for converting numbers to uppercase hecidecimal digit
 * This array can also be used to convert a number to an octal digit
 */
char hexsym[16] =
{
	'0', '1', '2', '3', '4', '5', '6', '7',
	'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};


/*
 * Hack -- the "basic" sound names (see "SOUND_xxx")
 */
//#ifdef USE_SOUND
//#ifndef USE_SOUND_2010
cptr sound_names[SOUND_MAX] =
{
	"",
	"hit",
	"miss",
	"flee",
	"drop",
	"kill",
	"level",
	"death",
};
//#endif
//#endif


/*
 * Abbreviations of healthy stats
 */
cptr stat_names[6] =
{
	"STR: ", "INT: ", "WIS: ", "DEX: ", "CON: ", "CHR: "
};

/*
 * Abbreviations of damaged stats
 */
cptr stat_names_reduced[6] =
{
	"Str: ", "Int: ", "Wis: ", "Dex: ", "Con: ", "Chr: "
};


/*
 * Standard window names
 */
char ang_term_name[ANGBAND_TERM_MAX][40] =
{
	"TomeNET",
	"Msg/Chat",
	"Inventory",
	"Character",
	"Chat",
	"Equipment",
	"Term-6",
	"Term-7"
};


/*
 * Certain "screens" always use the main screen, including News, Birth,
 * Dungeon, Tomb-stone, High-scores, Macros, Colors, Visuals, Options.
 *
 * Later, special flags may allow sub-windows to "steal" stuff from the
 * main window, including File dump (help), File dump (artifacts, uniques),
 * Character screen, Small scale map, Previous Messages, Store screen, etc.
 *
 * The "ctrl-i" (tab) command flips the "Display inven/equip" and "Display
 * equip/inven" flags for all windows.
 *
 * The "ctrl-g" command (or pseudo-command) should perhaps grab a snapshot
 * of the main screen into any interested windows.
 */
#if 0
cptr window_flag_desc[32] =
{
	"Display inven/equip",
	"Display equip/inven",
	NULL,
	"Display character",
	"Display lag-o-meter",
	NULL,
	"Display messages/chat",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	"Display chat",
	"Display msgs except chat",
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};
#else
cptr window_flag_desc[8] =
{
	"Display inven/equip",
	"Display equip/inven",
	"Display character",
	"Display non-chat messages",
	"Display all messages",
	"Display chat messages",
//	"Display mini-map",//
	NULL,
	"Display lag-o-meter",
};
#endif

/*
 * Available Options
 *
 * Option Screen Sets:
 *
 *	Set 1: User Interface
 *	Set 2: Disturbance
 *	Set 3: Inventory
 *	Set 4: Game Play
 *
 * Note that bits 28-31 of set 0 are currently unused.
 */
/*
 * XXX XXX hard-coded in sync_options!
 */
/*
   bool *o_var;

   byte	o_norm;

   byte	o_page;

   byte	o_set;
   byte	o_bit;		//deprecated
   byte o_enabled;	//deprecated

   cptr	o_text;
   cptr	o_desc;
*/
option_type option_info[OPT_MAX] =
{
	{ &c_cfg.rogue_like_commands,	FALSE,	1,	0, 0, TRUE,
	"rogue_like_commands",		"Rogue-like commands" },

#if 0
	{ &c_cfg.quick_messages,	FALSE,	1,	0, 1, TRUE,
	"quick_messages",		"Activate quick messages (skill etc)" },
#else
	{ &c_cfg.warn_unique_credit,	FALSE,	1,	0, 1, TRUE,
	"warn_unique_credit",		"Beep on attacking a unique you already killed" },
#endif

	{ &c_cfg.other_query_flag,	FALSE,	2,	0, 2, TRUE,
	"other_query_flag",		"Prompt for various information (mimic polymorph)" },

#if 0
	{ &c_cfg.carry_query_flag,	FALSE,	2,	0, 3, FALSE,
	"carry_query_flag",		"(broken) Prompt before picking things up" },
#else
	{ &c_cfg.newbie_hints,		TRUE,	4,	0, 3, TRUE,
	"newbie_hints",			"Display hints/warnings for new players" },
#endif

	{ &c_cfg.use_old_target,	TRUE,	2,	0, 4, TRUE,
	"use_old_target",		"Use old target by default" },

	{ &c_cfg.always_pickup,		FALSE,	2,	0, 5, TRUE,
	"always_pickup",		"Pick things up by default" },

	{ &c_cfg.always_repeat,		TRUE,	2,	0, 6, TRUE,
	"always_repeat",		"Repeat obvious commands" },

	{ &c_cfg.depth_in_feet,		TRUE,	1,	0, 7, TRUE,
	"depth_in_feet",		"Show dungeon level in feet" },

	{ &c_cfg.stack_force_notes,	TRUE,	2,	0, 8, TRUE,
	"stack_force_notes",		"Merge inscriptions when stacking" },

	{ &c_cfg.stack_force_costs,	TRUE,	2,	0, 9, TRUE,
	"stack_force_costs",		"Merge discounts when stacking" },

	//10

#if 0
	{ &c_cfg.show_labels,		TRUE,	1,	0, 10, FALSE,
	"show_labels",			"(broken) Show labels in object listings" },
#else
	{ &c_cfg.hilite_chat,		TRUE,	4,	0, 10, TRUE,
	"hilite_chat",			"Highlight chat messages containing your name" },
#endif
	{ &c_cfg.show_weights,		TRUE,	1,	0, 11, TRUE,
	"show_weights",			"Show weights in object listings" },
#if 0
	{ &c_cfg.show_choices,		FALSE,	1,	0, 12, FALSE,
	"show_choices",			"(broken) Show choices in certain sub-windows" },
#else
	{ &c_cfg.hibeep_chat,		TRUE,	4,	0, 12, TRUE,
	"hibeep_chat",			"Beep on chat messages containing your name" },
#endif

#if 0
	{ &c_cfg.show_details,		TRUE,	1,	0, 13, FALSE,
	"show_details",			"(broken) Show details in certain sub-windows" },
#else
	{ &c_cfg.font_map_solid_walls,	FALSE,	1,	0, 13, TRUE,
	"font_map_solid_walls",		"Certain fonts only: Walls look like solid blocks" },
#endif

	{ &c_cfg.use_color,		TRUE,	1,	0, 14, TRUE,
	"use_color",			"Use color if possible" },

	{ &c_cfg.ring_bell,		TRUE,	1,	0, 15, TRUE,
	"ring_bell",			"Beep on misc warnings and errors" },



	/*** Disturbance ***/

	{ &c_cfg.find_ignore_stairs,	FALSE,	2,	0, 16, TRUE,
	"find_ignore_stairs",		"Run past stairs" },

	{ &c_cfg.find_ignore_doors,	TRUE,	2,	0, 17, TRUE,
	"find_ignore_doors",		"Run through open doors" },

	{ &c_cfg.find_cut,		TRUE,	2,	0, 18, TRUE,
	"find_cut",			"Run past known corners" },

	{ &c_cfg.find_examine,		TRUE,	2,	0, 19, TRUE,
	"find_examine",			"Run into potential corners" },

	//20

	{ &c_cfg.disturb_move,		FALSE,	2,	0, 20, TRUE,
	"disturb_move",			"Disturb whenever any monster moves" },

	{ &c_cfg.disturb_near,		FALSE,	2,	0, 21, TRUE,
	"disturb_near",			"Disturb whenever viewable monster moves" },

	{ &c_cfg.disturb_panel,		FALSE,	2,	0, 22, TRUE,
	"disturb_panel",		"Disturb whenever map panel changes" },

	{ &c_cfg.disturb_state,		FALSE,	2,	0, 23, TRUE,
	"disturb_state",		"Disturb whenever player state changes" },

	{ &c_cfg.disturb_minor,		FALSE,	2,	0, 24, TRUE,
	"disturb_minor",		"Disturb whenever boring things happen" },

	{ &c_cfg.disturb_other,		FALSE,	2,	0, 25, TRUE,
	"disturb_other",		"Disturb whenever various things happen" },

	{ &c_cfg.alert_hitpoint,	FALSE,	1,	0, 26, TRUE,
	"alert_hitpoint",		"Beep about critical hitpoints/sanity" },

	{ &c_cfg.alert_afk_dam,		TRUE,	1,	0, 27, TRUE,
	"alert_afk_dam",		"Beep when taking damage while AFK" },//alert_failure



	{ &c_cfg.auto_afk,		TRUE,	2,	1, 0, TRUE,	/* former auto_haggle */
	"auto_afk",			"Set 'AFK mode' automatically" },

	{ &c_cfg.newb_suicide,		TRUE,	1,	1, 1, TRUE,	/* former auto_scum */
	"newb_suicide",			"Display newbie suicides" },

	//30

	{ &c_cfg.stack_allow_items,	TRUE,	2,	1, 2, TRUE,
	"stack_allow_items",		"Allow weapons and armor to stack" },

	{ &c_cfg.stack_allow_wands,	TRUE,	2,	1, 3, TRUE,
	"stack_allow_wands",		"Allow wands/staffs/rods to stack" },

#if 0
	{ &c_cfg.expand_look,		FALSE,	1,	1, 4, FALSE,
	"expand_look",			"(broken) Expand the power of the look command" },
#else
	{ &c_cfg.uniques_alive,		FALSE,	4,	1, 24, TRUE,
	"uniques_alive",		"List only unslain uniques for your local party" },
#endif

#if 0
	{ &c_cfg.expand_list,		FALSE,	1,	1, 5, FALSE,
	"expand_list",			"(broken) Expand the power of the list commands" },
#else
	{ &c_cfg.overview_startup,	FALSE,	4,	1, 5, TRUE,
	"overview_startup",		"Display overview resistance/boni page at startup" },
#endif
	{ &c_cfg.view_perma_grids,	TRUE,	2,	1, 6, TRUE,
	"view_perma_grids",		"Map remembers all perma-lit grids" },

	{ &c_cfg.view_torch_grids,	FALSE,	2,	1, 7, TRUE,
	"view_torch_grids",		"Map remembers all torch-lit grids" },

	{ &c_cfg.no_verify_destroy,	FALSE,	4,	0, 8, TRUE,	/* former dungeon_align */
	"no_verify_destroy",		"Skip safety question when destroying items" },

	{ &c_cfg.whole_ammo_stack,	FALSE,	3,	0, 9, TRUE,	/* former dungeon_stair */
	"whole_ammo_stack",		"For ammo/misc items always operate on whole stack" },

	{ &c_cfg.recall_flicker,	TRUE,	1,	1, 10, TRUE,
	"recall_flicker",		"Flicker messages in recall" },

	/* currently problematic: best might be to move line-splitting to client side, from util.c
	   For now, let's just insert hourly chat marker lines instead. - C. Blue */
	{ &c_cfg.time_stamp_chat,	FALSE,	4,	0, 11, TRUE,
	"time_stamp_chat",		"Add hourly time stamps to chat window" },

	//40

	{ &c_cfg.page_on_privmsg,	FALSE,	4,	1, 12, TRUE,
	"page_on_privmsg",		"Beep when receiving a private message" },

	{ &c_cfg.page_on_afk_privmsg,	TRUE,	4,	1, 13, TRUE,
	"page_on_afk_privmsg",		"Beep when receiving a private message while AFK" },

	{ &c_cfg.auto_untag,		FALSE,	3,	1, 14, TRUE,
	"auto_untag",			"Remove unique monster inscription on pick-up" },

	{ &c_cfg.big_map,		FALSE,	2,	1, 15, TRUE,
	"big_map",			"Double height of the map shown in the main window" },//smart_cheat


	{ &c_cfg.view_reduce_lite,	FALSE,	3,	1, 16, TRUE,	/* (44) */
	"view_reduce_lite",		"Reduce lite-radius when running" },

	{ &c_cfg.view_reduce_view,	FALSE,	3,	1, 17, TRUE,
	"view_reduce_view",		"Reduce view-radius in town" },

	{ &c_cfg.safe_float,		FALSE,	3,	1, 18, TRUE, /* was avoid_abort (obsolete) */
	"safe_float",			"Prevent floating for a short while after death" },

#if 0
	{ &c_cfg.avoid_other,		FALSE,	1,	1, 19, FALSE,
	"avoid_other",			"(broken) Avoid processing special colors" },
#else
	{ &c_cfg.no_combat_sfx,		FALSE,	5,	1, 19, TRUE,
	"no_combat_sfx",		"Don't play melee/launcher attack/miss sound fx" },
#endif

#if 0
	{ &c_cfg.flush_failure,		TRUE,	1,	1, 20, FALSE, /* (resurrect me?) */
	"flush_failure",		"(broken) Flush input on various failures" },
#else
	{ &c_cfg.no_magicattack_sfx,	FALSE,	5,	1, 21, TRUE,
	"no_magicattack_sfx",		"Don't play basic spell/device attack sound fx" },
#endif

#if 0
	{ &c_cfg.flush_disturb,		FALSE,	1,	1, 21, FALSE,
	"flush_disturb",		"(broken) Flush input whenever disturbed" },
#else
	{ &c_cfg.no_defense_sfx,	FALSE,	5,	1, 20, TRUE,
	"no_defense_sfx",		"Don't play attack-avoiding/neutralizing sound fx" },
#endif

	//50

	{ &c_cfg.player_list,		FALSE,	4,	1, 22, TRUE,
	"player_list",			"Show a more compact player list in @ screen" },//flush_command

	{ &c_cfg.player_list2,		FALSE,	4,	1, 23, TRUE,
	"player_list2",			"Compacts the player list in @ screen even more" },//fresh_before

#if 0
	{ &c_cfg.fresh_after,           FALSE,  1,      1, 24, FALSE,
	"fresh_after",                  "(obsolete) Flush output after every command" },
#else
	{ &c_cfg.view_animated_lite,	FALSE,	1,	1, 24, TRUE,
	"view_animated_lite",		"Animate lantern light, flickering in colour" },
#endif

	{ &c_cfg.censor_swearing,	TRUE,	4,	1, 25, TRUE,
	"censor_swearing",		"Censor certain swear words in public messages" },//fresh_message

	{ &c_cfg.safe_macros,		TRUE,	3,	1, 26, TRUE, /* was compress_savefile (broken&obsolete); use Term_flush() to clear macro execution */
	"safe_macros",			"Abort macro execution if an action fails" },

#if 0 /* resurrecting this further down */
	{ &c_cfg.hilite_player,		FALSE,	1,	1, 27, FALSE, /* (resurrect me) */
	"hilite_player",		"(broken) Hilite the player with the cursor" },
#else
	{ &c_cfg.view_bright_lite2,	FALSE,	1,	1, 27, TRUE,
	"view_bright_lite2",		"Use special colors to shade wall grids" },
#endif

	{ &c_cfg.view_yellow_lite,	TRUE,	1,	1, 28, TRUE,
	"view_yellow_lite",		"Use special colors for torch-lit grids" },

	{ &c_cfg.view_bright_lite,	TRUE,	1,	1, 29, TRUE,
	"view_bright_lite",		"Use special colors to shade floor grids" },

	{ &c_cfg.view_granite_lite,	TRUE,	1,	1, 30, TRUE,
	"view_granite_lite",		"Use special colors for lit wall grids" },

	{ &c_cfg.view_special_lite,	TRUE,	1,	1, 31, TRUE,	/* (59) */
	"view_special_lite",		"Use special colors for lit floor grids" },


	//60

	{ &c_cfg.easy_open,		TRUE,	3,	9, 60, TRUE, //#24 on page 2
	"easy_open",			"Automatically open doors" },

	{ &c_cfg.easy_disarm,		FALSE,	3,	9, 61, TRUE,
	"easy_disarm",			"Automatically disarm traps" },

	{ &c_cfg.easy_tunnel,		FALSE,	3,	9, 62, TRUE,
	"easy_tunnel",			"Automatically tunnel walls" },

#if 0
	{ &c_cfg.auto_destroy,		FALSE,	3,	9, 63, FALSE,
	"auto_destroy",			"(broken) No query to destroy known junks" },
#else
	{ &c_cfg.clear_inscr,		FALSE,	3,	9, 63, TRUE,
	"clear_inscr",			"Clear @-inscriptions on taking item ownership" },
#endif

#if 0
	{ &c_cfg.auto_inscribe,		FALSE,	3,	9, 64, FALSE,
	"auto_inscribe",		"Automatically inscribe books and so on" },
#else
	{ &c_cfg.auto_inscribe,		FALSE,	3,	9, 64, TRUE,
	"auto_inscribe",		"Use additional predefined auto-inscriptions" },
#endif

	{ &c_cfg.taciturn_messages,	FALSE,	4,	9, 65, TRUE,
	"taciturn_messages",		"Suppress server messages as far as possible" },

	{ &c_cfg.last_words,		TRUE,	1,	9, 66, TRUE,
	"last_words",			"Get last words when the character dies" },

	{ &c_cfg.limit_chat,		FALSE,	1,	9, 67, TRUE,
	"limit_chat",			"Chat only with players on the same floor" },

	{ &c_cfg.thin_down_flush,	TRUE,	3,	9, 68, TRUE,
	"thin_down_flush",		"Thin down screen flush signals to avoid freezing" },

	{ &c_cfg.auto_target,		FALSE,	3,	9, 69, TRUE,
	"auto_target",			"Automatically set target to the nearest enemy" },

	//70

	{ &c_cfg.autooff_retaliator,	FALSE,	3,	9, 70, TRUE,
	"autooff_retaliator",		"Stop the retaliator when protected by GoI etc" },

	{ &c_cfg.wide_scroll_margin,	TRUE,	3,	9, 71, TRUE,
	"wide_scroll_margin",		"Scroll the screen more frequently" },

	{ &c_cfg.fail_no_melee,		FALSE,	3,	9, 72, TRUE,
	"fail_no_melee",		"Stay still when item-retaliation fails" },

	{ &c_cfg.always_show_lists,	FALSE,	1,	9, 73, TRUE,
	"always_show_lists",		"Always show lists in item/skill selection" },

	{ &c_cfg.target_history,	FALSE,	4,	9, 74, TRUE,
	"target_history",		"Add target informations to the message history" },

	{ &c_cfg.linear_stats,		FALSE,	1,	9, 75, TRUE,
	"linear_stats",			"Stats are represented in a linear way" },

	{ &c_cfg.exp_need,		FALSE,	1,	9, 76, TRUE,
	"exp_need",			"Show the experience needed for next level" },

	{ &c_cfg.short_item_names,      FALSE,	4,	0, 77, TRUE,
        "short_item_names", 		"Don't display 'flavours' in item names" },



	{ &c_cfg.disable_flush,		FALSE,	3,	9, 78, TRUE,
	"disable_flush",		"Disable delays from flush signals" },

#if 0
	{ &c_cfg.speak_unique,		TRUE,	1,	13, xx, TRUE,
	"speak_unique",                 "Allow shopkeepers and uniques to speak" },
#endif

	{ &c_cfg.hide_unusable_skills,	TRUE,	4,	0, 79, TRUE,
	"hide_unusable_skills",		"Hide unusable skills" },

	//80

	{ &c_cfg.allow_paging,		TRUE,	4,	0, 80, TRUE,
	"allow_paging",			"Allow users to page you (recommended!)" },

	{ &c_cfg.audio_paging,		TRUE,	5,	0, 81, TRUE,
	"audio_paging",			"Use audio system for paging sounds, if available" },

	{ &c_cfg.paging_master_volume,	TRUE,	5,	0, 82, TRUE,
	"paging_master_vol",		"Play page sound at master volume" },

	{ &c_cfg.paging_max_volume,	TRUE,	5,	0, 83, TRUE,
	"paging_max_vol",		"Play page sound at maximum volume" },

	{ &c_cfg.no_ovl_close_sfx,	TRUE,	5,	0, 84, TRUE,
	"no_ovl_close_sfx",		"Prevent re-playing sfx received after <100ms gap" },

	{ &c_cfg.ovl_sfx_attack,	TRUE,	5,	0, 85, TRUE,
	"ovl_sfx_attack",		"Allow overlapping combat sounds of same type" },

	{ &c_cfg.half_sfx_attack,	FALSE,	5,	0, 86, TRUE,
	"half_sfx_attack",		"Skip every second attack sound" },

	{ &c_cfg.cut_sfx_attack,	FALSE,	5,	0, 87, TRUE,
	"cut_sfx_attack",		"Skip attack sounds based on speed and bpr" },

	{ &c_cfg.ovl_sfx_command,	TRUE,	5,	0, 88, TRUE,
	"ovl_sfx_command",		"Allow overlapping command sounds of same type" },

	{ &c_cfg.ovl_sfx_misc,		TRUE,	5,	0, 89, TRUE,
	"ovl_sfx_misc",			"Allow overlapping misc sounds of same type" },

	//90

	{ &c_cfg.ovl_sfx_mon_attack,	TRUE,	5,	0, 90, TRUE,
	"ovl_sfx_mon_attack",		"Allow overlapping monster attack sfx of same type" },

	{ &c_cfg.ovl_sfx_mon_spell,	TRUE,	5,	0, 91, TRUE, /* includes breaths, basically it's all S-flags */
	"ovl_sfx_mon_spell",		"Allow ovl. monster spell/breath sfx of same type" },

	{ &c_cfg.ovl_sfx_mon_misc,	TRUE,	5,	0, 92, TRUE,
	"ovl_sfx_mon_misc",		"Allow overlapping misc monster sfx of same type" },

	{ &c_cfg.no_monsterattack_sfx,	FALSE,	5,	1, 93, TRUE,
	"no_monsterattack_sfx",		"Don't play basic monster attack sound fx" },

	{ &c_cfg.no_shriek_sfx,		FALSE,	5,	1, 94, TRUE,
	"no_shriek_sfx",		"Don't play shriek (monster hasting) sound fx" },

	{ &c_cfg.keep_topline,		FALSE,	4,	0, 95, TRUE,
	"keep_topline",			"Don't clear messages in the top line if avoidable" },

	{ &c_cfg.no_store_sfx,		TRUE,	5,	1, 96, TRUE,
	"no_store_sfx",			"Don't play sound fx when entering/leaving a store" },

	{ &c_cfg.quiet_house_sfx,	TRUE,	5,	1, 97, TRUE,
	"quiet_house_sfx",		"Play quieter ambient/weather sound in buildings" },

	{ &c_cfg.no_house_sfx,		FALSE,	5,	1, 98, TRUE,
	"no_house_sfx",			"Don't play ambient/weather sound in buildings" },

	{ &c_cfg.no_weather,		FALSE,	4,	1, 99, TRUE,
	"no_weather",			"Disable weather visuals and sounds completely" },

	{ &c_cfg.hilite_player,		FALSE,	4,	1, 100, TRUE,
	"hilite_player",		"Hilite your character icon with the cursor" },
};


cptr melee_techniques[16] =
{
  "Sprint",
  "Taunt",
  "Jump",
  "Distract",

#if 0
  "Stab",
  "Slice",
  "Quake",
  "Sweep",

  "Bash",
  "Knock-back",
  "Charge",
  "Flash bomb",

  "Spin",
  "Berserk",
  "Shadow jump",
  "Instant cloak",

#else

  "Bash",
  "Knock back",
  "Charge",
  "Flash bomb",

  "Cloak",
  "Spin",
  "Assassinate",
  "Berserk",

  "",
  "Shadow jump",
  "Shadow run",
  "Instant cloak",
#endif
};

cptr ranged_techniques[16] =
{

  "Flare missile",
  "Precision shot",
  "Craft some ammunition",
  "Double-shot",

  "Barrage",
  "XXX",
  "XXX",
  "XXX",

  "XXX",
  "XXX",
  "XXX",
  "XXX",

  "XXX",
  "XXX",
  "XXX",
  "XXX",
};

