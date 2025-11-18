/*
==============================================================================

QUAKE PHANTOM SWORDSMAN

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_sword.h"

static cached_soundindex	sound_sword1;
static cached_soundindex	sound_sword2;
static cached_soundindex	sound_death;
static cached_soundindex	sound_gib;
static cached_soundindex	sound_pain;
static cached_soundindex	sound_idle;
static cached_soundindex	sound_sight;

constexpr spawnflags_t SPAWNFLAG_SWORD_TRIGGERED = 8_spawnflag;

void sword_stand(edict_t* self);
void sword_walk(edict_t* self);
void sword_run(edict_t* self);
void sword_check_dist(edict_t* self);
void sword_hit(edict_t* self);

//
// SOUNDS
//

MONSTERINFO_SIGHT(sword_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);

	// Start delay timer on first sight
	if (self->timestamp == 0_ms)
	{
		gtime_t delay_time;
		if (self->delay > 0)
			delay_time = gtime_t::from_sec(self->delay);
		else
			delay_time = 10_sec;

		self->timestamp = level.time + delay_time;
		self->monsterinfo.aiflags |= AI_STAND_GROUND; // Don't move during delay
	}
}

MONSTERINFO_SEARCH(sword_search) (edict_t* self) -> void
{
	gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
}

void sword_sword_sound(edict_t* self)
{
	if (frandom() <= 0.5f)
		gi.sound(self, CHAN_VOICE, sound_sword1, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_sword2, 1, ATTN_NORM, 0);
}

//
// STAND
//

mframe_t sword_frames_stand[] =
{
	{ai_stand},
};
MMOVE_T(sword_move_stand) = { FRAME_stand1, FRAME_stand1, sword_frames_stand, sword_stand };

MONSTERINFO_STAND(sword_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &sword_move_stand);
}

//
// WALK
//

mframe_t sword_frames_walk[] =
{
	{ai_walk,	0}
};
MMOVE_T(sword_move_walk) = { FRAME_stand1, FRAME_stand1, sword_frames_walk, sword_walk };

MONSTERINFO_WALK(sword_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &sword_move_walk);
}

//
// RUN
//

void sword_activate_effects(edict_t* self)
{
	self->s.effects |= EF_COLOR_SHELL;
	self->s.renderfx |= RF_SHELL_RED;
}

mframe_t sword_frames_run[] =
{
	{ai_run,	16, sword_activate_effects},
	{ai_run,	20},
	{ai_run,	13},
	{ai_run,	7},
	{ai_run,	16},
	{ai_run,	20},
	{ai_run,	14},
	{ai_run,	6,		sword_check_dist}
};
MMOVE_T(sword_move_run) = { FRAME_runb1, FRAME_runb8, sword_frames_run, nullptr };

MONSTERINFO_RUN(sword_run) (edict_t* self) -> void
{
	// Check if still in delay period
	if (self->timestamp > 0_ms && level.time < self->timestamp)
	{
		M_SetAnimation(self, &sword_move_stand);
		return;
	}

	// Delay expired, allow movement
	if (self->timestamp > 0_ms && level.time >= self->timestamp)
	{
		self->timestamp = 0_ms;
		self->monsterinfo.aiflags &= ~AI_STAND_GROUND;
	}

	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &sword_move_stand);
	else
		M_SetAnimation(self, &sword_move_run);
}

//
// ATTACK
//

mframe_t sword_frames_attack[] =
{
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge,	0,	sword_hit},
	{ai_charge,	0,	sword_hit},
	{ai_charge,	0,	sword_hit},
	{ai_charge},
	{ai_charge},
	{ai_charge,	0,	sword_check_dist}
};
MMOVE_T(sword_move_attack) = { FRAME_attackb1, FRAME_attackb10, sword_frames_attack, sword_run };

MONSTERINFO_MELEE(sword_melee) (edict_t* self) -> void
{
	sword_sword_sound(self);
	M_SetAnimation(self, &sword_move_attack);
}

void sword_check_dist(edict_t* self)
{
	if (!self->enemy || !self->enemy->inuse || self->enemy->health <= 0)
		return;

	if (range_to(self, self->enemy) <= RANGE_MELEE)
	{
		M_SetAnimation(self, &sword_move_attack);
	}
	else
	{
		M_SetAnimation(self, &sword_move_run);
	}
}

void sword_hit(edict_t* self)
{
	vec3_t aim = { MELEE_DISTANCE, self->mins[0], 4 };
	if (fire_hit(self, aim, irandom(8, 16), 100))
		sword_sword_sound(self);
	else
	{
		sword_sword_sound(self);
		self->monsterinfo.melee_debounce_time = level.time + 1.5_sec;
	}
}

//
// PAIN
//

mframe_t sword_frames_pain1[] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(sword_move_pain1) = { FRAME_runb1, FRAME_runb8, sword_frames_pain1, sword_run };

PAIN(sword_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;

	if (!M_ShouldReactToPain(self, mod))
		return;

	if (frandom() < 0.5f)
		gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

	M_SetAnimation(self, &sword_move_pain1);
}

//
// DEATH
//
void sword_shrink(edict_t* self)
{
	self->maxs[2] = 0;
	self->svflags |= SVF_DEADMONSTER;
	gi.linkentity(self);
}

void sword_death_sound(edict_t* self)
{
	gi.sound(self, CHAN_BODY, gi.soundindex("player/axhit2.wav"), 0.5f, ATTN_NORM, 0);
}

mframe_t sword_frames_death1[] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 0,	sword_shrink},
	{ai_move},
	{ai_move, 0,	sword_death_sound},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(sword_move_death1) = { FRAME_death1, FRAME_death10, sword_frames_death1, monster_dead };

mframe_t sword_frames_death2[] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 0,	sword_shrink},
	{ai_move, 0,	sword_death_sound},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(sword_move_death2) = { FRAME_deathb1, FRAME_deathb11, sword_frames_death2, monster_dead };

DIE(sword_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	self->s.effects = EF_HYPERBLASTER; // Keep only base effect

	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/sm_metal/tris.md2", GIB_DEBRIS },
			{ "models/objects/gibs/sm_metal/tris.md2", GIB_DEBRIS | GIB_HEAD }
			});

		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	if (frandom() < 0.5f)
		M_SetAnimation(self, &sword_move_death1);
	else
		M_SetAnimation(self, &sword_move_death2);
}

//
// TRIGGER ACTIVATION
//

USE(sword_activate) (edict_t* self, edict_t* other, edict_t* activator) -> void
{
	self->use = nullptr;
	self->takedamage = true;

	// Find an enemy
	if (activator && activator->client)
	{
		self->enemy = activator;
		FoundTarget(self);
	}
	else
	{
		self->monsterinfo.idle_time = level.time + 0.5_sec;
		self->monsterinfo.stand(self);
	}
}

static void sword_set_fly_parameters(edict_t* self)
{
	self->monsterinfo.fly_pinned = false;
	self->monsterinfo.fly_thrusters = true;
	self->monsterinfo.fly_position_time = 0_sec;
	self->monsterinfo.fly_acceleration = 10.f;
	self->monsterinfo.fly_speed = 180.f;
	self->monsterinfo.fly_min_distance = 0.f;
	self->monsterinfo.fly_max_distance = 10.f;
}

/*QUAKED monster_sword (1 .5 0) (-16 -16 -24) (16 16 40) Ambush Trigger_Spawn Sight Triggered
The Invisible Swordsman from Quake Mission Pack 1.

Spawnflags:
8 - Triggered: Sword is inactive and invulnerable until triggered

Keys:
delay - seconds after first sighting before it attacks (default 10)

model="models/monsters/sword/tris.md2"
*/
void SP_monster_sword(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_sword1.assign("sword/sword1.wav");
	sound_sword2.assign("sword/sword2.wav");
	sound_death.assign("sword/kdeath.wav");
	sound_pain.assign("sword/khurt.wav");
	sound_idle.assign("sword/idle.wav");
	sound_sight.assign("sword/ksight.wav");

	gi.soundindex("player/axhit2.wav"); // Precache death impact sound

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex("models/monsters/sword/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 40 };

	self->health = 200 * st.health_multiplier;
	self->gib_health = -80;
	self->mass = 120;

	self->pain = sword_pain;
	self->die = sword_die;

	self->s.effects |= EF_HYPERBLASTER;

	self->monsterinfo.stand = sword_stand;
	self->monsterinfo.walk = sword_walk;
	self->monsterinfo.run = sword_run;
	self->monsterinfo.dodge = M_MonsterDodge;
	self->monsterinfo.attack = nullptr;
	self->monsterinfo.melee = sword_melee;
	self->monsterinfo.sight = sword_sight;
	self->monsterinfo.search = sword_search;

	// Set delay (default 10 seconds like Q1)
	gtime_t delay_time;
	if (!st.was_key_specified("delay"))
		delay_time = 10_sec;
	else
		delay_time = gtime_t::from_sec(self->delay);

	self->timestamp = 0_ms; // Will be set on first sight

	gi.linkentity(self);
	self->flags |= FL_DEEPONE;

	M_SetAnimation(self, &sword_move_stand);

	self->monsterinfo.combat_style = COMBAT_MELEE;
	self->monsterinfo.scale = MODEL_SCALE;
	self->monsterinfo.aiflags |= AI_ALTERNATE_FLY;
	sword_set_fly_parameters(self);

	// Triggered mode - invisible and invulnerable until activated
	if (self->spawnflags.has(SPAWNFLAG_SWORD_TRIGGERED))
	{
		self->use = sword_activate;
		self->takedamage = false;
		gi.linkentity(self);
	}
	else
	{
		flymonster_start(self);
	}
}