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