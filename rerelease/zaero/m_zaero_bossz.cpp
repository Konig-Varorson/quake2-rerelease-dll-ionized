// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

z-sentien

==============================================================================
*/

#include "../g_local.h"
#include "m_zaero_bossz.h"
#include "../m_flash.h"

static cached_soundindex	sound_pain1;
static cached_soundindex	sound_pain2;
static cached_soundindex	sound_pain3;
static cached_soundindex	sound_die1;
static cached_soundindex	sound_die2;
static cached_soundindex	sound_hookimpact;
static cached_soundindex	sound_sight;
static cached_soundindex	sound_hooklaunch;
static cached_soundindex	sound_hookfly;
static cached_soundindex	sound_swing;
static cached_soundindex	sound_idle1;
static cached_soundindex	sound_idle2;
static cached_soundindex	sound_walk;
static cached_soundindex	sound_raisegun;
static cached_soundindex	sound_lowergun;
static cached_soundindex	sound_switchattacks;
static cached_soundindex	sound_plamsaballfly;
static cached_soundindex	sound_plamsaballexplode;
static cached_soundindex	sound_plamsaballfire;
static cached_soundindex	sound_taunt1;
static cached_soundindex	sound_taunt2;
static cached_soundindex	sound_taunt3;

void fire_empnuke(edict_t* ent, vec3_t center, int radius);
void SV_AddGravity(edict_t* ent);

void zboss_stand(edict_t* self);
void zboss_run(edict_t* self);
void zboss_run2(edict_t* self);
void zboss_walk(edict_t* self);
void zboss_walk2(edict_t* self);
void zboss_chooseNextAttack(edict_t* self);
void zboss_reelInGrapple(edict_t* self);
void zboss_posthook(edict_t* self);
void HookDragThink(edict_t* self);
void zboss_attack(edict_t* self);

void zboss_walksound(edict_t* self)
{
	gi.sound(self, CHAN_BODY, sound_walk, 1, ATTN_NORM, 0);
}

MONSTERINFO_SIGHT(zboss_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}


MONSTERINFO_IDLE(zboss_idle) (edict_t* self) -> void
{
	float r = frandom();

	if (frandom() < 0.10)
	{
		if (r < 0.33)
		{
			gi.sound(self, CHAN_VOICE, sound_taunt1, 1, ATTN_NORM, 0);
		}
		else if (r < 0.66)
		{
			gi.sound(self, CHAN_VOICE, sound_taunt2, 1, ATTN_NORM, 0);
		}
		else
		{
			gi.sound(self, CHAN_VOICE, sound_taunt3, 1, ATTN_NORM, 0);
		}
	}
}

//
// STAND
//

mframe_t zboss_frames_stand1[] =
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
	{ai_stand},  // 9

	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},  // 19

	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},  // 29

	{ai_stand},
	{ai_stand}
};
MMOVE_T(zboss_movestand1) = { FRAME_idle101, FRAME_idle132, zboss_frames_stand1, zboss_stand };

mframe_t zboss_frames_stand2[] =
{
	{ai_stand}, // 32
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}, // 41

	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}, // 51

	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}	 // 56
};
MMOVE_T(zboss_movestand2) = { FRAME_idle201, FRAME_idle225, zboss_frames_stand2, zboss_stand };


void zboss_standidle(edict_t* self)
{
	if (!self)
	{
		return;
	}

	if (frandom() < 0.8)
	{
		gi.sound(self, CHAN_VOICE, sound_idle1, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &zboss_movestand1);
	}
	else
	{
		gi.sound(self, CHAN_VOICE, sound_idle2, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &zboss_movestand2);
	}
}

//
// Post WALK/RUN leading into ilde.
//

mframe_t zboss_frames_postwalk[] =
{
	{ai_walk,  3}, // 177
	{ai_walk,  3},
	{ai_walk,  3},
	{ai_walk,  3},
	{ai_walk,  3},
	{ai_walk,  3},
	{ai_walk,  3},
	{ai_walk,  3} // 184
};
MMOVE_T(zboss_move_postwalk) = { FRAME_walk401, FRAME_walk408, zboss_frames_postwalk, zboss_standidle };


void zboss_postWalkRun(edict_t* self)
{
	M_SetAnimation(self, &zboss_move_postwalk);
}

//
// WALK
//

mframe_t zboss_frames_prewalk[] =
{
	{ai_walk,  3}, //154
	{ai_walk,  3},
	{ai_walk,  3},
	{ai_walk,  3},
	{ai_walk,  3},
	{ai_walk,  3},
	{ai_walk,  3} // 160
};
MMOVE_T(zboss_move_prewalk) = { FRAME_walk101, FRAME_walk107, zboss_frames_prewalk, zboss_walk2 };

mframe_t zboss_frames_walk[] =
{
	{ai_walk,  2},	//161
	{ai_walk,  3},
	{ai_walk,  3},
	{ai_walk,  4},
	{ai_walk,  4},
	{ai_walk,  4},
	{ai_walk,  4},
	{ai_walk,  3, zboss_walksound},
	{ai_walk,  4},
	{ai_walk,  4},	// 170
	{ai_walk,  4},
	{ai_walk,  4},
	{ai_walk,  3},
	{ai_walk,  2},
	{ai_walk,  2},
	{ai_walk,  3, zboss_walksound }						// 176
};
MMOVE_T(zboss_move_walk) = { FRAME_walk201, FRAME_walk307, zboss_frames_walk, zboss_walk2 };

MONSTERINFO_WALK(zboss_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &zboss_move_prewalk);
}

void zboss_walk2(edict_t* self)
{
	M_SetAnimation(self, &zboss_move_walk);
}

//
// RUN
//

mframe_t zboss_frames_prerun[] =
{
	{ai_run,  3}, //154
	{ai_run,  3},
	{ai_run,  3},
	{ai_run,  3},
	{ai_run,  3},
	{ai_run,  3},
	{ai_run,  3} // 160
};
MMOVE_T(zboss_move_prerun) = { FRAME_walk101, FRAME_walk107, zboss_frames_prerun, zboss_run2 };

mframe_t zboss_frames_run[] =
{
	{ai_run,  2},	//161
	{ai_run,  3},
	{ai_run,  3},
	{ai_run,  4},
	{ai_run,  4},
	{ai_run,  4},
	{ai_run,  4},
	{ai_run,  3, zboss_walksound},
	{ai_run,  4},
	{ai_run,  4},	// 170
	{ai_run,  4},
	{ai_run,  4},
	{ai_run,  3},
	{ai_run,  2},
	{ai_run,  2},
	{ai_run,  3, zboss_walksound}					// 176
};
MMOVE_T(zboss_move_run) = { FRAME_walk201, FRAME_walk307, zboss_frames_run, nullptr };

MONSTERINFO_RUN(zboss_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		zboss_stand(self);
	else
		M_SetAnimation(self, &zboss_move_prerun);
}

void zboss_run2(edict_t* self)
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		zboss_stand(self);
	else
		M_SetAnimation(self, &zboss_move_run);
}

//
// main stand function
//

MONSTERINFO_STAND(zboss_stand) (edict_t* self) -> void
{
	if (self->monsterinfo.active_move == &zboss_move_prewalk ||
		self->monsterinfo.active_move == &zboss_move_walk ||
		self->monsterinfo.active_move == &zboss_move_prerun ||
		self->monsterinfo.active_move == &zboss_move_run)
	{
		zboss_postWalkRun(self);
	}
	else
	{
		zboss_standidle(self);
	}
}

//
// PAIN
//

mframe_t zboss_frames_pain1[] =
{
	{ai_move},	 // 185
	{ai_move},
	{ai_move}	 // 187
};
MMOVE_T(zboss_move_pain1) = { FRAME_pain101, FRAME_pain103, zboss_frames_pain1, zboss_run };

mframe_t zboss_frames_pain2[] =
{
	{ai_move},	 // 188
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}	 // 192
};
MMOVE_T(zboss_move_pain2) = { FRAME_pain201, FRAME_pain205, zboss_frames_pain2, zboss_run };

mframe_t zboss_frames_pain3[] =
{
	{ai_move},	// 193
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},	// 202

	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},	// 212

	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move} // 217
};
MMOVE_T(zboss_move_pain3) = { FRAME_pain301, FRAME_pain325, zboss_frames_pain3, zboss_run };

PAIN(zboss_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 5_sec;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	if (self->proboscus)
		return;		// while hook is out.

	if (self->bossFireCount && self->bossFireTimeout < level.time)
		self->bossFireCount = 0;

	if (self->bossFireCount > 40 && self->bossFireTimeout > level.time)
	{
		// that's it, we are pissed...
		if (range_to(self, self->enemy) <= RANGE_MELEE)
		{
			fire_empnuke(self, self->s.origin, 1024);
		}

		zboss_attack(self);
		self->bossFireCount = 0;
		self->bossFireTimeout = 0_sec;
		return;
	}

	self->bossFireCount++;
	self->bossFireTimeout = level.time + 1_sec;

	if (self->health < (self->max_health / 4) && (range_to(self, self->enemy) <= RANGE_MELEE))
		fire_empnuke(self, self->s.origin, 1024);


	if (damage >= 150)
	{
		M_SetAnimation(self, &zboss_move_pain3);
		gi.sound(self, CHAN_VOICE, sound_pain3, 1, ATTN_NORM, 0);
	}
	else if (damage >= 80)
	{
		M_SetAnimation(self, &zboss_move_pain2);
		gi.sound(self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);
	}
	else
	{
		M_SetAnimation(self, &zboss_move_pain1);
		gi.sound(self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
	}
}

MONSTERINFO_SETSKIN(zboss_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 4))
		self->s.skinnum |= 2;
	else if (self->health < (self->max_health / 2))
		self->s.skinnum &= ~1;
	else
		self->s.skinnum &= ~2;
}

//
// MELEE
//

void zboss_swing(edict_t* self)
{
	static	vec3_t	aim = { MELEE_DISTANCE, 0, -24 };
	fire_hit(self, aim, (15 + (rand() % 6)), 800);
}

mframe_t zboss_frames_attack2c[] =
{
	{ai_charge},						// 110
	{ai_charge, 0,	zboss_swing},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0,	zboss_idle},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0,	zboss_swing},
	{ai_charge}						// 118
};
MMOVE_T(zboss_move_attack2c) = { FRAME_attak2c01, FRAME_attak2c09, zboss_frames_attack2c, zboss_posthook };

void zboss_melee2(edict_t* self)
{
	M_SetAnimation(self, &zboss_move_attack2c);
	gi.sound(self, CHAN_WEAPON, sound_swing, 1, ATTN_NORM, 0);
}

mframe_t zboss_frames_premelee[] =
{
	{ai_charge},	// 57
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0,	zboss_idle},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge}	// 66
};
MMOVE_T(zboss_move_premelee) = { FRAME_rhook01, FRAME_rhook10, zboss_frames_premelee, zboss_melee2 };

MONSTERINFO_MELEE(zboss_melee) (edict_t* self) -> void
{
	gi.sound(self, CHAN_BODY, sound_raisegun, 1, ATTN_NORM, 0);
	M_SetAnimation(self, &zboss_move_premelee);
}

//
// ATTACK
//


// Rocket attack

mframe_t zboss_frames_attack1b[] =
{
	{ai_charge},	// 92
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge}	// 98
};
MMOVE_T(zboss_move_attack1b) = { FRAME_attak1b01, FRAME_attak1b07, zboss_frames_attack1b, zboss_chooseNextAttack };


void zboss_reloadRockets(edict_t* self)
{
	self->monsterinfo.aiflags &= ~AI_ONESHOTTARGET;
	M_SetAnimation(self, &zboss_move_attack1b);
}


static vec3_t	rocketoffset[] =
{
	{-5, -50, 33},
	{-5, -39, 27},
	{-5, -39, 39},
	{-5, -44, 27},
	{-5, -44, 39},
	{-5, -48, 29},
	{-5, -48, 29},
};

void FireFlare(edict_t* self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;

	int offset = (self->s.frame - 71) / 3;

	AngleVectors(self->s.angles, forward, right, NULL);

	start = G_ProjectSource(self->s.origin, rocketoffset[offset], forward, right);

	if (self->monsterinfo.aiflags & AI_ONESHOTTARGET)
	{
		vec = self->monsterinfo.shottarget;
	}
	else
	{
		vec = self->enemy->s.origin, vec;
		vec[2] += self->enemy->viewheight;
	}

	dir = vec - start;
	dir.normalize();

	fire_flare(self, start, dir, 10, 1000, 10, 10, (crandom_open() * 10.0f), (200 + crandom_open() * 10.0f));

	// play shooting sound
	gi.sound(self, CHAN_WEAPON, gi.soundindex("weapons/flare/shoot.wav"), 1, ATTN_NORM, 0);
}

void FireRocket(edict_t* self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;

	if (EMPNukeCheck(self, self->s.origin))
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
		return;
	}

	int offset = (self->s.frame - 71) / 3;

	AngleVectors(self->s.angles, forward, right, NULL);

	start = G_ProjectSource(self->s.origin, rocketoffset[offset], forward, right);

	if (self->monsterinfo.aiflags & AI_ONESHOTTARGET)
	{
		vec = self->monsterinfo.shottarget;
	}
	else
	{
		vec = self->enemy->s.origin, vec;
		vec[2] += self->enemy->viewheight;
	}

	vec[0] += (100 - (200 * frandom()));
	vec[1] += (100 - (200 * frandom()));
	vec[2] += (40 - (80 * frandom()));

	dir = vec - start;
	dir.normalize();

	fire_rocket(self, start, dir, 70, 500, 70 + 20, 70);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(MZ2_BOSS2_ROCKET_1);
	gi.multicast(start, MULTICAST_PVS, false);
}

mframe_t zboss_frames_attack1a[] =
{
	{ai_charge, 0,	FireFlare},	 // 71
	{ai_charge},
	{ai_charge},
	{ai_charge, 0,	FireRocket},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0,	FireRocket},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0,	FireRocket},
	{ai_charge, 0,	zboss_idle},
	{ai_charge},
	{ai_charge, 0,	FireFlare},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0,	FireRocket},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0,	FireRocket},
	{ai_charge},
	{ai_charge}				 // 91
};
MMOVE_T(zboss_move_attack1a) = { FRAME_attak1a01, FRAME_attak1a21, zboss_frames_attack1a, zboss_reloadRockets };

// hook

void zboss_reelInGrapple2(edict_t* self)
{
	vec3_t	vec, dir;
	float length;
	edict_t* enemy;
	vec3_t hookoffset = { -5, -24, 34 };
	vec3_t forward, right;

	enemy = self->proboscus->enemy;

	AngleVectors(self->s.angles, forward, right, nullptr);
	vec = G_ProjectSource(self->s.origin, hookoffset, forward, right);
	dir = vec - self->proboscus->s.origin;
	length = dir.length();

	if (length <= 80 || (self->proboscus->think == HookDragThink && self->proboscus->powerarmor_time < level.time))
	{
		G_FreeEdict(self->proboscus);
		self->proboscus = nullptr;

		self->s.modelindex3 = gi.modelindex("models/monsters/bossz/grapple/tris.md2");

		if (enemy)
		{
			enemy->velocity = {};
			zboss_melee2(self);
		}
		else
		{
			zboss_chooseNextAttack(self);
		}
	}
	else
	{
		zboss_reelInGrapple(self);
	}
}

mframe_t zboss_frames_attack2b[] =
{
	{ai_charge},		 // 107
	{ai_charge},
	{ai_charge}		 // 109
};
MMOVE_T(zboss_move_attack2b) = { FRAME_attak2b01, FRAME_attak2b03, zboss_frames_attack2b, zboss_reelInGrapple2 };



#if 0

THINK(hook_reset) (edict_t* self) -> void
{
	self->owner->proboscus = nullptr;
	G_FreeEdict(self->proboscus);
	G_FreeEdict(self);
}


DIE(hook_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (mod.id == MOD_CRUSH)
		hook_reset(self);
}

static void hook_retract(edict_t* self)
{
	// start retract animation
	if (self->owner->monsterinfo.active_move == &parasite_move_fire_proboscis)
		self->owner->monsterinfo.nextframe = FRAME_drain12;

	// mark as retracting
	self->movetype = MOVETYPE_NONE;
	self->solid = SOLID_NOT;
	// come back real hard
	if (self->style != 2)
		self->speed *= 2.0f;
	self->style = 2;
	gi.linkentity(self);
}


TOUCH(hook_touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	// owner isn't trying to probe any more, don't touch anything
	if (self->owner->monsterinfo.active_move != &parasite_move_fire_proboscis)
		return;

	vec3_t p;

	// hit what we want to succ
	if ((other->svflags & SVF_PLAYER) || other == self->owner->enemy)
	{
		if (tr.startsolid)
			p = tr.endpos;
		else
			p = tr.endpos - ((self->s.origin - tr.endpos).normalized() * 12);

		self->owner->monsterinfo.nextframe = FRAME_drain06;
		self->movetype = MOVETYPE_NONE;
		self->solid = SOLID_NOT;
		self->style = 1;
		// stick to this guy
		self->move_origin = p - other->s.origin;
		self->enemy = other;
		self->s.alpha = 0.35f;
		gi.sound(self, CHAN_WEAPON, sound_suck, 1, ATTN_NORM, 0);
	}
	else
	{
		p = tr.endpos + tr.plane.normal;
		// hit monster, don't suck but do small damage
		// and retract immediately
		if (other->svflags & (SVF_MONSTER | SVF_DEADMONSTER))
			hook_retract(self);
		else
		{
			// hit wall; stick to it and do break animation
			self->owner->monsterinfo.active_move = &parasite_move_break;
			self->movetype = MOVETYPE_NONE;
			self->solid = SOLID_NOT;
			self->style = 1;
			self->owner->s.angles[YAW] = self->s.angles[YAW];
		}
	}

	if (other->takedamage)
		T_Damage(other, self, self->owner, tr.plane.normal, tr.endpos, tr.plane.normal, 5, 0, DAMAGE_NONE, MOD_UNKNOWN);

	gi.positioned_sound(tr.endpos, self->owner, CHAN_AUTO, sound_impact, 1, ATTN_NORM, 0);

	self->s.origin = p;
	self->nextthink = level.time + FRAME_TIME_S; // start doing stuff on next frame
	gi.linkentity(self);
}

THINK(hook_think) (edict_t* self) -> void
{
	self->nextthink = level.time + FRAME_TIME_S; // start doing stuff on next frame

	// retracting; keep pulling until we hit the parasite
	if (self->style == 2)
	{
		vec3_t start = parasite_get_proboscis_start(self->owner);
		vec3_t dir = (self->s.origin - start);
		float dist = dir.normalize();

		if (dist <= (self->speed * 2) * gi.frame_time_s)
		{
			// reached target; free self on next frame, let parasite know
			self->style = 3;
			self->think = hook_reset;
			self->s.origin = start;
			gi.linkentity(self);
			return;
		}

		// pull us in
		self->s.origin -= dir * (self->speed * gi.frame_time_s);
		gi.linkentity(self);
	}
	// stuck on target; do damage, suck health
	// and check if target goes away
	else if (self->style == 1)
	{
		if (!self->enemy)
		{
			// stuck in wall
		}
		else if (!self->enemy->inuse || self->enemy->health <= 0 || !self->enemy->takedamage)
		{
			// target gone, retract early
			hook_retract(self);
		}
		else
		{
			// update our position
			self->s.origin = self->enemy->s.origin + self->move_origin;

			vec3_t start = parasite_get_proboscis_start(self->owner);

			self->s.angles = vectoangles((self->s.origin - start).normalized());

			// see if we got cut by the world
			trace_t tr = gi.traceline(start, self->s.origin, nullptr, MASK_SOLID);

			if (tr.fraction != 1.0f)
			{
				// blocked, so retract
				hook_retract(self);
				self->s.origin = self->s.old_origin;
			}
			else
			{
				// succ & drain
				if (self->timestamp <= level.time)
				{
					T_Damage(self->enemy, self, self->owner, tr.plane.normal, tr.endpos, tr.plane.normal, 2, 0, DAMAGE_NONE, MOD_UNKNOWN);
					self->owner->health = min(self->owner->max_health, self->owner->health + 2);
					self->owner->monsterinfo.setskin(self->owner);
					self->timestamp = level.time + 10_hz;
				}
			}

			gi.linkentity(self);
		}
	}
	// flying
	else if (self->style == 0)
	{
		// owner gone away?
		if (!self->owner->enemy || !self->owner->enemy->inuse || self->owner->enemy->health <= 0)
		{
			hook_retract(self);
			return;
		}

		// if we're well behind our target and missed by 2x velocity,
		// be smart enough to pull in automatically
		vec3_t to_target = (self->s.origin - self->owner->enemy->s.origin);
		float dist_to_target = to_target.normalize();

		if (dist_to_target > (self->speed * 2) / 15.f)
		{
			vec3_t from_owner = (self->s.origin - self->owner->s.origin).normalized();
			float dot = to_target.dot(from_owner);

			if (dot > 0.f)
			{
				hook_retract(self);
				return;
			}
		}
	}
}

PRETHINK(hook_segment_draw) (edict_t* self) -> void
{
	vec3_t start = { -5, -24, 34 };

	self->s.origin = start;
	self->s.old_origin = self->owner->s.origin - ((self->owner->s.origin - start).normalized() * 8.f);
	gi.linkentity(self);
}


void fire_hook(edict_t* self)
{
	vec3_t start, dir;
	float speed = 1250;
	vec3_t f, r, start;
	vec3_t offset;

	AngleVectors(self->s.angles, f, r, nullptr);
	offset = { -5, -24, 34 };
	start = M_ProjectFlashSource(self, offset, f, r);

	PredictAim(self, self->enemy, start, speed, false, crandom_open() * 0.1f, &dir, nullptr);

	edict_t* tip = G_Spawn();
	tip->s.angles = vectoangles(dir);
	tip->s.modelindex = gi.modelindex("models/monsters/parasite/tip/tris.md2");
	tip->movetype = MOVETYPE_FLYMISSILE;
	tip->owner = self;
	self->proboscus = tip;
	tip->clipmask = MASK_PROJECTILE & ~CONTENTS_DEADMONSTER;
	tip->s.origin = tip->s.old_origin = start;
	tip->speed = speed;
	tip->velocity = dir * speed;
	tip->solid = SOLID_BBOX;
	tip->takedamage = true;
	tip->flags |= FL_NO_DAMAGE_EFFECTS | FL_NO_KNOCKBACK;
	tip->die = hook_die;
	tip->touch = hook_touch;
	tip->think = hook_think;
	tip->nextthink = level.time + FRAME_TIME_S; // start doing stuff on next frame
	tip->svflags |= SVF_PROJECTILE;

	edict_t* segment = G_Spawn();
	segment->s.modelindex = gi.modelindex("models/monsters/parasite/segment/tris.md2");
	segment->s.renderfx = RF_BEAM;
	segment->postthink = hook_segment_draw;

	tip->proboscus = segment;
	segment->owner = tip;

	trace_t tr = gi.traceline(tip->s.origin, tip->s.origin + (tip->velocity * gi.frame_time_s), self, tip->clipmask);
	if (tr.startsolid)
	{
		tr.plane.normal = -dir;
		tr.endpos = start;
		tip->touch(tip, tr.ent, tr, false);
	}
	else if (tr.fraction < 1.0f)
		tip->touch(tip, tr.ent, tr, false);

	segment->s.origin = start;
	segment->s.old_origin = tip->s.origin + ((tip->s.origin - start).normalized() * 8.f);

	gi.linkentity(tip);
	gi.linkentity(segment);
}
#endif










THINK(HookDragThink) (edict_t* self) -> void
{
	vec3_t	dir, vec;
	float	speed;
	vec3_t	hookoffset = { -5, -24, 34 };
	vec3_t	forward, right;

	if (self->enemy && self->enemy->health > 0)
	{
		self->s.origin = self->enemy->s.origin;
	}

	dir = self->owner->s.origin - self->s.origin;

	AngleVectors(self->owner->s.angles, forward, right, nullptr);
	vec = G_ProjectSource(self->owner->s.origin, hookoffset, forward, right);

	dir = vec - self->s.origin;
	speed = dir.length();
	dir.normalize();

	speed = 1500;
	self->velocity = dir * speed;

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_MEDIC_CABLE_ATTACK);
	gi.WriteShort(self - g_edicts);
	gi.WritePosition(self->s.origin);
	gi.WritePosition(vec);
	gi.multicast(self->s.origin, MULTICAST_PVS, false);

	self->nextthink = level.time + 0.1_sec;
}

TOUCH(hook_touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (other == self->owner)
		return;

	if (other->takedamage)
	{
		gi.sound(self, CHAN_WEAPON, sound_hookimpact, 1, ATTN_NORM, 0);
		T_Damage(other, self, self->owner, self->velocity, self->s.origin, tr.plane.normal, 1, 0, DAMAGE_NONE, MOD_ROCKET);
	}

	if (other->client && other->health > 0)
	{ // alive... Let's drag the bastard back...
		self->enemy = other;
		vec3_t forward;
		self->s.origin[2] += 1;
		AngleVectors(self->s.angles, forward, nullptr, nullptr);
		self->enemy->velocity = forward * -1200;
	}

	self->powerarmor_time = level.time + 15_sec;
	self->velocity = {};
	self->nextthink = level.time + 0.1_sec;
	self->think = HookDragThink;
	self->s.frame = 283;
}


THINK(hook_think) (edict_t* self) -> void
{
	vec3_t	vec;
	vec3_t	hookoffset = { -3, -24, 34 };
	vec3_t	forward, right;

	if (self->powerarmor_time < level.time)
	{
		self->powerarmor_time = level.time + 15_sec;
		self->velocity = {};
		self->enemy = nullptr;
		self->think = HookDragThink;
		self->s.frame = 283;
	}


	AngleVectors(self->owner->s.angles, forward, right, nullptr);
	vec = G_ProjectSource(self->owner->s.origin, hookoffset, forward, right);

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_MEDIC_CABLE_ATTACK);
	gi.WriteShort(self - g_edicts);
	gi.WritePosition(self->s.origin);
	gi.WritePosition(vec);
	gi.multicast(self->s.origin, MULTICAST_PVS, false);

	self->nextthink = level.time + 0.1_sec;
}

void fire_hook(edict_t* self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;
	vec3_t	hookoffset = { -1, -24, 34 };
	edict_t* hook;
	float speed;

	AngleVectors(self->s.angles, forward, right, nullptr);

	start = G_ProjectSource(self->s.origin, hookoffset, forward, right);
	vec = self->enemy->s.origin;
	vec[2] += self->enemy->viewheight;
	dir = vec - start;
	dir.normalize();

	self->s.modelindex3 = 0;

	speed = 2000;

	gi.sound(self, CHAN_WEAPON, sound_hooklaunch, 1, ATTN_NORM, 0);

	self->proboscus = hook = G_Spawn();
	hook->s.origin = start;
	hook->movedir = dir;
	hook->s.angles = vectoangles(dir);
	hook->velocity = dir * speed;
	hook->movetype = MOVETYPE_FLYMISSILE;
	hook->clipmask = MASK_SHOT;
	hook->solid = SOLID_BBOX;
	hook->mins = {};
	hook->maxs = {};
	hook->s.modelindex = gi.modelindex("models/monsters/bossz/grapple/tris.md2");
	hook->s.frame = 282;
	hook->owner = self;
	hook->touch = hook_touch;
	hook->powerarmor_time = level.time + 8000_ms / speed;
	hook->nextthink = level.time + 0.4_sec;
	hook->think = hook_think;
	hook->s.sound = sound_hookfly; // replace...
	hook->classname = "bosshook";

	gi.linkentity(hook);
}

void zboss_reelInGrapple(edict_t* self)
{
	M_SetAnimation(self, &zboss_move_attack2b);
}

mframe_t zboss_frames_attack2a[] =
{
	{ai_charge},			 // 99
	{ai_charge},
	{ai_charge},
	{ai_charge, 0,	zboss_idle},
	{ai_charge},
	{ai_charge, 0,	fire_hook},	 // 104
	{ai_charge},
	{ai_charge}			 // 106
};
MMOVE_T(zboss_move_attack2a) = { FRAME_attak2a01, FRAME_attak2a08, zboss_frames_attack2a, zboss_reelInGrapple };

mframe_t zboss_frames_posthook[] =
{
	{ai_charge},	 // 136
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge}	 // 141
};
MMOVE_T(zboss_move_posthook) = { FRAME_lhook01, FRAME_lhook06, zboss_frames_posthook, zboss_run };

void zboss_posthook(edict_t* self)
{
	M_SetAnimation(self, &zboss_move_posthook);
}

void zboss_chooseHookRocket(edict_t* self)
{
	if (frandom() < 0.2 && !(self->monsterinfo.aiflags & AI_ONESHOTTARGET))
	{
		M_SetAnimation(self, &zboss_move_attack2a);
	}
	else
	{
		M_SetAnimation(self, &zboss_move_attack1a);
	}
}

mframe_t zboss_frames_prehook[] =
{
	{ai_charge},	// 57
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge}	// 66
};
MMOVE_T(zboss_move_prehook) = { FRAME_rhook01, FRAME_rhook10, zboss_frames_prehook, zboss_chooseHookRocket };

// Plasma Cannon

THINK(PlasmaballBlastAnim) (edict_t* ent) -> void
{
	ent->s.frame++;
	ent->s.skinnum++;

	if (ent->s.frame > 1)
	{
		G_FreeEdict(ent);
		return;
	}
	else
	{
		ent->nextthink = level.time + 0.1_sec;
	}
}

THINK(Plasmaball_Explode) (edict_t* ent) -> void
{
	//FIXME: if we are onground then raise our Z just a bit since we are a point?
	if (ent->enemy)
	{
		float	points;
		vec3_t	v;
		vec3_t	dir;

		v =ent->enemy->mins + ent->enemy->maxs;
		v = ent->enemy->s.origin + (v * 0.5);
		v = ent->s.origin - v;
		points = ent->dmg - 0.5 * v.length();
		dir = ent->enemy->s.origin - ent->s.origin;
		T_Damage(ent->enemy, ent, ent->owner, dir, ent->s.origin, vec3_origin, (int)points, (int)points, DAMAGE_RADIUS, MOD_UNKNOWN);
	}

	T_RadiusDamage(ent, ent->owner, ent->dmg, ent->enemy, ent->dmg_radius, DAMAGE_ENERGY, MOD_UNKNOWN);

	ent->s.origin = ent->s.origin + (ent->velocity * -0.02);
	ent->velocity = {};

	ent->movetype = MOVETYPE_NONE;
	ent->s.modelindex = gi.modelindex("models/objects/b_explode/tris.md2");
	ent->s.effects &= ~EF_BFG & ~EF_ANIM_ALLFAST;
	ent->s.frame = 0;
	ent->s.skinnum = 6;

	gi.sound(ent, CHAN_AUTO, sound_plamsaballexplode, 1, ATTN_NORM, 0);

	ent->think = PlasmaballBlastAnim;
	ent->nextthink = level.time + 0.1_sec;
}

TOUCH(Plasmaball_Touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (tr.surface && (tr.surface->flags & SURF_SKY))
	{
		G_FreeEdict(self);
		return;
	}

	self->enemy = other;
	Plasmaball_Explode(self);
}

void fire_plasmacannon(edict_t* self, vec3_t start, vec3_t aimdir, int damage, int speed, float damage_radius, float distance)
{
	edict_t* plasmaball;
	vec3_t	dir;
	vec3_t	forward, right, up;

	dir = vectoangles(aimdir);
	AngleVectors(dir, forward, right, up);

	plasmaball = G_Spawn();
	plasmaball->s.origin = start;
	plasmaball->velocity = aimdir * speed;
	plasmaball->velocity = plasmaball->velocity + (up * ((distance - 500) + crandom() * 10.0));
	plasmaball->velocity = plasmaball->velocity + (right * (crandom() * 10.0));
	plasmaball->avelocity = { 300, 300, 300 };
	plasmaball->movetype = MOVETYPE_BOUNCE;
	plasmaball->clipmask = MASK_SHOT;
	plasmaball->solid = SOLID_BBOX;
	plasmaball->mins = {};
	plasmaball->maxs = {};
	plasmaball->s.modelindex = gi.modelindex("sprites/plasma1.sp2");
	plasmaball->s.effects = EF_BFG | EF_ANIM_ALLFAST;
	plasmaball->owner = self;
	plasmaball->touch = Plasmaball_Touch;
	plasmaball->nextthink = level.time + 2.5_sec;
	plasmaball->think = Plasmaball_Explode;
	plasmaball->dmg = damage;
	plasmaball->dmg_radius = damage_radius;
	plasmaball->classname = "plasmaball";
	plasmaball->s.sound = sound_plamsaballfly;

	gi.sound(self, CHAN_AUTO, sound_plamsaballfire, 1, ATTN_NORM, 0);
	gi.linkentity(plasmaball);
}


static vec3_t cannonoffset[] =
{
	{-19, -44, 30},
	{-14, -33, 32},
	{-4 , -45, 32},
	{-2 , -34, 32},
	{  7, -49, 32},
	{  6, -36, 34},
	{  6, -36, 34},
};

void FireCannon(edict_t* self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;
	float distance;

	int offset = (self->s.frame - 119) / 2;

	if (EMPNukeCheck(self, self->s.origin))
	{
		gi.sound(self, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
		return;
	}
	
	AngleVectors(self->s.angles, forward, right, nullptr);

	start = G_ProjectSource(self->s.origin, cannonoffset[offset], forward, right);

	if (self->monsterinfo.aiflags & AI_ONESHOTTARGET)
	{
		vec = self->monsterinfo.shottarget;
	}
	else
	{
		vec = self->enemy->s.origin;
		vec[2] += self->enemy->viewheight;
	}

	if (self->timeout)
	{
		if (self->seq)
		{
			right = -right;
		}
		vec = vec + (right * self->timeout.seconds());

	}
	self->timeout -= 50_sec;

	dir = vec - start;
	dir.normalize();

	vec = self->enemy->s.origin - self->s.origin;
	distance = vec.length();

	if (distance < 700)
	{
		distance = 700;
	}

	if (skill->integer < 2)
	{
		fire_plasmacannon(self, start, dir, 90, 700, 90 + 40, distance);
	}
	else if (skill->integer < 3)
	{
		fire_plasmacannon(self, start, dir, 90, (int)(distance * 1.2), 90 + 40, distance);
	}
	else
	{
		fire_plasmacannon(self, start, dir, 90, (int)(distance * 1.6), 90 + 40, distance);
	}
}

mframe_t zboss_frames_attack3[] =
{
	{ai_charge, 0,	FireCannon},	// 119
	{ai_charge},
	{ai_charge, 0,	FireCannon},	// 121
	{ai_charge},
	{ai_charge, 0,	FireCannon},	// 123
	{ai_charge},
	{ai_charge, 0,	FireCannon},	// 125
	{ai_charge, 0,	zboss_idle},
	{ai_charge, 0,	FireCannon},	// 127
	{ai_charge},
	{ai_charge, 0,	FireCannon},	// 129
	{ai_charge},
	{ai_charge, 0,	FireCannon},	// 131
	{ai_charge}				// 132
};
MMOVE_T(zboss_move_attack3) = { FRAME_attak301, FRAME_attak314, zboss_frames_attack3, zboss_chooseNextAttack };


void zboss_fireCannons(edict_t* self)
{
	M_SetAnimation(self, &zboss_move_attack3);

	self->seq = 0;
	self->timeout = 150_sec;
}

mframe_t zboss_frames_precannon[] =
{
	{ai_charge},	// 67
	{ai_charge},
	{ai_charge},
	{ai_charge}	// 70
};
MMOVE_T(zboss_move_precannon) = { FRAME_rcannon01, FRAME_rcannon04, zboss_frames_precannon, zboss_fireCannons };

mframe_t zboss_frames_postcannon[] =
{
	{ai_charge},	// 133
	{ai_charge},
	{ai_charge}	// 135
};
MMOVE_T(zboss_move_postcannon) = { FRAME_lcannon01, FRAME_lcannon03, zboss_frames_postcannon, zboss_run };


void zboss_postcannon(edict_t* self)
{
	M_SetAnimation(self, &zboss_move_postcannon);
}

// switching in mid attack...

mframe_t zboss_frames_h2c[] =
{
	{ai_charge},	// 142
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge}	// 147
};
MMOVE_T(zboss_move_h2c) = { FRAME_h2c01, FRAME_h2c06, zboss_frames_h2c, zboss_fireCannons };


mframe_t zboss_frames_c2h[] =
{
	{ai_charge},	// 148
	{ai_charge},
	{ai_charge},
	{ai_charge, 0,	zboss_idle},
	{ai_charge},
	{ai_charge}	// 153
};
MMOVE_T(zboss_move_c2h) = { FRAME_c2h01, FRAME_c2h06, zboss_frames_c2h, zboss_chooseHookRocket };

void zboss_chooseNextAttack(edict_t* self)
{
	if (self->enemy == nullptr)
		return;

	self->monsterinfo.aiflags &= ~AI_ONESHOTTARGET;

	if (frandom() < 0.5 && self->enemy)
	{
		if (frandom() < 0.4)
		{
			if (self->monsterinfo.active_move == &zboss_move_attack3)
			{
				gi.sound(self, CHAN_BODY, sound_switchattacks, 1, ATTN_NORM, 0);
				M_SetAnimation(self, &zboss_move_c2h);
			}
			else
			{
				zboss_chooseHookRocket(self);
			}
		}
		else
		{
			if (self->monsterinfo.active_move == &zboss_move_attack3)
			{
				zboss_fireCannons(self);
			}
			else
			{
				gi.sound(self, CHAN_BODY, sound_switchattacks, 1, ATTN_NORM, 0);
				M_SetAnimation(self, &zboss_move_h2c);
			}
		}
	}
	else
	{
		gi.sound(self, CHAN_BODY, sound_lowergun, 1, ATTN_NORM, 0);

		if (self->monsterinfo.active_move == &zboss_move_attack3)
		{
			zboss_postcannon(self);
		}
		else
		{
			zboss_posthook(self);
		}
	}
}

MONSTERINFO_ATTACK(zboss_attack) (edict_t* self) -> void
{
	if (self->enemy == nullptr)
		return;

	gi.sound(self, CHAN_BODY, sound_raisegun, 1, ATTN_NORM, 0);

	if (frandom() < 0.4)
	{
		M_SetAnimation(self, &zboss_move_prehook);
	}
	else
	{
		M_SetAnimation(self, &zboss_move_precannon);
	}
}

/*
===
Death Stuff Starts
===
*/
static void zboss_gib(edict_t* self)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_EXPLOSION1_BIG);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	self->s.sound = 0;
	self->s.skinnum /= 2;

	ThrowGibs(self, 500, {
		{ 2, "models/objects/gibs/bone/tris.md2" },
		{ 3, "models/objects/gibs/sm_meat/tris.md2" },
		{ 6, "models/objects/gibs/sm_metal/tris.md2", GIB_METALLIC },
		{ 3, "models/objects/gibs/gear/tris.md2", GIB_METALLIC },
		{ "models/objects/gibs/chest/tris.md2" },
		{ "models/objects/gibs/head2/tris.md2", GIB_HEAD }
		});
}

void zboss_dead(edict_t* self)
{
	// no blowy on deady
	if (self->spawnflags.has(SPAWNFLAG_MONSTER_DEAD))
	{
		self->deadflag = false;
		self->takedamage = true;
		return;
	}

	zboss_gib(self);
}

mframe_t zboss_frames_death1[] =
{
	{ai_move, 0, BossExplode},	// 218
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},	// 227

	{ai_move}, // 228
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}	// 236
};
MMOVE_T(zboss_move_death1) = { FRAME_death101, FRAME_death119, zboss_frames_death1, zboss_dead };

void fire_deadrocket1(edict_t* self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	rocketoffset = { -26, -26, 25 };

	AngleVectors(self->s.angles, forward, right, nullptr);

	start = G_ProjectSource(self->s.origin, rocketoffset, forward, right);

	fire_rocket(self, start, forward, 70, 500, 70 + 20, 70);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(MZ2_BOSS2_ROCKET_1);
	gi.multicast(start, MULTICAST_PVS, false);
}

void fire_deadrocket2(edict_t* self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	rocketoffset = { -16, -21, 20 };

	AngleVectors(self->s.angles, forward, right, nullptr);

	start = G_ProjectSource(self->s.origin, rocketoffset, forward, right);

	forward[1] += 10;
	fire_rocket(self, start, forward, 70, 500, 70 + 20, 70);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(MZ2_BOSS2_ROCKET_1);
	gi.multicast(start, MULTICAST_PVS, false);
}

void fire_deadrocket3(edict_t* self)
{
	vec3_t	forward, right, up;
	vec3_t	start;
	vec3_t	rocketoffset = { -17, -20, 30 };

	AngleVectors(self->s.angles, forward, right, up);

	start = G_ProjectSource(self->s.origin, rocketoffset, forward, right);

	fire_rocket(self, start, up, 70, 500, 70 + 20, 70);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(MZ2_BOSS2_ROCKET_1);
	gi.multicast(start, MULTICAST_PVS, false);
}

void fire_deadrocket4(edict_t* self)
{

	vec3_t	forward, right, up;
	vec3_t	start;
	vec3_t	rocketoffset = { -8, -16, 17 };

	AngleVectors(self->s.angles, forward, right, up);

	start = G_ProjectSource(self->s.origin, rocketoffset, forward, right);

	fire_rocket(self, start, up, 70, 500, 70 + 20, 70);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(MZ2_BOSS2_ROCKET_1);
	gi.multicast(start, MULTICAST_PVS, false);
}

void fire_deadrocket5(edict_t* self)
{
	vec3_t	forward, right, up;
	vec3_t	start;
	vec3_t	rocketoffset = { -10, -16, 30 };

	AngleVectors(self->s.angles, forward, right, up);

	start = G_ProjectSource(self->s.origin, rocketoffset, forward, right);
	forward = -forward;

	fire_rocket(self, start, forward, 70, 500, 70 + 20, 70);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(MZ2_BOSS2_ROCKET_1);
	gi.multicast(start, MULTICAST_PVS, false);
}

void fire_deadrocket6(edict_t* self)
{
	vec3_t	forward, right, up;
	vec3_t	start;
	vec3_t	rocketoffset = { 0, -18, 25 };

	AngleVectors(self->s.angles, forward, right, up);

	start = G_ProjectSource(self->s.origin, rocketoffset, forward, right);
	forward = -forward;
	forward[1] -= 10;

	fire_rocket(self, start, forward, 70, 500, 70 + 20, 70);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(MZ2_BOSS2_ROCKET_1);
	gi.multicast(start, MULTICAST_PVS, false);
}

void fire_deadrocket7(edict_t* self)
{
	vec3_t	forward, right, up;
	vec3_t	start;
	vec3_t	rocketoffset = { 17, -27, 30 };

	AngleVectors(self->s.angles, forward, right, up);

	start = G_ProjectSource(self->s.origin, rocketoffset, forward, right);
	forward = -forward;
	forward[1] -= 10;

	fire_rocket(self, start, forward, 70, 500, 70 + 20, 70);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(MZ2_BOSS2_ROCKET_1);
	gi.multicast(start, MULTICAST_PVS, false);
}

void FireDeadCannon1(edict_t* self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	cannonoffset = { 9, -46, 33 };

	AngleVectors(self->s.angles, forward, right, nullptr);

	start = G_ProjectSource(self->s.origin, cannonoffset, forward, right);

	fire_plasmacannon(self, start, forward, 90, 700, 90 + 40, 700);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(MZ2_GUNNER_GRENADE_1);
	gi.multicast(start, MULTICAST_PVS, false);
}

void FireDeadCannon2(edict_t* self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	cannonoffset = { 3, -31, 37 };

	AngleVectors(self->s.angles, forward, right, nullptr);

	start = G_ProjectSource(self->s.origin, cannonoffset, forward, right);

	fire_plasmacannon(self, start, forward, 90, 700, 90 + 40, 700);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(MZ2_GUNNER_GRENADE_1);
	gi.multicast(start, MULTICAST_PVS, false);
}

void FireDeadCannon3(edict_t* self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	cannonoffset = { -21, -19, 24 };

	AngleVectors(self->s.angles, forward, right, nullptr);

	start = G_ProjectSource(self->s.origin, cannonoffset, forward, right);

	fire_plasmacannon(self, start, forward, 90, 700, 90 + 40, 700);

	gi.WriteByte(svc_muzzleflash2);
	gi.WriteShort(self - g_edicts);
	gi.WriteByte(MZ2_GUNNER_GRENADE_1);
	gi.multicast(start, MULTICAST_PVS, false);
}

TOUCH(deadhook_touch) (edict_t* ent, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (other == ent->owner)
		return;

	if (other->takedamage)
	{
		gi.sound(ent, CHAN_WEAPON, sound_hookimpact, 1, ATTN_NORM, 0);
		T_Damage(other, ent, ent->owner, ent->velocity, ent->s.origin, tr.plane.normal, 10, 0, DAMAGE_NONE, MOD_ROCKET);
	}

	G_FreeEdict(ent);
}

void fire_deadhook(edict_t* self)
{
	vec3_t	forward, right, up;
	vec3_t	start;
	vec3_t	hookoffset = { -35, 8, 28 };
	edict_t* hook;
	float speed;

	if (self->s.modelindex3 == 0)  // hook already out...
		return;

	AngleVectors(self->s.angles, forward, right, up);

	start = G_ProjectSource(self->s.origin, hookoffset, forward, right);

	self->s.modelindex3 = 0;

	speed = 500;

	gi.sound(self, CHAN_WEAPON, sound_hooklaunch, 1, ATTN_NORM, 0);

	hook = G_Spawn();
	hook->s.origin = start;
	hook->movedir = up;
	hook->s.angles = vectoangles(up);
	hook->velocity = up * speed;
	hook->movetype = MOVETYPE_FLYMISSILE;
	hook->clipmask = MASK_SHOT;
	hook->solid = SOLID_BBOX;
	hook->mins = {};
	hook->maxs = {};
	hook->s.modelindex = gi.modelindex("models/monsters/bossz/grapple/tris.md2");
	hook->s.frame = 282;
	hook->owner = self;
	hook->touch = deadhook_touch;
	hook->nextthink = level.time + 8000_ms / speed;
	hook->think = G_FreeEdict;
	hook->s.sound = sound_hookfly; // replace...
	hook->classname = "bosshook";

	gi.linkentity(hook);
}

mframe_t zboss_frames_death2[] =
{
	{ai_move},							// 237
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},							// 246

	{ai_move},							// 247
	{ai_move},
	{ai_move, 0,	fire_deadrocket1},	// 249
	{ai_move, 0,	fire_deadrocket2},	// 250
	{ai_move, 0,	fire_deadrocket3},	// 251
	{ai_move, 0,	fire_deadrocket4},	// 252
	{ai_move, 0,	fire_deadrocket5},	// 253
	{ai_move, 0,	fire_deadrocket6},	// 254
	{ai_move, 0,	fire_deadrocket7},	// 255
	{ai_move},						  // 256

	{ai_move, 0,	FireDeadCannon1},  // 257
	{ai_move, 0,	FireDeadCannon2},	// 258
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 0,	FireDeadCannon3},	// 264
	{ai_move, 0, BossExplode},
	{ai_move},							// 266

	{ai_move},							// 267
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},							// 276

	{ai_move},							// 277
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 0,	fire_deadhook}	// 281
};
MMOVE_T(zboss_move_death2) = { FRAME_death200, FRAME_death245, zboss_frames_death2, zboss_dead };

DIE(zboss_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{

	if (self->proboscus)
	{
		G_FreeEdict(self->proboscus);
		self->proboscus = nullptr;
	}

	if (self->spawnflags.has(SPAWNFLAG_MONSTER_DEAD))
	{
		// check for gib
		if (M_CheckGib(self, mod))
		{
			zboss_gib(self);
			self->deadflag = true;
			return;
		}

		if (self->deadflag)
			return;
	}
	else
	{
		self->deadflag = true;
		self->takedamage = false;
	}

	if (frandom() < 0.5)
	{
		gi.sound(self, CHAN_VOICE, sound_die1, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &zboss_move_death1);
	}
	else
	{
		gi.sound(self, CHAN_VOICE, sound_die2, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &zboss_move_death2);
	}
}


/*
===
End Death Stuff
===
*/

/*QUAKED monster_zboss (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/
void SP_monster_zboss(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_pain1.assign("bossz/bpain1.wav");
	sound_pain2.assign("bossz/bpain2.wav");
	sound_pain3.assign("bossz/bpain3.wav");
	sound_die1.assign("bossz/bdeth1.wav");
	sound_die2.assign("bossz/bdeth2.wav");
	sound_hooklaunch.assign("bossz/bhlaunch.wav");
	sound_hookimpact.assign("bossz/bhimpact.wav");
	sound_hookfly.assign("bossz/bhfly.wav");
	sound_sight.assign("bossz/bsight1.wav");
	sound_swing.assign("bossz/bswing.wav");
	sound_idle1.assign("bossz/bidle1.wav");
	sound_idle2.assign("bossz/bidle2.wav");
	sound_walk.assign("bossz/bwalk.wav");
	sound_raisegun.assign("bossz/braisegun.wav");
	sound_lowergun.assign("bossz/blowergun.wav");
	sound_switchattacks.assign("bossz/bswitch.wav");
	sound_plamsaballfly.assign("bossz/bpbfly.wav");
	sound_plamsaballexplode.assign("bossz/bpbexplode.wav");
	sound_plamsaballfire.assign("bossz/bpbfire.wav");
	sound_taunt1.assign("bossz/btaunt1.wav");
	sound_taunt2.assign("bossz/btaunt2.wav");
	sound_taunt3.assign("bossz/btaunt3.wav");

	gi.modelindex("sprites/plasma1.sp2");
	gi.modelindex("models/objects/b_explode/tris.md2");
	gi.soundindex("items/empnuke/emp_trg.wav");

	self->s.modelindex = gi.modelindex("models/monsters/bossz/mech/tris.md2");
	self->s.modelindex2 = gi.modelindex("models/monsters/bossz/pilot/tris.md2");
	self->s.modelindex3 = gi.modelindex("models/monsters/bossz/grapple/tris.md2");
	self->mins = { -24, -24, -30 };
	self->maxs = { 24, 24, 74 };
	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->health = max(4500, 4500 + 1250 * (skill->integer - 1)) * st.health_multiplier;
	if (!st.was_key_specified("armor_type"))
		self->monsterinfo.armor_type = IT_ARMOR_BODY;
	if (!st.was_key_specified("armor_power"))
		self->monsterinfo.armor_power = max(500, 500 + 150 * (skill->integer - 1));
	if (!st.was_key_specified("power_armor_type"))
		self->monsterinfo.power_armor_type = IT_ITEM_POWER_SHIELD;
	if (!st.was_key_specified("power_armor_power"))
		self->monsterinfo.power_armor_power = max(500, 500 + 150 * (skill->integer - 1));
	if (coop->integer)
	{
		self->health += (500 * skill->integer) + (500 * (CountPlayers() - 1));
		self->monsterinfo.armor_power += (250 * skill->integer) + (250 * (CountPlayers() - 1));
		self->monsterinfo.power_armor_power += (250 * skill->integer) + (250 * (CountPlayers() - 1));
	}

	self->gib_health = -700;
	self->mass = 1000;

	self->pain = zboss_pain;
	self->die = zboss_die;

	self->monsterinfo.stand = zboss_stand;
	self->monsterinfo.walk = zboss_walk;
	self->monsterinfo.run = zboss_run;
	self->monsterinfo.attack = zboss_attack;
	self->monsterinfo.melee = zboss_melee;
	self->monsterinfo.sight = zboss_sight;
	self->monsterinfo.idle = zboss_idle;
	self->monsterinfo.setskin = zboss_setskin;

	gi.linkentity(self);

	M_SetAnimation(self, &zboss_movestand1);
	self->monsterinfo.scale = MODEL_SCALE;

	walkmonster_start(self);

	self->monsterinfo.aiflags |= AI_IGNORE_SHOTS;
}

/*QUAKED target_zboss_target
*/

USE(trigger_zboss) (edict_t* self, edict_t* other, edict_t* activator) -> void
{
	edict_t* boss = NULL;

	while ((boss = G_FindByString<&edict_t::targetname>(boss, self->target)) != nullptr)
	{
		if (boss->health > 0)
		{
			boss->monsterinfo.shottarget = self->s.origin;
			boss->monsterinfo.aiflags |= AI_ONESHOTTARGET;
			boss->monsterinfo.attack(boss);
		}
	}
}

void SP_target_zboss(edict_t* self)
{
	if (!self->target)
	{
		G_FreeEdict(self);
		return;
	}

	self->movetype = MOVETYPE_NONE;
	self->svflags |= SVF_NOCLIENT;

	self->solid = SOLID_NOT;
	self->use = trigger_zboss;

	gi.linkentity(self);
}
