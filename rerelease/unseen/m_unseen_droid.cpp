// Copyright (c) ZeniMax Media Inc.
// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

DROID

==============================================================================
*/

#include "../g_local.h"
#include "m_unseen_droid.h"
#include "../m_flash.h"

//TODO
// Sound for firing
// Add alt attack - multigrenade
// Add Nightmare alt
// Add sidestep

static cached_soundindex sound_idle;
static cached_soundindex sound_sight1;
static cached_soundindex sound_sight2;
static cached_soundindex sound_pain1;
static cached_soundindex sound_pain2;
static cached_soundindex sound_death;
static cached_soundindex sound_cock;

void BossPowerups(edict_t* self);

void droid_start_charge(edict_t *self)
{
	self->monsterinfo.aiflags |= AI_CHARGING;
}

void droid_stop_charge(edict_t *self)
{
	self->monsterinfo.aiflags &= ~AI_CHARGING;
}

void droid_idle(edict_t *self)
{
	if (frandom() > 0.8f)
		gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
}

// STAND

void droid_stand(edict_t *self);

mframe_t droid_frames_stand1[] = {
	{ ai_stand, 0, droid_idle },
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
MMOVE_T(droid_move_stand) = { FRAME_stand01, FRAME_stand40, droid_frames_stand1, droid_stand };

MONSTERINFO_STAND(droid_stand) (edict_t *self) -> void
{
	M_SetAnimation(self, &droid_move_stand);
}

//
// WALK
//

mframe_t droid_frames_walk[] = {
	{ ai_walk, 4 },
	{ ai_walk, 4, monster_footstep },
	{ ai_walk, 9 },
	{ ai_walk, 8 },
	{ ai_walk, 5, monster_footstep },
	{ ai_walk, 1 }
};
MMOVE_T(droid_move_walk) = { FRAME_walk1, FRAME_walk6, droid_frames_walk, nullptr };

MONSTERINFO_WALK(droid_walk) (edict_t *self) -> void
{
	M_SetAnimation(self, &droid_move_walk);
}

//
// RUN
//

void droid_run(edict_t *self);

mframe_t droid_frames_run[] = {
	{ ai_run, 11 },
	{ ai_run, 10, monster_footstep },
	{ ai_run, 16 },
	{ ai_run, 11 },
	{ ai_run, 10, monster_footstep },
	{ ai_run, 16 }
};
MMOVE_T(droid_move_run) = { FRAME_run1, FRAME_run6, droid_frames_run, nullptr };

MONSTERINFO_RUN(droid_run) (edict_t *self) -> void
{
	monster_done_dodge(self);

	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
	{
		M_SetAnimation(self, &droid_move_stand);
		return;
	}

	if (self->monsterinfo.active_move == &droid_move_walk ||
		self->monsterinfo.active_move == &droid_move_run)
	{
		M_SetAnimation(self, &droid_move_run);
	}
	else
	{
		M_SetAnimation(self, &droid_move_run);
	}
}

//
// PAIN
//

mframe_t droid_frames_pain1[] = {
	{ ai_move, -1 },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(droid_move_pain1) = { FRAME_pain101, FRAME_pain104, droid_frames_pain1, droid_run };

mframe_t droid_frames_pain2[] = {
	{ ai_move, -1 },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(droid_move_pain2) = { FRAME_pain201, FRAME_pain204, droid_frames_pain2, droid_run };

mframe_t droid_frames_pain3[] = {
	{ ai_move, -8 },
	{ ai_move },
	{ ai_move, -4 },
	{ ai_move, -1 }
};
MMOVE_T(droid_move_pain3) = { FRAME_pain301, FRAME_pain304, droid_frames_pain3, droid_run };

PAIN(droid_pain) (edict_t *self, edict_t *other, float kick, int damage, const mod_t &mod) -> void
{
	float r;

	r = frandom();

	monster_done_dodge(self);
	droid_stop_charge(self);

	// if we're blind firing, this needs to be turned off here
	self->monsterinfo.aiflags &= ~AI_MANUAL_STEERING;

	if (level.time < self->pain_debounce_time)
	{
		if ((self->velocity[2] > 100) && ((self->monsterinfo.active_move == &droid_move_pain1) || (self->monsterinfo.active_move == &droid_move_pain2) || (self->monsterinfo.active_move == &droid_move_pain3)))
		{
			// PMM - clear duck flag
			if (self->monsterinfo.aiflags & AI_DUCKED)
				monster_duck_up(self);
			M_SetAnimation(self, &droid_move_pain3);
		}
		return;
	}

	self->pain_debounce_time = level.time + 3_sec;

	if (r < 0.5f)
		gi.sound(self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);

	if (self->velocity[2] > 100)
	{
		// PMM - clear duck flag
		if (self->monsterinfo.aiflags & AI_DUCKED)
			monster_duck_up(self);
		M_SetAnimation(self, &droid_move_pain3);
		return;
	}
	
	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	if (r < 0.5f)
		M_SetAnimation(self, &droid_move_pain1);
	else
		M_SetAnimation(self, &droid_move_pain2);

	// PMM - clear duck flag
	if (self->monsterinfo.aiflags & AI_DUCKED)
		monster_duck_up(self);
}

MONSTERINFO_CHECKATTACK(droid_CheckAttack) (edict_t* self) -> bool
{
	if (!self->enemy)
		return false;

	BossPowerups(self);

	return M_CheckAttack_Base(self, 0.4f, 0.8f, 0.6f, 0.7f, 0.85f, 0.f);
}

MONSTERINFO_SETSKIN(droid_setskin) (edict_t *self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
// ATTACK
//

void droid_fire(edict_t *self)
{
	vec3_t					 start;
	vec3_t					 dir;
	vec3_t					 forward, right, up;
	vec3_t					 aim;
	vec3_t					 vec;
	vec3_t					 end;
	float					 r, u;
	int dmg;

	if (!self->enemy || !self->enemy->inuse) // PGM
		return;								 // PGM

	AngleVectors(self->s.angles, forward, right, up);
	start = M_ProjectFlashSource(self, monster_flash_offset[MZ2_ACTOR_MACHINEGUN_1], forward, right);

	vec = self->s.angles;
	AngleVectors(vec, forward, nullptr, nullptr);

	r = crandom() * 1000;
	u = crandom() * 500;

	end = start + (forward * 8192);
	end += (right * r);
	end += (up * u);

	aim = end - start;
	aim.normalize();

	dmg = 25 + 5 * (skill->integer - 1);

	bool hit = monster_fire_railgun(self, start, aim, dmg, dmg * 2.0f, MZ2_ACTOR_MACHINEGUN_1);

	if (hit)
		self->count = 0;
	else
		self->count++;
}

void droid_fire2(edict_t* self)
{
	vec3_t					 start;
	vec3_t					 forward, right, up;
	vec3_t					 aim;
	monster_muzzleflash_id_t flash_number;
	float					 pitch = 0;
	// PMM
	vec3_t target;
	bool   blindfire = false;

	if (!self->enemy || !self->enemy->inuse) // PGM
		return;								 // PGM

	flash_number = MZ2_ACTOR_MACHINEGUN_1;

	// pmm
	if (self->monsterinfo.aiflags & AI_MANUAL_STEERING)
		blindfire = true;

	if ((blindfire) && (!visible(self, self->enemy)))
	{
		if (!self->monsterinfo.blind_fire_target)
			return;

		target = self->monsterinfo.blind_fire_target;
	}
	else
		target = self->enemy->s.origin;

	AngleVectors(self->s.angles, forward, right, up); // PGM
	start = M_ProjectFlashSource(self, monster_flash_offset[flash_number], forward, right);

	if (self->enemy)
	{
		float dist;

		aim = target - self->s.origin;
		dist = aim.length();

		// aim up if they're on the same level as me and far away.
		if ((dist > 512) && (aim[2] < 64) && (aim[2] > -64))
		{
			aim[2] += (dist - 512);
		}

		aim.normalize();
		pitch = aim[2];
		if (pitch > 0.4f)
			pitch = 0.4f;
		else if (pitch < -0.5f)
			pitch = -0.5f;
	}

	// mortar fires farther
	float speed = 600.f;

	// try search for best pitch
	monster_fire_multigrenade(self, start, aim, 40, speed, flash_number, (crandom_open() * 10.0f), frandom() * 10.f);
}

void droid_fire_alt(edict_t* self)
{
	droid_fire2(self);

	//TODO: Multigrenades
}

static void droid_blind_check(edict_t *self)
{
	if (self->monsterinfo.aiflags & AI_MANUAL_STEERING)
	{
		vec3_t aim = self->monsterinfo.blind_fire_target - self->s.origin;
		self->ideal_yaw = vectoyaw(aim);
	}
}

mframe_t droid_frames_attack1[] = {
	{ ai_charge, 0, droid_fire },
	{ ai_charge },
	{ ai_charge, 0, droid_fire },
	{ ai_charge },
	{ ai_charge, 0, droid_fire },
	{ ai_charge },
	{ ai_charge, 0, droid_fire },
	{ ai_charge }
};
MMOVE_T(droid_move_attack1) = { FRAME_attak1, FRAME_attak8, droid_frames_attack1, droid_run };

mframe_t droid_frames_attack2[] = {
	{ ai_charge, 0, droid_fire_alt },
	{ ai_charge },
	{ ai_charge, 0, droid_fire_alt },
	{ ai_charge },
	{ ai_charge, 0, droid_fire_alt },
	{ ai_charge },
	{ ai_charge, 0, droid_fire_alt },
	{ ai_charge }
};
MMOVE_T(droid_move_attack2) = { FRAME_runs1, FRAME_runs8, droid_frames_attack2, droid_run };

MONSTERINFO_ATTACK(droid_attack) (edict_t *self) -> void
{
	monster_done_dodge(self);
		
	if (self->count <= 3)
		M_SetAnimation(self, &droid_move_attack2);
	else
		M_SetAnimation(self, &droid_move_attack1);
}

//
// SIGHT
//

MONSTERINFO_SIGHT(droid_sight) (edict_t *self, edict_t *other) -> void
{
	if (frandom() < 0.5f)
		gi.sound(self, CHAN_VOICE, sound_sight1, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_sight2, 1, ATTN_NORM, 0);
}

mframe_t droid_frames_duck[] = {
	{ ai_move, -2, monster_duck_down },
	{ ai_move, -5, monster_duck_hold },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },

	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move, 0, monster_duck_up }
};
MMOVE_T(droid_move_duck) = { FRAME_duck1, FRAME_duck19, droid_frames_duck, droid_run };

//
// DEATH
//

static void droid_gib(edict_t* self)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_EXPLOSION1_BIG);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	self->s.sound = 0;
	self->s.skinnum /= 2;

	ThrowGibs(self, 150, {
		{ 2, "models/objects/gibs/sm_meat/tris.md2" },
		{ 3, "models/objects/gibs/sm_metal/tris.md2", GIB_METALLIC },
		{ "models/monsters/droid/weapon.md2", GIB_METALLIC },
		{ "models/objects/gibs/chest/tris.md2" },
		{ "models/objects/gibs/head2/tris.md2", GIB_HEAD },
		});
}

void droid_dead(edict_t* self)
{
	// no blowy on deady
	if (self->spawnflags.has(SPAWNFLAG_MONSTER_DEAD))
	{
		self->deadflag = false;
		self->takedamage = true;
		return;
	}

	droid_gib(self);
}

mframe_t droid_frames_death1[] = {
	{ ai_move, -10, BossExplode },
	{ ai_move, -10 },
	{ ai_move, -5 },
	{ ai_move, -5 },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(droid_move_death1) = { FRAME_death101, FRAME_death106, droid_frames_death1, droid_dead };

mframe_t droid_frames_death2[] = {
	{ ai_move, -10, BossExplode },
	{ ai_move, -10 },
	{ ai_move, -5 },
	{ ai_move, -5 },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(droid_move_death2) = { FRAME_death201, FRAME_death206, droid_frames_death2, droid_dead };

mframe_t droid_frames_death3[] = {
	{ ai_move, -5, BossExplode },
	{ ai_move, -5 },
	{ ai_move, 0 },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(droid_move_death3) = { FRAME_death301, FRAME_death308, droid_frames_death3, droid_dead };

DIE(droid_die) (edict_t *self, edict_t *inflictor, edict_t *attacker, int damage, const vec3_t &point, const mod_t &mod) -> void
{
	int n;

	self->s.modelindex2 = 0;

	if (self->spawnflags.has(SPAWNFLAG_MONSTER_DEAD))
	{
		// check for gib
		if (M_CheckGib(self, mod))
		{
			droid_gib(self);
			self->deadflag = true;
			return;
		}

		if (self->deadflag)
			return;
	}
	else
	{
		ThrowGib(self, "models/monsters/droid/weapon.md2", damage, GIB_METALLIC, self->s.scale);

		gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
		self->deadflag = true;
		self->takedamage = false;
	}

	n = irandom(3);

	if (n == 0)
		M_SetAnimation(self, &droid_move_death1);
	else if (n == 1)
		M_SetAnimation(self, &droid_move_death2);
	else if (n == 2)
		M_SetAnimation(self, &droid_move_death3);
}

//
// NEW DODGE CODE
//

MONSTERINFO_SIDESTEP(droid_sidestep) (edict_t *self) -> bool
{

	if (self->monsterinfo.active_move != &droid_move_run)
		M_SetAnimation(self, &droid_move_run);

	return true;
}

MONSTERINFO_BLOCKED(droid_blocked) (edict_t* self, float dist) -> bool
{
	// don't do anything if you're dodging
	if ((self->monsterinfo.aiflags & AI_DODGING) || (self->monsterinfo.aiflags & AI_DUCKED))
		return false;

	return blocked_checkplat(self, dist);
}

MONSTERINFO_DUCK(droid_duck) (edict_t *self, gtime_t eta) -> bool
{
	// don't duck during our firing or melee frames
	if ((self->monsterinfo.active_move == &droid_move_attack1) ||
		(self->monsterinfo.active_move == &droid_move_attack2))
	{
		self->monsterinfo.unduck(self);
		return false;
	}

	M_SetAnimation(self, &droid_move_duck);

	return true;
}

/*QUAKED monster_droid (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
 */
void SP_monster_droid(edict_t *self)
{
	const spawn_temp_t &st = ED_GetSpawnTemp();

	if( !M_AllowSpawn( self ) ) {
		G_FreeEdict( self );
		return;
	}

	self->s.modelindex = gi.modelindex("models/monsters/droid/tris.md2");
	self->s.modelindex2 = gi.modelindex("models/monsters/droid/weapon.md2");
	self->monsterinfo.scale = MODEL_SCALE;
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	sound_idle.assign("droid/droididle.wav");
	sound_sight1.assign("droid/sight.wav");
	sound_sight2.assign("droid/search.wav");
	sound_pain1.assign("droid/pain1.wav");
	sound_pain2.assign("droid/pain2.wav");
	sound_death.assign("droid/death1.wav");
	gi.soundindex("droid/solatck1.wav");

	self->health = max(1500, 1500 + 1000 * (skill->integer - 1)) * st.health_multiplier;
	self->gib_health = -500;
	self->mass = 500;
	if (!st.was_key_specified("armor_type"))
		self->monsterinfo.armor_type = IT_ARMOR_BODY;
	if (!st.was_key_specified("armor_power"))
		self->monsterinfo.armor_power = max(200, 200 + 50 * (skill->integer - 1));
	if (coop->integer)
	{
		self->health += (500 * skill->integer * (CountPlayers() - 1));
		self->monsterinfo.armor_power += (100 * skill->integer * (CountPlayers() - 1));
	}

	self->pain = droid_pain;
	self->die = droid_die;

	self->monsterinfo.stand = droid_stand;
	self->monsterinfo.walk = droid_walk;
	self->monsterinfo.run = droid_run;
	self->monsterinfo.dodge = M_MonsterDodge;
	self->monsterinfo.attack = droid_attack;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.sight = droid_sight;
	self->monsterinfo.checkattack = droid_CheckAttack;
	self->monsterinfo.setskin = droid_setskin;
	self->monsterinfo.blocked = droid_blocked;
	self->monsterinfo.duck = droid_duck;
	self->monsterinfo.unduck = monster_duck_up;
	self->monsterinfo.sidestep = droid_sidestep;

	gi.linkentity(self);

	self->monsterinfo.stand(self);

	walkmonster_start(self);
}