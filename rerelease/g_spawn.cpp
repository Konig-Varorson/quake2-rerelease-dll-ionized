// Copyright (c) ZeniMax Media Inc.
// Licensed under the GNU General Public License 2.0.

#include "g_local.h"

struct spawn_t
{
	const char *name;
	void (*spawn)(edict_t *ent);
};

void SP_info_player_start(edict_t *ent);
void SP_info_player_deathmatch(edict_t *ent);
void SP_info_player_coop(edict_t *ent);
void SP_info_player_intermission(edict_t *ent);

void SP_func_plat(edict_t *ent);
void SP_func_rotating(edict_t *ent);
void SP_func_button(edict_t *ent);
void SP_func_door(edict_t *ent);
void SP_func_door_secret(edict_t *ent);
void SP_func_door_rotating(edict_t *ent);
void SP_func_water(edict_t *ent);
void SP_func_train(edict_t *ent);
void SP_func_conveyor(edict_t *self);
void SP_func_wall(edict_t *self);
void SP_func_object(edict_t *self);
void SP_func_explosive(edict_t *self);
void SP_func_timer(edict_t *self);
void SP_func_areaportal(edict_t *ent);
void SP_func_clock(edict_t *ent);
void SP_func_killbox(edict_t *ent);
void SP_func_eye(edict_t *ent); // [Paril-KEX]
void SP_func_animation(edict_t *ent); // [Paril-KEX]
void SP_func_spinning(edict_t *ent); // [Paril-KEX]

void SP_trigger_always(edict_t *ent);
void SP_trigger_once(edict_t *ent);
void SP_trigger_multiple(edict_t *ent);
void SP_trigger_relay(edict_t *ent);
void SP_trigger_push(edict_t *ent);
void SP_trigger_hurt(edict_t *ent);
void SP_trigger_key(edict_t *ent);
void SP_trigger_counter(edict_t *ent);
void SP_trigger_elevator(edict_t *ent);
void SP_trigger_gravity(edict_t *ent);
void SP_trigger_monsterjump(edict_t *ent);
void SP_trigger_flashlight(edict_t *self); // [Paril-KEX]
void SP_trigger_fog(edict_t *self); // [Paril-KEX]
void SP_trigger_coop_relay(edict_t *self); // [Paril-KEX]
void SP_trigger_health_relay(edict_t *self); // [Paril-KEX]
void SP_trigger_safe_fall(edict_t *ent); // [Paril-KEX]

void SP_target_temp_entity(edict_t *ent);
void SP_target_speaker(edict_t *ent);
void SP_target_explosion(edict_t *ent);
void SP_target_changelevel(edict_t *ent);
void SP_target_secret(edict_t *ent);
void SP_target_goal(edict_t *ent);
void SP_target_splash(edict_t *ent);
void SP_target_spawner(edict_t *ent);
void SP_target_blaster(edict_t *ent);
void SP_target_crosslevel_trigger(edict_t *ent);
void SP_target_crosslevel_target(edict_t *ent);
void SP_target_crossunit_trigger(edict_t *ent); // [Paril-KEX]
void SP_target_crossunit_target(edict_t *ent); // [Paril-KEX]
void SP_target_laser(edict_t *self);
void SP_target_help(edict_t *ent);
void SP_target_actor(edict_t *ent);
void SP_target_lightramp(edict_t *self);
void SP_target_earthquake(edict_t *ent);
void SP_target_character(edict_t *ent);
void SP_target_string(edict_t *ent);
void SP_target_camera(edict_t* self); // [Sam-KEX]
void SP_target_gravity(edict_t* self); // [Sam-KEX]
void SP_target_soundfx(edict_t* self); // [Sam-KEX]
void SP_target_light(edict_t *self); // [Paril-KEX]
void SP_target_poi(edict_t *ent); // [Paril-KEX]
void SP_target_music(edict_t *ent);
void SP_target_healthbar(edict_t *self); // [Paril-KEX]
void SP_target_autosave(edict_t *self); // [Paril-KEX]
void SP_target_sky(edict_t *self); // [Paril-KEX]
void SP_target_achievement(edict_t *self); // [Paril-KEX]
void SP_target_story(edict_t *self); // [Paril-KEX]

void SP_worldspawn(edict_t *ent);

void SP_dynamic_light(edict_t* self);
void SP_light(edict_t *self);
void SP_light_mine1(edict_t *ent);
void SP_light_mine2(edict_t *ent);
void SP_info_null(edict_t *self);
void SP_info_notnull(edict_t *self);
void SP_info_landmark (edict_t* self); // [Paril-KEX]
void SP_info_world_text(edict_t * self);
void SP_misc_player_mannequin(edict_t * self);
void SP_misc_model(edict_t *self); // [Paril-KEX]
void SP_path_corner(edict_t *self);
void SP_point_combat(edict_t *self);
void SP_info_nav_lock(edict_t *self); // [Paril-KEX]

void SP_misc_explobox(edict_t *self);
void SP_misc_banner(edict_t *self);
void SP_misc_satellite_dish(edict_t *self);
void SP_misc_actor(edict_t *self);
void SP_misc_gib_arm(edict_t *self);
void SP_misc_gib_leg(edict_t *self);
void SP_misc_gib_head(edict_t *self);
void SP_misc_insane(edict_t *self);
void SP_misc_deadsoldier(edict_t *self);
void SP_misc_viper(edict_t *self);
void SP_misc_viper_bomb(edict_t *self);
void SP_misc_bigviper(edict_t *self);
void SP_misc_strogg_ship(edict_t *self);
void SP_misc_teleporter(edict_t *self);
void SP_misc_teleporter_dest(edict_t *self);
void SP_misc_blackhole(edict_t *self);
void SP_misc_eastertank(edict_t *self);
void SP_misc_easterchick(edict_t *self);
void SP_misc_easterchick2(edict_t *self);

void SP_misc_flare(edict_t* ent); // [Sam-KEX]
void SP_misc_hologram(edict_t *ent);
void SP_misc_lavaball(edict_t *ent);

void SP_monster_berserk(edict_t *self);
void SP_monster_gladiator(edict_t *self);
void SP_monster_gunner(edict_t *self);
void SP_monster_infantry(edict_t *self);
void SP_monster_soldier_light(edict_t *self);
void SP_monster_soldier(edict_t *self);
void SP_monster_soldier_ss(edict_t *self);
void SP_monster_tank(edict_t *self);
void SP_monster_medic(edict_t *self);
void SP_monster_flipper(edict_t *self);
void SP_monster_chick(edict_t *self);
void SP_monster_parasite(edict_t *self);
void SP_monster_flyer(edict_t *self);
void SP_monster_brain(edict_t *self);
void SP_monster_floater(edict_t *self);
void SP_monster_hover(edict_t *self);
void SP_monster_mutant(edict_t *self);
void SP_monster_supertank(edict_t *self);
void SP_monster_boss2(edict_t *self);
void SP_monster_jorg(edict_t *self);
void SP_monster_boss3_stand(edict_t *self);
void SP_monster_makron(edict_t *self);
// Paril
void SP_monster_tank_stand(edict_t *self);
void SP_monster_guardian(edict_t *self);
void SP_monster_arachnid(edict_t *self);
void SP_monster_guncmdr(edict_t *self);

void SP_monster_commander_body(edict_t *self);

void SP_turret_breach(edict_t *self);
void SP_turret_base(edict_t *self);
void SP_turret_driver(edict_t *self);

// RAFAEL 14-APR-98
void SP_monster_soldier_hypergun(edict_t *self);
void SP_monster_soldier_lasergun(edict_t *self);
void SP_monster_soldier_ripper(edict_t *self);
void SP_monster_fixbot(edict_t *self);
void SP_monster_gekk(edict_t *self);
void SP_monster_chick_heat(edict_t *self);
void SP_monster_gladb(edict_t *self);
void SP_monster_boss5(edict_t *self);
void SP_rotating_light(edict_t *self);
void SP_object_repair(edict_t *self);
void SP_misc_crashviper(edict_t *ent);
void SP_misc_viper_missile(edict_t *self);
void SP_misc_amb4(edict_t *ent);
void SP_target_mal_laser(edict_t *ent);
void SP_misc_transport(edict_t *ent);
// END 14-APR-98

void SP_misc_nuke(edict_t *ent);

//===========
// ROGUE
void SP_func_plat2(edict_t *ent);
void SP_func_door_secret2(edict_t *ent);
void SP_func_force_wall(edict_t *ent);
void SP_info_player_coop_lava(edict_t *self);
void SP_info_teleport_destination(edict_t *self);
void SP_trigger_teleport(edict_t *self);
void SP_trigger_disguise(edict_t *self);
void SP_monster_stalker(edict_t *self);
void SP_monster_turret(edict_t *self);
void SP_target_steam(edict_t *self);
void SP_target_anger(edict_t *self);
void SP_target_killplayers(edict_t *self);
// PMM - still experimental!
void SP_target_blacklight(edict_t *self);
void SP_target_orb(edict_t *self);
// pmm
void SP_hint_path(edict_t *self);
void SP_monster_carrier(edict_t *self);
void SP_monster_widow(edict_t *self);
void SP_monster_widow2(edict_t *self);
void SP_dm_tag_token(edict_t *self);
void SP_dm_dball_goal(edict_t *self);
void SP_dm_dball_ball(edict_t *self);
void SP_dm_dball_team1_start(edict_t *self);
void SP_dm_dball_team2_start(edict_t *self);
void SP_dm_dball_ball_start(edict_t *self);
void SP_dm_dball_speed_change(edict_t *self);
void SP_monster_kamikaze(edict_t *self);
void SP_turret_invisible_brain(edict_t *self);
void SP_misc_nuke_core(edict_t *self);
// ROGUE
//===========
//  ZOID
void SP_trigger_ctf_teleport(edict_t *self);
void SP_info_ctf_teleport_destination(edict_t *self);
// ZOID

void SP_monster_shambler(edict_t* self);

/*KONIG*/
//ZAERO
//void SP_misc_lasertripbomb(edict_t* self);
//void SP_load_mirrorlevel(edict_t* self);
//void SP_monster_autocannon(edict_t* self);
//void SP_monster_autocannon_floor(edict_t* self);
void SP_monster_sentien(edict_t* self);
//void SP_misc_securitycamera(edict_t* self);
void SP_monster_hound(edict_t* self);
void SP_monster_handler(edict_t* self);
void SP_misc_commdish(edict_t* self);
void SP_misc_crate(edict_t* self);
void SP_misc_crate_medium(edict_t* self);
void SP_misc_crate_small(edict_t* self);
//void SP_monster_zboss(edict_t* self);
//void SP_target_zboss_target(edict_t* self);
//void SP_func_barrier(edict_t* self);
void SP_misc_seat(edict_t* self);
//ZAERO INSPIRED
void SP_misc_bulldog_s(edict_t* self);
void SP_misc_bulldog_m(edict_t* self);
void SP_misc_bulldog_l(edict_t* self);
//OBLIVION
//void SP_misc_camera(edict_t* self);
//void SP_misc_camera_target(edict_t* self);
//void SP_misc_screenfade(edict_t* self);
//void SP_func_rotate_train(edict_t* self);
//void SP_trigger_misc_camera(edict_t* self);
//void SP_monster_kigrax(edict_t* self);
//void SP_monster_sentinel(edict_t* self);
//void SP_monster_tick(edict_t* self);
void SP_monster_soldier_deatom(edict_t* self);
//void SP_monster_cyborg(edict_t* self);
//UNSEEN
void SP_monster_psoldier(edict_t* self);
void SP_monster_psoldier_light(edict_t* self);
void SP_monster_psoldier_ss(edict_t* self);
void SP_monster_psoldier_special(edict_t* self);
void SP_monster_psoldier_elite(edict_t* self);
void SP_monster_psoldier_death(edict_t* self);
void SP_monster_psoldier_captain(edict_t* self);
void SP_monster_heavy(edict_t* self);
//PSX
//void SP_misc_dropship(edict_t* self);
//void SP_misc_fighter(edict_t* self);
//void SP_monster_flamer(edict_t* self);
//void SP_monster_blitz(edict_t* self);
//Q1
// void SP_monster_grunt(edict_t* self);
// void SP_monster_ogre(edict_t* self);
// void SP_monster_ogre_marksman(edict_t* self);
// void SP_monster_scrag(edict_t* self);
// void SP_monster_fiend(edict_t* self);
// void SP_monster_knight(edict_t* self);
// void SP_monster_zombie(edict_t* self);
// void SP_monster_chthon(edict_t* self);
// CITADEL
void SP_monster_soldier_cobalt(edict_t* self);
void SP_monster_boss5_gamma(edict_t* self);
void SP_monster_boss2_titan(edict_t* self);
// Q25
void SP_monster_bfgladiator(edict_t* self);
void SP_monster_soldier_dist(edict_t* self);
//IONIZED
void SP_trigger_setskill(edict_t* ent);
void SP_target_print(edict_t* ent);
void SP_target_shooter_blaster(edict_t* ent);
void SP_target_shooter_rockets(edict_t* ent);
void SP_item_foodcube(edict_t* self);
void SP_ammo_shells_small(edict_t* self);
void SP_ammo_bullets_small(edict_t* self);
void SP_ammo_rockets_small(edict_t* self);
void SP_ammo_cells_small(edict_t* self);
void SP_ammo_slugs_small(edict_t* self);
void SP_ammo_flechettes_small(edict_t* self);
void SP_ammo_prox_small(edict_t* self);
void SP_ammo_magslugs_small(edict_t* self);
void SP_ammo_rounds_small(edict_t* self);
void SP_misc_gib_head2(edict_t* self);
void SP_misc_gib_chest(edict_t* self);
void SP_misc_gib_gekk(edict_t* self);
void SP_misc_gib_mutant(edict_t* self);
void SP_misc_gib_berserk(edict_t* self);
void SP_misc_gib_chick(edict_t* self);
void SP_misc_gib_brain(edict_t* self);
void SP_misc_gib_glad(edict_t* self);
void SP_misc_gib_gunner(edict_t* self);
void SP_misc_gib_hover(edict_t* self);
void SP_misc_gib_infantry(edict_t* self);
void SP_misc_gib_medic(edict_t* self);
void SP_misc_gib_parasite(edict_t* self);
void SP_misc_gib_soldier(edict_t* self);
void SP_misc_gib_stalker(edict_t* self);
void SP_misc_gib_tank(edict_t* self);
void SP_misc_gib_boss1(edict_t* self);
void SP_misc_gib_boss2(edict_t* self);
void SP_misc_gib_boss3(edict_t* self);
void SP_misc_gib_carrier(edict_t* self);
void SP_misc_gib_strider(edict_t* self);
void SP_misc_gib_shambler(edict_t* self);
//cut monsters restored
void SP_monster_shocker(edict_t* self);
void SP_monster_gnorta(edict_t* self);
//DEMAKES
//void SP_monster_tactical_male(edict_t* self);
//void SP_monster_tactical_female(edict_t* self);
//void SP_monster_tactical_cyborg(edict_t* self);
void SP_monster_berserk2(edict_t* self);
void SP_monster_dropper(edict_t* self);
void SP_monster_protector(edict_t* self);
void SP_monster_fixbot2(edict_t* self);
// void SP_monster_voss(edict_t* self);
// void SP_monster_reject(edict_t* self);
//beta enemies
void SP_monster_soldier_discharger(edict_t* self);
void SP_monster_infantry_handler(edict_t* self);
void SP_monster_mimic(edict_t* self);
void SP_monster_tank_guardian(edict_t* self);
void SP_monster_boss2_guardian(edict_t* self);

// clang-format off
static const std::initializer_list<spawn_t> spawns = {
	{ "info_player_start", SP_info_player_start },
	{ "info_player_deathmatch", SP_info_player_deathmatch },
	{ "info_player_coop", SP_info_player_coop },
	{ "info_player_intermission", SP_info_player_intermission },

	{ "func_plat", SP_func_plat },
	{ "func_button", SP_func_button },
	{ "func_door", SP_func_door },
	{ "func_door_secret", SP_func_door_secret },
	{ "func_door_rotating", SP_func_door_rotating },
	{ "func_rotating", SP_func_rotating },
	{ "func_train", SP_func_train },
	{ "func_water", SP_func_water },
	{ "func_conveyor", SP_func_conveyor },
	{ "func_areaportal", SP_func_areaportal },
	{ "func_clock", SP_func_clock },
	{ "func_wall", SP_func_wall },
	{ "func_object", SP_func_object },
	{ "func_timer", SP_func_timer },
	{ "func_explosive", SP_func_explosive },
	{ "func_killbox", SP_func_killbox },
	{ "func_eye", SP_func_eye },
	{ "func_animation", SP_func_animation },
	{ "func_spinning", SP_func_spinning },

	{ "trigger_always", SP_trigger_always },
	{ "trigger_once", SP_trigger_once },
	{ "trigger_multiple", SP_trigger_multiple },
	{ "trigger_relay", SP_trigger_relay },
	{ "trigger_push", SP_trigger_push },
	{ "trigger_hurt", SP_trigger_hurt },
	{ "trigger_key", SP_trigger_key },
	{ "trigger_counter", SP_trigger_counter },
	{ "trigger_elevator", SP_trigger_elevator },
	{ "trigger_gravity", SP_trigger_gravity },
	{ "trigger_monsterjump", SP_trigger_monsterjump },
	{ "trigger_flashlight", SP_trigger_flashlight }, // [Paril-KEX]
	{ "trigger_fog", SP_trigger_fog }, // [Paril-KEX]
	{ "trigger_coop_relay", SP_trigger_coop_relay }, // [Paril-KEX]
	{ "trigger_health_relay", SP_trigger_health_relay }, // [Paril-KEX]
	{ "trigger_safe_fall", SP_trigger_safe_fall }, // [Paril-KEX]

	{ "target_temp_entity", SP_target_temp_entity },
	{ "target_speaker", SP_target_speaker },
	{ "target_explosion", SP_target_explosion },
	{ "target_changelevel", SP_target_changelevel },
	{ "target_secret", SP_target_secret },
	{ "target_goal", SP_target_goal },
	{ "target_splash", SP_target_splash },
	{ "target_spawner", SP_target_spawner },
	{ "target_blaster", SP_target_blaster },
	{ "target_crosslevel_trigger", SP_target_crosslevel_trigger },
	{ "target_crosslevel_target", SP_target_crosslevel_target },
	{ "target_crossunit_trigger", SP_target_crossunit_trigger }, // [Paril-KEX]
	{ "target_crossunit_target", SP_target_crossunit_target }, // [Paril-KEX]
	{ "target_laser", SP_target_laser },
	{ "target_help", SP_target_help },
	{ "target_actor", SP_target_actor },
	{ "target_lightramp", SP_target_lightramp },
	{ "target_earthquake", SP_target_earthquake },
	{ "target_character", SP_target_character },
	{ "target_string", SP_target_string },
	{ "target_camera", SP_target_camera }, // [Sam-KEX]
	{ "target_gravity", SP_target_gravity }, // [Sam-KEX]
	{ "target_soundfx", SP_target_soundfx }, // [Sam-KEX]
	{ "target_light", SP_target_light }, // [Paril-KEX]
	{ "target_poi", SP_target_poi }, // [Paril-KEX]
	{ "target_music", SP_target_music },
	{ "target_healthbar", SP_target_healthbar }, // [Paril-KEX]
	{ "target_autosave", SP_target_autosave }, // [Paril-KEX]
	{ "target_sky", SP_target_sky }, // [Paril-KEX]
	{ "target_achievement", SP_target_achievement }, // [Paril-KEX]
	{ "target_story", SP_target_story }, // [Paril-KEX]

	{ "worldspawn", SP_worldspawn },

	{ "dynamic_light", SP_dynamic_light },
	{ "light", SP_light },
	{ "light_mine1", SP_light_mine1 },
	{ "light_mine2", SP_light_mine2 },
	{ "info_null", SP_info_null },
	{ "func_group", SP_info_null },
	{ "info_notnull", SP_info_notnull },
	{ "info_landmark", SP_info_landmark },
	{ "info_world_text", SP_info_world_text },
	{ "path_corner", SP_path_corner },
	{ "point_combat", SP_point_combat },
	{ "info_nav_lock", SP_info_nav_lock },

	{ "misc_explobox", SP_misc_explobox },
	{ "misc_banner", SP_misc_banner },
	{ "misc_satellite_dish", SP_misc_satellite_dish },
	{ "misc_actor", SP_misc_actor },
	{ "misc_player_mannequin", SP_misc_player_mannequin },
	{ "misc_model", SP_misc_model }, // [Paril-KEX]
	{ "misc_gib_arm", SP_misc_gib_arm },
	{ "misc_gib_leg", SP_misc_gib_leg },
	{ "misc_gib_head", SP_misc_gib_head },
	{ "misc_insane", SP_misc_insane },
	{ "misc_deadsoldier", SP_misc_deadsoldier },
	{ "misc_viper", SP_misc_viper },
	{ "misc_viper_bomb", SP_misc_viper_bomb },
	{ "misc_bigviper", SP_misc_bigviper },
	{ "misc_strogg_ship", SP_misc_strogg_ship },
	{ "misc_teleporter", SP_misc_teleporter },
	{ "misc_teleporter_dest", SP_misc_teleporter_dest },
	{ "misc_blackhole", SP_misc_blackhole },
	{ "misc_eastertank", SP_misc_eastertank },
	{ "misc_easterchick", SP_misc_easterchick },
	{ "misc_easterchick2", SP_misc_easterchick2 },
	{ "misc_flare", SP_misc_flare }, // [Sam-KEX]
	{ "misc_hologram", SP_misc_hologram }, // Paril
	{ "misc_lavaball", SP_misc_lavaball }, // Paril

	{ "monster_berserk", SP_monster_berserk },
	{ "monster_gladiator", SP_monster_gladiator },
	{ "monster_gunner", SP_monster_gunner },
	{ "monster_infantry", SP_monster_infantry },
	{ "monster_soldier_light", SP_monster_soldier_light },
	{ "monster_soldier", SP_monster_soldier },
	{ "monster_soldier_ss", SP_monster_soldier_ss },
	{ "monster_tank", SP_monster_tank },
	{ "monster_tank_commander", SP_monster_tank },
	{ "monster_medic", SP_monster_medic },
	{ "monster_flipper", SP_monster_flipper },
	{ "monster_chick", SP_monster_chick },
	{ "monster_parasite", SP_monster_parasite },
	{ "monster_flyer", SP_monster_flyer },
	{ "monster_brain", SP_monster_brain },
	{ "monster_floater", SP_monster_floater },
	{ "monster_hover", SP_monster_hover },
	{ "monster_mutant", SP_monster_mutant },
	{ "monster_supertank", SP_monster_supertank },
	{ "monster_boss2", SP_monster_boss2 },
	{ "monster_boss3_stand", SP_monster_boss3_stand },
	{ "monster_jorg", SP_monster_jorg },
	// Paril: allow spawning makron
	{ "monster_makron", SP_monster_makron },
	// Paril: N64
	{ "monster_tank_stand", SP_monster_tank_stand },
	// Paril: PSX
	{ "monster_guardian", SP_monster_guardian },
	{ "monster_arachnid", SP_monster_arachnid },
	{ "monster_guncmdr", SP_monster_guncmdr },

	{ "monster_commander_body", SP_monster_commander_body },

	{ "turret_breach", SP_turret_breach },
	{ "turret_base", SP_turret_base },
	{ "turret_driver", SP_turret_driver },

	// RAFAEL
	{ "func_object_repair", SP_object_repair },
	{ "rotating_light", SP_rotating_light },
	{ "target_mal_laser", SP_target_mal_laser },
	{ "misc_crashviper", SP_misc_crashviper },
	{ "misc_viper_missile", SP_misc_viper_missile },
	{ "misc_amb4", SP_misc_amb4 },
	{ "misc_transport", SP_misc_transport },
	{ "misc_nuke", SP_misc_nuke },
	{ "monster_soldier_hypergun", SP_monster_soldier_hypergun },
	{ "monster_soldier_lasergun", SP_monster_soldier_lasergun },
	{ "monster_soldier_ripper", SP_monster_soldier_ripper },
	{ "monster_fixbot", SP_monster_fixbot },
	{ "monster_gekk", SP_monster_gekk },
	{ "monster_chick_heat", SP_monster_chick_heat },
	{ "monster_gladb", SP_monster_gladb },
	{ "monster_boss5", SP_monster_boss5 },
	// RAFAEL

	//==============
	// ROGUE
	{ "func_plat2", SP_func_plat2 },
	{ "func_door_secret2", SP_func_door_secret2 },
	{ "func_force_wall", SP_func_force_wall },
	{ "trigger_teleport", SP_trigger_teleport },
	{ "trigger_disguise", SP_trigger_disguise },
	{ "info_teleport_destination", SP_info_teleport_destination },
	{ "info_player_coop_lava", SP_info_player_coop_lava },
	{ "monster_stalker", SP_monster_stalker },
	{ "monster_turret", SP_monster_turret },
	{ "target_steam", SP_target_steam },
	{ "target_anger", SP_target_anger },
	{ "target_killplayers", SP_target_killplayers },
	// PMM - experiment
	{ "target_blacklight", SP_target_blacklight },
	{ "target_orb", SP_target_orb },
	// pmm
	{ "monster_daedalus", SP_monster_hover },
	{ "hint_path", SP_hint_path },
	{ "monster_carrier", SP_monster_carrier },
	{ "monster_widow", SP_monster_widow },
	{ "monster_widow2", SP_monster_widow2 },
	{ "monster_medic_commander", SP_monster_medic },
	{ "dm_tag_token", SP_dm_tag_token },
	{ "dm_dball_goal", SP_dm_dball_goal },
	{ "dm_dball_ball", SP_dm_dball_ball },
	{ "dm_dball_team1_start", SP_dm_dball_team1_start },
	{ "dm_dball_team2_start", SP_dm_dball_team2_start },
	{ "dm_dball_ball_start", SP_dm_dball_ball_start },
	{ "dm_dball_speed_change", SP_dm_dball_speed_change },
	{ "monster_kamikaze", SP_monster_kamikaze },
	{ "turret_invisible_brain", SP_turret_invisible_brain },
	{ "misc_nuke_core", SP_misc_nuke_core },
	// ROGUE
	//==============
	// ZOID
	{ "trigger_ctf_teleport", SP_trigger_ctf_teleport },
	{ "info_ctf_teleport_destination", SP_info_ctf_teleport_destination },
	{ "misc_ctf_banner", SP_misc_ctf_banner },
	{ "misc_ctf_small_banner", SP_misc_ctf_small_banner },
	{ "info_player_team1", SP_info_player_team1 },
	{ "info_player_team2", SP_info_player_team2 },
	// ZOID

	{ "monster_shambler", SP_monster_shambler },

	//KONIG
	//ZAERO
	//{ "misc_ired", SP_misc_lasertripbomb },
	//{ "monster_autocannon", SP_monster_autocannon },
	//{ "monster_autocannon_floor", SP_monster_autocannon_floor },
	{ "monster_sentien", SP_monster_sentien },
	//{ "misc_securitycamera", SP_misc_securitycamera },
	{ "monster_hound", SP_monster_hound },
	//{ "monster_handler", SP_monster_handler },
	//{ "monster_zboss", SP_monster_zboss },
	{ "misc_crate", SP_misc_crate },
	{ "misc_crate_medium", SP_misc_crate_medium },
	{ "misc_crate_small", SP_misc_crate_small },
	{ "misc_seat", SP_misc_seat },
	{ "misc_commdish", SP_misc_commdish },
	//{ "load_mirrorlevel", SP_load_mirrorlevel },
	//{ "func_barrier", SP_func_barrier },
	//{ "target_zboss_target", SP_target_zboss_target }
	// ZAERO INSPIRED
	{ "misc_bulldog_s", SP_misc_bulldog_s },
	{ "misc_bulldog_m", SP_misc_bulldog_m },
	{ "misc_bulldog_l", SP_misc_bulldog_l },
	//OBLIVION
	//{ "misc_camera", SP_misc_camera },
	//{ "misc_camera_target", SP_misc_camera_target },
	//{ "misc_screenfade", SP_misc_screenfade },
	//{ "func_rotate_train", SP_func_rotate_train },
	//{ "trigger_misc_camera", SP_trigger_misc_camera },
	//{ "monster_kigrax", SP_monster_kigrax },
	//{ "monster_sentinel", SP_monster_sentinel },
	//{ "monster_badass", SP_monster_sentinel }, //old compatibility
	//{ "monster_tick", SP_monster_tick },
	//{ "monster_spider", SP_monster_tick }, //old compatibility
	//{ "monster_soldier_deatom", SP_monster_soldier_deatom },
	//{ "monster_cyborg", SP_monster_cyborg },
	//UNSEEN
	{ "monster_psoldier_light", SP_monster_psoldier_light },
	{ "monster_psoldier", SP_monster_psoldier },
	{ "monster_psoldier_ss", SP_monster_psoldier_ss },
	{ "monster_psoldier_elite", SP_monster_psoldier_elite },
	{ "monster_psoldier_special", SP_monster_psoldier_special },
	{ "monster_psoldier_death", SP_monster_psoldier_death },
	{ "monster_psoldier_captain", SP_monster_psoldier_captain },
	{ "monster_heavy", SP_monster_heavy },
	//{ "monster_gunner2", SP_monster_guncmdr },
	//{ "monster_sauron", SP_monster_sauron },
	//{ "monster_droid", SP_monster_droid },
	//{ "monster_assassin", SP_monster_assassin },
	//{ "monster_datran", SP_monster_datran },
	//PSX
	//{ "misc_dropship", SP_misc_dropship },
	//{ "misc_fighter", SP_misc_fighter },
	//{ "monster_flamer", SP_monster_flamer },
	//{ "monster_blitz", SP_monster_blitz },
	//Q1
	//{ "light_flame_large_yellow", SP_light_flame_large_yellow },
	//{ "light_flame_small_yellow", SP_light_flame_small_yellow },
	//{ "light_flame_small_white", SP_light_flame_small_yellow },
	//{ "monster_grunt", SP_monster_grunt },
	//{ "monster_ogre", SP_monster_ogre },
	//{ "monster_ogre_marksman", SP_monster_ogre_marksman },
	//{ "monster_scrag", SP_monster_scrag },
	//{ "monster_fiend", SP_monster_fiend },
	//{ "monster_knight", SP_monster_knight },
	//{ "monster_zombie", SP_monster_zombie },
	//{ "monster_chthon", SP_monster_chthon },
	//CITADEL
	{ "monster_soldier_cobalt", SP_monster_soldier_cobalt },
	{ "monster_gunner_elite", SP_monster_guncmdr }, //Citadel compatibility
	{ "monster_chick_beta", SP_monster_chick_heat }, //Citadel compatibility
	{ "monster_gladiator_beta", SP_monster_gladb }, //Citadel compatibility
	{ "monster_boss5_gamma", SP_monster_boss5_gamma },
	{ "monster_boss2_titan", SP_monster_boss2_titan },
	//Q25
	{ "monster_bfgladiator", SP_monster_bfgladiator },
	{ "monster_soldier_dist", SP_monster_soldier_dist },
	//IONIZED
	{ "item_foodcube", SP_item_foodcube },
	{ "ammo_shells_small", SP_ammo_shells_small },
	{ "ammo_bullets_small", SP_ammo_bullets_small },
	{ "ammo_rockets_small", SP_ammo_rockets_small },
	{ "ammo_cells_small", SP_ammo_cells_small },
	{ "ammo_slugs_small", SP_ammo_slugs_small },
	{ "ammo_flechettes_small", SP_ammo_flechettes_small },
	{ "ammo_prox_small", SP_ammo_prox_small },
	{ "ammo_magslugs_small", SP_ammo_magslugs_small },
	{ "ammo_rounds_small", SP_ammo_rounds_small },

	{ "misc_gib_head2", SP_misc_gib_head2 },
	{ "misc_gib_chest", SP_misc_gib_chest },
	{ "misc_gib_gekk", SP_misc_gib_gekk },
	{ "misc_gib_mutant", SP_misc_gib_mutant },
	{ "misc_gib_berserk", SP_misc_gib_berserk },
	{ "misc_gib_chick", SP_misc_gib_chick },
	{ "misc_gib_brain", SP_misc_gib_brain },
	{ "misc_gib_glad", SP_misc_gib_glad },
	{ "misc_gib_gunner", SP_misc_gib_gunner },
	{ "misc_gib_hover", SP_misc_gib_hover },
	{ "misc_gib_infantry", SP_misc_gib_infantry },
	{ "misc_gib_medic", SP_misc_gib_medic },
	{ "misc_gib_parasite", SP_misc_gib_parasite },
	{ "misc_gib_soldier", SP_misc_gib_soldier },
	{ "misc_gib_stalker", SP_misc_gib_stalker },
	{ "misc_gib_strider", SP_misc_gib_strider },
	{ "misc_gib_tank", SP_misc_gib_tank },
	{ "misc_gib_boss1", SP_misc_gib_boss1 },
	{ "misc_gib_boss2", SP_misc_gib_boss2 },
	{ "misc_gib_boss3", SP_misc_gib_boss3 },
	{ "misc_gib_carrier", SP_misc_gib_carrier },
	{ "misc_gib_shambler", SP_misc_gib_shambler },
	{ "target_shooter_blaster", SP_target_shooter_blaster },
	{ "target_shooter_rockets", SP_target_shooter_rockets },
	//Cut monsters restored
	//{ "monster_gnorta", SP_monster_gnorta },
	//{ "monster_shocker", SP_monster_shocker },
	//Q4 demake enemies
	//{ "monster_tactical_male", SP_monster_tactical_male },
	//{ "monster_tactical_female", SP_monster_tactical_female },
	//{ "monster_tactical_cyborg", SP_monster_tactical_cyborg },
	{ "monster_berserk2", SP_monster_berserk2 },
	{ "monster_dropper", SP_monster_dropper },
	{ "monster_protector", SP_monster_protector },
	{ "monster_fixbot2", SP_monster_fixbot2 },
	//{ "monster_voss", SP_monster_voss },
	//{ "monster_reject", SP_monster_reject },
	//Beta enemies
	{ "monster_alphamutant", SP_monster_mutant },
	{ "monster_infantry_handler", SP_monster_infantry_handler },
	{ "monster_mimic", SP_monster_mimic },
	{ "monster_soldier_discharger", SP_monster_soldier_discharger },
	{ "monster_tank_guardian", SP_monster_tank_guardian }, //shortcut to monster_tank or monster_tank_commander spawnflags 8
	{ "monster_boss2_guardian", SP_monster_boss2_guardian } //shortcut to monster_boss2 spawnflags 8

};
// clang-format on

static const spawn_temp_t *current_st;
/*static*/ const spawn_temp_t spawn_temp_t::empty = {};

const spawn_temp_t &ED_GetSpawnTemp()
{
	if (!current_st)
	{
		gi.Com_Print("WARNING: empty spawntemp accessed; this is probably a code bug.\n");
		return spawn_temp_t::empty;
	}

	return *current_st;
}

/*
===============
ED_CallSpawn

Finds the spawn function for the entity and calls it
===============
*/
void ED_CallSpawn(edict_t *ent, const spawn_temp_t &spawntemp)
{
	gitem_t *item;
	int		 i;

	if (!ent->classname)
	{
		gi.Com_Print("ED_CallSpawn: nullptr classname\n");
		G_FreeEdict(ent);
		return;
	}

	current_st = &spawntemp;

	// PGM - do this before calling the spawn function so it can be overridden.
	ent->gravityVector[0] = 0.0;
	ent->gravityVector[1] = 0.0;
	ent->gravityVector[2] = -1.0;
	// PGM

	ent->sv.init = false;

	// FIXME - PMM classnames hack
	if (!strcmp(ent->classname, "weapon_nailgun"))
		ent->classname = GetItemByIndex(IT_WEAPON_ETF_RIFLE)->classname;
	if (!strcmp(ent->classname, "ammo_nails"))
		ent->classname = GetItemByIndex(IT_AMMO_FLECHETTES)->classname;
	if (!strcmp(ent->classname, "weapon_heatbeam"))
		ent->classname = GetItemByIndex(IT_WEAPON_PLASMABEAM)->classname;
	// pmm

	// check item spawn functions
	for (i = 0, item = itemlist; i < IT_TOTAL; i++, item++)
	{
		if (!item->classname)
			continue;
		if (!strcmp(item->classname, ent->classname))
		{
			// found it
			// before spawning, pick random item replacement
			if (g_dm_random_items->integer)
			{
				ent->item = item;
				item_id_t new_item = DoRandomRespawn(ent);

				if (new_item)
				{
					item = GetItemByIndex(new_item);
					ent->classname = item->classname;
				}
			}

			SpawnItem(ent, item, spawntemp);

			if (level.is_psx)
				ent->s.origin[2] += 15.f - (15.f * PSX_PHYSICS_SCALAR);

			current_st = nullptr;
			return;
		}
	}

	// check normal spawn functions
	for (auto &s : spawns)
	{
		if (!strcmp(s.name, ent->classname))
		{ // found it
			s.spawn(ent);

			// Paril: swap classname with stored constant if we didn't change it
			if (strcmp(ent->classname, s.name) == 0)
				ent->classname = s.name;

			current_st = nullptr;
			return;
		}
	}

	gi.Com_PrintFmt("{} doesn't have a spawn function\n", *ent);
	G_FreeEdict(ent);
	current_st = nullptr;
}

// Quick redirect to use empty spawntemp
void  ED_CallSpawn(edict_t *ent)
{
	ED_CallSpawn(ent, spawn_temp_t::empty);
}

/*
=============
ED_NewString
=============
*/
char *ED_NewString(const char *string)
{
	char	*newb, *new_p;
	int		i;
	size_t	l;

	l = strlen(string) + 1;

	newb = (char *) gi.TagMalloc(l, TAG_LEVEL);

	new_p = newb;

	for (i = 0; i < l; i++)
	{
		if (string[i] == '\\' && i < l - 1)
		{
			i++;
			if (string[i] == 'n')
				*new_p++ = '\n';
			else
				*new_p++ = '\\';
		}
		else
			*new_p++ = string[i];
	}

	return newb;
}

//
// fields are used for spawning from the entity string
//

struct field_t
{
	const char *name;
	void (*load_func) (edict_t *e, const char *s) = nullptr;
};

// utility template for getting the type of a field
template<typename>
struct member_object_container_type { };
template<typename T1, typename T2>
struct member_object_container_type<T1 T2::*> { using type = T2; };
template<typename T>
using member_object_container_type_t = typename member_object_container_type<std::remove_cv_t<T>>::type;

struct type_loaders_t
{
	template<typename T, std::enable_if_t<std::is_same_v<T, const char *>, int> = 0>
	static T load(const char *s)
	{
		return ED_NewString(s);
	}

	template<typename T, std::enable_if_t<std::is_integral_v<T>, int> = 0>
	static T load(const char *s)
	{
		return atoi(s);
	}

	template<typename T, std::enable_if_t<std::is_same_v<T, spawnflags_t>, int> = 0>
	static T load(const char *s)
	{
		return spawnflags_t(atoi(s));
	}

	template<typename T, std::enable_if_t<std::is_floating_point_v<T>, int> = 0>
	static T load(const char *s)
	{
		return atof(s);
	}

	template<typename T, std::enable_if_t<std::is_enum_v<T>, int> = 0>
	static T load(const char *s)
	{
		if constexpr (sizeof(T) > 4)
			return static_cast<T>(atoll(s));
		else
			return static_cast<T>(atoi(s));
	}

	template<typename T, std::enable_if_t<std::is_same_v<T, vec3_t>, int> = 0>
	static T load(const char *s)
	{
		vec3_t vec;
		static char vec_buffer[32];
		const char *token = COM_Parse(&s, vec_buffer, sizeof(vec_buffer));
		vec.x = atof(token);
		token = COM_Parse(&s);
		vec.y = atof(token);
		token = COM_Parse(&s);
		vec.z = atof(token);
		return vec;
	}
};

#define AUTO_LOADER_FUNC(M) \
	[](edict_t *e, const char *s) { \
		e->M = type_loaders_t::load<decltype(e->M)>(s); \
	}

static int32_t ED_LoadColor(const char *value)
{
	// space means rgba as values
	if (strchr(value, ' '))
	{
		static char color_buffer[32];
		std::array<float, 4> raw_values { 0, 0, 0, 1.0f };
		bool is_float = true;

		for (auto &v : raw_values)
		{
			const char *token = COM_Parse(&value, color_buffer, sizeof(color_buffer));

			if (*token)
			{
				v = atof(token);

				if (v > 1.0f)
					is_float = false;
			}
		}

		if (is_float)
			for (auto &v : raw_values)
				v *= 255.f;

		return ((int32_t) raw_values[3]) | (((int32_t) raw_values[2]) << 8) | (((int32_t) raw_values[1]) << 16) | (((int32_t) raw_values[0]) << 24);
	}

	// integral
	return atoi(value);
}

#define FIELD_COLOR(n, x) \
	{ n, [](edict_t *e, const char *s) { \
		e->x = ED_LoadColor(s); \
	} }

// clang-format off
// fields that get copied directly to edict_t
#define FIELD_AUTO(x) \
	{ #x, AUTO_LOADER_FUNC(x) }

#define FIELD_AUTO_NAMED(n, x) \
	{ n, AUTO_LOADER_FUNC(x) }

static const std::initializer_list<field_t> entity_fields = {
	FIELD_AUTO(classname),
	FIELD_AUTO(model),
	FIELD_AUTO(spawnflags),
	FIELD_AUTO(speed),
	FIELD_AUTO(accel),
	FIELD_AUTO(decel),
	FIELD_AUTO(target),
	FIELD_AUTO(targetname),
	FIELD_AUTO(pathtarget),
	FIELD_AUTO(deathtarget),
	FIELD_AUTO(healthtarget),
	FIELD_AUTO(itemtarget),
	FIELD_AUTO(killtarget),
	FIELD_AUTO(combattarget),
	FIELD_AUTO(message),
	FIELD_AUTO(team),
	FIELD_AUTO(wait),
	FIELD_AUTO(delay),
	FIELD_AUTO(random),
	FIELD_AUTO(move_origin),
	FIELD_AUTO(move_angles),
	FIELD_AUTO(style),
	FIELD_AUTO(style_on),
	FIELD_AUTO(style_off),
	FIELD_AUTO(crosslevel_flags),
	FIELD_AUTO(count),
	FIELD_AUTO(health),
	FIELD_AUTO(sounds),
	{ "light" },
	FIELD_AUTO(dmg),
	FIELD_AUTO(mass),
	FIELD_AUTO(volume),
	FIELD_AUTO(attenuation),
	FIELD_AUTO(map),
	FIELD_AUTO_NAMED("origin", s.origin),
	FIELD_AUTO_NAMED("angles", s.angles),
	{ "angle", [](edict_t *e, const char *value) {
		e->s.angles = {};
		e->s.angles[YAW] = atof(value);
	} },
	FIELD_COLOR("rgba", s.skinnum), // [Sam-KEX]
	FIELD_AUTO(hackflags), // [Paril-KEX] n64
	FIELD_AUTO_NAMED("alpha", s.alpha), // [Paril-KEX]
	FIELD_AUTO_NAMED("scale", s.scale), // [Paril-KEX]
	{ "mangle" }, // editor field
	FIELD_AUTO_NAMED("dead_frame", monsterinfo.start_frame), // [Paril-KEX]
	FIELD_AUTO_NAMED("frame", s.frame),
	FIELD_AUTO_NAMED("effects", s.effects),
	FIELD_AUTO_NAMED("renderfx", s.renderfx),

	// [Paril-KEX] fog keys
	FIELD_AUTO_NAMED("fog_color", fog.color),
	FIELD_AUTO_NAMED("fog_color_off", fog.color_off),
	FIELD_AUTO_NAMED("fog_density", fog.density),
	FIELD_AUTO_NAMED("fog_density_off", fog.density_off),
	FIELD_AUTO_NAMED("fog_sky_factor", fog.sky_factor),
	FIELD_AUTO_NAMED("fog_sky_factor_off", fog.sky_factor_off),
	
	FIELD_AUTO_NAMED("heightfog_falloff", heightfog.falloff),
	FIELD_AUTO_NAMED("heightfog_density", heightfog.density),
	FIELD_AUTO_NAMED("heightfog_start_color", heightfog.start_color),
	FIELD_AUTO_NAMED("heightfog_start_dist", heightfog.start_dist),
	FIELD_AUTO_NAMED("heightfog_end_color", heightfog.end_color),
	FIELD_AUTO_NAMED("heightfog_end_dist", heightfog.end_dist),

	FIELD_AUTO_NAMED("heightfog_falloff_off", heightfog.falloff_off),
	FIELD_AUTO_NAMED("heightfog_density_off", heightfog.density_off),
	FIELD_AUTO_NAMED("heightfog_start_color_off", heightfog.start_color_off),
	FIELD_AUTO_NAMED("heightfog_start_dist_off", heightfog.start_dist_off),
	FIELD_AUTO_NAMED("heightfog_end_color_off", heightfog.end_color_off),
	FIELD_AUTO_NAMED("heightfog_end_dist_off", heightfog.end_dist_off),

	// [Paril-KEX] func_eye stuff
	FIELD_AUTO_NAMED("eye_position", move_origin),
	FIELD_AUTO(vision_cone),

	// [Paril-KEX] for trigger_coop_relay
	FIELD_AUTO_NAMED("message2", map),
	FIELD_AUTO(mins),
	FIELD_AUTO(maxs),

	// [Paril-KEX] customizable bmodel animations
	FIELD_AUTO_NAMED("bmodel_anim_start", bmodel_anim.start),
	FIELD_AUTO_NAMED("bmodel_anim_end", bmodel_anim.end),
	FIELD_AUTO_NAMED("bmodel_anim_style", bmodel_anim.style),
	FIELD_AUTO_NAMED("bmodel_anim_speed", bmodel_anim.speed),
	FIELD_AUTO_NAMED("bmodel_anim_nowrap", bmodel_anim.nowrap),

	FIELD_AUTO_NAMED("bmodel_anim_alt_start", bmodel_anim.alt_start),
	FIELD_AUTO_NAMED("bmodel_anim_alt_end", bmodel_anim.alt_end),
	FIELD_AUTO_NAMED("bmodel_anim_alt_style", bmodel_anim.alt_style),
	FIELD_AUTO_NAMED("bmodel_anim_alt_speed", bmodel_anim.alt_speed),
	FIELD_AUTO_NAMED("bmodel_anim_alt_nowrap", bmodel_anim.alt_nowrap),

	// [Paril-KEX] customizable power armor stuff
	FIELD_AUTO_NAMED("power_armor_power", monsterinfo.power_armor_power),
	{ "power_armor_type", [](edict_t *s, const char *v)
		{
			int32_t type = atoi(v);

			if (type == 0)
				s->monsterinfo.power_armor_type = IT_NULL;
			else if (type == 1)
				s->monsterinfo.power_armor_type = IT_ITEM_POWER_SCREEN;
			else
				s->monsterinfo.power_armor_type = IT_ITEM_POWER_SHIELD;
		}
	},

	/* KONIG - customizable armor */
	FIELD_AUTO_NAMED("armor_power", monsterinfo.power_armor_power),
	{ "armor_type", [](edict_t* s, const char* v)
		{
			int32_t type = atoi(v);

			if (type == 0)
				s->monsterinfo.armor_type = IT_NULL;
			else if (type == 1)
				s->monsterinfo.armor_type = IT_ARMOR_JACKET;
			else if (type == 2)
				s->monsterinfo.armor_type = IT_ARMOR_COMBAT;
			else
				s->monsterinfo.armor_type = IT_ARMOR_BODY;
		}
	},
	FIELD_AUTO_NAMED("monster_slots", monsterinfo.monster_slots)
};

#undef AUTO_LOADER_FUNC

#define AUTO_LOADER_FUNC(M) \
	[](spawn_temp_t *e, const char *s) { \
		e->M = type_loaders_t::load<decltype(e->M)>(s); \
	}

struct temp_field_t
{
	const char *name;
	void (*load_func) (spawn_temp_t *e, const char *s) = nullptr;
};

// temp spawn vars -- only valid when the spawn function is called
// (copied to `st`)
static const std::initializer_list<temp_field_t> temp_fields = {
	FIELD_AUTO(lip),
	FIELD_AUTO(distance),
	FIELD_AUTO(height),
	FIELD_AUTO(noise),
	FIELD_AUTO(pausetime),
	FIELD_AUTO(item),

	FIELD_AUTO(gravity),
	FIELD_AUTO(sky),
	FIELD_AUTO(skyrotate),
	FIELD_AUTO(skyaxis),
	FIELD_AUTO(skyautorotate),
	FIELD_AUTO(minyaw),
	FIELD_AUTO(maxyaw),
	FIELD_AUTO(minpitch),
	FIELD_AUTO(maxpitch),
	FIELD_AUTO(nextmap),
	FIELD_AUTO(music),  // [Edward-KEX]
	FIELD_AUTO(instantitems),
	FIELD_AUTO(radius), // [Paril-KEX]
	FIELD_AUTO(hub_map),
	FIELD_AUTO(achievement),

	FIELD_AUTO_NAMED("shadowlightradius", sl.data.radius),
	FIELD_AUTO_NAMED("shadowlightresolution", sl.data.resolution),
	FIELD_AUTO_NAMED("shadowlightintensity", sl.data.intensity),
	FIELD_AUTO_NAMED("shadowlightstartfadedistance", sl.data.fade_start),
	FIELD_AUTO_NAMED("shadowlightendfadedistance", sl.data.fade_end),
	FIELD_AUTO_NAMED("shadowlightstyle", sl.data.lightstyle),
	FIELD_AUTO_NAMED("shadowlightconeangle", sl.data.coneangle),
	FIELD_AUTO_NAMED("shadowlightstyletarget", sl.lightstyletarget),

	FIELD_AUTO(goals),

	FIELD_AUTO(image),

	FIELD_AUTO(fade_start_dist),
	FIELD_AUTO(fade_end_dist),
	FIELD_AUTO(start_items),
	FIELD_AUTO(no_grapple),
	FIELD_AUTO(health_multiplier),
	FIELD_AUTO(physics_flags_sp),
	FIELD_AUTO(physics_flags_dm),

	FIELD_AUTO(reinforcements),
	FIELD_AUTO(noise_start),
	FIELD_AUTO(noise_middle),
	FIELD_AUTO(noise_end),

	FIELD_AUTO(loop_count),

	FIELD_AUTO(primary_objective_string),
	FIELD_AUTO(secondary_objective_string),

	FIELD_AUTO(primary_objective_title),
	FIELD_AUTO(secondary_objective_title)
};
// clang-format on

/*
===============
ED_ParseField

Takes a key/value pair and sets the binary values
in an edict
===============
*/
void ED_ParseField(const char *key, const char *value, edict_t *ent, spawn_temp_t &st)
{
	// check st first
	for (auto &f : temp_fields)
	{
		if (Q_strcasecmp(f.name, key))
			continue;

		st.keys_specified.emplace(f.name);

		// found it
		if (f.load_func)
			f.load_func(&st, value);
		
		return;
	}

	// now entity
	for (auto &f : entity_fields)
	{
		if (Q_strcasecmp(f.name, key))
			continue;

		st.keys_specified.emplace(f.name);

		// [Paril-KEX]
		if (!strcmp(f.name, "bmodel_anim_start") || !strcmp(f.name, "bmodel_anim_end"))
			ent->bmodel_anim.enabled = true;

		// found it
		if (f.load_func)
			f.load_func(ent, value);

		return;
	}

	gi.Com_PrintFmt("{} is not a valid field\n", key);
}

/*
====================
ED_ParseEdict

Parses an edict out of the given string, returning the new position
ed should be a properly initialized empty edict.
====================
*/
const char *ED_ParseEdict(const char *data, edict_t *ent, spawn_temp_t &st)
{
	bool  init;
	char  keyname[256];
	const char *com_token;

	init = false;
	
	// go through all the dictionary pairs
	while (1)
	{
		// parse key
		com_token = COM_Parse(&data);
		if (com_token[0] == '}')
			break;
		if (!data)
			gi.Com_Error("ED_ParseEntity: EOF without closing brace");

		Q_strlcpy(keyname, com_token, sizeof(keyname));

		// parse value
		com_token = COM_Parse(&data);
		if (!data)
			gi.Com_Error("ED_ParseEntity: EOF without closing brace");

		if (com_token[0] == '}')
			gi.Com_Error("ED_ParseEntity: closing brace without data");

		init = true;

		// keynames with a leading underscore are used for utility comments,
		// and are immediately discarded by quake
		if (keyname[0] == '_')
		{
			// [Sam-KEX] Hack for setting RGBA for shadow-casting lights
			if(!strcmp(keyname, "_color"))
				ent->s.skinnum = ED_LoadColor(com_token);

			continue;
		}

		ED_ParseField(keyname, com_token, ent, st);
	}

	if (!init)
		memset(ent, 0, sizeof(*ent));

	return data;
}

/*
================
G_FindTeams

Chain together all entities with a matching team field.

All but the first will have the FL_TEAMSLAVE flag set.
All but the last will have the teamchain field set to the next one
================
*/

// adjusts teams so that trains that move their children
// are in the front of the team
void G_FixTeams()
{
	edict_t *e, *e2, *chain;
	uint32_t i, j;
	uint32_t c;

	c = 0;
	for (i = 1, e = g_edicts + i; i < globals.num_edicts; i++, e++)
	{
		if (!e->inuse)
			continue;
		if (!e->team)
			continue;
		if (!strcmp(e->classname, "func_train") && e->spawnflags.has(SPAWNFLAG_TRAIN_MOVE_TEAMCHAIN))
		{
			if (e->flags & FL_TEAMSLAVE)
			{
				chain = e;
				e->teammaster = e;
				e->teamchain = nullptr;
				e->flags &= ~FL_TEAMSLAVE;
				e->flags |= FL_TEAMMASTER;
				c++;
				for (j = 1, e2 = g_edicts + j; j < globals.num_edicts; j++, e2++)
				{
					if (e2 == e)
						continue;
					if (!e2->inuse)
						continue;
					if (!e2->team)
						continue;
					if (!strcmp(e->team, e2->team))
					{
						chain->teamchain = e2;
						e2->teammaster = e;
						e2->teamchain = nullptr;
						chain = e2;
						e2->flags |= FL_TEAMSLAVE;
						e2->flags &= ~FL_TEAMMASTER;
						e2->movetype = MOVETYPE_PUSH;
						e2->speed = e->speed;
					}
				}
			}
		}
	}

	gi.Com_PrintFmt("{} teams repaired\n", c);
}

void G_FindTeams()
{
	edict_t *e, *e2, *chain;
	uint32_t i, j;
	uint32_t c, c2;

	c = 0;
	c2 = 0;
	for (i = 1, e = g_edicts + i; i < globals.num_edicts; i++, e++)
	{
		if (!e->inuse)
			continue;
		if (!e->team)
			continue;
		if (e->flags & FL_TEAMSLAVE)
			continue;
		chain = e;
		e->teammaster = e;
		e->flags |= FL_TEAMMASTER;
		c++;
		c2++;
		for (j = i + 1, e2 = e + 1; j < globals.num_edicts; j++, e2++)
		{
			if (!e2->inuse)
				continue;
			if (!e2->team)
				continue;
			if (e2->flags & FL_TEAMSLAVE)
				continue;
			if (!strcmp(e->team, e2->team))
			{
				c2++;
				chain->teamchain = e2;
				e2->teammaster = e;
				chain = e2;
				e2->flags |= FL_TEAMSLAVE;
			}
		}
	}

	// ROGUE
	G_FixTeams();
	// ROGUE

	gi.Com_PrintFmt("{} teams with {} entities\n", c, c2);
}

// inhibit entities from game based on cvars & spawnflags
inline bool G_InhibitEntity(edict_t *ent)
{
	// dm-only
	if (deathmatch->integer)
		return ent->spawnflags.has(SPAWNFLAG_NOT_DEATHMATCH);

	// coop flags
	if (coop->integer && ent->spawnflags.has(SPAWNFLAG_NOT_COOP))
		return true;
	else if (!coop->integer && ent->spawnflags.has(SPAWNFLAG_COOP_ONLY))
		return true;

	// skill
	return ((skill->integer == 0) && ent->spawnflags.has(SPAWNFLAG_NOT_EASY)) ||
		   ((skill->integer == 1) && ent->spawnflags.has(SPAWNFLAG_NOT_MEDIUM)) ||
		   ((skill->integer >= 2) && ent->spawnflags.has(SPAWNFLAG_NOT_HARD));
}

void setup_shadow_lights();

// [Paril-KEX]
void G_PrecacheInventoryItems()
{
	if (deathmatch->integer)
		return;

	for (size_t i = 0; i < game.maxclients; i++)
	{
		gclient_t *cl = g_edicts[i + 1].client;

		if (!cl)
			continue;

		for (item_id_t id = IT_NULL; id != IT_TOTAL; id = static_cast<item_id_t>(id + 1))
			if (cl->pers.inventory[id])
				PrecacheItem(GetItemByIndex(id));
	}
}

// [Paril-KEX]
static void G_PrecacheStartItems()
{
	if (!*g_start_items->string)
		return;

	char token_copy[MAX_TOKEN_CHARS];
	const char *token;
	const char *ptr = g_start_items->string;

	while (*(token = COM_ParseEx(&ptr, ";")))
	{
		Q_strlcpy(token_copy, token, sizeof(token_copy));
		const char *ptr_copy = token_copy;

		const char *item_name = COM_Parse(&ptr_copy);
		gitem_t *item = FindItemByClassname(item_name);

		if (!item || !item->pickup)
			gi.Com_ErrorFmt("Invalid g_start_item entry: {}\n", item_name);

		if (*ptr_copy)
			COM_Parse(&ptr_copy);

		PrecacheItem(item);
	}
}

#include <map>

/*
==============
SpawnEntities

Creates a server's entity / program execution context by
parsing textual entity definitions out of an ent file.
==============
*/
void SpawnEntities(const char *mapname, const char *entities, const char *spawnpoint)
{
	level.is_spawning = true;

	// clear cached indices
	cached_soundindex::clear_all();
	cached_modelindex::clear_all();
	cached_imageindex::clear_all();

	edict_t *ent;
	int		 inhibit;
	const char	 *com_token;

	int skill_level = clamp(skill->integer, 0, 3);
	if (skill->integer != skill_level)
		gi.cvar_forceset("skill", G_Fmt("{}", skill_level).data());

	SaveClientData();

	gi.FreeTags(TAG_LEVEL);

	memset(&level, 0, sizeof(level));
	memset(g_edicts, 0, game.maxentities * sizeof(g_edicts[0]));

	// all other flags are not important atm
	globals.server_flags &= SERVER_FLAG_LOADING;

	Q_strlcpy(level.mapname, mapname, sizeof(level.mapname));
	// Paril: fixes a bug where autosaves will start you at
	// the wrong spawnpoint if they happen to be non-empty
	// (mine2 -> mine3)
	if (!game.autosaved)
		Q_strlcpy(game.spawnpoint, spawnpoint, sizeof(game.spawnpoint));

	level.is_n64 = strncmp(level.mapname, "q64/", 4) == 0;
	level.is_psx = strncmp(level.mapname, "psx/", 4) == 0;

	level.coop_scale_players = 0;
	level.coop_health_scaling = clamp(g_coop_health_scaling->value, 0.f, 1.f);

	// set client fields on player ents
	for (uint32_t i = 0; i < game.maxclients; i++)
	{
		g_edicts[i + 1].client = game.clients + i;

		// "disconnect" all players since the level is switching
		game.clients[i].pers.connected = false;
		game.clients[i].pers.spawned = false;
	}

	ent = nullptr;
	inhibit = 0;

	// reserve some spots for dead player bodies for coop / deathmatch
	InitBodyQue();

	// parse ents
	while (1)
	{
		// parse the opening brace
		com_token = COM_Parse(&entities);
		if (!entities)
			break;
		if (com_token[0] != '{')
			gi.Com_ErrorFmt("ED_LoadFromFile: found \"{}\" when expecting {{", com_token);

		if (!ent)
			ent = g_edicts;
		else
			ent = G_Spawn();

		spawn_temp_t st {};

		entities = ED_ParseEdict(entities, ent, st);

		// remove things (except the world) from different skill levels or deathmatch
		if (ent != g_edicts)
		{
			if (G_InhibitEntity(ent))
			{
				G_FreeEdict(ent);
				inhibit++;
				continue;
			}

			ent->spawnflags &= ~SPAWNFLAG_EDITOR_MASK;
		}

		if (!ent)
			gi.Com_Error("invalid/empty entity string!");

		// PGM - do this before calling the spawn function so it can be overridden.
		ent->gravityVector[0] = 0.0;
		ent->gravityVector[1] = 0.0;
		ent->gravityVector[2] = -1.0;
		// PGM

		ED_CallSpawn(ent, st);

		ent->s.renderfx |= RF_IR_VISIBLE; // PGM
	}

	gi.Com_PrintFmt("{} entities inhibited\n", inhibit);

	// precache start_items
	G_PrecacheStartItems();

	// precache player inventory items
	G_PrecacheInventoryItems();

	G_FindTeams();

	// ZOID
	CTFSpawn();
	// ZOID

	// ROGUE
	if (deathmatch->integer)
	{
		if (g_dm_random_items->integer)
			PrecacheForRandomRespawn();
	}
	else
	{
		InitHintPaths(); // if there aren't hintpaths on this map, enable quick aborts
	}
	// ROGUE

	// ROGUE	-- allow dm games to do init stuff right before game starts.
	if (deathmatch->integer && gamerules->integer)
	{
		if (DMGame.PostInitSetup)
			DMGame.PostInitSetup();
	}
	// ROGUE

	setup_shadow_lights();

	if (gi.cvar("g_print_spawned_entities", "0", CVAR_NOFLAGS)->integer)
	{
		std::map<std::string, int> entities;
		int total_monster_health = 0;

		for (size_t i = 0; i < globals.num_edicts; i++)
		{
			edict_t *e = &globals.edicts[i];

			if (!e->inuse)
				continue;
			else if (!e->item && !e->monsterinfo.stand)
				continue;

			const char *cn = e->classname ? e->classname : "noclass";

			if (auto f = entities.find(cn); f != entities.end())
			{
				f->second++;
			}
			else
			{
				entities.insert({ cn, 1 });
			}

			if (e->monsterinfo.stand)
			{
				total_monster_health += e->health;
			}

			if (e->item && strcmp(e->classname, e->item->classname))
			{
				cn = e->item->classname ? e->item->classname : "noclass";

				if (auto f = entities.find(cn); f != entities.end())
				{
					f->second++;
				}
				else
				{
					entities.insert({ cn, 1 });
				}
			}
		}

		gi.Com_PrintFmt("total monster health: {}\n", total_monster_health);
		
		for (auto &e : entities)
		{
			gi.Com_PrintFmt("{}: {}\n", e.first, e.second);
		}
	}

	level.is_spawning = false;
}

//===================================================================

#include "g_statusbar.h"

// create & set the statusbar string for the current gamemode
static void G_InitStatusbar()
{
	statusbar_t sb;

	// ---- shared stuff that every gamemode uses ----
	sb.yb(-24);

	// health
	sb.xv(0).hnum().xv(50).pic(STAT_HEALTH_ICON);

	// ammo
	sb.ifstat(STAT_AMMO_ICON).xv(100).anum().xv(150).pic(STAT_AMMO_ICON).endifstat();

	// armor
	sb.ifstat(STAT_ARMOR_ICON).xv(200).rnum().xv(250).pic(STAT_ARMOR_ICON).endifstat();

	// selected item
	sb.ifstat(STAT_SELECTED_ICON).xv(296).pic(STAT_SELECTED_ICON).endifstat();

	sb.yb(-50);

	// picked up item
	sb.ifstat(STAT_PICKUP_ICON).xv(0).pic(STAT_PICKUP_ICON).xv(26).yb(-42).loc_stat_string(STAT_PICKUP_STRING).yb(-50).endifstat();

	// selected item name
	sb.ifstat(STAT_SELECTED_ITEM_NAME).yb(-34).xv(319).loc_stat_rstring(STAT_SELECTED_ITEM_NAME).yb(-58).endifstat();

	// timer
	sb.ifstat(STAT_TIMER_ICON).xv(262).num(2, STAT_TIMER).xv(296).pic(STAT_TIMER_ICON).endifstat();
	
	sb.yb(-50);

	// help / weapon icon
	sb.ifstat(STAT_HELPICON).xv(150).pic(STAT_HELPICON).endifstat();

	// ---- gamemode-specific stuff ----
	if (!deathmatch->integer)
	{
		// SP/coop
		// key display
		// move up if the timer is active
		// FIXME: ugly af
		sb.ifstat(STAT_TIMER_ICON).yb(-76).endifstat();
		sb.ifstat(STAT_SELECTED_ITEM_NAME)
			.yb(-58)
			.ifstat(STAT_TIMER_ICON)
				.yb(-84)
			.endifstat()
		.endifstat();
		sb.ifstat(STAT_KEY_A).xv(296).pic(STAT_KEY_A).endifstat();
		sb.ifstat(STAT_KEY_B).xv(272).pic(STAT_KEY_B).endifstat();
		sb.ifstat(STAT_KEY_C).xv(248).pic(STAT_KEY_C).endifstat();

		if (coop->integer)
		{
			// top of screen coop respawn display
			sb.ifstat(STAT_COOP_RESPAWN).xv(0).yt(0).loc_stat_cstring2(STAT_COOP_RESPAWN).endifstat();

			// coop lives
			sb.ifstat(STAT_LIVES).xr(-16).yt(2).lives_num(STAT_LIVES).xr(0).yt(28).loc_rstring("$g_lives").endifstat();
		}

		sb.ifstat(STAT_HEALTH_BARS).yt(24).health_bars().endifstat();
	}
	else if (G_TeamplayEnabled())
	{
		CTFPrecache();

		// ctf/tdm
		// red team
		sb.yb(-110).ifstat(STAT_CTF_TEAM1_PIC).xr(-26).pic(STAT_CTF_TEAM1_PIC).endifstat().xr(-78).num(3, STAT_CTF_TEAM1_CAPS);
		// joined overlay
		sb.ifstat(STAT_CTF_JOINED_TEAM1_PIC).yb(-112).xr(-28).pic(STAT_CTF_JOINED_TEAM1_PIC).endifstat();

		// blue team
		sb.yb(-83).ifstat(STAT_CTF_TEAM2_PIC).xr(-26).pic(STAT_CTF_TEAM2_PIC).endifstat().xr(-78).num(3, STAT_CTF_TEAM2_CAPS);
		// joined overlay
		sb.ifstat(STAT_CTF_JOINED_TEAM2_PIC).yb(-85).xr(-28).pic(STAT_CTF_JOINED_TEAM2_PIC).endifstat();

		if (ctf->integer)
		{
			// have flag graph
			sb.ifstat(STAT_CTF_FLAG_PIC).yt(26).xr(-24).pic(STAT_CTF_FLAG_PIC).endifstat();
		}

		// id view state
		sb.ifstat(STAT_CTF_ID_VIEW).xv(112).yb(-58).stat_pname(STAT_CTF_ID_VIEW).endifstat();

		// id view color
		sb.ifstat(STAT_CTF_ID_VIEW_COLOR).xv(96).yb(-58).pic(STAT_CTF_ID_VIEW_COLOR).endifstat();

		if (ctf->integer)
		{
			// match
			sb.ifstat(STAT_CTF_MATCH).xl(0).yb(-78).stat_string(STAT_CTF_MATCH).endifstat();
		}

		// team info
		sb.ifstat(STAT_CTF_TEAMINFO).xl(0).yb(-88).stat_string(STAT_CTF_TEAMINFO).endifstat();
	}
	else
	{ 
		// dm
		// frags
		sb.xr(-50).yt(2).num(3, STAT_FRAGS);

		// spectator
		sb.ifstat(STAT_SPECTATOR).xv(0).yb(-58).string2("SPECTATOR MODE").endifstat();

		// chase cam
		sb.ifstat(STAT_CHASE).xv(0).yb(-68).string("CHASING").xv(64).stat_string(STAT_CHASE).endifstat();
	}

	// ---- more shared stuff ----
	if (deathmatch->integer)
	{
		// tech
		sb.ifstat(STAT_CTF_TECH).yb(-137).xr(-26).pic(STAT_CTF_TECH).endifstat();
	}
	else
	{
		sb.story();
	}

	gi.configstring(CS_STATUSBAR, sb.sb.str().c_str());
}


/*QUAKED worldspawn (0 0 0) ?

Only used for the world.
"sky"	environment map name
"skyaxis"	vector axis for rotating sky
"skyrotate"	speed of rotation in degrees/second
"sounds"	music cd track number
"gravity"	800 is default gravity
"message"	text to print at user logon
*/
void SP_worldspawn(edict_t *ent)
{
	ent->movetype = MOVETYPE_PUSH;
	ent->solid = SOLID_BSP;
	ent->inuse = true; // since the world doesn't use G_Spawn()
	ent->s.modelindex = MODELINDEX_WORLD;
	ent->gravity = 1.0f;

	const spawn_temp_t &st = ED_GetSpawnTemp();

	if (st.hub_map)
	{
		level.hub_map = true;

		// clear helps
		game.help1changed = game.help2changed = 0;
		*game.helpmessage1 = *game.helpmessage2 = '\0';

		for (size_t i = 0; i < game.maxclients; i++)
		{
			game.clients[i].pers.game_help1changed = game.clients[i].pers.game_help2changed = 0;
			game.clients[i].resp.coop_respawn.game_help1changed = game.clients[i].resp.coop_respawn.game_help2changed = 0;
		}
	}

	if (st.achievement && st.achievement[0])
		level.achievement = st.achievement;

	//---------------

	// set configstrings for items
	SetItemNames();

	if (st.nextmap)
		Q_strlcpy(level.nextmap, st.nextmap, sizeof(level.nextmap));

	// make some data visible to the server

	if (ent->message && ent->message[0])
	{
		gi.configstring(CS_NAME, ent->message);
		Q_strlcpy(level.level_name, ent->message, sizeof(level.level_name));
	}
	else
		Q_strlcpy(level.level_name, level.mapname, sizeof(level.level_name));

	if (st.sky && st.sky[0])
		gi.configstring(CS_SKY, st.sky);
	else
		gi.configstring(CS_SKY, "unit1_");

	gi.configstring(CS_SKYROTATE, G_Fmt("{} {}", st.skyrotate, st.skyautorotate).data());

	gi.configstring(CS_SKYAXIS, G_Fmt("{}", st.skyaxis).data());

	if (st.music && st.music[0])
	{
		gi.configstring(CS_CDTRACK, st.music);
	}
	else
	{
		gi.configstring(CS_CDTRACK, G_Fmt("{}", ent->sounds).data());
	}

	if (level.is_n64)
		gi.configstring(CS_CD_LOOP_COUNT, "0");
	else if (st.was_key_specified("loop_count"))
		gi.configstring(CS_CD_LOOP_COUNT, G_Fmt("{}", st.loop_count).data());
	else
		gi.configstring(CS_CD_LOOP_COUNT, "");

	if (st.instantitems > 0 || level.is_n64)
	{
		level.instantitems = true;
	}

	// [Paril-KEX]
	if (!deathmatch->integer)
		gi.configstring(CS_GAME_STYLE, G_Fmt("{}", (int32_t) game_style_t::GAME_STYLE_PVE).data());
	else if (teamplay->integer || ctf->integer)
		gi.configstring(CS_GAME_STYLE, G_Fmt("{}", (int32_t) game_style_t::GAME_STYLE_TDM).data());
	else
		gi.configstring(CS_GAME_STYLE, G_Fmt("{}", (int32_t) game_style_t::GAME_STYLE_FFA).data());

	// [Paril-KEX]
	if (st.goals)
	{
		level.goals = st.goals;
		game.help1changed++;
	}

	if (st.start_items)
		level.start_items = st.start_items;

	if (st.no_grapple)
		level.no_grapple = st.no_grapple;

	gi.configstring(CS_MAXCLIENTS, G_Fmt("{}", game.maxclients).data());

	int override_physics = gi.cvar("g_override_physics_flags", "-1", CVAR_NOFLAGS)->integer;

	if (override_physics == -1)
	{
		if (deathmatch->integer && st.was_key_specified("physics_flags_dm"))
			override_physics = st.physics_flags_dm;
		else if (!deathmatch->integer && st.was_key_specified("physics_flags_sp"))
			override_physics = st.physics_flags_sp;
	}

	if (override_physics >= 0)
		pm_config.physics_flags = (physics_flags_t) override_physics;
	else
	{
		if (level.is_n64)
			pm_config.physics_flags |= PHYSICS_N64_MOVEMENT;

		if (level.is_psx)
			pm_config.physics_flags |= PHYSICS_PSX_MOVEMENT | PHYSICS_PSX_SCALE;

		if (deathmatch->integer)
			pm_config.physics_flags |= PHYSICS_DEATHMATCH;
	}

	gi.configstring(CONFIG_PHYSICS_FLAGS, G_Fmt("{}", (int) pm_config.physics_flags).data());
	
	level.primary_objective_string = "$g_primary_mission_objective";
	level.secondary_objective_string = "$g_secondary_mission_objective";

	if (st.primary_objective_string && st.primary_objective_string[0])
		level.primary_objective_string = st.primary_objective_string;
	if (st.secondary_objective_string && st.secondary_objective_string[0])
		level.secondary_objective_string = st.secondary_objective_string;
	
	level.primary_objective_title = "$g_pc_primary_objective";
	level.secondary_objective_title = "$g_pc_secondary_objective";

	if (st.primary_objective_title && st.primary_objective_title[0])
		level.primary_objective_title = st.primary_objective_title;
	if (st.secondary_objective_title && st.secondary_objective_title[0])
		level.secondary_objective_title = st.secondary_objective_title;

	// statusbar prog
	G_InitStatusbar();

	// [Paril-KEX] air accel handled by game DLL now, and allow
	// it to be changed in sp/coop
	gi.configstring(CS_AIRACCEL, G_Fmt("{}", sv_airaccelerate->integer).data());
	pm_config.airaccel = sv_airaccelerate->integer;

	game.airacceleration_modified = sv_airaccelerate->modified_count;

	//---------------

	// help icon for statusbar
	gi.imageindex("i_help");
	level.pic_health = gi.imageindex("i_health");
	gi.imageindex("help");
	gi.imageindex("field_3");

	if (!st.gravity)
	{
		level.gravity = 800.f;
		gi.cvar_set("sv_gravity", "800");
	}
	else
	{
		level.gravity = atof(st.gravity);
		gi.cvar_set("sv_gravity", st.gravity);
	}

	snd_fry.assign("player/fry.wav"); // standing in lava / slime
	
	PrecacheItem(GetItemByIndex(IT_ITEM_COMPASS));
	PrecacheItem(GetItemByIndex(IT_WEAPON_BLASTER));

	if (g_dm_random_items->integer)
		for (item_id_t i = static_cast<item_id_t>(IT_NULL + 1); i < IT_TOTAL; i = static_cast<item_id_t>(i + 1))
			PrecacheItem(GetItemByIndex(i));

	gi.soundindex("player/lava1.wav");
	gi.soundindex("player/lava2.wav");

	gi.soundindex("misc/pc_up.wav");
	gi.soundindex("misc/talk1.wav");

	// gibs
	gi.soundindex("misc/udeath.wav");

	gi.soundindex("items/respawn1.wav");
	gi.soundindex("misc/mon_power2.wav");

	// sexed sounds
	gi.soundindex("*death1.wav");
	gi.soundindex("*death2.wav");
	gi.soundindex("*death3.wav");
	gi.soundindex("*death4.wav");
	gi.soundindex("*fall1.wav");
	gi.soundindex("*fall2.wav");
	gi.soundindex("*gurp1.wav"); // drowning damage
	gi.soundindex("*gurp2.wav");
	gi.soundindex("*jump1.wav"); // player jump
	gi.soundindex("*pain25_1.wav");
	gi.soundindex("*pain25_2.wav");
	gi.soundindex("*pain50_1.wav");
	gi.soundindex("*pain50_2.wav");
	gi.soundindex("*pain75_1.wav");
	gi.soundindex("*pain75_2.wav");
	gi.soundindex("*pain100_1.wav");
	gi.soundindex("*pain100_2.wav");
	gi.soundindex("*drown1.wav"); // [Paril-KEX]

	// sexed models
	for (auto &item : itemlist)
		item.vwep_index = 0;

	for (auto &item : itemlist)
	{
		if (!item.vwep_model)
			continue;

		for (auto &check : itemlist)
		{
			if (check.vwep_model && !Q_strcasecmp(item.vwep_model, check.vwep_model) && check.vwep_index)
			{
				item.vwep_index = check.vwep_index;
				break;
			}
		}

		if (item.vwep_index)
			continue;

		item.vwep_index = gi.modelindex(item.vwep_model);

		if (!level.vwep_offset)
			level.vwep_offset = item.vwep_index;
	}

	//-------------------

	gi.soundindex("player/gasp1.wav"); // gasping for air
	gi.soundindex("player/gasp2.wav"); // head breaking surface, not gasping

	gi.soundindex("player/watr_in.wav");  // feet hitting water
	gi.soundindex("player/watr_out.wav"); // feet leaving water

	gi.soundindex("player/watr_un.wav"); // head going underwater

	gi.soundindex("player/u_breath1.wav");
	gi.soundindex("player/u_breath2.wav");

	gi.soundindex("player/wade1.wav");
	gi.soundindex("player/wade2.wav");
	gi.soundindex("player/wade3.wav");

	gi.soundindex("player/breathout1.wav");
	gi.soundindex("player/breathout2.wav");
	gi.soundindex("player/breathout3.wav");

	gi.soundindex("items/pkup.wav");   // bonus item pickup
	gi.soundindex("world/land.wav");   // landing thud
	gi.soundindex("misc/h2ohit1.wav"); // landing splash

	gi.soundindex("items/damage.wav");
	gi.soundindex("items/protect.wav");
	gi.soundindex("items/protect4.wav");
	gi.soundindex("weapons/noammo.wav");
	gi.soundindex("weapons/lowammo.wav");
	gi.soundindex("weapons/change.wav");

	gi.soundindex("infantry/inflies1.wav");

	sm_meat_index.assign("models/objects/gibs/sm_meat/tris.md2");
	gi.modelindex("models/objects/gibs/arm/tris.md2");
	gi.modelindex("models/objects/gibs/bone/tris.md2");
	gi.modelindex("models/objects/gibs/bone2/tris.md2");
	gi.modelindex("models/objects/gibs/chest/tris.md2");
	gi.modelindex("models/objects/gibs/skull/tris.md2");
	gi.modelindex("models/objects/gibs/head2/tris.md2");
	gi.modelindex("models/objects/gibs/sm_metal/tris.md2");

	level.pic_ping = gi.imageindex("loc_ping");

	//
	// Setup light animation tables. 'a' is total darkness, 'z' is doublebright.
	//

	// 0 normal
	gi.configstring(CS_LIGHTS + 0, "m");

	// 1 FLICKER (first variety)
	gi.configstring(CS_LIGHTS + 1, "mmnmmommommnonmmonqnmmo");

	// 2 SLOW STRONG PULSE
	gi.configstring(CS_LIGHTS + 2, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba");

	// 3 CANDLE (first variety)
	gi.configstring(CS_LIGHTS + 3, "mmmmmaaaaammmmmaaaaaabcdefgabcdefg");

	// 4 FAST STROBE
	gi.configstring(CS_LIGHTS + 4, "mamamamamama");

	// 5 GENTLE PULSE 1
	gi.configstring(CS_LIGHTS + 5, "jklmnopqrstuvwxyzyxwvutsrqponmlkj");

	// 6 FLICKER (second variety)
	gi.configstring(CS_LIGHTS + 6, "nmonqnmomnmomomno");

	// 7 CANDLE (second variety)`map
	gi.configstring(CS_LIGHTS + 7, "mmmaaaabcdefgmmmmaaaammmaamm");

	// 8 CANDLE (third variety)
	gi.configstring(CS_LIGHTS + 8, "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa");

	// 9 SLOW STROBE (fourth variety)
	gi.configstring(CS_LIGHTS + 9, "aaaaaaaazzzzzzzz");

	// 10 FLUORESCENT FLICKER
	gi.configstring(CS_LIGHTS + 10, "mmamammmmammamamaaamammma");

	// 11 SLOW PULSE NOT FADE TO BLACK
	gi.configstring(CS_LIGHTS + 11, "abcdefghijklmnopqrrqponmlkjihgfedcba");

	// [Paril-KEX] 12 N64's 2 (fast strobe)
	gi.configstring(CS_LIGHTS + 12, "zzazazzzzazzazazaaazazzza");

	// [Paril-KEX] 13 N64's 3 (half of strong pulse)
	gi.configstring(CS_LIGHTS + 13, "abcdefghijklmnopqrstuvwxyz");

	// [Paril-KEX] 14 N64's 4 (fast strobe)
	gi.configstring(CS_LIGHTS + 14, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba");

	// styles 32-62 are assigned by the light program for switchable lights

	// 63 testing
	gi.configstring(CS_LIGHTS + 63, "a");

	// coop respawn strings
	if (coop->integer)
	{
		gi.configstring(CONFIG_COOP_RESPAWN_STRING + 0, "$g_coop_respawn_in_combat");
		gi.configstring(CONFIG_COOP_RESPAWN_STRING + 1, "$g_coop_respawn_bad_area");
		gi.configstring(CONFIG_COOP_RESPAWN_STRING + 2, "$g_coop_respawn_blocked");
		gi.configstring(CONFIG_COOP_RESPAWN_STRING + 3, "$g_coop_respawn_waiting");
		gi.configstring(CONFIG_COOP_RESPAWN_STRING + 4, "$g_coop_respawn_no_lives");
	}
}
