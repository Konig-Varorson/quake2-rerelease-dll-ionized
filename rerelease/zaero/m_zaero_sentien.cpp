// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

SENTIEN

==============================================================================
*/

#include "../g_local.h"
#include "m_zaero_sentien.h"
#include "../m_flash.h"

static cached_soundindex sound_fend;
static cached_soundindex sound_pain1, sound_pain2, sound_pain3;
static cached_soundindex sound_idle1, sound_idle2, sound_idle3;
static cached_soundindex sound_die1, sound_die2;
static cached_soundindex sound_step;
static cached_soundindex sound_sight;
static cached_soundindex sound_attack1, sound_attack2;
static cached_soundindex sound_strike;

void sentien_stand_whatnow(edict_t* self);
void sentien_stand_earwax(edict_t* self);
void sentien_run(edict_t* self);
void sentien_walk(edict_t* self);
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

void sentien_sound_fend(edict_t* self)
{
	gi.sound(self, CHAN_BODY, sound_fend, 1, ATTN_NORM, 0);
}

//
// stand
//

mframe_t sentien_frames_stand1[] =
{
   {ai_stand, 0},
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
MMOVE_T(sentien_move_stand1) = { FRAME_idle101, FRAME_idle129, sentien_frames_stand1, sentien_stand_whatnow };

mframe_t sentien_frames_stand2[] =
{
   {ai_stand, 0},
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
MMOVE_T(sentien_move_stand2) = { FRAME_idle201, FRAME_idle220, sentien_frames_stand2, sentien_stand_whatnow };

mframe_t sentien_frames_stand3[] =
{
	{ai_stand, 0},
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
MMOVE_T(sentien_move_stand3) = { FRAME_idle301, FRAME_idle331, sentien_frames_stand3, sentien_stand_earwax };

MONSTERINFO_STAND(sentien_stand) (edict_t* self) -> void
{
	sentien_stand_whatnow(self);
}

void sentien_stand_whatnow(edict_t* self)
{
    float r;

    r = frandom();

    if (r < self->random)
    {
        M_SetAnimation(self, &sentien_move_stand1);
        gi.sound(self, CHAN_BODY, sound_idle1, 1, ATTN_NORM, 0);
        self->random -= 0.05f;
    }
    else
    {
        r = frandom();
        if (r < 0.5f)
		{
            M_SetAnimation(self, &sentien_move_stand2);
			gi.sound(self, CHAN_BODY, sound_idle2, 1, ATTN_NORM, 0);
		}
        else
		{
            M_SetAnimation(self, &sentien_move_stand3);
			gi.sound(self, CHAN_BODY, sound_idle3, 1, ATTN_NORM, 0);
		}

        self->random = 1.0f;
    }
}

void sentien_stand_earwax(edict_t* self)
{
    if (frandom() > 0.80f)
    {
        M_SetAnimation(self, &sentien_move_stand3);
		gi.sound(self, CHAN_BODY, sound_idle3, 1, ATTN_NORM, 0);
    }
    else
    {
        sentien_stand_whatnow(self);
    }
}

//
// walk
//

mframe_t sentien_frames_start_walk[] =
{
   {ai_walk},
   {ai_walk, 2},
   {ai_walk, 3},
   {ai_walk, 2},
   {ai_walk, 2},
   {ai_walk, 3},
   {ai_walk, 2},
   {ai_walk, 2, sentien_step}
};
MMOVE_T(sentien_move_start_walk) = { FRAME_walk01, FRAME_walk08, sentien_frames_start_walk, sentien_walk };

mframe_t sentien_frames_walk[] =
{
   {ai_walk},
   {ai_walk, 2},
   {ai_walk, 4},
   {ai_walk, 4},
   {ai_walk, 4},
   {ai_walk, 4},
   {ai_walk, 4},
   {ai_walk, 5, sentien_step},

   {ai_walk, 2},
   {ai_walk, 3},
   {ai_walk, 4},
   {ai_walk, 4},
   {ai_walk, 4},
   {ai_walk, 4},
   {ai_walk, 5},
   {ai_walk, 5, sentien_step}
};
MMOVE_T(sentien_move_walk) = { FRAME_walk09, FRAME_walk24, sentien_frames_walk, nullptr };

mframe_t sentien_frames_end_walk[] =
{
   {ai_walk, 1},
   {ai_walk, 1},
   {ai_walk, 2},
   {ai_walk, 1},
   {ai_walk, 2},
   {ai_walk, 1},
   {ai_walk, 2},
   {ai_walk, 2, sentien_step}
};
MMOVE_T(sentien_move_end_walk) = { FRAME_walk25, FRAME_walk32, sentien_frames_end_walk, sentien_stand };

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
   {ai_run, 2},
   {ai_run, 3},
   {ai_run, 2},
   {ai_run, 2},
   {ai_run, 3},
   {ai_run, 2},
   {ai_run, 2, sentien_step}
};
MMOVE_T(sentien_move_start_run) = { FRAME_walk01, FRAME_walk08, sentien_frames_start_run, sentien_run };

mframe_t sentien_frames_run[] =
{
   {ai_run},
   {ai_run, 2},
   {ai_run, 4},
   {ai_run, 4},
   {ai_run, 4},
   {ai_run, 4},
   {ai_run, 4},
   {ai_run, 5, sentien_step},

   {ai_run, 2},
   {ai_run, 3},
   {ai_run, 4},
   {ai_run, 4},
   {ai_run, 4},
   {ai_run, 4},
   {ai_run, 5},
   {ai_run, 5, sentien_step}
};
MMOVE_T(sentien_move_run) = { FRAME_walk09, FRAME_walk24, sentien_frames_run, nullptr };

mframe_t sentien_frames_run_end[] =
{
   {ai_run, 1},
   {ai_run, 1},
   {ai_run, 2},
   {ai_run, 1},
   {ai_run, 2},
   {ai_run, 1},
   {ai_run, 2},
   {ai_run, 2, sentien_step}
};
MMOVE_T(sentien_move_run_end) = { FRAME_walk25, FRAME_walk32, sentien_frames_run_end, sentien_stand };

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
	if (visible(self, self->enemy) && self->enemy->health > 0)
		M_SetAnimation(self, &sentien_move_blast_attack);
	else
		M_SetAnimation(self, &sentien_move_post_blast_attack);
}

void sentien_post_blast_attack(edict_t* self)
{
	float refire = 0.25f;
	float r;

	if (visible(self, self->enemy) &&
		infront(self, self->enemy))
	{
		if (skill->integer == 1)
			refire = 0.4f;
		else if (skill->integer == 2)
			refire = 0.6f;
		else if (skill->integer >= 3)
			refire = 0.75f;

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
	 {24, 25, 30},
	 {24, 25, 27},
	 {24, 28, 28},
	 {24, 27, 31},
	 {24, 25, 32},
	 {24, 23, 31},
	 {24, 23, 28}
};

void sentien_machinegun(edict_t* self)
{
	vec3_t					 start;
	vec3_t					 aim;
	vec3_t					 forward, right;
	int						 idx;

	idx = self->s.frame - FRAME_attak105 + 1;

	if (!self->enemy || !self->enemy->inuse) // PGM
		return;								 // PGM
		
	AngleVectors(self->s.angles, forward, right, nullptr);

	start = M_ProjectFlashSource(self, sentien_flash_offset[idx], forward, right);
	PredictAim(self, self->enemy, start, 0, true, -0.2f, &aim, nullptr);
	
	monster_fire_bullet(self, start, aim, 5, 4, DEFAULT_BULLET_HSPREAD, DEFAULT_BULLET_VSPREAD, MZ2_TANK_MACHINEGUN_14);

	gi.sound(self, CHAN_BODY, sound_attack1, 1, ATTN_NORM, 0);
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
		gi.sound(self, CHAN_BODY, sound_attack2, 1, ATTN_NORM, 0);
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
	{44, -23 + 1, 44 - 1},
	{44, -23 + 1, 44 - 1},
	{44, -23 + 1, 44 - 1},
	{43, -23 + 1, 44 - 1},
	{42, -23 + 1, 44 - 1},
	{42, -24 + 1, 44 - 1},
	{42, -23 + 1, 44 - 1},
	{43, -23 + 1, 44 - 1},
	{44, -23 + 1, 44 - 1},
	{44, -23 + 1, 44 - 1},
	{44, -23 + 1, 44 - 1}
};

PRETHINK(sentien_fire_update) (edict_t* laser) -> void
{
	edict_t* self = laser->owner;
	vec3_t forward, right;
	vec3_t start;

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, sentien_laser_offset[laser->spawnflags.has(SPAWNFLAG_DABEAM_SECONDARY) ? 1 : 0], forward, right);
	
	if (!self->deadflag)
		PredictAim(self, self->enemy, start, 0, false, frandom(0.1f, 0.2f), &forward, nullptr);
	
	laser->s.origin = start;
	laser->movedir = forward;
	gi.linkentity(laser);
	dabeam_update(laser, false);
}

void sentien_do_laser(edict_t* self)
{
	monster_fire_dabeam(self, 5, self->s.frame & 1, sentien_fire_update);
}

MONSTERINFO_ATTACK(sentien_attack) (edict_t* self) -> void
{
	vec3_t	vec;
	float	range;
	float	r;

	if (!self->enemy || self->enemy->health <= 0)
    {
        self->enemy = nullptr;
        M_SetAnimation(self, &sentien_move_run_end);
        return;
    }

	vec = self->enemy->s.origin - self->s.origin;
	range = vec.length();

	r = frandom();

	bool machinegun_visible = M_CheckClearShot(self, sentien_flash_offset[0]);
	bool laser_visible = M_CheckClearShot(self, sentien_laser_offset[0]);

	if (!machinegun_visible && !laser_visible)
		return;

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

MONSTERINFO_DODGE(sentien_fend) (edict_t* self, edict_t* attacker, gtime_t eta, trace_t* tr, bool gravity) -> void
{
	// don't flinch if attacking
	if (self->monsterinfo.active_move == &sentien_move_laser_attack ||
		self->monsterinfo.active_move == &sentien_move_blast_attack)
		return;

	if (self->monsterinfo.active_move == &sentien_move_fend)
        return;
    
    if (level.time < self->pain_debounce_time)
        return;
    
    self->pain_debounce_time = level.time + 5_sec;

	if (skill->integer == 0)
	{
		if (frandom() > 0.15)
			return;
	}
	else if (skill->integer == 1)
	{
		if (frandom() > 0.25)
			return;
	}
	else
	{
		if (frandom() > 0.35)
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
	vec3_t forward, right;
    vec3_t point, start, end;

    AngleVectors(self->s.angles, forward, right, nullptr);

    point = G_ProjectSource(self->s.origin, { 6, -70, 0 }, forward, right);
	start = point - self->s.origin;

    point = G_ProjectSource(self->s.origin, { 44, 10, 0 }, forward, right);
	end = point - self->s.origin;

    self->mins = 
	{
        std::min(start[0], end[0]),
        std::min(start[1], end[1]),
        -16
    };
    self->maxs = 
	{
        std::max(start[0], end[0]),
        std::max(start[1], end[1]),
        0
    };

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
   {ai_move},
   {ai_move, 0, sentien_shrink},
   {ai_move}
};
MMOVE_T(sentien_move_death1) = { FRAME_death101, FRAME_death128, sentien_frames_death1, sentien_dead };

mframe_t sentien_frames_death2[] =
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
	{
		gi.sound(self, CHAN_BODY, sound_die1, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &sentien_move_death1);
	}
	else
	{
		gi.sound(self, CHAN_BODY, sound_die2, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &sentien_move_death2);
	}
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

	sound_idle1.assign("sentien/sen_idle1.wav");
	sound_idle2.assign("sentien/sen_idle2.wav");
	sound_idle3.assign("sentien/sen_idle3.wav");
	sound_step.assign("sentien/sen_walk.wav");
	sound_fend.assign("sentien/sen_fend.wav");
	sound_pain1.assign("sentien/sen_pain1.wav");
	sound_pain2.assign("sentien/sen_pain2.wav");
	sound_pain3.assign("sentien/sen_pain3.wav");
	sound_die1.assign("sentien/sen_die1.wav");
	sound_die2.assign("sentien/sen_die2.wav");
	sound_attack1.assign("sentien/sen_att1.wav");
	sound_attack2.assign("sentien/sen_att2.wav");
	sound_sight.assign("sentien/sen_att3.wav");

	self->mass = 500;
	self->health = 750 * st.health_multiplier;
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
	self->monsterinfo.dodge = sentien_fend;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.sight = nullptr;
	self->monsterinfo.idle = nullptr;
	self->monsterinfo.setskin = sentien_setskin;

	self->monsterinfo.aiflags |= AI_IGNORE_SHOTS;

	gi.linkentity(self);

	M_SetAnimation(self, &sentien_move_stand1);
	self->monsterinfo.scale = MODEL_SCALE;

	walkmonster_start(self);
}