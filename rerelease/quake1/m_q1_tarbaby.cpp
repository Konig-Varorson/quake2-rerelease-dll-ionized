/*
==============================================================================

QUAKE TARBABY (BLOB)

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_tarbaby.h"

constexpr spawnflags_t SPAWNFLAG_HELLSPAWN_BABY = 8_spawnflag;

static cached_soundindex	sound_death;
static cached_soundindex	sound_hit;
static cached_soundindex	sound_land;
static cached_soundindex	sound_sight;

void tarbaby_stand (edict_t *self);
void tarbaby_check_landing (edict_t *self);
void tarbaby_fly (edict_t *self);
void tarbaby_takeoff (edict_t *self);
void tarbaby_die(edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod);

MONSTERINFO_SIGHT(tarbaby_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound (self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

mframe_t tarbaby_frames_stand [] =
{
	{ai_stand}
};
MMOVE_T(tarbaby_move_stand) = {FRAME_walk1, FRAME_walk1, tarbaby_frames_stand, tarbaby_stand};

MONSTERINFO_STAND(tarbaby_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &tarbaby_move_stand);
}

mframe_t tarbaby_frames_walk [] =
{
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
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2}
};
MMOVE_T(tarbaby_move_walk) = {FRAME_walk1, FRAME_walk25, tarbaby_frames_walk, nullptr };

MONSTERINFO_WALK(tarbaby_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &tarbaby_move_walk);
}


mframe_t tarbaby_frames_run [] =
{
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run, 2},
	{ai_run, 2},
	{ai_run, 2},
	{ai_run, 2},
	{ai_run, 2},
	{ai_run, 2},
	{ai_run, 2},
	{ai_run, 2},
	{ai_run, 2},
	{ai_run, 2},
	{ai_run, 2},
	{ai_run, 2},
	{ai_run, 2},
	{ai_run, 2},
	{ai_run, 2}
};
MMOVE_T(tarbaby_move_run) = {FRAME_run1, FRAME_run25, tarbaby_frames_run, nullptr };

MONSTERINFO_RUN(tarbaby_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &tarbaby_move_run);
}


mframe_t tarbaby_frames_fly [] =
{
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0,	 tarbaby_check_landing}
};
MMOVE_T(tarbaby_move_fly) = {FRAME_fly1, FRAME_fly4, tarbaby_frames_fly, tarbaby_fly};

void tarbaby_fly (edict_t *self)
{
	M_SetAnimation(self, &tarbaby_move_fly);
}

mframe_t tarbaby_frames_jump [] =
{
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0,	 tarbaby_takeoff},
	{ai_charge}
};
MMOVE_T(tarbaby_move_jump) = {FRAME_jump1, FRAME_jump6, tarbaby_frames_jump, tarbaby_fly};

TOUCH(tarbaby_jump_touch) (edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self) -> void
{
	if (self->health <= 0)
	{
		self->touch = nullptr;
		return;
	}

	if (self->style == 1 && other->takedamage)
	{
		if (self->velocity.length() > 400)
		{
			vec3_t point;
			vec3_t normal;
			int	   damage;

			normal = self->velocity;
			normal.normalize();
			point = self->s.origin + (normal * self->maxs[0]);
			damage = (int)frandom(10, 20);
			T_Damage(other, self, self, self->velocity, point, normal, damage, damage, DAMAGE_NONE, MOD_UNKNOWN);
			gi.sound(self, CHAN_VOICE, sound_hit, 1, ATTN_NORM, 0);
			self->style = 0;
		}
	}
	else
		gi.sound(self, CHAN_VOICE, sound_land, 1, ATTN_NORM, 0);

	if (!M_CheckBottom(self))
	{
		if (self->groundentity)
		{
			M_SetAnimation(self, &tarbaby_move_run);
			self->touch = nullptr;
		}
		return;
	}

	self->touch = nullptr;
	M_SetAnimation(self, &tarbaby_move_jump);
}

void tarbaby_check_landing (edict_t *self)
{
	monster_jump_finished(self);

	if (self->groundentity)
	{
		gi.sound(self, CHAN_WEAPON, sound_land, 1, ATTN_NORM, 0);
		self->monsterinfo.attack_finished = 0_sec;

		if (self->monsterinfo.unduck)
			self->monsterinfo.unduck(self);
		self->style = 0;
	}

	self->count++;
	if ( self->count >= 4 )
	{
		M_SetAnimation(self, &tarbaby_move_jump);
		self->monsterinfo.nextframe = FRAME_jump5;
		tarbaby_takeoff (self);
	}
}

void tarbaby_takeoff (edict_t *self)
{
	vec3_t forward;

	self->movetype = MOVETYPE_BOUNCE;
	self->s.origin[2] += 1;
	AngleVectors(self->s.angles, forward, nullptr, nullptr);
	self->velocity = forward * 600;
	self->velocity[2] = 200;
	self->groundentity = nullptr;
	self->monsterinfo.aiflags |= AI_DUCKED;
	self->monsterinfo.attack_finished = level.time + 3_sec;
	self->count = 0;
	self->style = 1;
	self->touch = tarbaby_jump_touch;
}

MONSTERINFO_ATTACK(tarbaby_jump) (edict_t* self) -> void
{
	if (!self->enemy)
		return;

	M_SetAnimation(self, &tarbaby_move_jump);
}

void tarbaby_mitosis(edict_t* self)
{
	vec3_t	 f, r, u, offset, startpoint, spawnpoint;
	edict_t* ent, * designated_enemy;
	int		 i;

	AngleVectors(self->s.angles, f, r, u);

	for (i = 0; i < 2; i++)
	{
		vec3_t hbaby_mins = { -16, -16, -24 };
		vec3_t hbaby_maxs = { 16, 16, 24 };


		offset = { 32, 0, 0};

		startpoint = G_ProjectSource2(self->s.origin, offset, f, r, u);

		if (FindSpawnPoint(startpoint, hbaby_mins, hbaby_maxs, spawnpoint, 64))
		{
			ent = CreateGroundMonster(spawnpoint, self->s.angles, hbaby_mins, hbaby_maxs, "monster_tarbaby", 256);

			if (!ent)
				continue;

			self->monsterinfo.monster_used++;
			ent->monsterinfo.commander = self;
			ent->monsterinfo.slots_from_commander = 1;

			ent->nextthink = level.time;
			ent->think(ent);

			ent->health = 90;
			ent->s.skinnum = 1;
			ent->s.scale = 0.7f;

			ent->monsterinfo.aiflags |= AI_SPAWNED_COMMANDER | AI_DO_NOT_COUNT | AI_IGNORE_SHOTS;

			if (!coop->integer)
			{
				designated_enemy = self->enemy;
			}
			else
			{
				designated_enemy = PickCoopTarget(ent);
				if (designated_enemy)
				{
					// try to avoid using my enemy
					if (designated_enemy == self->enemy)
					{
						designated_enemy = PickCoopTarget(ent);
						if (!designated_enemy)
							designated_enemy = self->enemy;
					}
				}
				else
					designated_enemy = self->enemy;
			}

			if ((designated_enemy->inuse) && (designated_enemy->health > 0))
			{
				ent->enemy = designated_enemy;
				FoundTarget(ent);
				ent->monsterinfo.attack(ent);
			}
		}
	}
}


PAIN(tarbaby_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	float chance;

	if (!(skill->integer >= 1))
		return;
		
	if (level.time < self->pain_debounce_time)
		return;

	if (!(strcmp(self->classname, "monster_tarbaby_hell") == 0))
		return;
	else
	{
		if (self->spawnflags.has(SPAWNFLAG_HELLSPAWN_BABY))
			return;
	}

	chance = frandom() * skill->integer;

	if (chance > 0.75f)
		tarbaby_mitosis(self);

	self->pain_debounce_time = level.time + 3_sec;
}

void tarbaby_dead (edict_t *self)
{
	T_RadiusDamage(self, self, 120, nullptr, 150, DAMAGE_NONE, MOD_UNKNOWN);

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_EXPLOSION1);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	self->touch = nullptr;

	ThrowGibs(self, 500, {
		{ "models/objects/gibs/sm_meat/tris.md2", GIB_HEAD }
		});
}

mframe_t tarbaby_frames_explode [] =
{
	{ai_move}
};
MMOVE_T(tarbaby_move_explode) = {FRAME_exp, FRAME_exp, tarbaby_frames_explode, tarbaby_dead};

DIE(tarbaby_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (self->deadflag)
		return;

	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	M_SetAnimation(self, &tarbaby_move_explode);
}


//
// SPAWN
//

/*QUAKED monster_tarbaby (1 .5 0) (-16 -16 -24) (16 16 24) Ambush Trigger_Spawn Sight
model="models/monsters/q1tarbaby/tris.md2"
*/
void SP_monster_tarbaby(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_death.assign("tarbaby/death1.wav");
	sound_hit.assign("tarbaby/hit1.wav");
	sound_land.assign("tarbaby/land1.wav");
	sound_sight.assign("tarbaby/sight1.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex("models/monsters/tarbaby/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 24 };


	if (strcmp(self->classname, "monster_tarbaby_hell") == 0)
	{
		if (!(self->spawnflags & SPAWNFLAG_HELLSPAWN_BABY))
		{
			self->health = 150 * st.health_multiplier;
			self->s.skinnum = 2;
		}
		else
		{
			self->health = 90 * st.health_multiplier;
			self->s.skinnum = 1;

			if (!self->s.scale)
				self->s.scale = 0.8f;
		}
	}
	else
	{
		self->health = 120 * st.health_multiplier;
	}

	self->gib_health = 0;
	self->mass = 100;

	self->pain = tarbaby_pain;
	self->die = tarbaby_die;

	self->monsterinfo.stand = tarbaby_stand;
	self->monsterinfo.walk = tarbaby_walk;
	self->monsterinfo.run = tarbaby_run;
	self->monsterinfo.dodge = nullptr;
	self->monsterinfo.attack = tarbaby_jump;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.sight = tarbaby_sight;
	self->monsterinfo.search = nullptr;

	gi.linkentity (self);
	self->flags |= FL_DEEPONE;

	M_SetAnimation(self, &tarbaby_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	walkmonster_start (self);
}

void SP_monster_tarbaby_hell(edict_t* self)
{
	SP_monster_tarbaby(self);
}