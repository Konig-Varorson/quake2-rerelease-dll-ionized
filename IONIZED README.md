# Quake II: Ionized
Source code for a Quake II Remaster mod.

## What is Ionized?
Quake II: Ionized is a mod designed with three purposes in mind.
* First, to rebalance the core gameplay loop to make more use of under-utilized elements, such as expanding the Black Widow Guardians' powerup copy to other bosses.
* Second, to finish and implement cut content present in the code and idVault, such as the gnorta and shocker.
* Third, to add additional resources for mappers, such as new enemies and new of triggers.

## Changelog
### Version 0.1
* g_items.cpp:
  * Pickup_Bandolier and Pickup_Pack now increase ammo cap for Tesla and Trap.
  * Gave Weapon_Grapple a worldspawn pickup
  * Allow dropping Weapon_Blaster
  * Prepared to re-add the beta's Disintegrator (weapon_beta_disintegrator).
  * Reduced pickup quantity of ammo_slug from 10 to 6.
  * Added small ammos that provide half the amount of medium ammos for Shells, Bullets, Rockets, Cells, and Slugs. Utilizes md2 models from idVault.
* g_local.h:
  * Prepared to re-add the beta's Disintegrator (weapon_beta_disintegrator).
  * Expanded end of unit splash to display 16 levels instead of default 8.
  * Added some variables for boss powerup (see g_monster.cpp).
  * Added cvar for target_print.
* g_misc.cpp:
  * Added spawnflags for misc_eastertank, misc_easterchick, and misc_easterchick2 to utilize alternative skins.
  * Added misc_gib to replace the three old gibs with spawnflags to spawn between them. Old functions still exist but will be hidden in FGD (so no breaking old maps).
  * Added the following gib entities, each with multiple spawnflags (if available, 4 gibs per monster): misc_gib_mutant, misc_gib_gekk, misc_gib_technician, misc_gib_shambler, misc_gib_strogg1, misc_gib_strogg2, misc_gib_strogg3, misc_gib_strogg_big1, misc_gib_strogg_big2.
     * TO DO: Randomize the gibs if without spawnflag for each.
  * Merged contents of rogue/g_rogue_misc.cpp and rogue/g_xatrix_misc.cpp; deleted old files.
* g_monster.cpp:
  * Copied Black Widow Guardian's powerup copy into here for finale boss usage (Jorg, Makron, BWG, BWG2).
    *  Expanded to copy Dualfire and Invisibility (WIP, commented out).
    *  Also adds combat armor in certain situations.
  * Made a weaker version of BWG's powerup copy for non-finale bosses (Supertanks, Hornets, Guardian, Carrier).
    *  These only add power armor outside of Nightmare; does nothing on Easy.
* g_spawn.cpp:
  * Added spawning tags for new misc and monsters, including but not limited to:
     * Xatrix trap meat: item_foodcube
     * small ammo: ammo_shells_small, ammo_bullets_small, ammo_rockets_small, ammo_cells_small, ammo_slugs_small
     * Above mentioned `misc_gib`s: misc_gib, misc_gib_mutant, misc_gib_gekk, misc_gib_technician, misc_gib_shambler, misc_gib_strogg1, misc_gib_strogg2, misc_gib_strogg3, misc_gib_strogg_big1, misc_gib_strogg_big2.
     * New scenic "mosnters": monster_guardian_stand, monster_widow_stand
     * New monsters (most WIP): monster_gnorta, monster_shocker, monster_berserk2, monster_dropper, monster_protector, monster_fixbot2, monster_alphamutant, monster_mimic, monster_soldier_railgun, monster_soldier_glight, monster_soldier_lightning
  * Made it possible to customize armor type and quantity for monsters.
* g_target.cpp:
  * Target_Goal no longer mutes, but plays track78 (from N64 campaign). Might change to a different track if this doesn't fit.
  * Muted target_blaster entity (still functional but simpler).
  * Added new entities:
     * target_shooter_blaster: replaces `target_blaster`; has additional spawnflags to swap default yellow blaster for blue blaster, green blaster, flechette, and ion ripper; lowered default volume (1->0.75) and allows scaling it in editor
     * target_shooter_rockets: shooter that fires rockets by default; additional spawnflags to swap for grenades, phalanx, and BFG; has same scaling volume
     * target_print: allows printing messages to all clients regardless of who triggers, to bypass messages only being shown if players trigger things.
     * target_give: gives the player an item of mapper's choice.
     * target_kill: automatically kills the activator (easier than target_hurt).
     * INCOMPLETE: target_remove: removes an item from player's inventory.
* g_trigger.cpp:
  * Added new entities:
     * trigger_setskill: Like Quake 1, allows changing the skill level in the map.
* g_weapon.cpp:
  * Merged contents of rogue/g_rogue_newweap.cpp and rogue/g_xatrix_weapons.cpp; deleted old files.
* m_arachnid.cpp:
  * Added code for pain skin.
  * Buffed attack damage to match original PSX numbers (railgun 35->50; melee 15->20).
  * Added combat armor (100, scaled on difficulty and coop).
  * Altered gibs that spawn on death.
  * Setup for beta (monster_protector), inspired by Quake 4 Stream Protectors:
     * Added code for skin
     * Has Combat Armor (100, scaled on difficulty)
     * Railgun attack replaced with heat-seeking rockets.
     * Stronger melee damage (20->30)
     * Will be further updated after Paril's PSX goes live in October.
* m_berserk.cpp:
  * Buffed jump attack damage (8->16) (original damage at remaster launch was 32).
  * New monster_berserk2, inspired by Quake 4 Berserker:
     * Added code for skin
     * Has Jacket Armor (100, scaled on difficulty)
     * Higher jump attack damage (16->24).
     * INCOMPLETE: Include lightning effects to attacks at a later date.
* m_boss2.cpp:
  * Added boss powerup.
  * Added code for alternate skin for N64 spawnflag.
  * Changed behavior for Hornets with N64 Guardian spawnflag:
     * Blasters replaced with Flechettes, increased damage (2->10).
     * Reduced health (2,000->1,500) but scales on difficulty and coop.
     * Added power armor (400, scaled on difficulty and coop), but is not available in /q64/ subfolder maps (such as Quake II 64 campaign).
  * Added body armor for non-N64 spawnflag Hornets (100, scales on difficulty and coop).
* m_boss31.cpp:
  * Added boss powerup copying.
  * Added spawnflag which alters behavior:
    * WITH SPAWNFLAG: Doesn't spawn makron; has both body armor (200) and power armor (600); health (8000) and all armor scales on difficulty and coop.
    * WITHOUT SPAWNFLAG: default behavior; reduced health (8,000->7,000) but added body armor (200), both health and armor scaled on difficulty and coop.
* m_boss32.cpp:
  * Added boss powerup.
  * Added health scaling on difficulty and coop.
  * Added power armor (600, scales on difficulty and coop).
* m_float.cpp:
  * Added code for additional skins.
  * Added new monster_mimic:
     * Blue blasters instead of yellow (same damage).
     * New skin code.
     * Mandatory use of barrel disguise spawnflag (they're _mimics_).
     * Added jacket armor (100, scales to difficulty).
     * INCOMPLETE: Add self-detonation on melee range.
* m_flyer.cpp:
  * Added code for additional skins; new skin num for monster_kamikaze.
* m_gladiator.cpp:
  * Rebalanced beta gladiator:
     * Buffed health back up a bit (250->400; was 750 in pre-remaster).
     * Reduced scale a tad to match reduced mass.
     * Power armor -> power screen by default; reduced armor value (250->100) but scales on difficulty and coop.
* m_guardian.cpp:
  * Added boss powerup.
  * Added code for pain skin.
  * Added code for a standing version of the Guardian (modified from monster_tank_stand).
  * Will be further updated after Paril's PSX goes live in October.
* m_guncmdr.cpp:
  * Power armor -> power screen by default; reduced armor value (200->100) but scales on difficulty and coop.
* m_medic.cpp:
  * Monster_medic_commander now has Combat Armor (100, scales to difficulty and coop).
* m_mutant.cpp:
  * Added code for more skins.
  * Added new monster_alphamutant:
     * Uses new skin
     * Buffed health (300->400, scales on difficulty and coop).
     * Buffed melee damage (40-50 -> 50-70)
     * Increased scale (1->1.25) and mass (300->400).
* m_parasite.cpp:
  * Add code for new skins
  * New monster_dropper:
     * Uses new skin
     * TO DO: Replace drainer with launching grenades that spawn various monster_soldiers. 
* m_shambler.cpp:
  * Added code for pain skin.
  * Fixed a bug with spawnflag_1 and aim accuracy (was supposed to be spawnflag_8).
  * Added code for gib head from Q1; expanded the gibs a bit. Chonky boy deserves more gibs.
  * Health now scales on difficulty and coop.
* m_soldier.cpp:
  * New monster_soldier_glight:
     * Fires a green blaster with higher accuracy than soldier_light.
     * Has combat armor (100, that scales on difficulty).
  * New monster_soldier_railgun:
     * Fires a railgun.
     * Has combat armor (100, that scales on difficulty).
  * New monster_soldier_lightning:
     * Fires a hitscan lightning bolt (like shambler). 
     * Has combat armor (100, that scales on difficulty).
* m_supertank.cpp:
  * Added boss powerup.
  * Separated heat-seeking rockets and powershield into two spawnflags for greater mapper control.
  * Rebalanced supertank:
     * Has body armor (100, scales to difficulty and coop). Negated by power shield spawnflag.
  * Rebalanced beta supertank:
     * Now uses armor-piercing Flechettes instead of bullets with higher damage (6->10).
* m_tank.cpp:
  * Added boss powerup for N64 Guardian spawnflag.
  * Added code for new skins for Tank Guardian and Tank Commander Guardian
  * Separated functionality of N64 Guardian spawnflag between Tank and Tank Commander and rebalance non-Guardian Tank and Tank Commander:
    * Tank: has blue blaster with reduced speed (800->600).
    * Tank Commander: Increased machinegun damage (20->25) and blaster damage (30->35), all Commanders have heat-seeking rockets now, added Combat armor (default 100, scaled on difficulty and coop)
    * Tank Guardian: Increased blaster damage (30->35), grenade launcher replaces machinegun (pending to change on balance check), and Combat armor (default 100, scaled on difficulty and coop)
    * Tank Commander Guardian: has green blaster with higher damage (30->40), railgun replaces machinegun that deal more damage (25->50), always has heat-seeking rockets now, increased health (2000, scaled on difficulty and coop), and Body armor (default 100, scaled on difficulty and coop)
  * Added spawnflags for monster_tank_stand to allow using any non-pain skin. Default is the Tank Commander Guardian (skin 6).
* p_client.cpp:
  * Increased default capacity of Tesla from 5 to 10.
* p_weapon.cpp:
  * Hypreblaster now uses blue blaster bolts.
  * Increased damage of railgun from 125 to 135 in single player (compensated by reduction of ammo pickup quantity in g_items.cpp).
* rogue/g_carrier.cpp:
  * Added boss powerup.
  * Rebalanced health scaling in coop.
  * Added body armor (100, scales on difficulty and coop).
  * Doubled scaling for reinforcement slots in coop.
* rogue/g_turret.cpp:
  * Finished code for Heatbeam turrets and added skin code for it.
* rogue/g_widow.cpp:
  * Replaced old powerup copy code with new boss powerup code.
  * Completely rebalanced reinforcement spawns:
     * ~~Expanded roster to include monster_arachnid.~~ (removed from v0.1 due to game crashing bug, hope to have fixed later)
     * Now uses same system as carrier for number of spawns.
  * Added code for a standing version of the Black Widow Guardian (modified from monster_tank_stand).
* rogue/g_widow2.cpp:
  * Replaced old powerup copy code with new boss powerup code.
  * Completely rebalanced reinforcement spawns:
     * ~~Expanded roster to include monster_arachnid.~~ (removed from v0.1 due to game crashing bug, hope to have fixed later)
     * Now uses same system as carrier for number of spawns.
  * Added Body Armor (200, scales to difficulty and coop).
  * Rebalanced health (2800->3000 default; easy and normal health is the same); rebalanced coop health scaling.
* xatrix/m_xatrix_fixbot.cpp:
  * New monster_fixbot2:
     * Replace blaster with laser, lower damage (15->1) and higher DPS.
     * Has Jacket Armor (50).
* ionized/g_gnorta.cpp:
  * Initial files for restoring/finishing Gnorta
* ionized/g_shocker.cpp:
  * Initial files for restoring/finishing Shocker.

## What's Next?
Short term goals:
* Make MP powerups available in SP
* Replace ambient music swap from track11 (which is boss2's track) to tracks 64-76 (N64 tracks)
* Add spawning effect (from Medic Commander/Black Widow) to spawn triggered foes
* See if possible to add green outline (power armor) and gray segmented (armor) to healthbar; add numbers for exact amount
* Rework Hunter Sphere to be usable in SP.
* Add additional attacks/abilities to enemies in Nightmare difficulty / Co-Op.

Long term goals:
* Add entities from Quake 1, Zaero, and Oblivion - dependent on legalities.
* Demake choice Quake 4 enemies.
