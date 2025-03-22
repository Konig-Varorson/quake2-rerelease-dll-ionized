// Copyright (c) ZeniMax Media Inc.
// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

SENTIEN

==============================================================================
*/

#include "g_local.h"
#include "m_sentien.h"
#include "m_flash.h"

void target_laser_think(edict_t* self);
void target_laser_on(edict_t* self);
void target_laser_off(edict_t* self);

static cached_soundindex sound_fend;
static cached_soundindex sound_pain1, sound_pain2, sound_pain3;
static cached_soundindex sound_idle1, sound_idle2, sound_idle3;
static cached_soundindex sound_die1, sound_die2;
static cached_soundindex sound_step;
static cached_soundindex sound_sight;
static cached_soundindex sound_attack1, sound_attack2, sound_attack3;
static cached_soundindex sound_strike;

//
// misc
//

void sentien_run(edict_t* self);
void sentien_walk(edict_t* self);
void sentien_dead(edict_t* self);
void sentien_machinegun(edict_t* self);
void sentien_blast_attack(edict_t* self);
void sentien_post_blast_attack(edict_t* self);
void sentien_laser_attack(edict_t* self);
void sentien_post_laser_attack(edict_t* self);

MONSTERINFO_SIGHT(sentien_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

void sentien_step(edict_t* self)
{
	gi.sound(self, CHAN_BODY, sound_step, 1, ATTN_NORM, 0);
}

MONSTERINFO_IDLE(sentien_idle) (edict_t* self) -> void
{
	int n = irandom(3);
	if (n == 0)
		gi.sound(self, CHAN_BODY, sound_idle1, 1, ATTN_NORM, 0);
	else if (n == 1)
		gi.sound(self, CHAN_BODY, sound_idle2, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_BODY, sound_idle3, 1, ATTN_NORM, 0);
}

void sentien_sound_attack1(edict_t* self)
{
	gi.sound(self, CHAN_BODY, sound_attack1, 1, ATTN_NORM, 0);
}

void sentien_sound_attack2(edict_t* self)
{
	gi.sound(self, CHAN_BODY, sound_attack2, 1, ATTN_NORM, 0);
}

void sentien_sound_attack3(edict_t* self)
{
	gi.sound(self, CHAN_BODY, sound_attack3, 1, ATTN_NORM, 0);
}

void sentien_sound_fend(edict_t* self)
{
	gi.sound(self, CHAN_BODY, sound_fend, 1, ATTN_NORM, 0);
}

void sentien_sound_die1(edict_t* self)
{
	gi.sound(self, CHAN_BODY, sound_die1, 1, ATTN_NORM, 0);
}

void sentien_sound_die2(edict_t* self)
{
	gi.sound(self, CHAN_BODY, sound_die2, 1, ATTN_NORM, 0);
}

//
// stand
//

mframe_t sentien_frames_stand1[] =
{
   {ai_stand, 0, sentien_idle},
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
MMOVE_T(sentien_move_stand1) = { FRAME_idle101, FRAME_idle129, sentien_frames_stand1, nullptr };

mframe_t sentien_frames_stand2[] =
{
   {ai_stand, 0, sentien_idle},
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
MMOVE_T(sentien_move_stand2) = { FRAME_idle201, FRAME_idle220, sentien_frames_stand2, nullptr };

mframe_t sentien_frames_stand3[] =
{
	{ai_stand, 0, sentien_idle},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}, //10

	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}, //20

	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}, //30

	{ai_stand}
};
MMOVE_T(sentien_move_stand3) = { FRAME_idle301, FRAME_idle331, sentien_frames_stand3, nullptr };

MONSTERINFO_STAND(sentien_stand) (edict_t* self) -> void
{
	float r;

	r = frandom();
	if (r < 0.33f)
		M_SetAnimation(self, &sentien_move_stand1);
	else if (r < 0.66f)
		M_SetAnimation(self, &sentien_move_stand2);
	else
		M_SetAnimation(self, &sentien_move_stand3);
}

//
// walk
//

mframe_t sentien_frames_start_walk[] =
{
   {ai_walk},
   {ai_walk, 1.5},
   {ai_walk, 2.9},
   {ai_walk, 2.4},
   {ai_walk, 2.1},
   {ai_walk, 2.6},
   {ai_walk, 2.1},
   {ai_walk, 1.8, sentien_step}
};
MMOVE_T(sentien_move_start_walk) = { FRAME_walk01, FRAME_walk08, sentien_frames_start_walk, sentien_walk };

mframe_t sentien_frames_walk[] =
{
   {ai_walk, 0.3},
   {ai_walk, 2.4},
   {ai_walk, 4.0},
   {ai_walk, 3.5},
   {ai_walk, 3.6},
   {ai_walk, 3.7 * 1.1},
   {ai_walk, 3.1 * 1.3},
   {ai_walk, 4.1 * 1.2, sentien_step},

   {ai_walk, 2.0},
   {ai_walk, 2.6}, // 2.4
   {ai_walk, 3.8}, // 3.9
   {ai_walk, 3.6},
   {ai_walk, 3.6},
   {ai_walk, 4.3},
   {ai_walk, 4.2 * 1.2},
   {ai_walk, 5.2, sentien_step} // 4.1
};
MMOVE_T(sentien_move_walk) = { FRAME_walk09, FRAME_walk24, sentien_frames_walk, nullptr };
#if 0
mframe_t sentien_frames_end_walk[] =
{
   {ai_walk, 0.8},
   {ai_walk, 1.0},
   {ai_walk, 1.6},
   {ai_walk, 1.4},
   {ai_walk, 1.5},
   {ai_walk, 1.4},
   {ai_walk, 1.5},
   {ai_walk, 1.8, sentien_step}
};
MMOVE_T(sentien_move_end_walk) = { FRAME_walk25, FRAME_walk32, sentien_frames_end_walk, sentien_stand };
#endif
MONSTERINFO_WALK(sentien_walk) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
	{
		M_SetAnimation(self, &sentien_move_stand1);
		return;
	}

	if (self->monsterinfo.active_move == &sentien_move_start_walk)
	{
		M_SetAnimation(self, &sentien_move_walk);
	}
	else
	{
		M_SetAnimation(self, &sentien_move_start_walk);
	}
}

//
// run
//

mframe_t sentien_frames_start_run[] =
{
   {ai_run},
   {ai_run, 1.5},
   {ai_run, 2.9},
   {ai_run, 2.4},
   {ai_run, 2.1},
   {ai_run, 2.6},
   {ai_run, 2.1},
   {ai_run, 1.8, sentien_step}
};
MMOVE_T(sentien_move_start_run) = { FRAME_walk01, FRAME_walk08, sentien_frames_start_run, sentien_run };

mframe_t sentien_frames_run[] =
{
   {ai_run, 0.3 * 1.2},
   {ai_run, 2.4},
   {ai_run, 4.0},
   {ai_run, 3.5},
   {ai_run, 3.6},
   {ai_run, 3.7 * 1.1},
   {ai_run, 3.1 * 1.3},
   {ai_run, 4.1 * 1.2, sentien_step},

   {ai_run, 2.0},
   {ai_run, 2.6}, // 2.4
   {ai_run, 3.8}, // 3.9
   {ai_run, 3.6},
   {ai_run, 3.6},
   {ai_run, 4.3},
   {ai_run, 4.2 * 1.2},
   {ai_run, 5.2, sentien_step} // 4.1
};
MMOVE_T(sentien_move_run) = { FRAME_walk09, FRAME_walk24, sentien_frames_run, nullptr };
#if 0
mframe_t sentien_frames_run_end[] =
{
   {ai_run, 0.8},
   {ai_run, 1.0},
   {ai_run, 1.6},
   {ai_run, 1.4},
   {ai_run, 1.5},
   {ai_run, 1.4},
   {ai_run, 1.5},
   {ai_run, 1.8, sentien_step}
};
MMOVE_T(sentien_move_run_end) = { FRAME_walk25, FRAME_walk32, sentien_frames_run_end, sentien_walk };
#endif
MONSTERINFO_RUN(sentien_run) (edict_t* self) -> void
{
	if (self->enemy && self->enemy->client)
		self->monsterinfo.aiflags |= AI_BRUTAL;
	else
		self->monsterinfo.aiflags &= ~AI_BRUTAL;

	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
	{
		M_SetAnimation(self, &sentien_move_stand1);
		return;
	}

	if (self->monsterinfo.active_move == &sentien_move_walk ||
		self->monsterinfo.active_move == &sentien_move_start_run)
	{
		M_SetAnimation(self, &sentien_move_run);
	}
	else
	{
		M_SetAnimation(self, &sentien_move_start_run);
	}
}

//
// pain
//

mframe_t sentien_frames_pain1[] =
{
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move}
};
MMOVE_T(sentien_move_pain1) = { FRAME_pain101, FRAME_pain104, sentien_frames_pain1, sentien_run };

mframe_t sentien_frames_pain2[] =
{
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move}
};
MMOVE_T(sentien_move_pain2) = { FRAME_pain201, FRAME_pain206, sentien_frames_pain2, sentien_run };

mframe_t sentien_frames_pain3[] =
{
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move}
};
MMOVE_T(sentien_move_pain3) = { FRAME_pain301, FRAME_pain321, sentien_frames_pain3, sentien_run };

PAIN(sentien_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	float r;

	if (mod.id != MOD_CHAINFIST && damage <= 10)
		return;

	if (level.time < self->pain_debounce_time)
		return;

	// less than this we don't flinch
	if (damage <= 10)
		return;

	if (mod.id != MOD_CHAINFIST)
	{
		if (damage <= 30)
			if (frandom() > 0.2f)
				return;

		// don't go into pain while attacking
		if (skill->integer >= 1)
		{
			if ((self->s.frame >= FRAME_attak301) && (self->s.frame <= FRAME_attak322))
				return;
			if ((self->s.frame >= FRAME_attak101) && (self->s.frame <= FRAME_attak114))
				return;
		}
	}

	self->pain_debounce_time = level.time + 3_sec;

	r = frandom();
	if (r < 0.33f)
		gi.sound(self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
	else if (r < 0.66f)
		gi.sound(self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_pain3, 1, ATTN_NORM, 0);

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	if (damage > 60)
		M_SetAnimation(self, &sentien_move_pain3);
	else if (damage > 30)
		M_SetAnimation(self, &sentien_move_pain2);
	else
		M_SetAnimation(self, &sentien_move_pain1);
}

MONSTERINFO_SETSKIN(sentien_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
// attacks
//

mframe_t sentien_frames_pre_blast_attack[] =
{
   {ai_charge},
   {ai_charge},
   {ai_charge},
   {ai_charge}
};
MMOVE_T(sentien_move_pre_blast_attack) = { FRAME_attak101, FRAME_attak104, sentien_frames_pre_blast_attack, sentien_blast_attack };

mframe_t sentien_frames_blast_attack[] =
{
   {ai_charge, 0, sentien_machinegun},
   {ai_charge, 0, sentien_machinegun},
   {ai_charge, 0, sentien_machinegun},
   {ai_charge, 0, sentien_machinegun},
   {ai_charge, 0, sentien_machinegun},
   {ai_charge, 0, sentien_machinegun}
};
MMOVE_T(sentien_move_blast_attack) = { FRAME_attak105, FRAME_attak110, sentien_frames_blast_attack, sentien_post_blast_attack };

mframe_t sentien_frames_post_blast_attack[] =
{
   {ai_charge},
   {ai_charge},
   {ai_charge},
   {ai_charge}
};
MMOVE_T(sentien_move_post_blast_attack) = { FRAME_attak111, FRAME_attak114, sentien_frames_post_blast_attack, sentien_run };

void sentien_blast_attack(edict_t* self)
{
	if (self->monsterinfo.aiflags & AI_MANUAL_STEERING)
	{
		self->monsterinfo.aiflags &= ~AI_MANUAL_STEERING;
		M_SetAnimation(self, &sentien_move_post_blast_attack);
		return;
	}

	if (visible(self, self->enemy))
		if (self->enemy->health > 0)
			if (frandom() <= 0.6f)
			{
				M_SetAnimation(self, &sentien_move_blast_attack);
				return;
			}
	M_SetAnimation(self, &sentien_move_post_blast_attack);
}

void sentien_post_blast_attack(edict_t* self)
{
	float refire = 0.25;
	float r;

	if (visible(self, self->enemy) &&
		infront(self, self->enemy))
	{
		if (skill->integer == 1)
			refire = 0.40;
		else if (skill->integer == 2)
			refire = 0.60;
		else if (skill->integer >= 3)
			refire = 0.75;

		r = frandom();
		if (r > refire)
			M_SetAnimation(self, &sentien_move_post_blast_attack);
	}
	else
		M_SetAnimation(self, &sentien_move_post_blast_attack);
}

vec3_t sentien_flash_offset[] =
{
	// frames 116+ (hex fire)
	 {23.7, 25.4, 29.6},
	 {23.7, 25.3, 26.7},
	 {23.7, 27.7, 28.1},
	 {23.7, 27.4, 31.2},
	 {23.7, 24.9, 32.3},
	 {23.7, 22.5, 30.6},
	 {23.7, 22.7, 27.8}
};

void sentien_machinegun(edict_t* self)
{
	vec3_t					 start;
	vec3_t					 aim;
	vec3_t					 forward, right;
	monster_muzzleflash_id_t flash_number;

	if (!self->enemy || !self->enemy->inuse) // PGM
		return;								 // PGM
	
	flash_number = static_cast<monster_muzzleflash_id_t>(MZ2_TANK_MACHINEGUN_1 + (self->s.frame - FRAME_attak105));
	
	AngleVectors(self->s.angles, forward, right, nullptr);

	start = M_ProjectFlashSource(self, monster_flash_offset[flash_number], forward, right);

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, monster_flash_offset[flash_number], forward, right);
	PredictAim(self, self->enemy, start, 0, true, -0.2f, &aim, nullptr);
	
	monster_fire_bullet(self, start, aim, 5, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, flash_number);

	sentien_sound_attack1(self);
}


/*=========================================================================
   Sentien laser attack.
  =========================================================================*/
void sentien_do_laser(edict_t* self);

mframe_t sentien_frames_pre_laser_attack[] =
{
   {ai_charge},
   {ai_charge},
   {ai_charge},
   {ai_charge},
   {ai_charge}
};
MMOVE_T(sentien_move_pre_laser_attack) = { FRAME_attak201, FRAME_attak205, sentien_frames_pre_laser_attack, sentien_laser_attack };

mframe_t sentien_frames_laser_attack[] =
{
		{nullptr, 0, sentien_do_laser},
		{nullptr, 0, sentien_do_laser},
		{nullptr, 0, sentien_do_laser},
		{nullptr, 0, sentien_do_laser},
		{nullptr, 0, sentien_do_laser},
		{nullptr, 0, sentien_do_laser},
		{nullptr, 0, sentien_do_laser},
		{nullptr, 0, sentien_do_laser},
		{nullptr, 0, sentien_do_laser},
		{nullptr, 0, sentien_do_laser},
		{nullptr, 0, sentien_do_laser}
};
MMOVE_T(sentien_move_laser_attack) = { FRAME_attak206, FRAME_attak216, sentien_frames_laser_attack, sentien_post_laser_attack };

mframe_t sentien_frames_post_laser_attack[] =
{
   {ai_charge},
   {ai_charge},
   {ai_charge},
   {ai_charge}
};
MMOVE_T(sentien_move_post_laser_attack) = { FRAME_attak217, FRAME_attak220, sentien_frames_post_laser_attack, sentien_run };

void sentien_laser_attack(edict_t* self)
{
	// is a player right infront?
	if (visible(self, self->enemy) &&
		infront(self, self->enemy))
	{
		M_SetAnimation(self, &sentien_move_laser_attack);
	}
	else
	{
		sentien_post_laser_attack(self);
	}
}

void sentien_post_laser_attack(edict_t* self)
{
	M_SetAnimation(self, &sentien_move_post_laser_attack);
}

vec3_t sentien_laser_offset[] =
{
	{43.8, -22.8 + 1, 43.6 - 0.8},
	{44.2, -22.9 + 1, 43.9 - 0.8},
	{43.9, -22.8 + 1, 44.0 - 0.8},
	{43.2, -23.0 + 1, 44.0 - 0.8},
	{42.4, -23.4 + 1, 43.9 - 0.8},
	{42.0, -23.5 + 1, 44.0 - 0.8},
	{42.4, -23.3 + 1, 44.0 - 0.8},
	{43.1, -23.1 + 1, 43.9 - 0.8},
	{43.8, -22.9 + 1, 43.9 - 0.8},
	{44.2, -22.8 + 1, 44.1 - 0.8},
	{43.8, -22.8 + 1, 43.5 - 0.8}
};

PRETHINK(sentien_fire_update) (edict_t* laser) -> void
{
	if (!laser->spawnflags.has(SPAWNFLAG_DABEAM_SPAWNED))
	{
		edict_t* self = laser->owner;

		vec3_t forward, right, target;
		vec3_t start;

		AngleVectors(self->s.angles, forward, right, nullptr);
		start = M_ProjectFlashSource(self, sentien_laser_offset[laser->spawnflags.has(SPAWNFLAG_DABEAM_SECONDARY) ? 1 : 0], forward, right);
		PredictAim(self, self->enemy, start, 0, false, 0.3f, &forward, &target);

		laser->s.origin = start;
		forward[0] += crandom() * 0.02f;
		forward[1] += crandom() * 0.02f;
		forward.normalize();
		laser->movedir = forward;
		gi.linkentity(laser);
	}
	dabeam_update(laser, false);
}

void sentien_do_laser(edict_t* self)
{
	sentien_sound_attack2(self);
	monster_fire_dabeam(self, 15, self->s.frame & 1, sentien_fire_update);
}

MONSTERINFO_ATTACK(sentien_attack) (edict_t* self) -> void
{
	vec3_t	vec;
	float	range;
	float	r;

	vec = self->enemy->s.origin - self->s.origin;
	range = vec.length();

	r = frandom();

	if (range <= 128)
		M_SetAnimation(self, &sentien_move_pre_blast_attack);
	else if (range <= 500)
	{
		if (r < 0.50)
			M_SetAnimation(self, &sentien_move_pre_blast_attack);
		else
			M_SetAnimation(self, &sentien_move_pre_laser_attack);
	}
	else
	{
		if (r < 0.25)
			M_SetAnimation(self, &sentien_move_pre_blast_attack);
		else
			M_SetAnimation(self, &sentien_move_pre_laser_attack);
	}
}


/*=========================================================================
   Sentien fending.
  =========================================================================*/
void sentien_fend_ready(edict_t* self)
{
	if (self->monsterinfo.aiflags & AI_REDUCEDDAMAGE)
		return;
	self->monsterinfo.pausetime = level.time + 1_sec;
}

void sentien_fend_hold(edict_t* self)
{
	if (level.time >= self->monsterinfo.pausetime)
	{
		self->monsterinfo.aiflags &= ~AI_HOLD_FRAME;
		self->monsterinfo.aiflags &= ~AI_REDUCEDDAMAGE;
	}
	else
	{
		self->monsterinfo.aiflags |= (AI_HOLD_FRAME | AI_REDUCEDDAMAGE);
	}
}


mframe_t sentien_frames_fend[] =
{
	{ai_move, 0,  sentien_sound_fend},
	{ai_move, 0},
	{ai_move, 0},
	{ai_move, 0},
	{ai_move, 0,  sentien_fend_ready},
	{ai_move, 0,  sentien_fend_hold},
	{ai_move, 0},
	{ai_move, 0},
	{ai_move, 0},
	{ai_move, 0},
	{ai_move, 0},
	{ai_move, 0},
	{ai_move, 0},
	{ai_move, 0},
	{ai_move, 0}
};
MMOVE_T(sentien_move_fend) = { FRAME_fend01, FRAME_fend15, sentien_frames_fend, sentien_run };

void sentien_fend(edict_t* self, edict_t* attacker, float eta)
{
	// don't flinch if attacking
	if (self->monsterinfo.active_move == &sentien_move_laser_attack ||
		self->monsterinfo.active_move == &sentien_move_blast_attack)
		return;

	if (skill->integer == 0)
	{
		if (frandom() > 0.45)
			return;
	}
	else if (skill->integer == 1)
	{
		if (frandom() > 0.60)
			return;
	}
	else
	{
		if (frandom() > 0.80)
			return;
	}

	if (!self->enemy)
		self->enemy = attacker;

	M_SetAnimation(self, &sentien_move_fend);
}

//
// DEATH
//

void sentien_dead(edict_t* self)
{
	self->mins = { -16, -16, -16 };
	self->maxs = { 16, 16, -0 };
	monster_dead(self);
}

static void sentien_shrink(edict_t* self)
{
	self->maxs[2] = 0;
	self->svflags |= SVF_DEADMONSTER;
	gi.linkentity(self);
}

mframe_t sentien_frames_death1[] =
{
   {ai_move, 0,  sentien_sound_die1},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move, 0, sentien_shrink},
   {ai_move}
};
MMOVE_T(sentien_move_death1) = { FRAME_death101, FRAME_death128, sentien_frames_death1, sentien_dead };

mframe_t sentien_frames_death2[] =
{
   {ai_move, 0,  sentien_sound_die2},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move, 0, sentien_shrink},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move},
   {ai_move}
};
MMOVE_T(sentien_move_death2) = { FRAME_death201, FRAME_death229, sentien_frames_death2, sentien_dead };


DIE(sentien_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ "models/objects/gibs/sm_meat/tris.md2" },
			{ 3, "models/objects/gibs/sm_metal/tris.md2", GIB_METALLIC },
			{ "models/objects/gibs/gear/tris.md2", GIB_METALLIC },
			//{ 2, "models/monsters/sentien/gibs/leg.md2", GIB_SKINNED | GIB_METALLIC },
			//{ "models/monsters/sentien/gibs/chest.md2", GIB_SKINNED },
			{ "models/objects/gibs/chest/tris.md2", GIB_SKINNED },
			//{ "models/monsters/sentien/gibs/head.md2", GIB_SKINNED | GIB_HEAD }
			{ "models/objects/gibs/head2/tris.md2", GIB_SKINNED | GIB_HEAD }
			});

		if (!self->style)
			ThrowGib(self, "models/monsters/tank/gibs/barm.md2", damage, GIB_SKINNED | GIB_UPRIGHT, self->s.scale);

		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	self->deadflag = true;
	self->takedamage = true;

	if (frandom() < 0.80)
		M_SetAnimation(self, &sentien_move_death1);
	else
			M_SetAnimation(self, &sentien_move_death2);
}


/*=========================================================================
   Spawn code.
  =========================================================================*/
void SP_monster_sentien(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	self->s.modelindex = gi.modelindex("models/monsters/sentien/tris.md2");
	self->mins = { -28, -28, -16 };
	self->maxs = { 28, 28, 64 };
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	sound_idle1.assign("monsters/sentien/sen_idle1.wav");
	sound_idle2.assign("monsters/sentien/sen_idle2.wav");
	sound_idle3.assign("monsters/sentien/sen_idle3.wav");
	sound_step.assign("monsters/sentien/sen_walk.wav");
	sound_fend.assign("monsters/sentien/sen_fend.wav");
	sound_pain1.assign("monsters/sentien/sen_pain1.wav");
	sound_pain2.assign("monsters/sentien/sen_pain2.wav");
	sound_pain3.assign("monsters/sentien/sen_pain3.wav");
	sound_die1.assign("monsters/sentien/sen_die1.wav");
	sound_die2.assign("monsters/sentien/sen_die2.wav");
	sound_attack1.assign("monsters/sentien/sen_att1.wav");
	sound_attack2.assign("monsters/sentien/sen_att2.wav");
	sound_attack3.assign("monsters/sentien/sen_att3.wav");
	sound_sight.assign("monsters/tank/sight1.wav");

	self->mass = 500;
	self->health = 900;
	self->gib_health = -425;
	self->yaw_speed = 10;
	self->random = 1;

	if (!st.was_key_specified("armor_type"))
		self->monsterinfo.armor_type = IT_ARMOR_COMBAT;
	if (!st.was_key_specified("armor_power"))
		self->monsterinfo.armor_power = 200;

	self->monsterinfo.scale = MODEL_SCALE;

	// setup the functions
	self->pain = sentien_pain;
	self->die = sentien_die;
	self->monsterinfo.stand = sentien_stand;
	self->monsterinfo.walk = sentien_walk;
	self->monsterinfo.run = sentien_run;
	self->monsterinfo.attack = sentien_attack;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.sight = nullptr;
	self->monsterinfo.idle = nullptr;

	self->monsterinfo.aiflags |= AI_IGNORE_SHOTS;

	gi.linkentity(self);

	M_SetAnimation(self, &sentien_move_stand1);
	self->monsterinfo.scale = MODEL_SCALE;

	walkmonster_start(self);
}