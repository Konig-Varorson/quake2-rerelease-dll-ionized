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
* g_misc.cpp:
  * Added spawnflags for misc_eastertank, misc_easterchick, and misc_easterchick2 to utilize alternative skins.
  * Added a *lot* of gibs mirroring misc_gib_head, etc. for target_spawner usage.
* g_monster.cpp:
  * Copied Black Widow Guardian's powerup copy into here for finale boss usage (Jorg, Makron, BWG, BWG2).
    *  Expanded to copy Dualfire and Invisibility (WIP, commented out).
    *  Also adds combat armor in certain situations.
  * Made a weaker version of BWG's powerup copy for non-finale bosses (Supertanks, Hornets, Guardian, Carrier).
    *  These only add power armor outside of Nightmare; does nothing on Easy.
* g_spawn.cpp:
  * Added spawning tags for new misc and monsters.
* g_target.cpp:
  * Target_Goal no longer mutes, but plays track78 (from N64 campaign). Might change to a different track if this doesn't fit.
* m_arachnid.cpp:
  * Added code for pain skin.
  * Buffed attack damage to match original PSX numbers (railgun 35->50; melee 15->20).
  * Altered gibs that spawn on death.
* m_berserk.cpp:
  * Buffed jump attack damage from 8 to 16 (original damage at remaster launch was 32).
* m_boss2.cpp:
  * Added boss powerup.
  * Added code for alternate skin for N64 spawnflag.
  * Changed N64 Hornet to using Flechettes instead of Blasters.
  * Buffed Hornets with powershield and lower health, and both with co-op scaling when in in a map in the /maps/q64 folder.
* m_boss31.cpp:
  * Added boss powerup.
  * Added co-op health and armor scaling.
* m_boss32.cpp:
  * Added boss powerup.
  * Added co-op health and armor scaling.
* m_guardian.cpp:
  * Added boss powerup.
  * Added code for pain skin.
  * Added code for a standing version of the Guardian (modified from monster_tank_stand).
* m_shambler.cpp:
  * Added code for pain skin.
  * Added code for gib head from Q1; expanded the gibs a bit. Chonky boy deserves more gibs.
* m_supertank.cpp:
  * Added boss powerup.
  * Beta Supertank now uses armor-piercing Flechettes instead of bullets.
  * Standard Supertank has slightly reduced health and added combat armor. Coop scaling to increase both.
  * Removed heat seeking rockets from Power Shield spawnflag, now only used by Beta tanks.
* m_tank.cpp:
  * Added boss powerup for N64 Guardian spawnflag.
  * N64 Guardian spawnflag changes yellow blasters to green blasters that deal more damage (30->40).
  * N64 Guardian spawnflag changes machinegun to railgun attacks that deal more damage (20->50).
  * Added an or condition to heat seeking rockets so N64 Guardian spawnflag also sets it.
  * Added Combat Armor to Tank Commander and Tank Guardian.
  * Added Co-Op scaling health and armor to Tank Guardian.
  * Added code for new skin for Tank Guardian.
  * Added spawnflags for monster_tank_guardian to allow using any skin. Default is the new third skinset.
* p_client.cpp:
  * Increased default capacity of Tesla from 5 to 10.
* p_weapon.cpp:
  * Increased damage of railgun from 125 to 135 in single player (compensated by reduction of ammo pickup quantity in g_items.cpp).
* rogue/g_widow.cpp:
  * Uses new boss powerup code.
  * Added code for a standing version of the Black Widow Guardian (modified from monster_tank_stand).
* rogue/g_widow2.cpp:
  * Uses new boss powerup code.
  * Added Body Armor, reduced health to comepnsate.
* ionized/*
  * Initial files for restoring/finishing Gnorta, Shocker, and red mutant enemies. Still WIP.

## What's Next?
Short term goals:
* Add spawnable for meat cube health from traps
* Make MP powerups available in SP
* Replace ambient music swap from track11 (which is boss2's track) to tracks 64-76 (N64 tracks)
* Add spawnflag to Jorg that prevents spawning a Makron
* Add spawning effect (from Medic Commander/Black Widow) to spawn triggered foes
* Add green outline (power armor) and gray segmented (armor) to healthbar; add numbers for exact amount
* Expand Target_Blaster with more projectiles and similar targets for more tricks and traps.
* Rework Hunter Sphere to be usable in SP.
* Add additional attacks/abilities to enemies in Nightmare difficulty.

Long term goals:
* Add entities from Quake 1, Zaero, and Oblivion - dependent on legalities.
* Demake choice Quake 4 enemies.
