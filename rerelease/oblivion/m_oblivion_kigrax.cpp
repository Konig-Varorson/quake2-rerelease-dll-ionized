// Copyright (c) ZeniMax Media Inc.
// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

KIGRAX

==============================================================================
*/

#include "../g_local.h"
#include "m_oblivion_kigrax.h"
#include "../m_flash.h"

static cached_soundindex sound_pain;
static cached_soundindex sound_death;
static cached_soundindex sound_sight;
static cached_soundindex sound_idle;
static cached_soundindex sound_attack;
static cached_soundindex sound_melee;
static cached_soundindex sound_melee2;

//
//SOUNDS
//

MONSTERINFO_SEARCH(kigrax_search) (edict_t* self) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

MONSTERINFO_SIGHT(kigrax_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

//
//STAND
//

mframe_t kigrax_frames_stand1[] =
{
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},

	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},

	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}
};
MMOVE_T(kigrax_move_stand1) = { FRAME_stand101, FRAME_stand128, kigrax_frames_stand1, nullptr };

mframe_t kigrax_frames_stand2[] =
{
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},

	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},

	{ai_stand}
};
MMOVE_T(kigrax_move_stand2) = { FRAME_stand201, FRAME_stand221, kigrax_frames_stand2, nullptr };

MONSTERINFO_IDLE(kigrax_idle) (edict_t* self) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_IDLE, 0);
	M_SetAnimation(self, &kigrax_move_stand2);
}

MONSTERINFO_STAND(kigrax_stand) (edict_t* self) -> void
{
	if (frandom() >= 0.5f)
		M_SetAnimation(self, &kigrax_move_stand1);
	else
		M_SetAnimation(self, &kigrax_move_stand2);
}

//
//WALKRUN
//

mframe_t kigrax_frames_settle [] =
{
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 5},
	{ai_walk, 5},
	{ai_walk, 4},
	{ai_walk, 4},
	{ai_walk, 2},
	{ai_walk, 1},
	{ai_walk},
	{ai_walk},

	{ai_walk},
	{ai_walk}
};
MMOVE_T(kigrax_move_settle) = { FRAME_settle101, FRAME_settle112, kigrax_frames_settle, kigrax_stand };

void kigrax_settle(edict_t* self);

mframe_t kigrax_frames_walk1[] =
{
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},

	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},

	{ai_walk, 6},
	{ai_walk, 6, kigrax_settle}
};
MMOVE_T(kigrax_move_walk1) = { FRAME_walk101, FRAME_walk122, kigrax_frames_walk1, nullptr };

mframe_t kigrax_frames_walk2[] =
{
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},

	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},
	{ai_walk, 6},

	{ai_walk, 6},
	{ai_walk, 6, kigrax_settle}
};
MMOVE_T(kigrax_move_walk2) = { FRAME_walk101, FRAME_walk122, kigrax_frames_walk2, nullptr };

void kigrax_settle(edict_t* self)
{
	if (frandom() >= 0.5f)
		M_SetAnimation(self, &kigrax_move_settle);
	else
		M_SetAnimation(self, &kigrax_move_walk1);
}

MONSTERINFO_WALK(kigrax_walk) (edict_t* self) -> void
{
	if (frandom() >= 0.5f)
		M_SetAnimation(self, &kigrax_move_walk1);
	else
		M_SetAnimation(self, &kigrax_move_walk2);
}

mframe_t kigrax_frames_run[] =
{
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},

	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12},
	{ai_run, 12}
};
MMOVE_T(kigrax_move_run) = { FRAME_run1, FRAME_run17, kigrax_frames_run, nullptr };

MONSTERINFO_RUN(kigrax_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &kigrax_move_stand1);
	else
		M_SetAnimation(self, &kigrax_move_run);
}

//
//ATTACK
//

void kigrax_fire(edict_t* self)
{
	vec3_t		start;
	vec3_t		forward, right;
	vec3_t		end;
	vec3_t		dir;
	vec3_t		o = { 0, 0, 30 };

	if (!self->enemy || !self->enemy->inuse) // PGM
		return;								 // PGM

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, o, forward, right);

	end = self->enemy->s.origin;
	end[2] += self->enemy->viewheight;
	dir = end - start;
	dir.normalize();

	monster_fire_blaster2(self, start, dir, 5, 800, MZ2_STALKER_BLASTER, EF_BLASTER);
}

mframe_t kigrax_frames_attack[] = {
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, kigrax_fire},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge}
};
MMOVE_T(kigrax_move_attack) = { FRAME_attack101, FRAME_attack110, kigrax_frames_attack, kigrax_run };

mframe_t kigrax_frames_attack2[] = {
	{ai_charge, 5},
	{ai_charge, 5},
	{ai_charge, 5},
	{ai_charge, 5},
	{ai_charge, 5},
	{ai_charge, 5, kigrax_fire},
	{ai_charge, 5},
	{ai_charge, 5},
	{ai_charge, 5},
	{ai_charge, 5}
};
MMOVE_T(kigrax_move_attack2) = { FRAME_attack101, FRAME_attack110, kigrax_frames_attack2, kigrax_run };

MONSTERINFO_ATTACK(kigrax_attack) (edict_t* self) -> void
{
	float chance = 0.5f;

	if (frandom() > chance)
	{
		M_SetAnimation(self, &kigrax_move_attack);
		self->monsterinfo.attack_state = AS_STRAIGHT;
	}
	else // circle strafe
	{
		if (frandom() <= 0.5f)
			self->monsterinfo.lefty = !self->monsterinfo.lefty;
		M_SetAnimation(self, &kigrax_move_attack2);
		self->monsterinfo.attack_state = AS_SLIDING;
	}

	M_SetAnimation(self, &kigrax_move_attack);
}

//
// MELEE
// 

void kigrax_swipe(edict_t* self)
{
	vec3_t aim = { MELEE_DISTANCE, self->mins[0], 2 };
	if (fire_hit(self, aim, irandom(10, 15), 100))
		gi.sound(self, CHAN_WEAPON, sound_melee, 1, ATTN_NORM, 0);
	else
	{
		gi.sound(self, CHAN_WEAPON, sound_melee, 1, ATTN_NORM, 0);
		self->monsterinfo.melee_debounce_time = level.time + 1.5_sec;
	}
}

void kigrax_smash(edict_t* self)
{
	if (!self->enemy)
		return;

	ai_charge(self, 0);

	if (!CanDamage(self->enemy, self))
		return;

	vec3_t aim = { MELEE_DISTANCE, self->mins[0], -4 };

	if (fire_hit(self, aim, irandom(25, 40), 120))
		gi.sound(self, CHAN_WEAPON, sound_melee2, 1, ATTN_NORM, 0);
	else
		self->monsterinfo.melee_debounce_time = level.time + 1.5_sec;
}

mframe_t kigrax_frames_swipe[] = {
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },

	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, kigrax_swipe },
	{ ai_charge },
	{ ai_charge },

	{ ai_charge },
	{ ai_charge, 0, kigrax_swipe },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(kigrax_move_swipe) = { FRAME_melee101, FRAME_melee115, kigrax_frames_swipe, kigrax_run };

mframe_t kigrax_frames_smash[] = {
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },

	{ ai_charge },
	{ ai_charge, 0, kigrax_smash },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },

	{ ai_charge }
};
MMOVE_T(kigrax_move_smash) = { FRAME_melee201, FRAME_melee211, kigrax_frames_smash, kigrax_run };

MONSTERINFO_MELEE(kigrax_melee) (edict_t* self) -> void
{
	float chance = frandom();

	if (chance > 0.4f)
		M_SetAnimation(self, &kigrax_move_swipe);
	else
		M_SetAnimation(self, &kigrax_move_smash);
}

//
//PAIN
//

PAIN(kigrax_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;
	gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	self->monsterinfo.aiflags &= ~AI_MANUAL_STEERING;

	float r = frandom();

	//M_SetAnimation(self, &kigrax_move_pain);
}

MONSTERINFO_SETSKIN(kigrax_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
//DEATH
//

void kigrax_dead(edict_t* self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, -8 };
	monster_dead(self);
}

static void kigrax_shrink(edict_t* self)
{
	self->maxs[2] = -4;
	self->svflags |= SVF_DEADMONSTER;
	gi.linkentity(self);
}

mframe_t kigrax_frames_die[] =
{
	{ai_move, -1 },
	{ai_move, -1 },
	{ai_move, -1 },
	{ai_move, -1 },
	{ai_move, -1 },

	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 0, kigrax_shrink},
	{ai_move, -1 },

	{ai_move, -1 },
	{ai_move, -1 },
	{ai_move, -1 },
	{ai_move},
	{ai_move},

	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(kigrax_move_die) = { FRAME_death1, FRAME_death19, kigrax_frames_die, kigrax_dead };

DIE(kigrax_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 4, "models/objects/gibs/sm_meat/tris.md2" },
			{ 2, "models/objects/gibs/gear/tris.md2" },
			{ "models/objects/gibs/chest/tris.md2" },
			{ "models/objects/gibs/head2/tris.md2", GIB_SKINNED | GIB_HEAD }
			});

		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	M_SetAnimation(self, &kigrax_move_die);
}

static void kigrax_set_fly_parameters(edict_t* self)
{
	self->monsterinfo.fly_thrusters = false;
	self->monsterinfo.fly_acceleration = 10.f;
	self->monsterinfo.fly_speed = 100.f;
	self->monsterinfo.fly_min_distance = 20.f;
	self->monsterinfo.fly_max_distance = 200.f;
}

/*QUAKED monster_kigrax (1 .5 0) (-20 -20 -32) (20 20 12) Ambush Trigger_Spawn Sight Corpse
*/
void SP_monster_kigrax(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_pain.assign("kigrax/pain.wav");
	sound_death.assign("kigrax/death.wav");
	sound_sight.assign("kigrax/sight.wav");
	sound_idle.assign("kigrax/idle.wav");
	sound_attack.assign("weapons/blastf1a.wav");
	sound_melee.assign("stalker/melee1.wav");
	sound_melee2.assign("shambler/smack.wav");

	self->s.modelindex = gi.modelindex("models/monsters/kigrax/tris.md2");
	self->mins = { -20, -20, -32 };
	self->maxs = { 20, 20, 12 };
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->health = 120 * st.health_multiplier;
	self->gib_health = -80;
	self->mass = 450;

	self->pain = kigrax_pain;
	self->die = kigrax_die;

	self->monsterinfo.sight = kigrax_sight;
	self->monsterinfo.idle = kigrax_idle;
	self->monsterinfo.search = kigrax_search;

	self->monsterinfo.stand = kigrax_stand;
	self->monsterinfo.walk = kigrax_walk;
	self->monsterinfo.run = kigrax_run;

	self->monsterinfo.attack = kigrax_attack;
	self->monsterinfo.melee = kigrax_melee;

	self->monsterinfo.setskin = kigrax_setskin;

	gi.linkentity(self);

	M_SetAnimation(self, &kigrax_move_stand1);
	self->monsterinfo.scale = MODEL_SCALE;

	flymonster_start(self);

	self->monsterinfo.aiflags |= AI_ALTERNATE_FLY;
	kigrax_set_fly_parameters(self);
}