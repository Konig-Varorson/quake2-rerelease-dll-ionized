/*
==============================================================================

QUAKE WYVERN

==============================================================================
*/

#include "../g_local.h"
#include "m_dragon.h"

static cached_soundindex	sound_sight1;
static cached_soundindex	sound_search1;
static cached_soundindex	sound_attack;
static cached_soundindex	sound_die1;
static cached_soundindex	sound_pain1;

void dragon_hover(edict_t* self);
void dragon_attack(edict_t* self);
void dragon_reattack(edict_t* self);
void dragon_walk(edict_t* self);
void dragon_run(edict_t* self);

MONSTERINFO_SIGHT(dragon_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight1, 1, ATTN_NONE, 0);
}

MONSTERINFO_SEARCH(dragon_search) (edict_t* self) -> void
{
	gi.sound(self, CHAN_VOICE, sound_search1, 1, ATTN_NONE, 0);
}

//================
// HOVER
//================
mframe_t dragon_frames_hover [] =
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
	{ai_stand},

	{ai_stand},
	{ai_stand},
	{ai_stand}
};
MMOVE_T(dragon_move_hover) = { FRAME_drgfly01, FRAME_drgfly13, dragon_frames_hover, nullptr };

MONSTERINFO_STAND(dragon_hover) (edict_t* self) -> void
{
	M_SetAnimation(self, &dragon_move_hover);
}

//================
// WALK
//================
mframe_t dragon_frames_walk [] =
{
	{ai_walk, 5},
	{ai_walk, 5},
	{ai_walk, 5},
	{ai_walk, 5},
	{ai_walk, 5},

	{ai_walk, 5},
	{ai_walk, 5},
	{ai_walk, 5},
	{ai_walk, 5},
	{ai_walk, 5},

	{ai_walk, 5},
	{ai_walk, 5},
	{ai_walk, 5}
};
MMOVE_T(dragon_move_walk) = { FRAME_drgfly01, FRAME_drgfly13, dragon_frames_walk, nullptr };


MONSTERINFO_WALK(dragon_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &dragon_move_walk);
}

//================
// FLY
//================
mframe_t dragon_frames_run[] =
{
	{ai_run, 10},
	{ai_run, 10},
	{ai_run, 10},
	{ai_run, 10},
	{ai_run, 10},

	{ai_run, 10},
	{ai_run, 10},
	{ai_run, 10},
	{ai_run, 10},
	{ai_run, 10},

	{ai_run, 10},
	{ai_run, 10},
	{ai_run, 10}
};
MMOVE_T(dragon_move_run) = { FRAME_drgfly01, FRAME_drgfly13, dragon_frames_run, nullptr };


MONSTERINFO_RUN(dragon_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &dragon_move_run);
}

//================
// PAIN
//================
mframe_t dragon_frames_pain1 [] =
{	
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(dragon_move_pain1) = { FRAME_drgpan1a, FRAME_drgpan1c, dragon_frames_pain1, dragon_run };

mframe_t dragon_frames_pain2[] =
{
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(dragon_move_pain2) = { FRAME_drgpan2a, FRAME_drgpan2c, dragon_frames_pain2, dragon_run };

mframe_t dragon_frames_pain3[] =
{
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(dragon_move_pain3) = { FRAME_drgpan3a, FRAME_drgpan3c, dragon_frames_pain3, dragon_run };

mframe_t dragon_frames_pain4[] =
{
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(dragon_move_pain4) = { FRAME_drgpan4a, FRAME_drgpan4c, dragon_frames_pain4, dragon_run };

mframe_t dragon_frames_pain5[] =
{
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(dragon_move_pain5) = { FRAME_drgpan5a, FRAME_drgpan5c, dragon_frames_pain5, dragon_run };

mframe_t dragon_frames_pain6[] =
{
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(dragon_move_pain6) = { FRAME_drgpan6a, FRAME_drgpan6c, dragon_frames_pain6, dragon_run };

PAIN(dragon_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	gi.sound(self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);

	if (damage < 30)
		M_SetAnimation(self, &dragon_move_pain1);
	else if(damage < 60)
	{
		if (frandom() >= 0.5)
			M_SetAnimation(self, &dragon_move_pain2);
		else
			M_SetAnimation(self, &dragon_move_pain3);
	}
	else if (damage > 120)
		M_SetAnimation(self, &dragon_move_pain6);
	else
	{
		if (frandom() >= 0.5)
			M_SetAnimation(self, &dragon_move_pain4);
		else
			M_SetAnimation(self, &dragon_move_pain5);
	}


}

MONSTERINFO_SETSKIN(dragon_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//================
// DEAD
//================
static void dragon_gib(edict_t* self)
{
	gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

	self->s.skinnum /= 2;

	ThrowGibs(self, 1000, {
		{ 2, "models/objects/gibs/bone/tris.md2" },
		{ 4, "models/objects/gibs/sm_meat/tris.md2" },
		{ "models/monsters/dragon/gibs/leg.md2"},
		{ "models/monsters/dragon/gibs/wing.md2" },
		{ "models/monsters/dragon/gibs/head.md2", GIB_HEAD }
		});
}

THINK(dragon_deadthink) (edict_t* self) -> void
{
	if (!self->groundentity && level.time < self->timestamp)
	{
		self->nextthink = level.time + FRAME_TIME_S;
		return;
	}

	return;
}

void dragon_dead(edict_t * self)
{
	self->mins = { -144, -136, -36 };
	self->maxs = { 88, 128, 24 };
	self->movetype = MOVETYPE_TOSS;
	self->think = dragon_deadthink;
	self->nextthink = level.time + FRAME_TIME_S;
	self->timestamp = level.time + 15_sec;
	gi.linkentity(self);

	dragon_gib(self);
}

mframe_t dragon_frames_die1 [] =
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
MMOVE_T(dragon_move_die1) = { FRAME_drgdth01, FRAME_drgdth21, dragon_frames_die1, dragon_dead};

DIE(dragon_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		dragon_gib(self);

		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	self->deadflag = true;
	self->takedamage = true;

	gi.sound(self, CHAN_VOICE, sound_die1, 1, ATTN_NORM, 0);
	M_SetAnimation(self, &dragon_move_die1);
}

//================
// ATTACK
//================

void dragon_fireball (edict_t *self)
{
	vec3_t		forward, right;
	vec3_t		start;
	vec3_t		dir;
	vec3_t		vec;
	trace_t		trace; // PMM - check target
	vec3_t		target;
	int			damage;
	int			speed;
	bool		blindfire = false;
	vec3_t		offset;
	float		fireCount;
	float		r;
	
	offset = {96, 0, 32};

	if (frandom() > 0.66f)
	{
		if (skill->integer > 1)
			fireCount = 2;
		else
			fireCount = 1;
		damage = 80 + frandom() * 20;
		speed = 1250;
		self->style = 1;
	}
	else
	{
		fireCount = frandom() * skill->integer;
		fireCount = rint(fireCount) + 1;
		damage = 90;
		speed = frandom() * 300 + 900;
		self->style = 0;
	}

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
	while (fireCount > 0)
	{
		if (blindfire)
		{
			if (!(trace.startsolid || trace.allsolid || (trace.fraction < 0.5f)))
			{
				if (self->style == 1)
					fire_plasmaball(self, start, dir, damage, speed, damage * 2);
				else
					fire_lavaball(self, start, dir, damage, speed, (float)damage, damage);
			}
			else
			{
				vec = target;
				vec += (right * -10);
				dir = vec - start;
				dir.normalize();
				trace = gi.traceline(start, vec, self, MASK_PROJECTILE);
				if (!(trace.startsolid || trace.allsolid || (trace.fraction < 0.5f)))
				{
					if (self->style == 1)
						fire_plasmaball(self, start, dir, damage, speed, damage * 2);
					else
						fire_lavaball(self, start, dir, damage, speed, (float)damage, damage);
				}
				else
				{
					vec = target;
					vec += (right * 10);
					dir = vec - start;
					dir.normalize();
					trace = gi.traceline(start, vec, self, MASK_PROJECTILE);
					if (!(trace.startsolid || trace.allsolid || (trace.fraction < 0.5f)))
					{
						if (self->style == 1)
							fire_plasmaball(self, start, dir, damage, speed, damage * 2);
						else
							fire_lavaball(self, start, dir, damage, speed, (float)damage, damage);
					}
				}
			}
		}
		else
		{
			r = crandom() * 1000;

			vec = start + (forward * 8192);
			vec += (right * r);

			dir = vec - start;
			dir.normalize();

			if (trace.fraction > 0.5f || trace.ent->solid != SOLID_BSP)
			{
				if (self->style == 1)
					fire_plasmaball(self, start, dir, damage, speed, damage * 2);
				else
					fire_lavaball(self, start, dir, damage, speed, (float)damage, damage);
			}
		}
		gi.sound(self, CHAN_VOICE, sound_attack, 1, ATTN_NORM, 0);

		fireCount -= 1;
	}
}

void dragon_tail(edict_t* self)
{
	if (fire_hit(self, vec3_t{ MELEE_DISTANCE, 0.f, -32.f }, (frandom() * 30) + 30, 400.f))
	{
		if (self->enemy && self->enemy->client && self->enemy->velocity.z < 270.f)
			self->enemy->velocity.z = 270.f;
	}
}

mframe_t dragon_frames_attack1 [] =
{
	{ai_charge, 17},
	{ai_charge, 12},
	{ai_charge, 7},
	{ai_charge, 2},
	{ai_charge},

	{ai_charge, -2},
	{ai_charge, -7},
	{ai_charge, -12, dragon_fireball},
	{ai_charge, -7},
	{ai_charge, -2, dragon_reattack}
};
MMOVE_T(dragon_move_attack1) = { FRAME_drgfir01, FRAME_drgfir10, dragon_frames_attack1, nullptr };

void dragon_postfix1(edict_t* self)
{
	M_SetAnimation(self, &dragon_move_run, false);
	self->monsterinfo.nextframe = FRAME_drgfly05;
}
void dragon_postfix2(edict_t* self)
{
	M_SetAnimation(self, &dragon_move_run, false);
	self->monsterinfo.nextframe = FRAME_drgfly07;
}
void dragon_postfix3(edict_t* self)
{
	M_SetAnimation(self, &dragon_move_run, false);
	self->monsterinfo.nextframe = FRAME_drgfly09;
}
void dragon_postfix4(edict_t* self)
{
	M_SetAnimation(self, &dragon_move_run, false);
	self->monsterinfo.nextframe = FRAME_drgfly11;
}
void dragon_postfix5(edict_t* self)
{
	M_SetAnimation(self, &dragon_move_run, false);
	self->monsterinfo.nextframe = FRAME_drgfly13;
}
void dragon_postfix6(edict_t* self)
{
	M_SetAnimation(self, &dragon_move_run, false);
	self->monsterinfo.nextframe = FRAME_drgfly03;
}

mframe_t dragon_frames_attack_fix1[] =
{
	{ai_charge, 17},
	{ai_charge, 17, dragon_fireball },
	{ai_charge, 17, dragon_postfix1 },
};
MMOVE_T(dragon_move_fix1) = { FRAME_drgfix1a, FRAME_drgfix1c, dragon_frames_attack_fix1, nullptr };

mframe_t dragon_frames_attack_fix2[] =
{
	{ai_charge, 17},
	{ai_charge, 17, dragon_fireball },
	{ai_charge, 17, dragon_postfix2 },
};
MMOVE_T(dragon_move_fix2) = { FRAME_drgfix2a, FRAME_drgfix2c, dragon_frames_attack_fix1, nullptr };

mframe_t dragon_frames_attack_fix3[] =
{
	{ai_charge, 17},
	{ai_charge, 17, dragon_fireball },
	{ai_charge, 17, dragon_postfix3 },
};
MMOVE_T(dragon_move_fix3) = { FRAME_drgfix3a, FRAME_drgfix3c, dragon_frames_attack_fix3, nullptr };

mframe_t dragon_frames_attack_fix4[] =
{
	{ai_charge, 17},
	{ai_charge, 17, dragon_fireball },
	{ai_charge, 17, dragon_postfix4 },
};
MMOVE_T(dragon_move_fix4) = { FRAME_drgfix4a, FRAME_drgfix4c, dragon_frames_attack_fix4, nullptr };

mframe_t dragon_frames_attack_fix5[] =
{
	{ai_charge, 17},
	{ai_charge, 17, dragon_fireball },
	{ai_charge, 17, dragon_postfix5 },
};
MMOVE_T(dragon_move_fix5) = { FRAME_drgfix5a, FRAME_drgfix5c, dragon_frames_attack_fix5, nullptr };

mframe_t dragon_frames_attack_fix6[] =
{
	{ai_charge, 17},
	{ai_charge, 17, dragon_fireball },
	{ai_charge, 17},
	{ai_charge, 17, dragon_postfix6 },
};
MMOVE_T(dragon_move_fix6) = { FRAME_drgfix6a, FRAME_drgfix6d, dragon_frames_attack_fix6, nullptr };

MONSTERINFO_ATTACK(dragon_attack) (edict_t* self) -> void
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
		M_SetAnimation(self, &dragon_move_attack1);
		self->monsterinfo.attack_finished = level.time + random_time(2_sec);
		return;
	}

	if (self->s.frame == FRAME_drgfly01)
		M_SetAnimation(self, &dragon_move_fix1);
	else if (self->s.frame == FRAME_drgfly03)
		M_SetAnimation(self, &dragon_move_fix2);
	else if (self->s.frame == FRAME_drgfly05)
		M_SetAnimation(self, &dragon_move_fix3);
	else if (self->s.frame == FRAME_drgfly07)
		M_SetAnimation(self, &dragon_move_fix4);
	else if (self->s.frame == FRAME_drgfly09)
		M_SetAnimation(self, &dragon_move_fix5);
	else if (self->s.frame == FRAME_drgfly11)
		M_SetAnimation(self, &dragon_move_fix6);
	else
		M_SetAnimation(self, &dragon_move_attack1);
}

void dragon_reattack(edict_t* self)
{
	if (self->enemy->health > 0)
		if (visible(self, self->enemy))
			if (frandom() <= 0.4f)
			{
				M_SetAnimation(self, &dragon_move_attack1);
				return;
			}

	dragon_run(self);
}

//
// MELEE
//

mframe_t dragon_frames_melee[] =
{
	{ai_charge, 12},
	{ai_charge, 12},
	{ai_charge, 12},
	{ai_charge, 12},
	{ai_charge, 12},

	{ai_charge, 12},
	{ai_charge, 12},
	{ai_charge, 12},
	{ai_charge, 12},
	{ai_charge, 0, dragon_tail},

	{ai_charge, 10},
	{ai_charge, 10},
	{ai_charge}
};
MMOVE_T(dragon_move_melee) = { FRAME_drgslh01, FRAME_drgslh13, dragon_frames_melee, dragon_run };

MONSTERINFO_MELEE(dragon_melee) (edict_t* self) -> void
{
	M_SetAnimation(self, &dragon_move_melee);
}


static void dragon_set_fly_parameters(edict_t* self)
{
	self->monsterinfo.fly_thrusters = false;
	self->monsterinfo.fly_acceleration = 20.f;
	self->monsterinfo.fly_speed = 120.f;
	self->monsterinfo.fly_min_distance = 550.f;
	self->monsterinfo.fly_max_distance = 750.f;
}

/*QUAKED monster_dragon (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
*/

void SP_monster_dragon(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_sight1.assign("dragon/see.wav");
	sound_search1.assign("dragon/active.wav");
	sound_pain1.assign("dragon/pain.wav");
	sound_die1.assign("dragon/death.wav");
	sound_attack.assign("dragon/attack.wav");
	
	self->s.modelindex = gi.modelindex ("models/monsters/dragon/tris.md2");

	self->mins = { -72, -48, -32 };
	self->maxs = { 104, 48, 48 };
	
	self->movetype = MOVETYPE_FLY;
	self->solid = SOLID_BBOX;

	self->health = max(3000, 3000 + 1250 * (skill->integer - 1)) * st.health_multiplier;
	if (!st.was_key_specified("armor_type"))
		self->monsterinfo.armor_type = IT_ARMOR_BODY;
	if (!st.was_key_specified("armor_power"))
		self->monsterinfo.armor_power = max(500, 500 + 150 * (skill->integer - 1));
	self->gib_health = -500;
	self->mass = 750;
	if (coop->integer)
	{
		self->health += (500 * skill->integer) + (500 * (CountPlayers() - 1));
		self->monsterinfo.armor_power += (250 * skill->integer) + (250 * (CountPlayers() - 1));
	}

	self->pain = dragon_pain;
	self->die = dragon_die;

	self->yaw_speed = 20;
	
	self->monsterinfo.stand = dragon_hover;
	self->monsterinfo.walk = dragon_walk;
	self->monsterinfo.run = dragon_run;
	self->monsterinfo.attack = dragon_attack;
	self->monsterinfo.melee = dragon_melee;
	self->monsterinfo.sight = dragon_sight;
	self->monsterinfo.search = dragon_search;
	self->monsterinfo.setskin = dragon_setskin;
	
	gi.linkentity (self);

	M_SetAnimation(self, &dragon_move_hover);	
	self->monsterinfo.scale = MODEL_SCALE;
	
	flymonster_start(self);

	self->monsterinfo.aiflags |= AI_ALTERNATE_FLY;
	dragon_set_fly_parameters(self);
}
