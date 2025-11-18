// Licensed under the GNU General Public License 2.0.

/*
==============================================================================

ASSASSIN

==============================================================================
*/

#include "../g_local.h"
#include "m_unseen_assassin.h"
#include "../m_flash.h"

static cached_soundindex sound_pain;
static cached_soundindex sound_die;
static cached_soundindex sound_sight;
static cached_soundindex sound_idle;

void BossPowerups(edict_t* self);

bool assassin_do_pounce(edict_t* self, const vec3_t& dest);
void assassin_walk(edict_t* self);
void assassin_dodge_jump(edict_t* self);
void assassin_jump_straightup(edict_t* self);
void assassin_jump_wait_land(edict_t* self);
void assassin_stand(edict_t* self);
void assassin_cloak(edict_t* self);
void assassin_uncloak(edict_t* self);

//
// SOUNDS
// 

MONSTERINFO_SIGHT(assassin_sight) (edict_t* self, edict_t* other) -> void
{
	assassin_uncloak(self);
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

void assassin_idle_noise(edict_t* self)
{
	gi.sound(self, CHAN_VOICE, sound_idle, 0.5, ATTN_IDLE, 0);
}

//
// CLOAKING
//

void assassin_cloak(edict_t* self)
{
	self->s.renderfx |= RF_TRANSLUCENT;
	self->s.alpha = 0.05f;
	self->monsterinfo.aiflags |= AI_MANUAL_STEERING;
	self->timestamp = level.time;
}

void assassin_uncloak(edict_t* self)
{
	self->s.renderfx &= ~RF_TRANSLUCENT;
	self->s.alpha = 1.0f;
	self->monsterinfo.aiflags &= ~AI_MANUAL_STEERING;
}

bool assassin_is_cloaked(edict_t* self)
{
	return (self->s.renderfx & RF_TRANSLUCENT) && (self->s.alpha < 0.1f);
}

mframe_t assassin_frames_idle[] = {
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand }
};
MMOVE_T(assassin_move_idle) = { FRAME_stand1, FRAME_stand40, assassin_frames_idle, assassin_stand };

mframe_t assassin_frames_idle2[] = {
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand }
};
MMOVE_T(assassin_move_idle2) = { FRAME_recin1, FRAME_recin40, assassin_frames_idle2, assassin_stand };

MONSTERINFO_IDLE(assassin_idle) (edict_t* self) -> void
{
	if (frandom() < 0.35f)
		M_SetAnimation(self, &assassin_move_idle);
	else
		M_SetAnimation(self, &assassin_move_idle2);
}

// 
// STAND
// 

mframe_t assassin_frames_stand[] = {
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand }
};
MMOVE_T(assassin_move_stand) = { FRAME_recin1, FRAME_recin40, assassin_frames_stand, assassin_stand };

MONSTERINFO_STAND(assassin_stand) (edict_t* self) -> void
{
	if (frandom() < 0.25f)
		M_SetAnimation(self, &assassin_move_stand);
	else
		M_SetAnimation(self, &assassin_move_idle);
}

// 
// RUN
// 

mframe_t assassin_frames_run[] = {
	{ ai_run, 13, monster_footstep },
	{ ai_run, 17 },
	{ ai_run, 21 },
	{ ai_run, 18, monster_footstep },
	{ ai_run, 17 },
	{ ai_run, 21 }
};
MMOVE_T(assassin_move_run) = { FRAME_walk1, FRAME_walk6, assassin_frames_run, nullptr };

MONSTERINFO_RUN(assassin_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &assassin_move_stand);
	else
		M_SetAnimation(self, &assassin_move_run);
}

// 
// WALK
// 

mframe_t assassin_frames_walk[] = {
	{ ai_walk, 4, monster_footstep },
	{ ai_walk, 6 },
	{ ai_walk, 8 },
	{ ai_walk, 5, monster_footstep },
	{ ai_walk, 4 },
	{ ai_walk, 6 }
};
MMOVE_T(assassin_move_walk) = { FRAME_walk1, FRAME_walk6, assassin_frames_walk, assassin_walk };

MONSTERINFO_WALK(assassin_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &assassin_move_walk);
}


// 
// PAIN
// 

mframe_t assassin_frames_pain1[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(assassin_move_pain1) = { FRAME_pain1, FRAME_pain4, assassin_frames_pain1, assassin_run };

mframe_t assassin_frames_pain2[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(assassin_move_pain2) = { FRAME_pain5, FRAME_pain12, assassin_frames_pain2, assassin_run };

PAIN(assassin_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (self->deadflag)
		return;

	if (assassin_is_cloaked(self))
		assassin_uncloak(self);

	if (self->groundentity == nullptr)
		return;

	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;

	gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

	if (mod.id == MOD_CHAINFIST || damage > 10)
	{
		if (self->groundentity && (frandom() < 0.5f))
			assassin_dodge_jump(self);
		else if (M_ShouldReactToPain(self, mod))
			if (frandom() < 0.20f)
			{
				M_SetAnimation(self, &assassin_move_pain2);
			}
			else
			{
				M_SetAnimation(self, &assassin_move_pain1);
			}
	}
}

MONSTERINFO_CHECKATTACK(assassin_checkattack) (edict_t* self) -> bool
{
	if (!self->enemy)
		return false;

	BossPowerups(self);

	return M_CheckAttack_Base(self, 0.4f, 0.8f, 0.6f, 0.7f, 0.85f, 0.f);
}

MONSTERINFO_SETSKIN(assassin_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

// 
// ATTACK
// 

void assassin_shoot_attack(edict_t* self)
{
	vec3_t	offset, start, f, r, dir;
	vec3_t	end;
	float	dist;
	trace_t trace;

	if (!has_valid_enemy(self))
		return;

	if (assassin_is_cloaked(self))
		assassin_uncloak(self);

	if (self->groundentity && frandom() < 0.33f)
	{
		dir = self->enemy->s.origin - self->s.origin;
		dist = dir.length();

		if ((dist > 256) || (frandom() < 0.5f))
			assassin_do_pounce(self, self->enemy->s.origin);
		else
			assassin_jump_straightup(self);
	}

	AngleVectors(self->s.angles, f, r, nullptr);
	offset = { 24, 0, 6 };
	start = M_ProjectFlashSource(self, offset, f, r);

	dir = self->enemy->s.origin - start;
	if (frandom() < 0.3f)
		PredictAim(self, self->enemy, start, 1000, true, 0, &dir, &end);
	else
		end = self->enemy->s.origin;

	trace = gi.traceline(start, end, self, MASK_PROJECTILE);
	if (trace.ent == self->enemy || trace.ent == world)
	{
		dir.normalize();
		if (frandom() < 0.2f)
			monster_fire_redflare(self, start, dir, 100, 800, MZ2_ACTOR_MACHINEGUN_1);
		else
			monster_fire_yellowflare(self, start, dir, 50, 800, MZ2_ACTOR_MACHINEGUN_1);
	}
}

void assassin_shoot_attack2(edict_t* self)
{
	if (frandom() < 0.5)
		assassin_shoot_attack(self);
}

mframe_t assassin_frames_shoot[] = {
	{ ai_charge, 0, assassin_shoot_attack },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, assassin_shoot_attack2 },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(assassin_move_shoot) = { FRAME_attak1, FRAME_attak8, assassin_frames_shoot, assassin_run };

MONSTERINFO_ATTACK(assassin_attack_ranged) (edict_t* self) -> void
{
	if (!has_valid_enemy(self))
		return;

	if (frandom() > 0.5f)
	{
		self->monsterinfo.attack_state = AS_STRAIGHT;
	}
	else
	{
		if (frandom() <= 0.5f) 
			self->monsterinfo.lefty = !self->monsterinfo.lefty;
		self->monsterinfo.attack_state = AS_SLIDING;
	}
	M_SetAnimation(self, &assassin_move_shoot);
}

// 
// POUNCE
// 

bool assassin_check_lz(edict_t* self, edict_t* target, const vec3_t& dest)
{
	if ((gi.pointcontents(dest) & MASK_WATER) || (target->waterlevel))
		return false;

	if (!target->groundentity)
		return false;

	vec3_t jumpLZ;

	jumpLZ[0] = self->enemy->mins[0];
	jumpLZ[1] = self->enemy->mins[1];
	jumpLZ[2] = self->enemy->mins[2] - 0.25f;
	if (!(gi.pointcontents(jumpLZ) & MASK_SOLID))
		return false;

	jumpLZ[0] = self->enemy->maxs[0];
	jumpLZ[1] = self->enemy->mins[1];
	if (!(gi.pointcontents(jumpLZ) & MASK_SOLID))
		return false;

	jumpLZ[0] = self->enemy->maxs[0];
	jumpLZ[1] = self->enemy->maxs[1];
	if (!(gi.pointcontents(jumpLZ) & MASK_SOLID))
		return false;

	jumpLZ[0] = self->enemy->mins[0];
	jumpLZ[1] = self->enemy->maxs[1];
	if (!(gi.pointcontents(jumpLZ) & MASK_SOLID))
		return false;

	return true;
}

bool assassin_do_pounce(edict_t* self, const vec3_t& dest)
{
	vec3_t	dist;
	float	length;
	vec3_t	jumpAngles;
	vec3_t	jumpLZ;
	float	velocity = 400.1f;

	if (!assassin_check_lz(self, self->enemy, dest))
		return false;

	dist = dest - self->s.origin;

	jumpAngles = vectoangles(dist);
	if (fabsf(jumpAngles[YAW] - self->s.angles[YAW]) > 45)
		return false;

	if (isnan(jumpAngles[YAW]))
		return false; 

	self->ideal_yaw = jumpAngles[YAW];
	M_ChangeYaw(self);

	length = dist.length();
	if (length > 450)
		return false;

	jumpLZ = dest;
	vec3_t dir = dist.normalized();

	while (velocity <= 800)
	{
		if (M_CalculatePitchToFire(self, jumpLZ, self->s.origin, dir, velocity, 3, false, true))
			break;

		velocity += 200;
	}

	if (velocity > 800)
		return false;

	self->velocity = dir * velocity;
	return true;
}

// 
// DODGE
// 

void assassin_jump_straightup(edict_t* self)
{
	if (self->deadflag)
		return;

	if (self->groundentity) 
	{
		self->velocity[0] += crandom() * 5;
		self->velocity[1] += crandom() * 5;
		self->velocity[2] += 400;
		self->groundentity = nullptr;
	}
}

mframe_t assassin_frames_jump_straightup[] = {
	{ ai_move, 1, assassin_jump_straightup },
	{ ai_move, 1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1, assassin_jump_wait_land },
	{ ai_move, -1, monster_footstep },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};

MMOVE_T(assassin_move_jump_straightup) = { FRAME_duck4, FRAME_duck18, assassin_frames_jump_straightup, assassin_run };

void assassin_dodge_jump(edict_t* self)
{
	M_SetAnimation(self, &assassin_move_jump_straightup);

	float cloak_chance = (skill->integer) * 0.2f;

	if (frandom() < cloak_chance)
	{
		assassin_cloak(self);
	}
}

#if 0
mframe_t assassin_frames_dodge_run[] = {
	{ ai_run, 13 },
	{ ai_run, 17 },
	{ ai_run, 21 },
	{ ai_run, 18, monster_done_dodge }
};
MMOVE_T(assassin_move_dodge_run) = { FRAME_run01, FRAME_run04, assassin_frames_dodge_run, nullptr };
#endif

MONSTERINFO_DODGE(assassin_dodge) (edict_t* self, edict_t* attacker, gtime_t eta, trace_t* tr, bool gravity) -> void
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

	assassin_dodge_jump(self);
}

void assassin_jump_down(edict_t* self)
{
	vec3_t forward, up;

	AngleVectors(self->s.angles, forward, nullptr, up);
	self->velocity += (forward * 100);
	self->velocity += (up * 300);
}

void assassin_jump_up(edict_t* self)
{
	vec3_t forward, up;

	AngleVectors(self->s.angles, forward, nullptr, up);
	self->velocity += (forward * 200);
	self->velocity += (up * 450);
}

void assassin_jump_wait_land(edict_t* self)
{
	if ((frandom() < 0.4f) && (level.time >= self->monsterinfo.attack_finished))
	{
		self->monsterinfo.attack_finished = level.time + 300_ms;
		assassin_shoot_attack(self);
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

mframe_t assassin_frames_jump_up[] = {
	{ ai_move, -8 },
	{ ai_move, -8 },
	{ ai_move, -8 },
	{ ai_move, 1, assassin_jump_straightup },
	{ ai_move, 1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1, assassin_jump_wait_land },
	{ ai_move, -1, monster_footstep },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(assassin_move_jump_up) = { FRAME_duck1, FRAME_duck18, assassin_frames_jump_up, assassin_run };

mframe_t assassin_frames_jump_down[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move, 0, assassin_jump_straightup },
	{ ai_move},
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move, 0, assassin_jump_wait_land },
	{ ai_move, 0, monster_footstep },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(assassin_move_jump_down) = { FRAME_duck1, FRAME_duck18, assassin_frames_jump_down, assassin_run };

void assassin_jump(edict_t* self, blocked_jump_result_t result)
{
	if (!self->enemy)
		return;

	if (result == blocked_jump_result_t::JUMP_JUMP_UP)
		M_SetAnimation(self, &assassin_move_jump_up);
	else
		M_SetAnimation(self, &assassin_move_jump_down);
}

//
// BLOCKED
//

MONSTERINFO_BLOCKED(assassin_blocked) (edict_t* self, float dist) -> bool
{
	if (!has_valid_enemy(self))
		return false;

	return false;
}

// 
// DEATH
// 

static void assassin_gib(edict_t* self)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_EXPLOSION1_BIG);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	self->s.sound = 0;
	self->s.skinnum /= 2;

	ThrowGibs(self, 500, {
		{ 2, "models/objects/gibs/sm_meat/tris.md2" },
		{ 2, "models/objects/gibs/sm_metal/tris.md2", GIB_METALLIC },
		{ "models/monsters/assassin/weapon.md2", GIB_SKINNED | GIB_METALLIC },
		{ "models/objects/gibs/chest/tris.md2" },
		{ "models/objects/gibs/head2/tris.md2", GIB_HEAD }
		//{ "models/monsters/assassin/gibs/chest.md2", GIB_SKINNED },
		//{ "models/monsters/assassin/gibs/arm.md2", GIB_SKINNED },
		//{ "models/monsters/assassin/gibs/leg.md2", GIB_SKINNED },
		//{ "models/monsters/assassin/gibs/head.md2", GIB_SKINNED | GIB_METALLIC | GIB_HEAD }
		});
}
void assassin_dead(edict_t* self)
{
	if (self->spawnflags.has(SPAWNFLAG_MONSTER_DEAD))
	{
		self->deadflag = false;
		self->takedamage = true;
		return;
	}

	assassin_gib(self);
}

mframe_t assassin_frames_death[] = {
	{ ai_move, -5, BossExplode },
	{ ai_move },
	{ ai_move },
	{ ai_move },

	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(assassin_move_death) = { FRAME_death18, FRAME_death25, assassin_frames_death, assassin_dead };

DIE(assassin_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	assassin_uncloak(self);

	if (self->spawnflags.has(SPAWNFLAG_MONSTER_DEAD))
	{
		// check for gib
		if (M_CheckGib(self, mod))
		{
			assassin_gib(self);
			self->deadflag = true;
			return;
		}

		if (self->deadflag)
			return;
	}
	else
	{
		gi.sound(self, CHAN_VOICE, sound_die, 1, ATTN_NORM, 0);
		self->deadflag = true;
		self->takedamage = false;
	}

	M_SetAnimation(self, &assassin_move_death);
}

/*QUAKED monster_assassin (1 .5 0) (-28 -28 -18) (28 28 18) Ambush Trigger_Spawn Sight NoJumping StartVisible
Spider Monster

NoJumping - Assassin will not jump onto/off objects
StartVisible - Assassin starts visible instead of cloaked
*/

constexpr spawnflags_t SPAWNFLAG_ASSASSIN_NOJUMPING = 16_spawnflag;
constexpr spawnflags_t SPAWNFLAG_ASSASSIN_STARTVISIBLE = 32_spawnflag;

void SP_monster_assassin(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_pain.assign("assassin/pain.wav");
	sound_die.assign("assassin/death.wav");
	sound_sight.assign("assassin/sight.wav");
	sound_idle.assign("assassin/idle.wav");

	gi.modelindex("models/objects/laser/tris.md2");

	self->s.modelindex = gi.modelindex("models/monsters/assassin/tris.md2");
	self->s.modelindex2 = gi.modelindex("models/monsters/assassin/weapon.md2");

	gi.modelindex("models/monsters/assassin/gibs/bodya.md2");
	gi.modelindex("models/monsters/assassin/gibs/bodyb.md2");
	gi.modelindex("models/monsters/assassin/gibs/claw.md2");
	gi.modelindex("models/monsters/assassin/gibs/foot.md2");
	gi.modelindex("models/monsters/assassin/gibs/head.md2");
	gi.modelindex("models/monsters/assassin/gibs/leg.md2");

	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->health = max(2000, 2000 + 1000 * (skill->integer - 1)) * st.health_multiplier;
	self->gib_health = -500;
	self->mass = 400;
	if (!st.was_key_specified("armor_type"))
		self->monsterinfo.armor_type = IT_ARMOR_BODY;
	if (!st.was_key_specified("armor_power"))
		self->monsterinfo.armor_power = max(200, 200 + 50 * (skill->integer - 1));
	if (coop->integer)
	{
		self->health += (500 * skill->integer * (CountPlayers() - 1));
		self->monsterinfo.armor_power += (100 * skill->integer * (CountPlayers() - 1));
	}

	self->pain = assassin_pain;
	self->die = assassin_die;

	self->monsterinfo.stand = assassin_stand;
	self->monsterinfo.walk = assassin_walk;
	self->monsterinfo.run = assassin_run;
	self->monsterinfo.attack = assassin_attack_ranged;
	self->monsterinfo.sight = assassin_sight;
	self->monsterinfo.idle = assassin_idle;
	self->monsterinfo.dodge = assassin_dodge;
	self->monsterinfo.blocked = assassin_blocked;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.setskin = assassin_setskin;
	self->monsterinfo.checkattack = assassin_checkattack;

	gi.linkentity(self);

	M_SetAnimation(self, &assassin_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	self->monsterinfo.can_jump = !self->spawnflags.has(SPAWNFLAG_ASSASSIN_NOJUMPING);
	self->monsterinfo.drop_height = 256;
	self->monsterinfo.jump_height = 68;

	if (!self->spawnflags.has(SPAWNFLAG_ASSASSIN_STARTVISIBLE))
	{
		assassin_cloak(self);
	}

	walkmonster_start(self);
}