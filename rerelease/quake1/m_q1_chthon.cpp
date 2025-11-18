/*
==============================================================================

CHTHON - MG1 STYLE

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_chthon.h"

static cached_soundindex	sound_pain;
static cached_soundindex	sound_death;
static cached_soundindex	sound_sight;
static cached_soundindex	sound_throw;
static cached_soundindex	sound_rise;

void chthon_stand(edict_t* self);
void chthon_run(edict_t* self);
void chthon_rise(edict_t* self);
static void chthon_check_attack(edict_t* self);

static void chthon_rise_sound (edict_t *self)
{
	gi.sound (self, CHAN_VOICE, sound_rise, 1, ATTN_NORM, 0);
}

static void chthon_sight_sound (edict_t *self)
{
	gi.sound (self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

static void chthon_sight_sound2 (edict_t *self)
{
	if(frandom() < 0.1f)
	gi.sound (self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

mframe_t chthon_frames_stand [] = {
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand, 0, chthon_check_attack},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand, 0, chthon_check_attack},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand, 0, chthon_check_attack},
	{ai_stand},
	{ai_stand},
	{ai_stand, 0, chthon_sight_sound},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand, 0, chthon_check_attack},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand, 0, chthon_check_attack},
	{ai_stand}
};
MMOVE_T(chthon_move_stand) = {FRAME_walk1, FRAME_walk31, chthon_frames_stand, nullptr };

MONSTERINFO_STAND(chthon_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &chthon_move_stand);
}

mframe_t chthon_frames_rise [] =
{
	{ai_move, 0, chthon_rise_sound},
	{ai_move},
	{ai_move, 0, chthon_sight_sound},
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
	{ai_move, 0, chthon_stand}
};
MMOVE_T(chthon_move_rise) = {FRAME_rise1, FRAME_rise17, chthon_frames_rise, nullptr };

void chthon_rise(edict_t *self)
{
	M_SetAnimation(self, &chthon_move_rise);
}

mframe_t chthon_frames_walk [] =
{
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk, 0, chthon_sight_sound2},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk, 0, chthon_check_attack},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk, 0, chthon_check_attack}
};
MMOVE_T(chthon_move_walk) = {FRAME_walk1, FRAME_walk31, chthon_frames_walk, nullptr };

MONSTERINFO_WALK(chthon_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &chthon_move_walk);
}

mframe_t chthon_frames_run [] =
{
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, chthon_sight_sound2},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, chthon_check_attack},
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
	{ai_charge, 0, chthon_check_attack},
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
	{ai_charge, 0, chthon_check_attack}
};
MMOVE_T(chthon_move_run) = {FRAME_walk1, FRAME_walk31, chthon_frames_walk, nullptr };

MONSTERINFO_RUN(chthon_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &chthon_move_run);
}

/// 
/// PAIN
/// 

mframe_t chthon_frames_shock1 [] =
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
	{ai_move, 0,	 chthon_check_attack}
};
MMOVE_T(chthon_frames_move_shock1) = {FRAME_shocka1, FRAME_shocka10, chthon_frames_shock1, chthon_walk};

mframe_t chthon_frames_shock2 [] =
{
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, 0,  chthon_check_attack}
};
MMOVE_T(chthon_frames_move_shock2) = {FRAME_shockb1, FRAME_shockb6, chthon_frames_shock2, chthon_walk};

mframe_t chthon_frames_shock3 [] =
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
	{ai_move, 0,	 chthon_check_attack}
};
MMOVE_T(chthon_frames_move_shock3) = {FRAME_shockc1, FRAME_shockc10, chthon_frames_shock3, chthon_walk};

PAIN(chthon_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{

	//Only energy weapons can damage him IF IT WORKED
	if (!IsEnergyWeapon(mod)) {
		self->health += damage;
		return;
	}

	if (level.time < self->pain_debounce_time)
		return;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	self->pain_debounce_time = level.time + 6_sec;
	gi.sound (self, CHAN_VOICE, sound_pain, 1.0, ATTN_NORM, 0);

	if(damage > 25)
	{
		if (self->health < 1500)
		{
			M_SetAnimation(self, &chthon_frames_move_shock1);
		
		}
		else if (self->health < 1000)
		{
			M_SetAnimation(self, &chthon_frames_move_shock2);
		}
		else if (self->health < 500)
		{
			M_SetAnimation(self, &chthon_frames_move_shock3);
		}
	}
}
MONSTERINFO_SETSKIN(chthon_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

///
/// DEATH
/// 

static void chthon_gib(edict_t* self)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_EXPLOSION1_BIG);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	self->s.sound = 0;
	self->s.skinnum /= 2;

	ThrowGibs(self, 500, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 1, "models/objects/gibs/bone2/tris.md2" },
			{ 3, "models/objects/gibs/sm_meat/tris.md2" },
			{ "models/objects/gibs/sm_meat/tris.md2" },
			{ 10, "models/monsters/shambler/gibs/head.md2" },
			{ 10, "models/monsters/zombie/gibs/head.md2" },
			{ "models/objects/gibs/chest/tris.md2", GIB_HEAD }
		});
}

void chthon_dead(edict_t *self)
{
	if (self->spawnflags.has(SPAWNFLAG_MONSTER_DEAD))
	{
		self->deadflag = false;
		self->takedamage = true;
		return;
	}

	chthon_gib(self);
}

mframe_t chthon_frames_death [] =
{
	{ai_move, 0, BossGibs},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(chthon_move_death) = {FRAME_death1, FRAME_death9, chthon_frames_death, chthon_dead};


DIE(chthon_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (self->spawnflags.has(SPAWNFLAG_MONSTER_DEAD))
	{
		// check for gib
		if (M_CheckGib(self, mod))
		{
			chthon_gib(self);
			self->deadflag = true;
			return;
		}

		if (self->deadflag)
			return;
	}
	else
	{
		gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
		self->deadflag = true;
		self->takedamage = false;
	}

	M_SetAnimation(self, &chthon_move_death);
}

void chthon_bolt(edict_t *self, int no)
{
	switch(no)
	{
	case 1:

		gi.sound (self, CHAN_VOICE, sound_pain, 1.0, ATTN_NORM, 0);	
		M_SetAnimation(self, &chthon_frames_move_shock1);
		self->pain_debounce_time = level.time + 6_sec;
		break;
	case 2:
		gi.sound (self, CHAN_VOICE, sound_pain, 1.0, ATTN_NORM, 0);	
		M_SetAnimation(self, &chthon_frames_move_shock2);
		self->pain_debounce_time = level.time + 6_sec;
		break;
	case 3:
		gi.sound (self, CHAN_VOICE, sound_pain, 1.0, ATTN_NORM, 0);	
		M_SetAnimation(self, &chthon_frames_move_shock3);
		self->pain_debounce_time = level.time + 6_sec;
		self->health = -50;
		gi.sound (self, CHAN_VOICE, sound_death, 1, ATTN_NONE, 0);
		self->deadflag = true;
		self->takedamage = true;
		M_SetAnimation(self, &chthon_move_death);
		monster_death_use(self);
		break;
	}
}

void chthon_attack_left (edict_t *self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;
	trace_t trace;
	vec3_t target;
	bool   blindfire = false;
	vec3_t offset = { 36, 160, 200 };
	int damage;
	int speed = 750;

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

	if (strcmp(self->classname, "monster_lavaman") == 0)
	{
		damage = 40;
	}
	else
	{
		damage = 100;
	}

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
}	

void chthon_attack_right (edict_t *self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;
	trace_t trace;
	vec3_t target;
	bool   blindfire = false;
	vec3_t  offset = { 36, -160, 200 };
	int damage;
	int speed = 750;

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

	if (strcmp(self->classname, "monster_lavaman") == 0)
	{
		damage = 40;
	}
	else
	{
		damage = 100;
	}

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
}	

mframe_t chthon_frames_attack [] =
{
	{ai_charge},
	{ai_charge, 0, chthon_sight_sound2},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, chthon_attack_left},
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
	{ai_charge, 0, chthon_attack_right},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, chthon_check_attack}
};
MMOVE_T(chthon_move_attack) = {FRAME_attack1, FRAME_attack23, chthon_frames_attack, chthon_walk};

MONSTERINFO_ATTACK(chthon_attack) (edict_t* self) -> void
{
	M_SetAnimation(self, &chthon_move_attack);
}

MONSTERINFO_SIGHT(chthon_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound (self, CHAN_VOICE, sound_sight, 1, ATTN_IDLE, 0);
}

static void chthon_check_attack (edict_t *self)
{
	if (!self->enemy || !self->enemy->inuse || self->enemy->health <= 0)
		return;
	M_SetAnimation(self, &chthon_move_attack);
}

MONSTERINFO_CHECKATTACK(chthon_checkattack) (edict_t* self) -> bool
{
	return M_CheckAttack_Base(self, 0.4f, 0.8f, 0.8f, 0.8f, 0.f, 0.f);
}

void SP_monster_chthon (edict_t *self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_pain.assign("chthon/pain.wav");	
	sound_death.assign("chthon/death.wav");
	sound_rise.assign("chthon/out1.wav");
	sound_throw.assign("chthon/throw.wav");
	sound_sight.assign("chthon/sight1.wav");
	
	self->movetype = MOVETYPE_NONE;

	self->solid = SOLID_BBOX;
	self->s.modelindex = gi.modelindex ("models/monsters/chthon/tris.md2");
	
	self->mins = { -128, -128, -24 };
	self->maxs = { 128, 128, 226 };

	if (strcmp(self->classname, "monster_lavaman") == 0)
	{
		self->s.skinnum = 2;
		self->health = max(1500, 1500 + 1000 * (skill->integer - 1)) * st.health_multiplier;
		if (!st.was_key_specified("armor_type"))
			self->monsterinfo.armor_type = IT_ARMOR_BODY;
		if (!st.was_key_specified("armor_power"))
			self->monsterinfo.armor_power = max(350, 350 + 100 * (skill->integer - 1));
		if (coop->integer)
		{
			self->health += (250 * (CountPlayers() - 1));
			self->monsterinfo.armor_power += (100 * (CountPlayers() - 1));
		}

		if (!self->s.scale)
			self->s.scale = 0.75f;
	}
	else
	{
		self->health = max(3000, 3000 + 1250 * (skill->integer - 1)) * st.health_multiplier;
		if (!st.was_key_specified("armor_type"))
			self->monsterinfo.armor_type = IT_ARMOR_BODY;
		if (!st.was_key_specified("armor_power"))
			self->monsterinfo.armor_power = max(500, 500 + 150 * (skill->integer - 1));
		if (coop->integer)
		{
			self->health += (500 * skill->integer) + (500 * (CountPlayers() - 1));
			self->monsterinfo.armor_power += (250 * skill->integer) + (250 * (CountPlayers() - 1));
		}
	}
	self->gib_health = -1000;
	self->mass = 1500;

	self->flags |= FL_IMMUNE_LAVA;

	self->pain = chthon_pain;
	self->die = chthon_die;

	self->monsterinfo.stand = chthon_stand; 
	self->monsterinfo.walk = chthon_walk;
	self->monsterinfo.run = chthon_run; 
	self->monsterinfo.dodge = nullptr;
	self->monsterinfo.attack = chthon_attack;
	self->monsterinfo.melee = nullptr; 
	self->monsterinfo.sight = chthon_sight;
	self->monsterinfo.search = nullptr;
	self->monsterinfo.checkattack = chthon_checkattack;
	self->monsterinfo.setskin = chthon_setskin;

	self->s.renderfx |= RF_FULLBRIGHT;
	
	gi.linkentity (self);

	self->monsterinfo.scale = MODEL_SCALE;
	M_SetAnimation(self, &chthon_move_rise);

	walkmonster_start(self);	
}

void SP_monster_lavaman(edict_t* self)
{
	SP_monster_chthon(self);
}