/*
==============================================================================

QUAKE SHALRATH

==============================================================================
*/

#include "../g_local.h"
#include "m_shalrath.h"

static cached_soundindex sound_death;
static cached_soundindex sound_gib;
static cached_soundindex sound_sight;
static cached_soundindex sound_pain1;
static cached_soundindex sound_attack;
static cached_soundindex sound_attack2;
static cached_soundindex sound_idle;

void shalrath_stand(edict_t* self);

// STAND
MONSTERINFO_SEARCH(shalrath_search) (edict_t* self) -> void
{
	if (frandom() < 0.2)
		gi.sound (self, CHAN_BODY, sound_idle, 1, ATTN_IDLE, 0);
}


mframe_t shalrath_frames_stand [] =
{
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},

	{ai_stand},
	{ai_stand}
};
MMOVE_T(shalrath_move_stand) = {FRAME_stand1, FRAME_stand7, shalrath_frames_stand, shalrath_stand};

MONSTERINFO_STAND(shalrath_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &shalrath_move_stand);
}

//
// WALK
//

mframe_t shalrath_frames_walk [] =
{
	{ai_walk, 6},
	{ai_walk, 4},
	{ai_walk},
	{ai_walk, 6},
	{ai_walk, 5},

	{ai_walk, 4},
	{ai_walk, 5}
};
MMOVE_T(shalrath_move_walk) = {FRAME_walk1, FRAME_walk7, shalrath_frames_walk, NULL};

MONSTERINFO_WALK(shalrath_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &shalrath_move_walk);
}

//
// RUN
//

void shalrath_run (edict_t *self);

mframe_t shalrath_frames_run [] =
{
	{ai_run, 6},
	{ai_run, 4},
	{ai_run},
	{ai_run, 6},
	{ai_run, 5},

	{ai_run, 4},
	{ai_run, 5}
};
MMOVE_T(shalrath_move_run) = {FRAME_walk1, FRAME_walk7, shalrath_frames_run, shalrath_run};

MONSTERINFO_RUN(shalrath_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
	{
		M_SetAnimation(self, &shalrath_move_stand);
		return;
	}
	else
		M_SetAnimation(self, &shalrath_move_run);
}

//
// PAIN
//

mframe_t shalrath_frames_pain1 [] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(shalrath_move_pain1) = {FRAME_pain1, FRAME_pain6, shalrath_frames_pain1, shalrath_run};

mframe_t shalrath_frames_pain2[] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(shalrath_move_pain2) = { FRAME_paina1, FRAME_paina6, shalrath_frames_pain2, shalrath_run };

PAIN(shalrath_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;
	
	self->pain_debounce_time = level.time + 3_sec;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	gi.sound (self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);

	if (frandom() > 0.5f)
		M_SetAnimation(self, &shalrath_move_pain1);
	else
		M_SetAnimation(self, &shalrath_move_pain2);
}

MONSTERINFO_SETSKIN(shalrath_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
// ATTACK
//

void shalrath_fire (edict_t *self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;
	trace_t trace; // PMM - check target
	int		rocketSpeed;
	vec3_t target;
	bool   blindfire = false;
	vec3_t offset = { 0, 0, 10 };
	int damage = 50;

	if (self->monsterinfo.aiflags & AI_MANUAL_STEERING)
		blindfire = true;
	else
		blindfire = false;

	if (!self->enemy || !self->enemy->inuse)
		return;

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, offset, forward, right);

	rocketSpeed = 600;

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
	
	gi.sound(self, CHAN_WEAPON|CHAN_RELIABLE, sound_attack2, 1, ATTN_NORM, 0);
}

void shalrath_attack_snd (edict_t *self)
{
	gi.sound (self, CHAN_AUTO, sound_attack, 1, ATTN_NORM, 0);
}

mframe_t shalrath_frames_attack [] =
{
	{ai_charge, 0, shalrath_attack_snd},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},

	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},

	{ai_charge},
	{ai_charge, 0, shalrath_fire},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	
	{ai_charge}
};
MMOVE_T(shalrath_move_attack) = {FRAME_throw1, FRAME_throw16, shalrath_frames_attack, shalrath_run};

MONSTERINFO_ATTACK(shalrath_attack) (edict_t* self) -> void
{
	vec3_t offset = { 0, 0, 10 };

	if (!M_CheckClearShot(self, offset))
		return;

	float r, chance;

	if (self->monsterinfo.attack_state == AS_BLIND)
	{
		// setup shot probabilities
		if (self->monsterinfo.blind_fire_delay < 1.0_sec)
			chance = 1.0;
		else if (self->monsterinfo.blind_fire_delay < 7.5_sec)
			chance = 0.4f;
		else
			chance = 0.1f;

		r = frandom();

		// minimum of 5.5 seconds, plus 0-1, after the shots are done
		self->monsterinfo.blind_fire_delay += random_time(5.5_sec, 6.5_sec);

		// don't shoot at the origin
		if (!self->monsterinfo.blind_fire_target)
			return;

		// don't shoot if the dice say not to
		if (r > chance)
			return;

		// turn on manual steering to signal both manual steering and blindfire
		self->monsterinfo.aiflags |= AI_MANUAL_STEERING;
		M_SetAnimation(self, &shalrath_move_attack);
		self->monsterinfo.attack_finished = level.time + random_time(2_sec);
		return;
	}

	M_SetAnimation(self, &shalrath_move_attack);
}

//
// SIGHT
//

MONSTERINFO_SIGHT(shalrath_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound (self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

//
// DEATH
//

void shalrath_dead (edict_t *self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, -8 };
	monster_dead(self);
}

mframe_t shalrath_frames_death1 [] =
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
	{ai_move}
};
MMOVE_T(shalrath_move_death) = {FRAME_die1, FRAME_die17, shalrath_frames_death1, shalrath_dead};

DIE(shalrath_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 3, "models/objects/gibs/sm_meat/tris.md2" },
			//{ "models/monsters/shalrath/gibs/chest.md2", },
			//{ "models/monsters/shalrath/gibs/claw.md2", GIB_UPRIGHT },
			{ "models/monsters/shalrath/gibs/head.md2", GIB_SKINNED | GIB_HEAD }
			});

		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	// regular death
	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	M_SetAnimation(self, &shalrath_move_death);
}


/*QUAKED monster_shalrath (1 .5 0) (-16 -16 -24) (16 16 40) Ambush Trigger_Spawn Sight
model="models/monsters/shalrath/tris.md2"
*/
void SP_monster_shalrath (edict_t *self)
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

	self->s.modelindex = gi.modelindex ("models/monsters/shalrath/tris.md2");
	self->mins = { -32, -32, -24 };
	self->maxs = { 32, 32, 48 };

	self->health = 400 * st.health_multiplier;
	self->gib_health = -80;
	self->mass = 125;

	self->pain = shalrath_pain;
	self->die = shalrath_die;



	self->monsterinfo.stand = shalrath_stand;
	self->monsterinfo.walk = shalrath_walk;
	self->monsterinfo.run = shalrath_run;
	self->monsterinfo.attack = shalrath_attack;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.sight = shalrath_sight;
	self->monsterinfo.search = shalrath_search;
	self->monsterinfo.setskin = shalrath_setskin;

	gi.linkentity (self);

	M_SetAnimation(self, &shalrath_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	walkmonster_start (self);
}