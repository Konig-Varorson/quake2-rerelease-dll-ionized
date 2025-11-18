/*==============================================================================

Quake AD FREDDIE (Edie from Quoth - Kell/Necros/Preach)
Reimplemented by Knightmare and MikeM

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_freddie.h"

static cached_soundindex	sound_pain1;
static cached_soundindex	sound_pain2;
static cached_soundindex	sound_death;
static cached_soundindex	sound_death2;
static cached_soundindex	sound_idle;
static cached_soundindex	sound_sight;
static cached_soundindex	sound_saw;
static cached_soundindex	sound_melee;
static cached_soundindex	sound_atk_spinup;
static cached_soundindex	sound_atk_nail;
static cached_soundindex	sound_atk_laser;
static cached_soundindex	sound_step1;
static cached_soundindex	sound_step2;
static cached_soundindex	sound_step3;

void freddie_fireweapon (edict_t *self);
void freddie_attack(edict_t *self);
void freddie_walk(edict_t* self);
void freddie_run(edict_t* self);
void freddie_run_frame(edict_t* self);
void freddie_do_sfire(edict_t* self);
void freddie_attack_spindown(edict_t* self);

MONSTERINFO_SEARCH(freddie_search) (edict_t* self) -> void
{
	gi.sound (self, CHAN_VOICE, sound_idle, 1, ATTN_IDLE, 0);
}

MONSTERINFO_SIGHT(freddie_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound (self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
	freddie_attack (self);
}

void freddie_sound_footstep (edict_t *self)
{
	static int	stepNum = 0;

	switch (stepNum)
	{
	case 0:
	default:
		gi.sound (self, CHAN_BODY, sound_step1, 1, ATTN_NORM, 0);
		break;
	case 1:
		gi.sound (self, CHAN_BODY, sound_step2, 1, ATTN_NORM, 0);
		break;
	case 2:
		gi.sound (self, CHAN_BODY, sound_step1, 1, ATTN_NORM, 0);
		break;
	case 3:
		gi.sound (self, CHAN_BODY, sound_step2, 1, ATTN_NORM, 0);
		break;
	case 4:
		gi.sound (self, CHAN_BODY, sound_step3, 1, ATTN_NORM, 0);
		break;
	}
	stepNum++;
	if (stepNum > 4)
		stepNum = 0;
}

//
// stand
//

void freddie_stand (edict_t *self);

mframe_t freddie_frames_stand [] =
{
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
};
mmove_t	freddie_move_stand = {FRAME_stand1, FRAME_stand8, freddie_frames_stand, freddie_stand};

MONSTERINFO_STAND(freddie_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &freddie_move_stand);
}

//
// walk
//

mframe_t freddie_frames_walk [] =
{
	{ai_walk, 20},
	{ai_walk, 16, freddie_sound_footstep},
	{ai_walk, 1},
	{ai_walk, 2},
	{ai_walk, 4},
	{ai_walk, 8},
	{ai_walk, 4},
	{ai_walk, 16},
	{ai_walk, 20},
	{ai_walk, 16, freddie_sound_footstep},
	{ai_walk, 1},
	{ai_walk, 2},
	{ai_walk, 4},
	{ai_walk, 8},
	{ai_walk, 4},
	{ai_walk, 16},
};
MMOVE_T(freddie_move_walk) = {FRAME_run1, FRAME_run6, freddie_frames_walk, nullptr};

MONSTERINFO_WALK(freddie_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &freddie_move_walk);
}

//
// run
//

mframe_t freddie_frames_run [] =
{
	{ai_run, 24,	freddie_run_frame},	// freddie_idle_sound,
	{ai_run, 19,	freddie_run_frame},	// freddie_sound_footstep,
	{ai_run, 1,		freddie_run_frame},
	{ai_run, 2,		freddie_run_frame},
	{ai_run, 5,		freddie_run_frame},
	{ai_run, 10,	freddie_run_frame},
	{ai_run, 5,		freddie_run_frame},
	{ai_run, 5,		freddie_run_frame},
	{ai_run, 24,	freddie_run_frame},
	{ai_run, 19,	freddie_run_frame},
	{ai_run, 1,		freddie_run_frame},	// freddie_sound_footstep,
	{ai_run, 2,		freddie_run_frame},
	{ai_run, 5,		freddie_run_frame},
	{ai_run, 10,	freddie_run_frame},
	{ai_run, 5,		freddie_run_frame},
	{ai_run, 5,		freddie_run_frame}
};
MMOVE_T(freddie_move_run) = {FRAME_run1, FRAME_run16, freddie_frames_run, nullptr };

MONSTERINFO_RUN(freddie_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &freddie_move_stand);
	else
		M_SetAnimation(self, &freddie_move_run);
}

//
// run + firing
//
#if 0
mframe_t freddie_frames_rfire [] =
{
	{ai_run, 24,	freddie_run_frame},	// freddie_idle_sound,
	{ai_run, 19,	freddie_run_frame},	// freddie_sound_footstep,
	{ai_run, 1,		freddie_run_frame},
	{ai_run, 2,		freddie_run_frame},
	{ai_run, 5,		freddie_run_frame},
	{ai_run, 10,	freddie_run_frame},
	{ai_run, 5,		freddie_run_frame},
	{ai_run, 5,		freddie_run_frame},
	{ai_run, 24,	freddie_run_frame},
	{ai_run, 19,	freddie_run_frame},
	{ai_run, 1,		freddie_run_frame},	// freddie_sound_footstep,
	{ai_run, 2,		freddie_run_frame},
	{ai_run, 5,		freddie_run_frame},
	{ai_run, 10,	freddie_run_frame},
	{ai_run, 5,		freddie_run_frame},
	{ai_run, 5,		freddie_run_frame}
};
MMOVE_T(freddie_move_rfire) = {FRAME_rfire1, FRAME_rfire16, freddie_frames_rfire, nullptr };

void freddie_rfire (edict_t *self)
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &freddie_move_stand);
	else
		M_SetAnimation(self, &freddie_move_rfire);
}
#endif

void freddie_run_frame (edict_t *self)
{
	int		startFrame = 0;

	if ( !self || (self->health <= 0) )
		return;

	// If enemy is dead or missing no firing
//	if ( !self->enemy || (self->enemy->health <= 0) )
//		self->fog_model = 0;

//	if (self->monsterinfo.currentmove == &freddie_move_run)
		startFrame = FRAME_run1;
//	else if (self->monsterinfo.currentmove == &freddie_move_rfire)
//		startFrame = FRAME_rfire1;

	switch (self->s.frame - startFrame)
	{
	case 0:
		VectorSet (self->muzzle, 50, -20, 32);
		break;
	case 1:
		VectorSet (self->muzzle, 43, -18, 33);	freddie_sound_footstep (self);
		break;
	case 2:
		VectorSet (self->muzzle, 53, -20, 27);
		break;
	case 3:
		VectorSet (self->muzzle, 47, -19, 31);
		break;
	case 4:
		VectorSet (self->muzzle, 54, -17, 29);
		break;
	case 5:
		VectorSet (self->muzzle, 47, -17, 35);
		break;
	case 6:
		VectorSet (self->muzzle, 53, -15, 32);
		break;
	case 7:
		VectorSet (self->muzzle, 47, -15, 36);
		break;
	case 8:
		VectorSet (self->muzzle, 50, -13, 32);
		break;
	case 9:
		VectorSet (self->muzzle, 46, -15, 33);
		break;
	case 10:
		VectorSet (self->muzzle, 51, -14, 33);	freddie_sound_footstep (self);
		break;
	case 11:
		VectorSet (self->muzzle, 47, -14, 32);
		break;
	case 12:
		VectorSet (self->muzzle, 52, -14, 33);
		break;
	case 13:
		VectorSet (self->muzzle, 47, -14, 36);
		break;
	case 14:
		VectorSet (self->muzzle, 49, -14, 35);
		break;
	case 15:
		VectorSet (self->muzzle, 45, -15, 34);
		break;
	default:
		VectorSet (self->muzzle, 0, 0, 0);
		break;
	}

	// Run + Fire time?
/*	if (self->fog_model > 0)
	{
		self->fog_index++;
		if (self->fog_index > 2)
			self->fog_model = 0;
		else
			freddie_fireweapon (self);
	}*/
}


//
// pain
//

mframe_t freddie_frames_pain1 [] =
{
	{ai_move},
	{ai_move, 10},
	{ai_move, 9},
	{ai_move, 4},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(freddie_move_pain1) = {FRAME_pain1, FRAME_pain12, freddie_frames_pain1, freddie_run};

mframe_t freddie_frames_pain2 [] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(freddie_move_pain2) = {FRAME_painb1, FRAME_painb5, freddie_frames_pain2, freddie_run};


PAIN(freddie_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	float r = frandom();

	if (level.time < self->pain_debounce_time)
		return;

	if (r < 0.5f)
		gi.sound (self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
	else
		gi.sound (self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);

	self->pain_debounce_time = level.time + 3_sec;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	if (damage <= 50) {
		M_SetAnimation(self, &freddie_move_pain2);
		self->pain_debounce_time = level.time + 2_sec;
	}
	else {
		M_SetAnimation(self, &freddie_move_pain1);
		self->pain_debounce_time = level.time + 3_sec;
	}
}

//
// fire (standing)
//

void freddie_attack_spinupsound (edict_t *self)
{
	gi.sound (self, CHAN_BODY, sound_atk_spinup, 1.0, ATTN_NORM, 0);
}

void freddie_attack_prefire (edict_t *self)
{
	self->fog_index = 0;
	self->fogclip = 4 + (int)skill->value * 2;
}

void freddie_fireweapon (edict_t *self)
{
	vec3_t	start, forward, right, dir, vec;	// attack_track
	float	speed, scale;

	if ( !self || (self->health <= 0) )
		return;
	if ( !self->enemy || (self->enemy->health < 1) )	// If no enemy or enemy dead then stop attack
		return;

	// Nail and laser use the same firing speed
	speed = 500 + (int)skill->value * 150;
	// Rotate offset vector
	AngleVectors (self->s.angles, forward, right);
	start = G_ProjectSource (self->s.origin, self->muzzle, forward, right);

/*	if ( (self->fog_model != 0) && ((int)skill->value > 1) )	// The mini burst mode (run+fire) can do tracking
	{
		if (self->fog_index == 1)
			Freddie_Tracking (self->enemy->s.origin, self->muzzle, self->enemy, speed, attack_track);
		else
			Freddie_Tracking (attack_track, self->muzzle, self->enemy, speed, attack_track);
		VectorSubtract (attack_track, start, dir);
		VectorNormalize (dir);
	}
	else*/	// Standard mode: spray bullets at player
	{
		scale = crandom() * (20 + (frandom() * 50));
		vec = right * scale;
		dir = self->enemy->s.origin + vec;
		dir = dir - start;
		dir.normalize();
	}

	if (self->sounds == 1)
	{
		gi.sound (self, CHAN_WEAPON|CHAN_RELIABLE, sound_atk_laser, 1.0, ATTN_NORM, 0);
		fire_blaster (self, start, dir, 15, speed, EF_BLASTER);
	}
	else {
		gi.sound (self, CHAN_WEAPON|CHAN_RELIABLE, sound_atk_nail, 1.0, ATTN_NORM, 0);
		fire_flechette (self, start, dir, 9, speed, true);
	}
}

void freddie_stand_fire (edict_t *self)
{
	bool	quitNext = false;

	// Easy = 7 shots, Normal = 11 shots, Hard/NM = 15 shots
	switch (self->s.frame)
	{
	case FRAME_sfire9:
		VectorSet (self->muzzle, 45, -10, 20);
		break;
	case FRAME_sfire10:
		VectorSet (self->muzzle, 45, -10, 20);
		break;
	case FRAME_sfire11:
		VectorSet (self->muzzle, 45, -10, 22);
		break;
	case FRAME_sfire12:
		VectorSet (self->muzzle, 45, -10, 22);
		break;
	case FRAME_sfire13:
		VectorSet (self->muzzle, 45, -12, 23);
		break;
	case FRAME_sfire14:
		VectorSet (self->muzzle, 42, -12, 25);
		break;
	case FRAME_sfire15:
		VectorSet (self->muzzle, 42, -14, 23);
		if ((int)skill->value == 0)
			quitNext = true;
		break;
	case FRAME_sfire16:
		VectorSet (self->muzzle, 38, -14, 27);
		break;
	case FRAME_sfire17:
		VectorSet (self->muzzle, 38, -17, 26);
		break;
	case FRAME_sfire18:
		VectorSet (self->muzzle, 36, -17, 28);
		break;
	case FRAME_sfire19:
		VectorSet (self->muzzle, 36, -17, 26);
		if ((int)skill->value == 1)
			quitNext = true;
		break;
	case FRAME_sfire20:
		VectorSet (self->muzzle, 30, -17, 28);
		break;
	case FRAME_sfire21:
		VectorSet (self->muzzle, 30, -18, 25);
		break;
	case FRAME_sfire22:
		VectorSet (self->muzzle, 25, -18, 27);
		break;
	case FRAME_sfire23:
		VectorSet (self->muzzle, 25, -18, 27);
		break;
	default:
		VectorSet (self->muzzle, 0, 0, 0);
		break;
	}

	if ( !self->enemy || (self->enemy->health < 1) )	// If no enemy or enemy dead then stop attack
		self->fog_index = self->fogclip + 1;
	else if ( visible(self, self->enemy) )				// Check if enemy is visible to weapon
		self->fog_index++;
	else												// Reset counter and keep firing
		self->fog_index = 0;	

	if (self->fog_index > self->fogclip)
		freddie_attack_spindown (self);
	else
		freddie_fireweapon (self);

	if (quitNext)	// this is our last firing frame based on skill level
		self->fog_index = self->fogclip + 1;
}

mframe_t freddie_frames_sfire_start [] =
{
	{ai_charge, 0, freddie_attack_spinupsound},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, freddie_sound_footstep},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, freddie_attack_prefire}
};
MMOVE_T(freddie_move_sfire_start) = {FRAME_sfire1, FRAME_sfire8, freddie_frames_sfire_start, freddie_do_sfire};

mframe_t freddie_frames_sfire [] =
{
	{ai_charge, 0, freddie_stand_fire},
	{ai_charge, 0, freddie_stand_fire},
	{ai_charge, 0, freddie_stand_fire},
	{ai_charge, 0, freddie_stand_fire},
	{ai_charge, 0, freddie_stand_fire},
	{ai_charge, 0, freddie_stand_fire},
	{ai_charge, 0, freddie_stand_fire},
	{ai_charge, 0, freddie_stand_fire},
	{ai_charge, 0, freddie_stand_fire},
	{ai_charge, 0, freddie_stand_fire},
	{ai_charge, 0, freddie_stand_fire},
	{ai_charge, 0, freddie_stand_fire},
	{ai_charge, 0, freddie_stand_fire},
	{ai_charge, 0, freddie_stand_fire},
	{ai_charge, 0, freddie_stand_fire}
};
MMOVE_T(freddie_move_sfire) = {FRAME_sfire9, FRAME_sfire23, freddie_frames_sfire, freddie_attack_spindown};

mframe_t freddie_frames_sfire_stop [] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(freddie_move_sfire_stop) = {FRAME_sfire24, FRAME_sfire27, freddie_frames_sfire_stop, freddie_run};

void freddie_do_sfire (edict_t *self)
{
	M_SetAnimation(self, &freddie_move_sfire);
}

void freddie_attack_spindown (edict_t *self)
{
	M_SetAnimation(self, &freddie_move_sfire_stop);
}

MONSTERINFO_ATTACK(freddie_attack) (edict_t* self) -> void
{
	M_SetAnimation(self, &freddie_move_sfire_start);
}

//
// melee
//

void freddie_melee (edict_t *self);
void freddie_melee_repeat (edict_t *self);

void freddie_saw_sound (edict_t *self)
{
	gi.sound (self, CHAN_VOICE, sound_saw, 1, ATTN_NORM, 0);
}

void freddie_blade (edict_t *self, float side)
{
	float	damage;
	vec3_t	aim;

	// 1-45 (3x15) damage
	damage = max(10, irandom(45));

	if ( fire_hit (self, vec3_t{ 64, 0.f, -32.f }, damage, 100) )
		gi.sound (self, CHAN_VOICE, sound_melee, 1, ATTN_NORM, 0);
}

void freddie_blade_right (edict_t *self)
{
	freddie_blade (self, -200);
}

void freddie_blade_left (edict_t *self)
{
	freddie_blade (self, 200);
}

mframe_t freddie_frames_fswing [] =
{
	{ai_charge, 8, freddie_saw_sound},
	{ai_charge, 16, freddie_sound_footstep},
	{ai_charge, 10},
	{ai_charge, 4, freddie_sound_footstep},
	{ai_charge, 2},
	{ai_charge, 1},
	{ai_charge, 1},
	{ai_charge, 2, freddie_blade_right},
	{ai_charge, 2},
	{ai_charge, 2},
	{ai_charge},
};
MMOVE_T(freddie_move_fswing) = {FRAME_fswing1, FRAME_fswing11, freddie_frames_fswing, freddie_melee_repeat};

mframe_t freddie_frames_bswing [] =
{
	{ai_charge, 20, freddie_saw_sound},
	{ai_charge, 12},
	{ai_charge, 4},
	{ai_charge, 2, freddie_sound_footstep},
	{ai_charge, 6, freddie_blade_left},
	{ai_charge, 4},
	{ai_charge, 4},
	{ai_charge, 8},
	{ai_charge, 8, freddie_sound_footstep},
	{ai_charge, 8},
	{ai_charge, 8},
	{ai_charge, 12},
	{ai_charge, 10},
	{ai_charge, 10},
};
MMOVE_T(freddie_move_bswing) = {FRAME_bswing1, FRAME_bswing14, freddie_frames_bswing, freddie_run};

mframe_t freddie_frames_fswinge [] =
{
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, freddie_sound_footstep},
};
MMOVE_T(freddie_move_fswinge) = {FRAME_fswinge1, FRAME_fswinge10, freddie_frames_fswinge, freddie_run};

void freddie_melee_repeat  (edict_t *self)
{
	int r;

	if (!self->enemy)
		return;
	if (self->enemy->health <= 0)
		return;

	// Check if within range to do a second (backward) swing attack
	r = range_to(self, self->enemy);
	if (r == RANGE_MELEE)
	{
		M_SetAnimation(self, &freddie_move_bswing);
	}
	else
		M_SetAnimation(self, &freddie_move_fswinge);
}

MONSTERINFO_MELEE(freddie_melee) (edict_t* self) -> void
{
	int r;
	
	if (!self->enemy)
		return;
	if (self->enemy->health <= 0)
		return;

	r = range_to(self, self->enemy);
	if (r == RANGE_MELEE)
	{
		M_SetAnimation(self, &freddie_move_fswing);
	}
	else if (visible(self, self->enemy) && infront(self,self->enemy)
		&& (r < RANGE_MID) && !(self->monsterinfo.aiflags & AI_SOUND_TARGET))
	{
		M_SetAnimation(self, &freddie_move_sfire_start);
	}
	else
		M_SetAnimation(self, &freddie_move_run);
}

//
// death
//

void freddie_dead (edict_t *self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, -8 };
	monster_dead(self);
}

mframe_t freddie_frames_death [] =
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
MMOVE_T(freddie_move_death) = {FRAME_death1, FRAME_death35, freddie_frames_death, freddie_dead};

DIE(freddie_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum = 0;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 3, "models/objects/gibs/sm_meat/tris.md2" },
			{ 1, "models/objects/gibs/gear/tris.md2" },
			{ "models/monsters/ogre/gibs/head.md2", GIB_HEAD | GIB_SKINNED }
		});
		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	gi.sound (self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	M_SetAnimation(self, &freddie_move_death);
}

/*QUAKED monster_freddie (1 0 0) (-32 -32 -24) (32 32 64) Ambush Trigger_Spawn Sight
model="models/monsters/freddie/tris.md2"
*/
void SP_monster_freddie (edict_t *self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_pain1.assign("freddie/pain.wav");
	sound_pain2.assign("freddie/painshrt.wav");
	sound_death.assign("freddie/death.wav");
	sound_death2.assign("freddie/explode_major.wav");
	sound_idle.assign("freddie/idle1.wav");
	sound_sight.assign("freddie/sight.wav");
	sound_saw.assign("freddie/sawstart.wav");
	sound_melee.assign("freddie/mangle.wav");
	sound_atk_spinup.assign("freddie/stfire.wav");
	sound_atk_nail.assign("q1weapons/rocket1i.wav");
	sound_atk_laser.assign("q1enforcer/enfire.wav");
	sound_step1.assign("freddie/step.wav");
	sound_step2.assign("freddie/step2.wav");
	sound_step3.assign("freddie/step3.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex ("models/monsters/freddie/tris.md2");
	self->mins = { -24, -24, -24 };
	self->maxs = { 24, 24, 64};

	self->health = 500;
	self->gib_health = -80;
	self->mass = 500;

	self->pain = freddie_pain;
	self->die = freddie_die;

	//self->flags |= FL_Q1_MONSTER;

	self->monsterinfo.stand = freddie_stand;
	self->monsterinfo.walk = freddie_walk;
	self->monsterinfo.run = freddie_run;
	self->monsterinfo.dodge = nullptr;
	self->monsterinfo.attack = freddie_attack;
	self->monsterinfo.melee = freddie_melee;
	self->monsterinfo.sight = freddie_sight;
	self->monsterinfo.search = freddie_search;

	gi.linkentity (self);

	M_SetAnimation(self, &freddie_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	walkmonster_start (self);
}
