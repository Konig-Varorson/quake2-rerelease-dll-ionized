# Quake II: Ionized
Source code for a Quake II Remaster mod.

## What is Ionized?
Quake II: Ionized is a mod designed with three purposes in mind.
* First, to rebalance the core gameplay loop to make more use of under-utilized elements.
* Second, to finish and implement cut content present in the code and idVault, such as the gnorta and shocker.
* Third, to add additional resources for mappers, such as making the grapple and trap's meatcubes worldspawns.

### What's Next?
Version 0.2 began encorporating compatibilities with Unseen, Zaero, Citadel, and 25th Anniversary Mappack.
Version 0.3 will be focused on finishing Zaero compatbility and finishing Quake 4 Demake entities. It will also make some progress on porting choice Quake 1 enemies, and restoring some cut content, but this won't be the focus.

Ionized won't be doing a full compatibility with Unseen, as most of the remaining content ranges from silly to limited. Several concepts will be carried over with Q4 demakes (for example: Sauron->Heavy Hover Tank, Dathren and Assassin -> Tacticals), however.

## Changelog
### Version 0.2
* Remade foundation using Paril's PSX Mod: https://www.moddb.com/mods/quake-ii-psx All code was redone from scratch.
* bg_local.h:
  * Added some needed array listings for new ammos and powerups.
* g_items.cpp:
  * WIP of Disintegrator, Discharger (PSX Cut weapon), FlameGun (PSX Cut weapon, redubbed Immolator), and some Zaero weapon integrations
  * New ammo types: Flares (Zaero), Fuel (Cut PSX) and Battery (Cut PSX)
  * New keys: Clearance Pass (Zaero), Lab Key (Zaero), Landing Area Pass (Zaero), Slime Key (Zaero), Lava Key (Zaero), Energy Key (Zaero), and an alternative Green Key (PSX)
* g_local.h:
  * Prepped Zaero, Oblivion, and cut weapons and powerups.
* g_misc.cpp:
  * Removed spawnflags for skin changes on misc_eastertank, misc_easterchick, and misc_easterchick2 - found out that you can use rgba key in editor to alter skin of models, will use this in FGD instead
  * Added misc_crate, misc_crate_medium, misc_crate_small, and misc_seat as movable objects from Zaero
  * Added misc_commdish from Zaero
  * Added misc_bulldog_s, misc_bulldog_m, and misc_bulldog_l (models from Zaero) as new trains/objects like misc_stroggship and misc_bigviper.
  * Reworked spawnable gibs to be randomized based on enemy. New gibs can be spawned via misc_gib_(enemy) as depicted in FGD.
* g_monster.cpp:
  * Copied PSX Guardian's boss powerup response for use on all bosses, which adds power shields to bosses when players use powerups (Quad, Double, Dualfire, Invulnerability for now)
  * Removed old Widow's powerup copy (didn't actually work properly before I found out, code has been moved to Jorg and Makron instead).
  * Added Gladiator Beta's phalanx attack, Gekk spit attack, and a new lightning attack for universal use. (TODO: add guardian rockets here too)
* g_spawn.cpp:
  * Added ability to spawn new misc objects and enemies. Full list of finished spawnables in FGD.
  * Removed monster_guardian_stand and monster_widow_stand (truth be told, their function can be done by mappers; might readd later)
* g_target.cpp:
  * Removed target_give, target_remove, and target_kill as they weren't functioning properly. Plan to re-add later.
* g_weapon.cpp:
  * Added code for Gekk Acid Spit, and prototypes of Discharger and Oblivion's PlasmaPistol/PlasmaRifle's attacks.
* m_boss2.cpp:
  * Changed boss powerup to PSX Guardian's.
  * Rebalanced health and armor for standard Hornet and Hornet Guardian (N64 Hornet).
  * New spawnable: monster_boss2_guardian. Spawns a monster_boss2 with Spawnflag 8 behavior.
  * New monster: Hornet Titan, monster_boss2_titan (from Citadel)
    * Scaled 1.5x in size
    * Has body armor and power shield, scaled health by skill and player count
    * Fires classic-styled bullets and N64-styled homing rockets.
* m_boss31.cpp:
  * Revamped Powerup Copying.
  * Rebalanced health and armor.
* m_boss32.cpp:
  * Revamped Powerup Copying.
  * Rebalanced health and armor.
* m_brain.cpp:
  * Added multiple skins code.
* m_flipper.cpp:
  * Added multiple skins code.
* m_gladiator.cpp
  * New monster: BFGladiator, monster_bfgladiator (from 25th Anniversary Mappack)
    * Scaled 1.15x
    * Has 500 health, 100 Body Armor.
    * Fires a BFG.
* m_guardian.cpp
  * Added multiple skins code.
* m_gunner.cpp
  * Added multiple skins code.
* m_guncmdr.cpp
  * Added multiple skins code.
  * Removed scaling on power screen.
* m_hound.cpp + m_hound.h
  * Hellhound enemy, ported from Zaero.
  * Fast speed, medium damage, low health.
* m_infantry.cpp
  * Added multiple skins code.
  * New enemy: Handler Infantry, monster_infantry_handler.
    * Uses idVault camo skin with custom pain skin.
    * Fires flechettes instead of bullets.
  * New enemy: Heavy Infantry, monster_heavy, from Unseen.
    * Scaled 1.15x
    * Has 160 health, 100 combat armor.
    * Fires rockets instead of bullets.
* m_medic.cpp
  * Removed scaling on Medic Commander's armor.
* m_mutant.cpp:
  * Removed alpha mutant's health scaling on difficulty and coop.
* m_shambler.cpp:
  * Removed health scaling on difficulty and coop.
* m_sentien.cpp + m_sentien.h:
  * Sentien enemy, ported from Zaero.
  * Slow speed, hitscan damage, high health. Tank alternative.
* m_soldier.cpp:
  * New enemy: Light Soldier P, monster_psoldier_light, from Unseen.
    * Copy of Light Soldier with new skin, 30 health, and 100 combat armor.
  * New enemy: Shotgun Soldier P, monster_psoldier, from Unseen.
    * Copy of Shotgun Soldier with new skin, 40 health, and 100 combat armor.
  * New enemy: Machinegun Soldier P, monster_psoldier_ss, from Unseen.
    * Copy of Machinegun Soldier with new skin, 50 health, and 100 combat armor.
  * New enemy: Hyper Light Soldier P, monster_psoldier_special, from Unseen.
    * Rapid fire Light Soldier, has 50 health and 100 combat armor.
  * New enemy: Hyper Shotgun Soldier P, monster_psoldier_elite, from Unseen.
    * Rapid fire Shotgun Soldier, has 50 health and 100 combat armor.
  * Changed monster_soldier_railgun to monster_psoldier_death, from Unseen.
  * New enemy: Soldier Captain P, monster_psoldier_captain, from Unseen.
    * Miniboss version of psoldier_death.
    * Fires more accurate rails, has 240 health, 100 combat armor, and 100 power screen.
  * Changed monster_soldier_glight to monster_soldier_cobalt, from Citadel. Changed combat armor to power screen.
  * New enemy: Disruptor Soldier, monster_soldier_dist, from 25th Anniversary Mappack
     * Fires a disruptor tracker. Has 80 health, 100 power screen.
  * New enemy: Deatomizer Soldier, monster_soldier_deatom, from Oblivion.
     * Fires a disruptor tracker (temporary). Has 80 health, 100 combat armor.
* m_supertank.cpp:
  * Changed boss powerup to PSX Guardian's.
  * New monster: Super Tank Gamma Class, monster_boss5_gamma, from Citadel.
    * Scaled at 1.25x
    * Has both Body Armor and Power Shield.
    * Currently fires bullets, grenades, and heat-seeking rockets.
* m_tank.cpp:
  * Merged Tank Guardian and Tank Commander Guardian into one. Skin will change if map is placed in /q64/ folder.
  * New spawnable: monster_tank_guardian. Spawns a monster_tank with Spawnflag 8 behavior.
* p_weapon.cpp:
  * Prepping weapons from Zaero and cut content.
* rogue/g_carrier.cpp:
  * Changed boss powerup to PSX Guardian's.
  * Added multiple skins code.
  * Rebalanced health and armor.
* rogue/g_turret.cpp:
  * Finished code for Heatbeam turrets and added skin code for it.
* rogue/g_widow.cpp:
  * Restored and simplified old powerup copy code.
  * Added multiple skins code.
  * Rebalanced health and armor.
* rogue/g_widow2.cpp:
  * Copied simplified powerup copy to fix bug that prevented widow2's damage from increasing when copying double or quad powerups.
  * Added multiple skins code.
  * Rebalanced health and armor.
* xatrix/m_xatrix_gekk.cpp:
  * Added multiple skins code.

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
