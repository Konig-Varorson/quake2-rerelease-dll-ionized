/*
==============================================================================

QUAKE FIEND

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_fiend.h"

constexpr spawnflags_t SPAWNFLAG_FIEND_NOJUMPING = 8_spawnflag;

static cached_soundindex	sound_swing;
static cached_soundindex	sound_hit;
static cached_soundindex	sound_jump;
static cached_soundindex	sound_death;
static cached_soundindex	sound_idle1;
static cached_soundindex	sound_idle2;
static cached_soundindex	sound_pain;
static cached_soundindex	sound_sight1;
static cached_soundindex	sound_sight2;
static cached_soundindex	sound_land;

//
// SOUNDS
//

MONSTERINFO_SIGHT(fiend_sight) (edict_t* self, edict_t* other) -> void
{
	if (frandom() > 0.5f)
		gi.sound(self, CHAN_VOICE, sound_sight1, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_sight2, 1, ATTN_NORM, 0);
}

MONSTERINFO_SEARCH(fiend_search) (edict_t* self) -> void
{
	if (frandom() > 0.5f)
		gi.sound(self, CHAN_VOICE, sound_idle1, 1, ATTN_STATIC, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_idle2, 1, ATTN_STATIC, 0);
}

//
// STAND
//

mframe_t fiend_frames_stand [] =
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
	{ai_stand}
};
MMOVE_T(fiend_move_stand) = { FRAME_stand1, FRAME_stand13, fiend_frames_stand, nullptr };

MONSTERINFO_STAND(fiend_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &fiend_move_stand);
}

//
// WALK
//

mframe_t fiend_frames_walk [] =
{
	{ai_walk,	8},
	{ai_walk,	6},
	{ai_walk,	6},
	{ai_walk,	7},
	{ai_walk,	4},
	{ai_walk,	6},
	{ai_walk,	10},
	{ai_walk,	10}
};
MMOVE_T(fiend_move_walk) = { FRAME_walk1, FRAME_walk8, fiend_frames_walk, nullptr };

MONSTERINFO_WALK(fiend_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &fiend_move_walk);
}

//
// RUN
//

mframe_t fiend_frames_run [] =
{
	{ai_run,	20},
	{ai_run,	15},
	{ai_run,	36},
	{ai_run,	20},
	{ai_run,	15},
	{ai_run,	36}
};
MMOVE_T(fiend_move_run) = { FRAME_run1, FRAME_run6, fiend_frames_run, nullptr };

MONSTERINFO_RUN(fiend_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &fiend_move_stand);
	else
		M_SetAnimation(self, &fiend_move_run);
}

//
// MELEE
//

void fiend_hit_left (edict_t *self)
{
	vec3_t aim = { MELEE_DISTANCE, self->mins[0], 8 };
	if (fire_hit(self, aim, irandom(5, 15), 100))
		gi.sound(self, CHAN_WEAPON, sound_hit, 1, ATTN_NORM, 0);
	else
	{
		gi.sound(self, CHAN_WEAPON, sound_swing, 1, ATTN_NORM, 0);
		self->monsterinfo.melee_debounce_time = level.time + 1.5_sec;
	}
}

void fiend_hit_right (edict_t *self)
{
	vec3_t aim = { MELEE_DISTANCE, self->maxs[0], 8 };
	if (fire_hit(self, aim, irandom(5, 15), 100))
		gi.sound(self, CHAN_WEAPON, sound_hit, 1, ATTN_NORM, 0);
	else
	{
		gi.sound(self, CHAN_WEAPON, sound_swing, 1, ATTN_NORM, 0);
		self->monsterinfo.melee_debounce_time = level.time + 1.5_sec;
	}
}

void fiend_check_refire (edict_t *self)
{
	if (!self->enemy || !self->enemy->inuse || self->enemy->health <= 0)
		return;

	if ((self->monsterinfo.melee_debounce_time <= level.time) && ((frandom() < 0.5f) || (range_to(self, self->enemy) <= RANGE_MELEE)))
		self->monsterinfo.nextframe = FRAME_attacka1;
}

mframe_t fiend_frames_attack [] =
{
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge,	0,	fiend_hit_left},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},

	{ai_charge},
	{ai_charge,	0,	fiend_hit_right},
	{ai_charge},
	{ai_charge},
	{ai_charge,	0,	fiend_check_refire}
};
MMOVE_T(fiend_move_attack) = { FRAME_attacka1, FRAME_attacka15, fiend_frames_attack, fiend_run };

MONSTERINFO_MELEE(fiend_melee) (edict_t* self) -> void
{
	M_SetAnimation(self, &fiend_move_attack);
}

//
// ATTACK
//

TOUCH(fiend_jump_touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (self->health <= 0)
	{
		self->touch = nullptr;
		return;
	}

	if (self->style == 1 && other->takedamage)
	{
		if (self->velocity.length() > 30)
		{
			vec3_t point;
			vec3_t normal;
			int	   damage;

			normal = self->velocity;
			normal.normalize();
			point = self->s.origin + (normal * self->maxs[0]);
			damage = (int)frandom(40, 50);
			T_Damage(other, self, self, self->velocity, point, normal, damage, damage, DAMAGE_NONE, MOD_UNKNOWN);
			self->style = 0;
		}
	}

	if (!M_CheckBottom (self))
	{
		if (self->groundentity)
		{
			self->monsterinfo.nextframe = FRAME_attacka1;
			self->touch = nullptr;
		}
		return;
	}

	self->touch = nullptr;
}

void fiend_jump_takeoff (edict_t *self)
{
	vec3_t forward;

	gi.sound(self, CHAN_VOICE, sound_jump, 1, ATTN_NORM, 0);
	AngleVectors(self->s.angles, forward, nullptr, nullptr);
	self->s.origin[2] += 1;
	self->velocity = forward * 425;
	self->velocity[2] = 160;
	self->groundentity = nullptr;
	self->monsterinfo.aiflags |= AI_DUCKED;
	self->monsterinfo.attack_finished = level.time + 3_sec;
	self->style = 1;
	self->touch = fiend_jump_touch;
}

void fiend_check_landing (edict_t *self)
{
	monster_jump_finished(self);

	self->owner = nullptr;

	if (self->groundentity)
	{
		gi.sound(self, CHAN_WEAPON, sound_land, 1, ATTN_NORM, 0);
		self->monsterinfo.attack_finished = level.time + random_time(500_ms, 1.5_sec);

		if (self->monsterinfo.unduck)
			self->monsterinfo.unduck(self);

		if (range_to(self, self->enemy) <= RANGE_MELEE * 2.f)
			self->monsterinfo.melee(self);

		return;
	}

	if (level.time > self->monsterinfo.attack_finished)
		self->monsterinfo.nextframe = FRAME_attacka1;
	else
		self->monsterinfo.nextframe = FRAME_attacka8;
}

mframe_t fiend_frames_jump [] =
{
	{ai_charge,	 5},
	{ai_charge,	 5},
	{ai_charge,	 5,	fiend_jump_takeoff},
	{ai_charge,	10},
	{ai_charge,	10},
	{ai_charge,	15},
	{ai_charge,	15},
	{ai_charge,	10},
	{ai_charge,	10},
	{ai_charge,	 5},

	{ai_charge,	 5,	fiend_check_landing},
	{ai_charge,	 5}
};
MMOVE_T(fiend_move_jump) = { FRAME_leap1, FRAME_leap12, fiend_frames_jump, fiend_run };

MONSTERINFO_ATTACK(fiend_jump) (edict_t* self) -> void
{
	M_SetAnimation(self, &fiend_move_jump);
}

//
// CHECKATTACK
//

bool fiend_check_melee(edict_t* self)
{
	return range_to(self, self->enemy) <= RANGE_MELEE && self->monsterinfo.melee_debounce_time <= level.time;
}

bool fiend_check_jump(edict_t *self)
{
	vec3_t	v;
	float	distance;

	if (self->absmin[2] > (self->enemy->absmin[2] + 0.75 * self->enemy->size[2]))
		return false;

	if (self->absmax[2] < (self->enemy->absmin[2] + 0.25 * self->enemy->size[2]))
		return false;

	v[0] = self->s.origin[0] - self->enemy->s.origin[0];
	v[1] = self->s.origin[1] - self->enemy->s.origin[1];
	v[2] = 0;
	distance = v.length();

	if (distance < 100 && self->monsterinfo.melee_debounce_time <= level.time)
		return false;
	if (distance > 100)
	{
		if (frandom() < 0.9f)
			return false;
	}

	return true;
}

MONSTERINFO_CHECKATTACK(fiend_checkattack) (edict_t* self) -> bool
{
	if (!self->enemy || self->enemy->health <= 0)
		return false;

	if (fiend_check_melee(self))
	{
		self->monsterinfo.attack_state = AS_MELEE;
		return true;
	}

	if (!self->spawnflags.has(SPAWNFLAG_FIEND_NOJUMPING) && fiend_check_jump(self))
	{
		self->monsterinfo.attack_state = AS_MISSILE;
		return true;
	}

	return false;
}

//
// PAIN
//

mframe_t fiend_frames_pain [] =
{
	{ai_move,	4},
	{ai_move,	-3},
	{ai_move,	-8},
	{ai_move,	-3},
	{ai_move,	2},
	{ai_move,	5}
};
MMOVE_T(fiend_move_pain) = { FRAME_pain1, FRAME_pain6, fiend_frames_pain, fiend_run };

PAIN(fiend_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

	M_SetAnimation(self, &fiend_move_pain);
}

MONSTERINFO_SETSKIN(fiend_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
// DEATH
//

void fiend_shrink(edict_t* self)
{
	self->maxs[2] = 0;
	self->svflags |= SVF_DEADMONSTER;
	gi.linkentity(self);
}

mframe_t fiend_frames_death [] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 0, fiend_shrink},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(fiend_move_death) = { FRAME_death1, FRAME_death9, fiend_frames_death, monster_dead };

DIE(fiend_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 4, "models/objects/gibs/sm_meat/tris.md2" },
			//{ "models/monsters/fiend/gibs/chest.md2", GIB_SKINNED },
			{ "models/objects/gibs/chest/tris.md2" },
			{ "models/monsters/fiend/gibs/head.md2", GIB_SKINNED | GIB_HEAD }
			});

		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	gi.sound (self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;
	M_SetAnimation(self, &fiend_move_death);
}

//================
// ROGUE
void fiend_jump_down(edict_t* self)
{
	vec3_t forward, up;

	AngleVectors(self->s.angles, forward, nullptr, up);
	self->velocity += (forward * 100);
	self->velocity += (up * 300);
}

void fiend_jump_up(edict_t* self)
{
	vec3_t forward, up;

	AngleVectors(self->s.angles, forward, nullptr, up);
	self->velocity += (forward * 200);
	self->velocity += (up * 450);
}

void fiend_jump_wait_land(edict_t* self)
{
	if (!monster_jump_finished(self) && self->groundentity == nullptr)
		self->monsterinfo.nextframe = self->s.frame;
	else
		self->monsterinfo.nextframe = self->s.frame + 1;
}

mframe_t fiend_frames_jump_up[] = {
	{ ai_move, -8 },
	{ ai_move },
	{ ai_move, -8, fiend_jump_up },
	{ ai_move },
	{ ai_move },
	{ ai_move, 0, fiend_jump_wait_land },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(fiend_move_jump_up) = { FRAME_leap1, FRAME_leap12, fiend_frames_jump_up, fiend_run };

mframe_t fiend_frames_jump_down[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move, 0, fiend_jump_down },
	{ ai_move },
	{ ai_move },
	{ ai_move, 0, fiend_jump_wait_land },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(fiend_move_jump_down) = { FRAME_leap1, FRAME_leap12, fiend_frames_jump_down, fiend_run };

void fiend_jump_updown(edict_t* self, blocked_jump_result_t result)
{
	if (!self->enemy)
		return;

	if (result == blocked_jump_result_t::JUMP_JUMP_UP)
		M_SetAnimation(self, &fiend_move_jump_up);
	else
		M_SetAnimation(self, &fiend_move_jump_down);
}

/*
===
Blocked
===
*/
MONSTERINFO_BLOCKED(fiend_blocked) (edict_t* self, float dist) -> bool
{
	if (auto result = blocked_checkjump(self, dist); result != blocked_jump_result_t::NO_JUMP)
	{
		if (result != blocked_jump_result_t::JUMP_TURN)
			fiend_jump_updown(self, result);
		return true;
	}

	if (blocked_checkplat(self, dist))
		return true;

	return false;
}
// ROGUE
//================

//
// SPAWN
//

/*QUAKED monster_fiend (1 .5 0) (-32 -32 -24) (32 32 48) Ambush Trigger_Spawn Sight NoJumping
model="models/monsters/fiend/tris.md2"
*/
void SP_monster_fiend(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_swing.assign("mutant/mutatck1.wav");
	sound_hit.assign("fiend/dhit2.wav");
	sound_jump.assign("fiend/djump.wav");
	sound_death.assign("fiend/ddeath.wav");
	sound_idle1.assign("fiend/idle1.wav");
	sound_idle2.assign("fiend/idle2.wav");
	sound_pain.assign("fiend/dpain1.wav");
	sound_sight1.assign("fiend/sight1.wav");
	sound_sight2.assign("fiend/sight2.wav");
	sound_land.assign("fiend/dland2.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex("models/monsters/fiend/tris.md2");
	self->mins = { -32, -32, -24 };
	self->maxs = { 32, 32, 48 };

	self->health = 250 * st.health_multiplier;
	self->gib_health = -180;
	self->mass = 250;

	self->pain = fiend_pain;
	self->die = fiend_die;

	self->monsterinfo.stand = fiend_stand;
	self->monsterinfo.walk = fiend_walk;
	self->monsterinfo.run = fiend_run;
	self->monsterinfo.dodge = nullptr;
	self->monsterinfo.attack = fiend_jump;
	self->monsterinfo.melee = fiend_melee;
	self->monsterinfo.sight = fiend_sight;
	self->monsterinfo.search = fiend_search;
	self->monsterinfo.checkattack = fiend_checkattack;
	self->monsterinfo.blocked = fiend_blocked;
	self->monsterinfo.setskin = fiend_setskin;

	gi.linkentity (self);
	
	M_SetAnimation(self, &fiend_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;
	self->monsterinfo.can_jump = !(self->spawnflags & SPAWNFLAG_FIEND_NOJUMPING);
	self->monsterinfo.drop_height = 256;
	self->monsterinfo.jump_height = 68;

	walkmonster_start (self);
}
