// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"
/*QUAKED target_shooter_blaster (1 0 0) (-8 -8 -8) (8 8 8) NOTRAIL NOEFFECTS BLUEBLASTER GREENBLASTER NAILS RIPPER
Fires a projectile attack in the set direction when triggered.

dmg		default is 15
speed	default is 1000
volume	default is 0.75
*/
/* KONIG - spawnflags for alternative projectiles; sound variable by volume entity key; default to 0.75*/
constexpr spawnflags_t SPAWNFLAG_BLASTER_NOTRAIL = 1_spawnflag;
constexpr spawnflags_t SPAWNFLAG_BLASTER_NOEFFECTS = 2_spawnflag;
constexpr spawnflags_t SPAWNFLAG_BLASTER_BLUEBLASTER = 4_spawnflag;
constexpr spawnflags_t SPAWNFLAG_BLASTER_GREENBLASTER = 8_spawnflag;
constexpr spawnflags_t SPAWNFLAG_BLASTER_NAILS = 16_spawnflag;
constexpr spawnflags_t SPAWNFLAG_BLASTER_RIPPER = 32_spawnflag;

USE(use_target_shooter_blaster) (edict_t* self, edict_t* other, edict_t* activator) -> void
{
	effects_t effect;

	if (self->spawnflags.has(SPAWNFLAG_BLASTER_NOEFFECTS))
		effect = EF_NONE;
	else if (self->spawnflags.has(SPAWNFLAG_BLASTER_NOTRAIL))
		effect = EF_HYPERBLASTER;
	else
		effect = EF_BLASTER;

	if (self->spawnflags.has(SPAWNFLAG_BLASTER_NAILS))
	{
		self->spawnflags |= SPAWNFLAG_BLASTER_NOEFFECTS;
		fire_flechette(self, self->s.origin, self->movedir, self->dmg, (int)self->speed, effect);
	}
	else if (self->spawnflags.has(SPAWNFLAG_BLASTER_RIPPER))
	{
		self->spawnflags |= SPAWNFLAG_BLASTER_NOEFFECTS;
		fire_ionripper(self, self->s.origin, self->movedir, self->dmg, (int)self->speed, EF_IONRIPPER);
	}
	else if (self->spawnflags.has(SPAWNFLAG_BLASTER_BLUEBLASTER))
	{
		fire_blueblaster(self, self->s.origin, self->movedir, self->dmg, (int)self->speed, EF_BLUEHYPERBLASTER);
	}
	else if (self->spawnflags.has(SPAWNFLAG_BLASTER_GREENBLASTER))
	{
		fire_blaster2(self, self->s.origin, self->movedir, self->dmg, (int)self->speed, effect, MOD_TARGET_BLASTER);
	}
	else
	{
		fire_blaster(self, self->s.origin, self->movedir, self->dmg, (int)self->speed, effect, MOD_TARGET_BLASTER);
	}

	gi.sound(self, CHAN_VOICE, self->noise_index, self->volume, ATTN_NORM, 0);
}

void SP_target_shooter_blaster(edict_t* self)
{
	self->use = use_target_shooter_blaster;
	G_SetMovedir(self->s.angles, self->movedir);
	if (self->spawnflags.has(SPAWNFLAG_BLASTER_NAILS))
	{
		self->noise_index = gi.soundindex("weapons/nail1.wav");
	}
	else if (self->spawnflags.has(SPAWNFLAG_BLASTER_RIPPER))
	{
		self->noise_index = gi.soundindex("weapons/rippfire.wav");
	}
	else if (self->spawnflags.has(SPAWNFLAG_BLASTER_BLUEBLASTER))
	{
		self->noise_index = gi.soundindex("weapons/laser2.wav");
	}
	else if (self->spawnflags.has(SPAWNFLAG_BLASTER_GREENBLASTER))
	{
		self->noise_index = gi.soundindex("weapons/laser2.wav");
	}
	else
	{
		self->noise_index = gi.soundindex("weapons/laser2.wav");
	}

	if (!self->dmg)
		self->dmg = 15;
	if (!self->speed)
		self->speed = 1000;
	if (!self->volume)
		self->volume = 0.75;

	self->svflags = SVF_NOCLIENT;
}

//==========================================================

/*QUAKED target_shooter_rockets (1 0 0) (-8 -8 -8) (8 8 8) GRENADE PHALANX BFG
Fires an explosive projectile in the set direction when triggered.

dmg		default is 15
speed	default is 1000
volume  default is 0.75
*/
constexpr spawnflags_t SPAWNFLAG_GRENADE = 1_spawnflag;
constexpr spawnflags_t SPAWNFLAG_PHALANX = 2_spawnflag;
constexpr spawnflags_t SPAWNFLAG_BFG = 4_spawnflag;

USE(use_target_shooter_rockets) (edict_t* self, edict_t* other, edict_t* activator) -> void
{
	if (self->spawnflags.has(SPAWNFLAG_GRENADE))
	{
		fire_grenade(self, self->s.origin, self->movedir, self->dmg, (int)self->speed, 2.5_sec, self->dmg, (crandom_open() * 10.0f), (200 + crandom_open() * 10.0f), true);
	}
	else if (self->spawnflags.has(SPAWNFLAG_PHALANX))
	{
		fire_plasma(self, self->s.origin, self->movedir, self->dmg, (int)self->speed, 120, 30);
	}
	else if (self->spawnflags.has(SPAWNFLAG_BFG))
	{
		fire_bfg(self, self->s.origin, self->movedir, self->dmg, (int)self->speed, 1000);
	}
	else
	{
		fire_rocket(self, self->s.origin, self->movedir, self->dmg, (int)self->speed, self->dmg, self->dmg);
	}

	gi.sound(self, CHAN_VOICE, self->noise_index, self->volume, ATTN_NORM, 0);
}

void SP_target_shooter_rockets(edict_t* self)
{
	self->use = use_target_shooter_rockets;
	G_SetMovedir(self->s.angles, self->movedir);

	if (self->spawnflags.has(SPAWNFLAG_GRENADE))
	{
		self->noise_index = gi.soundindex("weapons/grenlf1a.wav");
	}
	else if (self->spawnflags.has(SPAWNFLAG_PHALANX))
	{
		self->noise_index = gi.soundindex("weapons/plasshot.wav");
		if (!self->dmg)
			self->dmg = 80;
		if (!self->speed)
			self->speed = 725;
	}
	else if (self->spawnflags.has(SPAWNFLAG_BFG))
	{
		self->noise_index = gi.soundindex("makron/bfg_fire.wav");
		if (!self->dmg)
			self->dmg = deathmatch->integer ? 200 : 500;;
		if (!self->speed)
			self->speed = 400;
	}
	else
	{
		self->noise_index = gi.soundindex("weapons/rocklf1a.wav");
		if (!self->dmg)
			self->dmg = 120;
		if (!self->speed)
			self->speed = 600;
	}

	if (!self->volume)
		self->volume = 0.75;

	self->svflags = SVF_NOCLIENT;
}

//==========================================================

/*QUAKED target_shooter_magic (1 0 0) (-8 -8 -8) (8 8 8) GRENADE PHALANX BFG
Fires an explosive projectile in the set direction when triggered.

dmg		default is 15
speed	default is 1000
volume  default is 0.75
*/
constexpr spawnflags_t SPAWNFLAG_FLAME = 1_spawnflag;
constexpr spawnflags_t SPAWNFLAG_VOREPOD = 2_spawnflag;
constexpr spawnflags_t SPAWNFLAG_WRATHPOD = 4_spawnflag;
constexpr spawnflags_t SPAWNFLAG_PLASMABALL = 8_spawnflag;

USE(use_target_shooter_magic) (edict_t* self, edict_t* other, edict_t* activator) -> void
{
	if (self->spawnflags.has(SPAWNFLAG_FLAME))
	{
		fire_flame(self, self->s.origin, self->movedir, self->dmg, (int)self->speed);
	}
	else if (self->spawnflags.has(SPAWNFLAG_VOREPOD))
	{
		fire_vorepod(self, self->s.origin, self->movedir, self->dmg, (int)self->speed, 120, 30, 0, 0);
	}
	else if (self->spawnflags.has(SPAWNFLAG_WRATHPOD))
	{
		fire_vorepod(self, self->s.origin, self->movedir, self->dmg, (int)self->speed, 120, 30, 0, 1);
	}
	else if (self->spawnflags.has(SPAWNFLAG_PLASMABALL))
	{
		fire_plasmaball(self, self->s.origin, self->movedir, self->dmg, (int)self->speed, 1000);
	}
	else
	{
		fire_rocket(self, self->s.origin, self->movedir, self->dmg, (int)self->speed, self->dmg, self->dmg);
	}

	gi.sound(self, CHAN_VOICE, self->noise_index, self->volume, ATTN_NORM, 0);
}

void SP_target_shooter_magic(edict_t* self)
{
	self->use = use_target_shooter_magic;
	G_SetMovedir(self->s.angles, self->movedir);

	if (self->spawnflags.has(SPAWNFLAG_GRENADE))
	{
		self->noise_index = gi.soundindex("weapons/grenlf1a.wav");
	}
	else if (self->spawnflags.has(SPAWNFLAG_PHALANX))
	{
		self->noise_index = gi.soundindex("weapons/plasshot.wav");
		if (!self->dmg)
			self->dmg = 80;
		if (!self->speed)
			self->speed = 725;
	}
	else if (self->spawnflags.has(SPAWNFLAG_BFG))
	{
		self->noise_index = gi.soundindex("makron/bfg_fire.wav");
		if (!self->dmg)
			self->dmg = deathmatch->integer ? 200 : 500;;
		if (!self->speed)
			self->speed = 400;
	}
	else
	{
		self->noise_index = gi.soundindex("weapons/rocklf1a.wav");
		if (!self->dmg)
			self->dmg = 120;
		if (!self->speed)
			self->speed = 600;
	}

	if (!self->volume)
		self->volume = 0.75;

	self->svflags = SVF_NOCLIENT;
}
#if 0
/*
==================

MUFFMODE ADDITIONS

==================
*/

/*QUAKED target_remove_weapons (1 0 0) (-8 -8 -8) (8 8 8) BLASTER x x x x x x x NOT_EASY NOT_MEDIUM NOT_HARD NOT_DM NOT_COOP
Takes away all the activator's weapons and ammo (except blaster).
BLASTER : also remove blaster
*/
static USE(target_remove_weapons_use) (edict_t* ent, edict_t* other, edict_t* activator) -> void {
	if (!activator->client)
		return;

	for (size_t i = 0; i < IT_TOTAL; i++) {
		if (!activator->client->pers.inventory[i])
			continue;

		if (itemlist[i].flags & IF_WEAPON | IF_AMMO && itemlist[i].id != IT_WEAPON_BLASTER)
			activator->client->pers.inventory[i] = 0;
	}

	NoAmmoWeaponChange(ent, false);

	activator->client->pers.weapon = activator->client->newweapon;
	if (activator->client->newweapon)
		activator->client->pers.selected_item = activator->client->newweapon->id;
	activator->client->newweapon = nullptr;
	activator->client->pers.lastweapon = activator->client->pers.weapon;
}

void SP_target_remove_weapons(edict_t* ent) {
	ent->use = target_remove_weapons_use;
}


/*QUAKED target_give (1 0 0) (-8 -8 -8) (8 8 8) x x x x x x x x NOT_EASY NOT_MEDIUM NOT_HARD NOT_DM NOT_COOP
Gives the activator the targetted item.
*/
static USE(target_give_use) (edict_t* ent, edict_t* other, edict_t* activator) -> void {
	if (!activator->client)
		return;

	ent->item->pickup(ent, other);
}

void SP_target_give(edict_t* ent) {
	edict_t* target_ent = G_PickTarget(ent->target);
	if (!target_ent || !target_ent->classname[0]) {
		gi.Com_PrintFmt("{}: Invalid target entity, removing.\n", *ent);
		G_FreeEntity(ent);
		return;
	}

	gitem_t* it = FindItemByClassname(target_ent->classname);
	if (!it || !it->pickup) {
		gi.Com_PrintFmt("{}: Targetted entity is not an item, removing.\n", *ent);
		G_FreeEntity(ent);
		return;
	}

	ent->item = it;
	ent->use = target_give_use;
	ent->svflags = SVF_NOCLIENT;
}

/*QUAKED target_print (1 0 0) (-8 -8 -8) (8 8 8) REDTEAM BLUETEAM PRIVATE x x x x x NOT_EASY NOT_MEDIUM NOT_HARD NOT_DM NOT_COOP
Sends a center-printed message to clients.
"message"	text to print
If "private", only the activator gets the message. If no checks, all clients get the message.
*/
static USE(target_print_use) (edict_t* ent, edict_t* other, edict_t* activator) -> void {
	if (activator && activator->client && ent->spawnflags.has(4_spawnflag)) {
		gi.LocClient_Print(activator, PRINT_CENTER, "{}", ent->message);
		return;
	}

	if (ent->spawnflags.has(3_spawnflag)) {
		if (ent->spawnflags.has(1_spawnflag))
			BroadcastTeamMessage(TEAM_RED, PRINT_CENTER, G_Fmt("{}", ent->message).data());
		if (ent->spawnflags.has(2_spawnflag))
			BroadcastTeamMessage(TEAM_BLUE, PRINT_CENTER, G_Fmt("{}", ent->message).data());
		return;
	}

	gi.LocBroadcast_Print(PRINT_CENTER, "{}", ent->message);
}

void SP_target_print(edict_t* ent) {
	if (!ent->message[0]) {
		gi.Com_PrintFmt("{}: No message, removing.\n", *ent);
		G_FreeEntity(ent);
		return;
	}
	ent->use = target_print_use;
	ent->svflags = SVF_NOCLIENT;
}
#endif