/*
==============================================================================

QUAKE VORE

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_vore.h"

static cached_soundindex sound_death;
static cached_soundindex sound_sight;
static cached_soundindex sound_pain1;
static cached_soundindex sound_attack;
static cached_soundindex sound_attack2;
static cached_soundindex sound_idle;

constexpr spawnflags_t SPAWNFLAG_VORE_ONROOF = 8_spawnflag;
constexpr spawnflags_t SPAWNFLAG_VORE_NOJUMPING = 16_spawnflag;

void vore_walk(edict_t* self);
void vore_dodge_jump(edict_t* self);
void vore_jump_straightup(edict_t* self);
void vore_jump_wait_land(edict_t* self);
bool vore_ok_to_transition(edict_t* self);
void vore_stand(edict_t* self);

//
//CEILING PREP
//

inline bool vore_on_ceiling(edict_t* ent)
{
	return (ent->gravityVector[2] > 0);
}

bool vore_ok_to_transition(edict_t* self)
{
	trace_t trace;
	vec3_t	pt, start;
	float	max_dist;
	float	margin;
	float	end_height;

	if (vore_on_ceiling(self))
	{
		if (!self->groundentity)
			return true;

		max_dist = -384;
		margin = self->mins[2] - 8;
	}
	else
	{
		max_dist = 256;
		margin = self->maxs[2] + 8;
	}

	pt = self->s.origin;
	pt[2] += max_dist;
	trace = gi.trace(self->s.origin, self->mins, self->maxs, pt, self, MASK_MONSTERSOLID);

	if (trace.fraction == 1.0f ||
		!(trace.contents & CONTENTS_SOLID) ||
		(trace.ent != world))
	{
		if (vore_on_ceiling(self))
		{
			if (trace.plane.normal[2] < 0.9f)
				return false;
		}
		else
		{
			if (trace.plane.normal[2] > -0.9f)
				return false;
		}
	}

	end_height = trace.endpos[2];

	pt[0] = self->absmin[0];
	pt[1] = self->absmin[1];
	pt[2] = trace.endpos[2] + margin;
	start = pt;
	start[2] = self->s.origin[2];
	trace = gi.traceline(start, pt, self, MASK_MONSTERSOLID);
	if (trace.fraction == 1.0f || !(trace.contents & CONTENTS_SOLID) || (trace.ent != world))
		return false;
	if (fabsf(end_height + margin - trace.endpos[2]) > 8)
		return false;

	pt[0] = self->absmax[0];
	pt[1] = self->absmin[1];
	start = pt;
	start[2] = self->s.origin[2];
	trace = gi.traceline(start, pt, self, MASK_MONSTERSOLID);
	if (trace.fraction == 1.0f || !(trace.contents & CONTENTS_SOLID) || (trace.ent != world))
		return false;
	if (fabsf(end_height + margin - trace.endpos[2]) > 8)
		return false;

	pt[0] = self->absmax[0];
	pt[1] = self->absmax[1];
	start = pt;
	start[2] = self->s.origin[2];
	trace = gi.traceline(start, pt, self, MASK_MONSTERSOLID);
	if (trace.fraction == 1.0f || !(trace.contents & CONTENTS_SOLID) || (trace.ent != world))
		return false;
	if (fabsf(end_height + margin - trace.endpos[2]) > 8)
		return false;

	pt[0] = self->absmin[0];
	pt[1] = self->absmax[1];
	start = pt;
	start[2] = self->s.origin[2];
	trace = gi.traceline(start, pt, self, MASK_MONSTERSOLID);
	if (trace.fraction == 1.0f || !(trace.contents & CONTENTS_SOLID) || (trace.ent != world))
		return false;
	if (fabsf(end_height + margin - trace.endpos[2]) > 8)
		return false;

	return true;
}

//
//SOUNDS
//

MONSTERINFO_SEARCH(vore_search) (edict_t* self) -> void
{
	if (frandom() < 0.2f)
		gi.sound (self, CHAN_BODY, sound_idle, 1, ATTN_IDLE, 0);
}

MONSTERINFO_SIGHT(vore_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

//
//STAND
//

mframe_t vore_frames_stand [] =
{
	{ai_stand}
};
MMOVE_T(vore_move_stand) = {FRAME_walk1, FRAME_walk1, vore_frames_stand, nullptr };

MONSTERINFO_STAND(vore_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &vore_move_stand);
}

//
// WALK
//

mframe_t vore_frames_walk [] =
{
	{ai_walk, 6},
	{ai_walk, 4},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk, 5},
	{ai_walk, 6},
	{ai_walk, 5},
	{ai_walk},
	{ai_walk, 4},
	{ai_walk, 5}
};
MMOVE_T(vore_move_walk) = {FRAME_walk1, FRAME_walk12, vore_frames_walk, nullptr };

MONSTERINFO_WALK(vore_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &vore_move_walk);
}


//
// RUN
//

void vore_run (edict_t *self);

mframe_t vore_frames_run [] =
{
	{ai_run, 6},
	{ai_run, 4},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run, 5},
	{ai_run, 6},
	{ai_run, 5},
	{ai_run},
	{ai_run, 4},
	{ai_run, 5}
};
MMOVE_T(vore_move_run) = {FRAME_walk1, FRAME_walk12, vore_frames_run, vore_run};

MONSTERINFO_RUN(vore_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
	{
		M_SetAnimation(self, &vore_move_stand);
		return;
	}
	else
		M_SetAnimation(self, &vore_move_run);
}

//
// ATTACK
//

void vore_fire(edict_t* self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;
	trace_t trace;
	int		rocketSpeed;
	vec3_t target;
	bool   blindfire = false;
	vec3_t offset = { 0, 0, 10 };
	int damage = 40;

	if (self->monsterinfo.aiflags & AI_MANUAL_STEERING)
		blindfire = true;
	else
		blindfire = false;

	if (!self->enemy || !self->enemy->inuse)
		return;

	if (vore_on_ceiling(self))
	{
		offset = { 0, 0, -10 };
	}

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, offset, forward, right);

	rocketSpeed = 350;

	if (blindfire)
		target = self->monsterinfo.blind_fire_target;
	else
		target = self->enemy->s.origin;

	if (blindfire)
	{
		vec = target;
		dir = vec - start;
	}

	else if (frandom() < 0.33f || (start[2] < self->enemy->absmin[2]))
	{
		vec = target;
		vec[2] += self->enemy->viewheight;
		dir = vec - start;
	}
	else
	{
		vec = target;
		vec[2] = self->enemy->absmin[2] + 1;
		dir = vec - start;
	}

	if ((!blindfire) && (frandom() < 0.35f))
		PredictAim(self, self->enemy, start, rocketSpeed, false, 0.f, &dir, &vec);

	dir.normalize();

	trace = gi.traceline(start, vec, self, MASK_PROJECTILE);
	if (blindfire)
	{
		if (!(trace.startsolid || trace.allsolid || (trace.fraction < 0.5f)))
			fire_vorepod(self, start, dir, damage, rocketSpeed, (float)damage, damage, 0.075f, 0);
		else
		{
			vec = target;
			vec += (right * -10);
			dir = vec - start;
			dir.normalize();
			trace = gi.traceline(start, vec, self, MASK_PROJECTILE);
			if (!(trace.startsolid || trace.allsolid || (trace.fraction < 0.5f)))
				fire_vorepod(self, start, dir, damage, rocketSpeed, (float)damage, damage, 0.075f, 0);
			else
			{
				vec = target;
				vec += (right * 10);
				dir = vec - start;
				dir.normalize();
				trace = gi.traceline(start, vec, self, MASK_PROJECTILE);
				if (!(trace.startsolid || trace.allsolid || (trace.fraction < 0.5f)))
					fire_vorepod(self, start, dir, damage, rocketSpeed, (float)damage, damage, 0.075f, 0);
			}
		}
	}
	else
	{
		if (trace.fraction > 0.5f || trace.ent->solid != SOLID_BSP)
			fire_vorepod(self, start, dir, damage, rocketSpeed, (float)damage, damage, 0.15f, 0);
	}

	gi.sound(self, CHAN_WEAPON | CHAN_RELIABLE, sound_attack2, 1, ATTN_NORM, 0);
}

void vore_attack_snd(edict_t* self)
{
	gi.sound(self, CHAN_AUTO, sound_attack, 1, ATTN_NORM, 0);
}

mframe_t vore_frames_attack[] =
{
	{ai_charge, 0, vore_attack_snd},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, vore_fire},
	{ai_charge}
};
MMOVE_T(vore_move_attack) = { FRAME_attack1, FRAME_attack11, vore_frames_attack, vore_run };

MONSTERINFO_ATTACK(vore_attack) (edict_t* self) -> void
{
	vec3_t offset;

	if (vore_on_ceiling(self))
		offset = { 0, 0, -10 };
	else
		offset = { 0, 0, 10 };

	if (!M_CheckClearShot(self, offset))
		return;

	float r, chance;

	monster_done_dodge(self);

	if (self->monsterinfo.attack_state == AS_BLIND)
	{
		if (self->monsterinfo.blind_fire_delay < 1.0_sec)
			chance = 1.0;
		else if (self->monsterinfo.blind_fire_delay < 7.5_sec)
			chance = 0.4f;
		else
			chance = 0.1f;

		r = frandom();

		self->monsterinfo.blind_fire_delay += random_time(5.5_sec, 6.5_sec);

		if (!self->monsterinfo.blind_fire_target)
			return;

		if (r > chance)
			return;

		self->monsterinfo.aiflags |= AI_MANUAL_STEERING;
		M_SetAnimation(self, &vore_move_attack);
		self->monsterinfo.attack_finished = level.time + random_time(2_sec);
		return;
	}

	M_SetAnimation(self, &vore_move_attack);
}

//
// DODGE
// 

void vore_jump_straightup(edict_t* self)
{
	if (self->deadflag)
		return;

	if (vore_on_ceiling(self))
	{
		if (vore_ok_to_transition(self))
		{
			self->gravityVector[2] = -1;
			self->s.angles[2] += 180.0f;
			if (self->s.angles[2] > 360.0f)
				self->s.angles[2] -= 360.0f;
			self->groundentity = nullptr;
		}
	}
	else if (self->groundentity)
	{
		self->velocity[0] += crandom() * 5;
		self->velocity[1] += crandom() * 5;
		self->velocity[2] += -400 * self->gravityVector[2];
		if (vore_ok_to_transition(self))
		{
			self->gravityVector[2] = 1;
			self->s.angles[2] = 180.0;
			self->groundentity = nullptr;
		}
	}
}

mframe_t vore_frames_jump_straightup[] = {
	{ ai_move, 1, vore_jump_straightup },
	{ ai_move, 1, vore_jump_wait_land },
	{ ai_move, -1, monster_footstep },
	{ ai_move, -1 }
};
MMOVE_T(vore_move_jump_straightup) = { FRAME_walk5, FRAME_walk8, vore_frames_jump_straightup, vore_run };

void vore_dodge_jump(edict_t* self)
{
	M_SetAnimation(self, &vore_move_jump_straightup);
}

MONSTERINFO_DODGE(vore_dodge) (edict_t* self, edict_t* attacker, gtime_t eta, trace_t* tr, bool gravity) -> void
{
	if (!self->groundentity || self->health <= 0)
		return;

	if (!self->enemy)
	{
		self->enemy = attacker;
		FoundTarget(self);
		return;
	}

	if ((eta < FRAME_TIME_MS) || (eta > 5_sec))
		return;

	if (self->timestamp > level.time)
		return;

	self->timestamp = level.time + random_time(1_sec, 5_sec);

	vore_dodge_jump(self);
}

void vore_jump_down(edict_t* self)
{
	vec3_t forward, up;

	AngleVectors(self->s.angles, forward, nullptr, up);
	self->velocity += (forward * 100);
	self->velocity += (up * 300);
}

void vore_jump_up(edict_t* self)
{
	vec3_t forward, up;

	AngleVectors(self->s.angles, forward, nullptr, up);
	self->velocity += (forward * 200);
	self->velocity += (up * 450);
}

void vore_jump_wait_land(edict_t* self)
{
	if ((frandom() < 0.4f) && (level.time >= self->monsterinfo.attack_finished))
	{
		self->monsterinfo.attack_finished = level.time + 300_ms;
		vore_attack(self);
	}

	if (self->groundentity == nullptr)
	{
		self->gravity = 1.3f;
		self->monsterinfo.nextframe = self->s.frame;

		if (monster_jump_finished(self))
		{
			self->gravity = 1;
			self->monsterinfo.nextframe = self->s.frame + 1;
		}
	}
	else
	{
		self->gravity = 1;
		self->monsterinfo.nextframe = self->s.frame + 1;
	}
}

mframe_t vore_frames_jump_up[] = {
	{ ai_move, -8 },
	{ ai_move, -8 },
	{ ai_move, -8 },
	{ ai_move, -8 },

	{ ai_move, 0, vore_jump_up },
	{ ai_move, 0, vore_jump_wait_land },
	{ ai_move, 0, monster_footstep }
};
MMOVE_T(vore_move_jump_up) = { FRAME_walk5, FRAME_walk11, vore_frames_jump_up, vore_run };

mframe_t vore_frames_jump_down[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },

	{ ai_move, 0, vore_jump_down },
	{ ai_move, 0, vore_jump_wait_land },
	{ ai_move, 0, monster_footstep }
};
MMOVE_T(vore_move_jump_down) = { FRAME_walk5, FRAME_walk11, vore_frames_jump_down, vore_run };

void vore_jump(edict_t* self, blocked_jump_result_t result)
{
	if (!self->enemy)
		return;

	if (result == blocked_jump_result_t::JUMP_JUMP_UP)
		M_SetAnimation(self, &vore_move_jump_up);
	else
		M_SetAnimation(self, &vore_move_jump_down);
}

// 
// Blocked
// 

MONSTERINFO_BLOCKED(vore_blocked) (edict_t* self, float dist) -> bool
{
	if (!has_valid_enemy(self))
		return false;

	bool onCeiling = vore_on_ceiling(self);

	if (!onCeiling)
	{
		if (auto result = blocked_checkjump(self, dist); result != blocked_jump_result_t::NO_JUMP)
		{
			if (result != blocked_jump_result_t::JUMP_TURN)
				vore_jump(self, result);
			return true;
		}

		if (blocked_checkplat(self, dist))
			return true;
	}
	else
	{
		if (vore_ok_to_transition(self))
		{
			self->gravityVector[2] = -1;
			self->s.angles[2] += 180.0f;
			if (self->s.angles[2] > 360.0f)
				self->s.angles[2] -= 360.0f;
			self->groundentity = nullptr;
			return true;
		}
	}

	return false;
}

MONSTERINFO_PHYSCHANGED(vore_physics_change) (edict_t* self) -> void
{
	if (vore_on_ceiling(self) && !self->groundentity)
	{
		self->mins = { -32, -32, -32 };
		self->maxs = { 32, 32, 16 };

		self->gravityVector[2] = -1;
		self->s.angles[2] += 180.0f;
		if (self->s.angles[2] > 360.0f)
			self->s.angles[2] -= 360.0f;
	}
	else
	{
		self->mins = { -32, -32, -24 };
		self->maxs = { 32, 32, 32 };
	}
}

//
// PAIN
//

mframe_t vore_frames_pain1 [] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(vore_move_pain1) = {FRAME_pain1, FRAME_pain5, vore_frames_pain1, vore_run};

PAIN(vore_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	gi.sound (self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);

	M_SetAnimation(self, &vore_move_pain1);
}

MONSTERINFO_SETSKIN(vore_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
// DEATH
//

void vore_dead (edict_t *self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, -8 };
	monster_dead(self);
}

static void vore_shrink(edict_t* self)
{
	self->maxs[2] = -4;
	self->svflags |= SVF_DEADMONSTER;
	gi.linkentity(self);
}

mframe_t vore_frames_death1 [] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 0, vore_shrink},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(vore_move_death) = { FRAME_death1, FRAME_death7, vore_frames_death1, vore_dead };

DIE(vore_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	self->movetype = MOVETYPE_TOSS;
	self->s.angles[2] = 0;
	self->gravityVector = { 0, 0, -1 };

	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 3, "models/objects/gibs/sm_meat/tris.md2" },
			//{ "models/monsters/vore/gibs/chest.md2", },
			//{ "models/monsters/vore/gibs/claw.md2", GIB_UPRIGHT },
			{ "models/monsters/vore/gibs/head.md2", GIB_SKINNED | GIB_HEAD }
			});

		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	M_SetAnimation(self, &vore_move_death);
}


//
// SPAWN
//

/*QUAKED monster_vore (1 .5 0) (-16 -16 -24) (16 16 40) Ambush Trigger_Spawn Sight
model="models/monsters/vore/tris.md2"
*/
void SP_monster_vore(edict_t *self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}
	
	sound_sight.assign("vore/sight.wav");
	sound_pain1.assign("vore/pain.wav");
	sound_death.assign("vore/death.wav");
	sound_attack.assign("vore/attack.wav");
	sound_attack2.assign("vore/attack2.wav");
	sound_idle.assign("vore/idle.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;


	self->s.modelindex = gi.modelindex ("models/monsters/vore/tris.md2");

	self->mins = { -32, -32, -24 };
	self->maxs = { 32, 32, 32 };
	
	self->health = 600 * st.health_multiplier;
	self->gib_health = -90;
	self->mass = 125;

	self->pain = vore_pain;
	self->die = vore_die;

	self->monsterinfo.sight = vore_sight;
	self->monsterinfo.search = vore_search;

	self->monsterinfo.stand = vore_stand;
	self->monsterinfo.walk = vore_walk;
	self->monsterinfo.run = vore_run;

	self->monsterinfo.attack = vore_attack;
	self->monsterinfo.melee = nullptr;

	self->monsterinfo.setskin = vore_setskin;
	self->monsterinfo.dodge = vore_dodge;
	self->monsterinfo.blocked = vore_blocked;
	self->monsterinfo.physics_change = vore_physics_change;

	gi.linkentity (self);

	if (self->spawnflags.has(SPAWNFLAG_VORE_ONROOF))
	{
		self->s.angles[2] = 180;
		self->gravityVector[2] = 1;
	}

	self->monsterinfo.can_jump = !self->spawnflags.has(SPAWNFLAG_VORE_NOJUMPING);
	self->monsterinfo.drop_height = 256;
	self->monsterinfo.jump_height = 68;

	M_SetAnimation(self, &vore_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	walkmonster_start (self);
}