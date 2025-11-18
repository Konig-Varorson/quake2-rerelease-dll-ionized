// Copyright (c) ZeniMax Media Inc.
// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

TACTICAL - MALE

==============================================================================
*/

#include "../g_local.h"
#include "m_tactical.h"
#include "../m_flash.h"

static cached_soundindex sound_pain;
static cached_soundindex sound_death;
static cached_soundindex sound_idle;
static cached_soundindex sound_search;
static cached_soundindex sound_sight;
static cached_soundindex sound_jump;

constexpr spawnflags_t SPAWNFLAG_TACTICAL_NOJUMPING = 8_spawnflag;
constexpr spawnflags_t SPAWNFLAG_TACTICAL_WEAPON1 = 16_spawnflag;
constexpr spawnflags_t SPAWNFLAG_TACTICAL_WEAPON2 = 32_spawnflag;
constexpr spawnflags_t SPAWNFLAG_TACTICAL_WEAPON3 = 64_spawnflag;

//
//SOUNDS
//

MONSTERINFO_IDLE(tactm_idle) (edict_t* self) -> void
{
	if (frandom() > 0.8f)
		gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
}

MONSTERINFO_SEARCH(tactm_search) (edict_t* self) -> void
{
	if (frandom() > 0.8f)
		gi.sound(self, CHAN_VOICE, sound_search, 1, ATTN_NORM, 0);
}

MONSTERINFO_SIGHT(tactm_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

//
//STAND
//

void tactm_stand(edict_t* self);
void tactm_fidget(edict_t* self);

mframe_t tactm_frames_stand1[] = {
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand, 0, tactm_fidget },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand, 0, tactm_fidget },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand, 0, tactm_fidget },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand, 0, tactm_fidget }
};
MMOVE_T(tactm_move_stand1) = { FRAME_stand01, FRAME_stand40, tactm_frames_stand1, nullptr };

mframe_t tactm_frames_stand2[] = {
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand, 0, tactm_fidget },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand, 0, tactm_fidget },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand, 0, tactm_fidget },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand, 0, tactm_fidget }
};
MMOVE_T(tactm_move_stand2) = { FRAME_stand01, FRAME_stand40, tactm_frames_stand2, nullptr };

mframe_t tactm_frames_flip[] = {
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
MMOVE_T(tactm_move_flip) = { FRAME_flip01, FRAME_flip12, tactm_frames_flip, tactm_stand };

mframe_t tactm_frames_salute[] = {
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
MMOVE_T(tactm_move_salute) = { FRAME_salute01, FRAME_salute11, tactm_frames_salute, tactm_stand };

mframe_t tactm_frames_taunt[] = {
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
MMOVE_T(tactm_move_taunt) = { FRAME_taunt01, FRAME_taunt17, tactm_frames_taunt, tactm_stand };

mframe_t tactm_frames_wave[] = {
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
MMOVE_T(tactm_move_wave) = { FRAME_wave01, FRAME_wave11, tactm_frames_wave, tactm_stand };

mframe_t tactm_frames_point[] = {
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
MMOVE_T(tactm_move_point) = { FRAME_point01, FRAME_point12, tactm_frames_point, tactm_stand };

void tactm_fidget(edict_t* self)
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		return;
	else if (self->enemy)
		return;

	if (frandom() <= 0.01f)
	{
		if (self->monsterinfo.aiflags & AI_DUCKED)
			monster_duck_up(self); 
		M_SetAnimation(self, &tactm_move_point);
	}
	else if (frandom() <= 0.02f)
	{
		if (self->monsterinfo.aiflags & AI_DUCKED)
			monster_duck_up(self);
		M_SetAnimation(self, &tactm_move_wave);
	}
	else if (frandom() <= 0.03f)
	{
		if (self->monsterinfo.aiflags & AI_DUCKED)
			monster_duck_up(self);
		M_SetAnimation(self, &tactm_move_taunt);
	}
	else if (frandom() <= 0.04f)
	{
		if (self->monsterinfo.aiflags & AI_DUCKED)
			monster_duck_up(self);
		M_SetAnimation(self, &tactm_move_flip);
	}
	else if (frandom() <= 0.05f)
	{
		if (self->monsterinfo.aiflags & AI_DUCKED)
			monster_duck_up(self);
		M_SetAnimation(self, &tactm_move_salute);
	}
}

MONSTERINFO_STAND(tactm_stand) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_DUCKED)
		monster_duck_up(self);

	if (frandom() < 0.8f)
		M_SetAnimation(self, &tactm_move_stand1);
	else
	{
		M_SetAnimation(self, &tactm_move_stand2);
		monster_duck_down(self);
	}
}

//
//WALK
//

mframe_t tactm_frames_walk1[] = {
	{ ai_walk, 4 },
	{ ai_walk, 15, monster_footstep },
	{ ai_walk, 15 },
	{ ai_walk, 8 },
	{ ai_walk, 20, monster_footstep },
	{ ai_walk, 15 }
};
MMOVE_T(tactm_move_walk1) = { FRAME_run1, FRAME_run6, tactm_frames_walk1, nullptr };

mframe_t tactm_frames_walk2[] = {
	{ ai_walk },
	{ ai_walk, 6, monster_footstep },
	{ ai_walk, 10 },
	{ ai_walk, 3 },
	{ ai_walk, 2, monster_footstep },
	{ ai_walk, 10 }
};
MMOVE_T(tactm_move_walk2) = { FRAME_crwalk1, FRAME_crwalk6, tactm_frames_walk2, nullptr };

MONSTERINFO_WALK(tactm_walk) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_DUCKED)
		M_SetAnimation(self, &tactm_move_walk2);
	else
		M_SetAnimation(self, &tactm_move_walk1);
}

//
//RUN
//

mframe_t tactm_frames_run[] = {
	{ ai_run, 4 },
	{ ai_run, 15, monster_footstep },
	{ ai_run, 15 },
	{ ai_run, 8 },
	{ ai_run, 20, monster_footstep },
	{ ai_run, 15 }
};
MMOVE_T(tactm_move_run) = { FRAME_run1, FRAME_run6, tactm_frames_run, nullptr };

MONSTERINFO_RUN(tactm_run) (edict_t* self) -> void
{
	monster_done_dodge(self);

	if ((frandom() >= 0.8f) && (self->monsterinfo.aiflags & AI_DUCKED))
		monster_duck_up(self);

	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &tactm_move_stand1);
	else if (self->monsterinfo.aiflags & AI_DUCKED)
	{
		M_SetAnimation(self, &tactm_move_walk2);
	}
	else
		monster_duck_up(self);
		M_SetAnimation(self, &tactm_move_run);
}

//
//ATTACKS
//

void tactm_fire_chainfist(edict_t* self) //chainfist
{
	constexpr vec3_t aim = { MELEE_DISTANCE, 0, -24 };

	if (!fire_hit(self, aim, irandom(10, 20), 150))
		self->monsterinfo.melee_debounce_time = level.time + 1.2_sec;
}

void tactm_fire_shotgun(edict_t* self) //shotgun
{
	vec3_t					 start;
	vec3_t					 forward, right;
	vec3_t					 aim;
	monster_muzzleflash_id_t flash_number;

	if (!self->enemy || !self->enemy->inuse) // PGM
		return;								 // PGM

	flash_number = MZ2_ACTOR_MACHINEGUN_1;

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, monster_flash_offset[flash_number], forward, right);
	PredictAim(self, self->enemy, start, 800, false, frandom() * 0.3f, &aim, nullptr);
	for (int i = 0; i < 3; i++)
		aim[i] += crandom_open() * 0.025f;

	gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/shotgf1b.wav"), 1, ATTN_NORM, 0);
	monster_fire_shotgun(self, start, aim, 5, 1, 1500, 750, 9, flash_number);
}

void tactm_fire_nailgun(edict_t* self) //ETF Rifle
{
	vec3_t					 start;
	vec3_t					 forward, right;
	vec3_t					 aim;
	monster_muzzleflash_id_t flash_number;

	if (!self->enemy || !self->enemy->inuse) // PGM
		return;								 // PGM

	flash_number = MZ2_ACTOR_MACHINEGUN_1;

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, monster_flash_offset[flash_number], forward, right);
	PredictAim(self, self->enemy, start, 800, false, frandom() * 0.3f, &aim, nullptr);
	for (int i = 0; i < 3; i++)
		aim[i] += crandom_open() * 0.025f;

	if (!(self->monsterinfo.aiflags & AI_HOLD_FRAME))
		self->monsterinfo.fire_wait = level.time + random_time(300_ms, 1.1_sec);

	gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/nail1.wav"), 1, ATTN_NORM, 0);
	monster_fire_flechette(self, start, aim, 8, 800, flash_number);

	if (level.time >= self->monsterinfo.fire_wait)
		self->monsterinfo.aiflags &= ~AI_HOLD_FRAME;
	else
		self->monsterinfo.aiflags |= AI_HOLD_FRAME;
}

void tactm_fire_glauncher(edict_t* self) //grenade
{

	vec3_t					 start;
	vec3_t					 forward, right, up;
	vec3_t					 aim;
	monster_muzzleflash_id_t flash_number;
	float					 spread;
	float					 pitch = 0;
	vec3_t target;
	bool   blindfire = false;

	if (!self->enemy || !self->enemy->inuse)
		return;

	if (self->monsterinfo.aiflags & AI_MANUAL_STEERING)
		blindfire = true;

	if (frandom() > 0.4f)
		self->monsterinfo.aiflags &= ~AI_MANUAL_STEERING;
	spread = frandom(-0.10f, 0.10f);
	flash_number = MZ2_ACTOR_MACHINEGUN_1;
	
	if ((blindfire) && (!visible(self, self->enemy)))
	{
		if (!self->monsterinfo.blind_fire_target)
			return;

		target = self->monsterinfo.blind_fire_target;
	}
	else
		target = self->enemy->s.origin;

	AngleVectors(self->s.angles, forward, right, up);
	start = M_ProjectFlashSource(self, monster_flash_offset[flash_number], forward, right);

	if (self->enemy)
	{
		float dist;

		aim = target - self->s.origin;
		dist = aim.length();

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

	aim = forward + (right * spread);
	aim += (up * pitch);

	if (M_CalculatePitchToFire(self, target, start, aim, 600, 2.5f, false))
	{
		gi.sound(self, CHAN_WEAPON, gi.soundindex("guncmdr/gcdratck3.wav"), 1, ATTN_NORM, 0);
		monster_fire_grenade(self, start, aim, 50, 600, flash_number, (crandom_open() * 10.0f), frandom() * 10.f);
	}
	else
	{
		gi.sound(self, CHAN_WEAPON, gi.soundindex("guncmdr/gcdratck3.wav"), 1, ATTN_NORM, 0);
		monster_fire_grenade(self, start, aim, 50, 600, flash_number, (crandom_open() * 10.0f), 200.f + (crandom_open() * 10.0f));
	}
}

void tactm_fire_railgun(edict_t* self)
{
	vec3_t start;
	vec3_t dir;
	vec3_t forward, right;

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, monster_flash_offset[MZ2_ACTOR_MACHINEGUN_1], forward, right);

	dir = self->pos1 - start;
	dir.normalize();

	gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/railgf1a.wav"), 1, ATTN_NORM, 0);
	monster_fire_railgun(self, start, dir, 50, 100, MZ2_ACTOR_MACHINEGUN_1);

	self->monsterinfo.attack_finished = level.time + 3_sec;
}

void tactm_fire(edict_t* self)
{
	if (self->style == 1)
		tactm_fire_chainfist(self); 
	else if (self->style == 2)
		tactm_fire_shotgun(self);
	else if (self->style == 3)
		tactm_fire_glauncher(self);
	else if (self->style == 4)
		tactm_fire_nailgun(self);
	else if (self->style == 5)
	{
		if (level.time < self->monsterinfo.attack_finished)
			return;
		tactm_fire_railgun(self);
	}
}

void tactm_fire_check(edict_t* self)
{
	if (self->style == 1)
		tactm_fire_chainfist(self);
	else
		return;
}

void tactm_fire_check2(edict_t* self)
{
	if ((self->style == 2) || (self->style == 3))
	{
		if ((self->s.frame == FRAME_attak3) && (frandom() > 0.6f))
			self->monsterinfo.nextframe = FRAME_attak1;
		if ((self->s.frame == FRAME_crattak3) && (frandom() > 0.6f))
			self->monsterinfo.nextframe = FRAME_crattak1;
	}
	else
		return;
}

mframe_t tactm_frames_attack1[] = {
	{ ai_charge, -2, tactm_fire },
	{ ai_charge, -2, tactm_fire_check },
	{ ai_charge, 3, tactm_fire_check2 },
	{ ai_charge, 2 },
	{ ai_charge },

	{ ai_charge },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(tactm_move_attack1) = { FRAME_attak1, FRAME_attak8, tactm_frames_attack1, tactm_run };

mframe_t tactm_frames_attack2[] = {
	{ ai_charge, -2, tactm_fire },
	{ ai_charge, -2, tactm_fire_check },
	{ ai_charge, 3, tactm_fire_check2 },
	{ ai_charge, 2 },
	{ ai_charge },

	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(tactm_move_attack2) = { FRAME_crattak1, FRAME_crattak9, tactm_frames_attack2, tactm_run };

MONSTERINFO_ATTACK(tactm_attack) (edict_t* self) -> void
{
	float r, chance;
	
	if (!M_CheckClearShot(self, monster_flash_offset[MZ2_ACTOR_MACHINEGUN_1]))
		return;

	if (self->monsterinfo.aiflags & AI_DUCKED)
		monster_duck_up(self);

	monster_done_dodge(self);

	if (self->enemy->health <= 0)
	{
		M_SetAnimation(self, &tactm_move_flip);
		self->monsterinfo.aiflags &= ~AI_BRUTAL;
		return;
	}

	if (range_to(self, self->enemy) <= MELEE_DISTANCE)
	{
		self->s.modelindex2 = 0;
		self->style = 1;
		self->s.modelindex2 = gi.modelindex("players/male/w_chainfist.md2");
	}
	else if (range_to(self, self->enemy) <= (RANGE_NEAR /3))
	{
		self->s.modelindex2 = 0;
		self->style = 2;
		self->s.modelindex2 = gi.modelindex("players/male/w_shotgun.md2");
	}
	else if (range_to(self, self->enemy) <= (RANGE_MID /3))
	{
		self->s.modelindex2 = 0;
		self->style = 3;
		self->s.modelindex2 = gi.modelindex("players/male/w_glauncher.md2");
	}
	else if (range_to(self, self->enemy) <= (RANGE_MID))
	{
		self->s.modelindex2 = 0;
		self->style = 4;
		self->s.modelindex2 = gi.modelindex("players/male/w_etfrifle.md2");
	}
	else if (range_to(self, self->enemy) <= RANGE_MID)
	{
		self->s.modelindex2 = 0;
		self->style = 5;
		self->s.modelindex2 = gi.modelindex("players/male/w_railgun.md2");
	}

	if (self->monsterinfo.attack_state == AS_BLIND)
	{
		if (self->monsterinfo.blind_fire_delay < 1.0_sec)
			chance = 1.0;
		else if (self->monsterinfo.blind_fire_delay < 7.5_sec)
			chance = 0.4f;
		else
			chance = 0.1f;

		r = frandom();

		self->monsterinfo.blind_fire_delay += random_time(3.5_sec, 6.5_sec);

		if (!self->monsterinfo.blind_fire_target)
			return;

		if (r > chance)
			return;

		self->monsterinfo.aiflags |= AI_MANUAL_STEERING;
		M_SetAnimation(self, &tactm_move_attack1);
		self->monsterinfo.attack_finished = level.time + random_time(2_sec);
		return;
	}

	if (frandom() >= 0.4f)
		M_SetAnimation(self, &tactm_move_attack1);
	else
	{
		M_SetAnimation(self, &tactm_move_attack2);
		monster_duck_down(self);
	}
}

//
//BLOCKED and JUMPING
//

void tactm_jump_now(edict_t* self)
{
	vec3_t forward, up;

	AngleVectors(self->s.angles, forward, nullptr, up);
	self->velocity += (forward * 100);
	self->velocity += (up * 300);
}

void tactm_jump2_now(edict_t* self)
{
	vec3_t forward, up;

	AngleVectors(self->s.angles, forward, nullptr, up);
	self->velocity += (forward * 150);
	self->velocity += (up * 400);
}

void tactm_jump_wait_land(edict_t* self)
{
	if (self->groundentity == nullptr)
	{
		self->monsterinfo.nextframe = self->s.frame;

		if (monster_jump_finished(self))
			self->monsterinfo.nextframe = self->s.frame + 1;
	}
	else
		self->monsterinfo.nextframe = self->s.frame + 1;
}

mframe_t tactm_frames_jump[] = {
	{ ai_move, 0, tactm_jump_now },
	{ ai_move },
	{ ai_move, 0, tactm_jump_wait_land },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(tactm_move_jump) = { FRAME_jump1, FRAME_jump6, tactm_frames_jump, tactm_run };

mframe_t tactm_frames_jump2[] = {
	{ ai_move, -8, tactm_jump2_now },
	{ ai_move, -4 },
	{ ai_move, -4, tactm_jump_wait_land },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(tactm_move_jump2) = { FRAME_jump1, FRAME_jump6, tactm_frames_jump2, tactm_run };

void tactm_jump(edict_t* self, blocked_jump_result_t result)
{
	if (!self->enemy)
		return;

	monster_done_dodge(self);

	gi.sound(self, CHAN_VOICE, sound_jump, 1, ATTN_NORM, 0);

	if (result == blocked_jump_result_t::JUMP_JUMP_UP)
		M_SetAnimation(self, &tactm_move_jump2);
	else
		M_SetAnimation(self, &tactm_move_jump);
}

MONSTERINFO_BLOCKED(tactm_blocked) (edict_t* self, float dist) -> bool
{
	if (blocked_checkplat(self, dist))
		return true;

	if (auto result = blocked_checkjump(self, dist); result != blocked_jump_result_t::NO_JUMP)
	{
		if (result != blocked_jump_result_t::JUMP_TURN)
			tactm_jump(self, result);

		return true;
	}

	return false;
}

//
//DODGES
//

MONSTERINFO_DUCK(tactm_duck) (edict_t* self, gtime_t eta) -> bool
{
	if (self->monsterinfo.active_move == &tactm_move_attack1)
	{
		self->monsterinfo.unduck(self);
		return false;
	}

	M_SetAnimation(self, &tactm_move_attack2);
	monster_duck_down(self);

	return true;
}

MONSTERINFO_SIDESTEP(tactm_sidestep) (edict_t* self) -> bool
{
	if ((self->monsterinfo.active_move == &tactm_move_attack1) ||
		(self->monsterinfo.active_move == &tactm_move_attack2))
	{
		return false;
	}


	if (self->monsterinfo.aiflags & AI_DUCKED)
		M_SetAnimation(self, &tactm_move_walk2);
	else if (self->monsterinfo.active_move != &tactm_move_run)
		M_SetAnimation(self, &tactm_move_run);
	else
		M_SetAnimation(self, &tactm_move_jump);

	return true;
}

//
//PAIN
//

mframe_t tactm_frames_pain1[] = {
	{ ai_move, -5 },
	{ ai_move, 4 },
	{ ai_move, 1 },
	{ ai_move }
};
MMOVE_T(tactm_move_pain1) = { FRAME_pain101, FRAME_pain104, tactm_frames_pain1, tactm_run };

mframe_t tactm_frames_pain2[] = {
	{ ai_move, -4 },
	{ ai_move, 4 },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(tactm_move_pain2) = { FRAME_pain201, FRAME_pain204, tactm_frames_pain2, tactm_run };

mframe_t tactm_frames_pain3[] = {
	{ ai_move, -1 },
	{ ai_move, 1 },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(tactm_move_pain3) = { FRAME_pain301, FRAME_pain304, tactm_frames_pain3, tactm_run };

mframe_t tactm_frames_pain4[] = {
	{ ai_move, -5 },
	{ ai_move, 4 },
	{ ai_move, 1 },
	{ ai_move }
};
MMOVE_T(tactm_move_pain4) = { FRAME_crpain1, FRAME_crpain4, tactm_frames_pain4, tactm_run };

PAIN(tactm_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (self->monsterinfo.active_move == &tactm_move_jump)
		return;

	if (!M_ShouldReactToPain(self, mod))
	{
		if (frandom() < 0.3)
			self->monsterinfo.dodge(self, other, FRAME_TIME_S, nullptr, false);

		return; // no pain anims in nightmare
	}


	gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

	if (self->monsterinfo.aiflags & AI_DUCKED)
		M_SetAnimation(self, &tactm_move_pain4);
	else if (damage < 35)
		M_SetAnimation(self, &tactm_move_pain1);
	else if (damage < 50)
		M_SetAnimation(self, &tactm_move_pain2);
	else
		M_SetAnimation(self, &tactm_move_pain3);
}

MONSTERINFO_SETSKIN(tactm_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
//DEATH
//

void tactm_dead(edict_t* self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, -8 };
	monster_dead(self);
}

static void tactm_shrink(edict_t* self)
{
	self->maxs[2] = -4;
	self->svflags |= SVF_DEADMONSTER;
	gi.linkentity(self);
}

mframe_t tactm_frames_death1[] = {
	{ ai_move, -13 },
	{ ai_move, 14 },
	{ ai_move, 3 },
	{ ai_move, -2, tactm_shrink },
	{ ai_move, 1 },
	{ ai_move }
};
MMOVE_T(tactm_move_death1) = { FRAME_death101, FRAME_death106, tactm_frames_death1, tactm_dead };

mframe_t tactm_frames_death2[] = {
	{ ai_move, -13 },
	{ ai_move, 14 },
	{ ai_move, 3, tactm_shrink },
	{ ai_move, -2 },
	{ ai_move, 1 },
	{ ai_move }
};
MMOVE_T(tactm_move_death2) = { FRAME_death201, FRAME_death206, tactm_frames_death2, tactm_dead };

mframe_t tactm_frames_death3[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },

	{ ai_move, 0 , tactm_shrink },
	{ ai_move },
	{ ai_move },
};
MMOVE_T(tactm_move_death3) = { FRAME_death301, FRAME_death308, tactm_frames_death3, tactm_dead };

mframe_t tactm_frames_death4[] = {
	{ ai_move, -13 },
	{ ai_move, 14 },
	{ ai_move, 3 },
	{ ai_move, -2, tactm_shrink },
	{ ai_move }
};
MMOVE_T(tactm_move_death4) = { FRAME_crdeath1, FRAME_crdeath5, tactm_frames_death4, tactm_dead };

DIE(tactm_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	self->s.modelindex2 = 0;

	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/arm/tris.md2" },
			{ 2, "models/objects/gibs/leg/tris.md2" },
			{ 2, "models/objects/gibs/sm_meat/tris.md2" },
			{ "models/objects/gibs/sm_metal/tris.md2", GIB_METALLIC },
			{ "models/objects/gibs/chest/tris.md2" },
			{ "models/objects/gibs/head/tris.md2", GIB_HEAD }
			});
		self->deadflag = true;

		return;
	}

	if (self->deadflag)
		return;

	self->deadflag = true;
	self->takedamage = true;

	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);

	if (self->monsterinfo.aiflags & AI_DUCKED)
	{
		M_SetAnimation(self, &tactm_move_death4);
	}
	if (frandom() < 0.33f)
	{
		M_SetAnimation(self, &tactm_move_death1);
	}
	else if (frandom() < 0.66f)
	{
		M_SetAnimation(self, &tactm_move_death2);
	}
	else
	{
		M_SetAnimation(self, &tactm_move_death3);
	}
}

/*QUAKED monster_tactical_male (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight NoJumping
 */
void SP_monster_tactical_male(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self))
	{
		G_FreeEdict(self);
		return;
	}

	sound_death.assign("tactical/death.wav");
	sound_pain.assign("tactical/pain.wav");
	sound_idle.assign("tactical/idle.wav");
	sound_search.assign("tactical/search.wav");
	sound_sight.assign("tactical/sight.wav");
	sound_jump.assign("tactical/jump.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;
	self->s.modelindex = gi.modelindex("models/monsters/tactical/male/tris.md2");
	self->s.modelindex2 = gi.modelindex("players/male/w_shotgun.md2");

	self->mins = vec3_t { -16, -16, -24 };
	self->maxs = vec3_t { 16, 16, 32 };

	self->health = 350 * st.health_multiplier;
	self->gib_health = -70;
	self->mass = 200;

	if (!st.was_key_specified("armor_type"))
		self->monsterinfo.armor_type = IT_ARMOR_COMBAT;
	if (!st.was_key_specified("armor_power"))
		self->monsterinfo.armor_power = 100;
	if (!st.was_key_specified("power_armor_type"))
		self->monsterinfo.power_armor_type = IT_ITEM_POWER_SCREEN;
	if (!st.was_key_specified("power_armor_power"))
		self->monsterinfo.power_armor_power = 100;

	self->pain = tactm_pain;
	self->die = tactm_die;

	self->monsterinfo.stand = tactm_stand;
	self->monsterinfo.walk = tactm_walk;
	self->monsterinfo.run = tactm_run;

	self->monsterinfo.dodge = M_MonsterDodge;
	self->monsterinfo.duck = tactm_duck;
	self->monsterinfo.unduck = monster_duck_up;
	self->monsterinfo.sidestep = tactm_sidestep;
	self->monsterinfo.blocked = tactm_blocked;

	self->monsterinfo.attack = tactm_attack;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.sight = tactm_sight;
	self->monsterinfo.search = tactm_search;
	self->monsterinfo.setskin = tactm_setskin;
	
	self->monsterinfo.aiflags |= ( AI_STINKY | AI_IGNORE_SHOTS );
	self->monsterinfo.blindfire = true;
	self->monsterinfo.can_jump = !self->spawnflags.has(SPAWNFLAG_TACTICAL_NOJUMPING);
	self->monsterinfo.drop_height = 192;
	self->monsterinfo.jump_height = 40;

	M_SetAnimation(self, &tactm_move_stand1);
	self->monsterinfo.scale = MODEL_SCALE;

	walkmonster_start(self);

}
