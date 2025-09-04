/*
==============================================================================

QUAKE WYVERN

==============================================================================
*/

#include "../g_local.h"
#include "m_wyvern.h"

static cached_soundindex	sound_sight1;
static cached_soundindex	sound_sight2;
static cached_soundindex	sound_search1;
static cached_soundindex	sound_search2;
static cached_soundindex	sound_search3;
static cached_soundindex	sound_attack;
static cached_soundindex	sound_die1;
static cached_soundindex	sound_die2;
static cached_soundindex	sound_pain1;
static cached_soundindex	sound_pain2;
static cached_soundindex	sound_flame;

void wyvern_reattack(edict_t* self);

MONSTERINFO_SIGHT(wyvern_sight) (edict_t* self, edict_t* other) -> void
{
	if (frandom() > 0.5f)
		gi.sound(self, CHAN_VOICE, sound_sight2, 1, ATTN_NONE, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_sight1, 1, ATTN_NONE, 0);
}

MONSTERINFO_SEARCH(wyvern_search) (edict_t* self) -> void
{
	if (frandom() > 0.66f)
		gi.sound(self, CHAN_VOICE, sound_search3, 1, ATTN_NONE, 0);
	else if (frandom() > 0.33f)
		gi.sound(self, CHAN_VOICE, sound_search2, 1, ATTN_NONE, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_search1, 1, ATTN_NONE, 0);
}

//================
// HOVER
//================
mframe_t wyvern_frames_hover [] =
{
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},

	{ai_stand},
	{ai_stand},
	{ai_stand}
};
MMOVE_T(wyvern_move_hover) = {FRAME_fly1, FRAME_fly8, wyvern_frames_hover, nullptr };

MONSTERINFO_STAND(wyvern_hover) (edict_t* self) -> void
{
	M_SetAnimation(self, &wyvern_move_hover);
}

//================
// WALK
//================
mframe_t wyvern_frames_walk [] =
{
	{ai_walk, 5},
	{ai_walk, 5},
	{ai_walk, 5},
	{ai_walk, 5},
	{ai_walk, 5},

	{ai_walk, 5},
	{ai_walk, 5},
	{ai_walk, 5}
};
MMOVE_T(wyvern_move_walk) = {FRAME_fly1, FRAME_fly8, wyvern_frames_walk, nullptr };


MONSTERINFO_WALK(wyvern_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &wyvern_move_walk);
}

//================
// FLY
//================
mframe_t wyvern_frames_run[] =
{
	{ai_run, 10},
	{ai_run, 10},
	{ai_run, 10},
	{ai_run, 10},
	{ai_run, 10},

	{ai_run, 10},
	{ai_run, 10},
	{ai_run, 10}
};
MMOVE_T(wyvern_move_run) = { FRAME_fly1, FRAME_fly8, wyvern_frames_run, nullptr };


MONSTERINFO_RUN(wyvern_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &wyvern_move_run);
}

//================
// PAIN
//================
mframe_t wyvern_frames_pain1 [] =
{	
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },

	{ ai_move }
};
MMOVE_T(wyvern_move_pain1) = {FRAME_pain1, FRAME_pain6, wyvern_frames_pain1, wyvern_run };

mframe_t wyvern_frames_pain2[] =
{
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },

	{ ai_move }
};
MMOVE_T(wyvern_move_pain2) = { FRAME_painb1, FRAME_painb6, wyvern_frames_pain2, wyvern_run };

mframe_t wyvern_frames_pain3[] =
{
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },

	{ ai_move }
};
MMOVE_T(wyvern_move_pain3) = { FRAME_painc1, FRAME_painc6, wyvern_frames_pain3, wyvern_run };

PAIN(wyvern_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 1_sec;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	if (damage < 30)
	{
		gi.sound(self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &wyvern_move_pain1);
	}
	else
	{
		gi.sound(self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);
		if (frandom() >= 0.5)
			M_SetAnimation(self, &wyvern_move_pain2);
		else
			M_SetAnimation(self, &wyvern_move_pain3);
	}
}

MONSTERINFO_SETSKIN(wyvern_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//================
// DEAD
//================
static void wyvern_gib(edict_t* self)
{
	gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

	self->s.skinnum /= 2;

	ThrowGibs(self, 1000, {
		{ 2, "models/objects/gibs/bone/tris.md2" },
		{ 4, "models/objects/gibs/sm_meat/tris.md2" },
		{ "models/monsters/wyvern/gibs/tail.md2", GIB_SKINNED | GIB_HEAD },
		{ "models/monsters/wyvern/gibs/rwing.md2", GIB_SKINNED },
		{ "models/monsters/wyvern/gibs/lwing.md2", GIB_SKINNED },
		//{ "models/monsters/wyvern/gibs/head.md2", GIB_SKINNED | GIB_HEAD }
		});
}

THINK(wyvern_deadthink) (edict_t* self) -> void
{
	if (!self->groundentity && level.time < self->timestamp)
	{
		self->nextthink = level.time + FRAME_TIME_S;
		return;
	}

	return;
}

void wyvern_dead(edict_t * self)
{
	self->mins = { -144, -136, -36 };
	self->maxs = { 88, 128, 24 };
	self->movetype = MOVETYPE_TOSS;
	self->think = wyvern_deadthink;
	self->nextthink = level.time + FRAME_TIME_S;
	self->timestamp = level.time + 15_sec;
	gi.linkentity(self);

	wyvern_gib(self);
}

mframe_t wyvern_frames_die1 [] =
{	
	{ai_move, 0, Q1BossExplode},
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
MMOVE_T(wyvern_move_die1) = {FRAME_die1, FRAME_die13, wyvern_frames_die1, wyvern_dead};

mframe_t wyvern_frames_die2[] =
{
	{ai_move, 0, Q1BossExplode},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},

	{ai_move}
};
MMOVE_T(wyvern_move_die2) = { FRAME_dieb1, FRAME_dieb6, wyvern_frames_die2, wyvern_dead };

DIE(wyvern_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		wyvern_gib(self);

		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	if (frandom() > 0.5f)
	{
		gi.sound(self, CHAN_VOICE, sound_die1, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &wyvern_move_die1);
	}
	else
	{
		gi.sound(self, CHAN_VOICE, sound_die2, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &wyvern_move_die2);
	}
	self->deadflag = true;
	self->takedamage = true;
}

//================
// ATTACK
//================

void wyvern_fireball (edict_t *self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;
	trace_t trace; // PMM - check target
	vec3_t target;
	bool   blindfire = false;
	vec3_t		offset = { 73, 0, -22 };
	int speed = 750;
	int damage = 100;

	if (self->monsterinfo.aiflags & AI_MANUAL_STEERING)
		blindfire = true;
	else
		blindfire = false;

	if (!self->enemy || !self->enemy->inuse)
		return;

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, offset, forward, right);

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
		PredictAim(self, self->enemy, start, 750, false, 0.f, &dir, &vec);

	dir.normalize();

	trace = gi.traceline(start, vec, self, MASK_PROJECTILE);
	if (blindfire)
	{
		if (!(trace.startsolid || trace.allsolid || (trace.fraction < 0.5f)))
			fire_lavaball(self, start, dir, damage, speed, (float)damage, damage);
		else
		{
			vec = target;
			vec += (right * -10);
			dir = vec - start;
			dir.normalize();
			trace = gi.traceline(start, vec, self, MASK_PROJECTILE);
			if (!(trace.startsolid || trace.allsolid || (trace.fraction < 0.5f)))
				fire_lavaball(self, start, dir, damage, speed, (float)damage, damage);
			else
			{
				vec = target;
				vec += (right * 10);
				dir = vec - start;
				dir.normalize();
				trace = gi.traceline(start, vec, self, MASK_PROJECTILE);
				if (!(trace.startsolid || trace.allsolid || (trace.fraction < 0.5f)))
					fire_lavaball(self, start, dir, damage, speed, (float)damage, damage);
			}
		}
	}
	else
	{
		if (trace.fraction > 0.5f || trace.ent->solid != SOLID_BSP)
			fire_lavaball(self, start, dir, damage, speed, (float)damage, damage);
	}
	gi.sound(self, CHAN_VOICE, sound_attack, 1, ATTN_NORM, 0);
}

void wyvern_firebreath(edict_t* self)
{
	vec3_t					offset = { 73, 0, -22 };
	vec3_t					 start;
	vec3_t					 forward, right, up;
	vec3_t					 aim;
	vec3_t					 dir;
	vec3_t					 end;
	float					 r, u;
	vec3_t					 aim_good;
	int						fireCount;

	fireCount = min(3, 1 * (skill->integer + 1));

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, offset, forward, right);

	end = self->enemy->s.origin;
	end[2] += self->enemy->viewheight;
	aim = end - start;
	aim_good = end;

	dir = vectoangles(aim);
	AngleVectors(dir, forward, right, up);

	while (fireCount > 0)
	{
		r = crandom() * 2000;
		u = crandom() * 1000;

		end = start + (forward * 8192);
		end += (right * r);
		end += (up * u);

		aim = end - start;
		aim.normalize();

		fire_flame(self, start, aim, 12, 500);
		gi.sound(self, CHAN_VOICE, sound_attack, 1, ATTN_NORM, 0);

		fireCount -= 1;
	}
}

mframe_t wyvern_frames_attack1 [] =
{
	{ai_charge, 45},
	{ai_charge, 30},
	{ai_charge, 15},					
	{ai_charge,  0},				
	{ai_charge,  0},			
	{ai_charge,  0,	wyvern_fireball},
	{ai_charge},
	{ai_charge},
	{ai_charge, 15, wyvern_reattack}
};
MMOVE_T(wyvern_move_attack1) = { FRAME_attack1, FRAME_attack9, wyvern_frames_attack1, nullptr };

mframe_t wyvern_frames_attack2[] =
{
	{ai_charge, 0},
	{ai_charge, 0},
	{ai_charge, 0, wyvern_firebreath},
	{ai_charge,  0, wyvern_firebreath},
	{ai_charge,  0, wyvern_firebreath},
	{ai_charge,  0, wyvern_firebreath},
	{ai_charge},
	{ai_charge},
	{ai_charge, 15, wyvern_reattack}
};
MMOVE_T(wyvern_move_attack2) = { FRAME_attack1, FRAME_attack9, wyvern_frames_attack2, nullptr };

MONSTERINFO_ATTACK(wyvern_attack) (edict_t* self) -> void
{

	if (!M_CheckClearShot(self, { 0, 96, 32 }))
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
		M_SetAnimation(self, &wyvern_move_attack1);
		self->monsterinfo.attack_finished = level.time + random_time(2_sec);
		return;
	}
	else
	{
		if (range_to(self, self->enemy) <= (RANGE_NEAR) && frandom() >= 0.5f)
			M_SetAnimation(self, &wyvern_move_attack2);
		else if (range_to(self, self->enemy) <= (RANGE_NEAR/3))
			M_SetAnimation(self, &wyvern_move_attack2);
		else
			M_SetAnimation(self, &wyvern_move_attack1);
	}
}

void wyvern_reattack(edict_t* self)
{
	if (self->enemy->health > 0)
		if (visible(self, self->enemy))
			if (frandom() <= 0.6f)
			{
				M_SetAnimation(self, &wyvern_move_attack1);
				return;
			}
	wyvern_run(self);
}

MONSTERINFO_CHECKATTACK(wyvern_checkattack) (edict_t* self) -> bool
{
	if (!self->enemy)
		return false;

	return M_CheckAttack_Base(self, 0.4f, 0.8f, 0.8f, 0.8f, 0.f, 0.f);
}

static void wyvern_set_fly_parameters(edict_t* self)
{
	self->monsterinfo.fly_thrusters = false;
	self->monsterinfo.fly_acceleration = 20.f;
	self->monsterinfo.fly_speed = 120.f;
	self->monsterinfo.fly_min_distance = 550.f;
	self->monsterinfo.fly_max_distance = 750.f;
}

/*QUAKED monster_wyvern(1 .5 0) (-16 - 16 - 24) (16 16 32) Ambush Trigger_Spawn Sight
model="models/monsters/wyvern/tris.md2"
*/

void SP_monster_wyvern(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_sight1.assign("wyvern/sight1.wav");
	sound_sight2.assign("wyvern/sight2.wav");
	sound_search1.assign("wyvern/idle1.wav");
	sound_search2.assign("wyvern/idle2.wav");
	sound_search3.assign("wyvern/idlefly.wav");
	sound_pain1.assign("wyvern/pain1.wav");
	sound_pain2.assign("wyvern/pain2.wav");
	sound_die1.assign("wyvern/death.wav");
	sound_die2.assign("wyvern/death2.wav");
	sound_attack.assign("wyvern/fire.wav");
	sound_flame.assign("hknight/attack1.wav");
	
	self->s.modelindex = gi.modelindex ("models/monsters/wyvern/tris.md2");

	self->mins = { -48, -64, -36 };
	self->maxs = { 48, 64, 24 };
	
	self->movetype = MOVETYPE_FLY;
	self->solid = SOLID_BBOX;

	self->health = max(2000, 2000 + 1000 * (skill->integer - 1)) * st.health_multiplier;
	if (!st.was_key_specified("armor_type"))
		self->monsterinfo.armor_type = IT_ARMOR_BODY;
	if (!st.was_key_specified("armor_power"))
		self->monsterinfo.armor_power = max(350, 350 + 100 * (skill->integer - 1));
	self->gib_health = -500;
	self->mass = 500;
	if (coop->integer)
	{
		self->health += (250 * (CountPlayers() - 1));
		self->monsterinfo.armor_power += (100 * (CountPlayers() - 1));
	}

	self->pain = wyvern_pain;
	self->die = wyvern_die;

	self->yaw_speed = 20;
	
	self->monsterinfo.stand = wyvern_hover;
	self->monsterinfo.walk = wyvern_walk;
	self->monsterinfo.run = wyvern_run;
	self->monsterinfo.attack = wyvern_attack;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.sight = wyvern_sight;
	self->monsterinfo.search = wyvern_search;
	self->monsterinfo.setskin = wyvern_setskin;
	self->monsterinfo.checkattack = wyvern_checkattack;
	
	gi.linkentity (self);

	M_SetAnimation(self, &wyvern_move_hover);	
	self->monsterinfo.scale = MODEL_SCALE;
	
	flymonster_start(self);

	self->monsterinfo.aiflags |= AI_ALTERNATE_FLY;
	wyvern_set_fly_parameters(self);
}
