
/*
==============================================================================

QUAKE SHUB-NIGGURATH

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_oldone.h"

static cached_soundindex sound_idle;
static cached_soundindex sound_sight;
static cached_soundindex sound_death1;
static cached_soundindex sound_death2;
static cached_soundindex sound_spawn;
static cached_soundindex sound_pain;

constexpr const char* default_reinforcements = "monster_scrag 1;monster_fiend 3;monster_vore 5;monster_shambler 6";
constexpr int32_t default_monster_slots_base = 6;

void oldone_run(edict_t* self);

//
// SOUNDS
//

MONSTERINFO_IDLE(oldone_idle) (edict_t* self) -> void
{
	gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_NORM, 0);
}

MONSTERINFO_SIGHT(oldone_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

//
// STAND
//

mframe_t oldone_frames_stand[] =
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
	{ai_stand},  // 10

	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},  // 20

	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},  // 30

	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},  // 40

	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}
};
MMOVE_T(oldone_move_stand) = { FRAME_old1, FRAME_old46, oldone_frames_stand, nullptr };

MONSTERINFO_STAND(oldone_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &oldone_move_stand);
}

mframe_t oldone_frames_walk[] =
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
	{ai_walk},  // 10

	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},  // 20

	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},  // 30

	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},  // 40

	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk},
	{ai_walk}
};
MMOVE_T(oldone_move_walk) = { FRAME_old1, FRAME_old46, oldone_frames_walk, nullptr };

MONSTERINFO_WALK(oldone_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &oldone_move_walk);
}

mframe_t oldone_frames_run[] =
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
	{ai_run},  // 10

	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},  // 20

	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},  // 30

	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},  // 40

	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run},
	{ai_run}
};
MMOVE_T(oldone_move_run) = { FRAME_old1, FRAME_old46, oldone_frames_run, oldone_run };

MONSTERINFO_RUN(oldone_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &oldone_move_run);
}

//
// ATTACK
//
void oldoneCoopCheck(edict_t* self)
{
	// no more than 8 players in coop, so..
	std::array<edict_t*, MAX_SPLIT_PLAYERS> targets;
	uint32_t num_targets = 0;
	int32_t  target;
	edict_t* ent;
	trace_t	 tr;

	// if we're not in coop, this is a noop
	// [Paril-KEX] might as well let this work in SP too, so he fires it
	// if you get below him
	//if (!coop->integer)
	//	return;
	// if we are, and we have recently fired, bail
	if (self->monsterinfo.fire_wait > level.time)
		return;

	targets = {};

	// cycle through players
	for (uint32_t player = 1; player <= game.maxclients; player++)
	{
		ent = &g_edicts[player];
		if (!ent->inuse)
			continue;
		if (!ent->client)
			continue;
		if (inback(self, ent) || below(self, ent))
		{
			tr = gi.traceline(self->s.origin, ent->s.origin, self, MASK_SOLID);
			if (tr.fraction == 1.0f)
				targets[num_targets++] = ent;
		}
	}

	if (!num_targets)
		return;

	// get a number from 0 to (num_targets-1)
	target = irandom(num_targets);

	// save off the real enemy
	ent = self->enemy;
	// set the new guy as temporary enemy
	self->enemy = targets[target];
	// put the real enemy back
	self->enemy = ent;

	// we're done
	return;
}

void oldoneSpawn(edict_t* self)
{
	vec3_t	 f, r, offset, startpoint, spawnpoint;
	edict_t* ent;

	offset = { 225, 0, -58 }; // real distance needed is (sqrt (56*56*2) + sqrt(16*16*2)) or 101.8
	AngleVectors(self->s.angles, f, r, nullptr);

	startpoint = M_ProjectFlashSource(self, offset, f, r);

	if (self->monsterinfo.chosen_reinforcements[0] == 255)
		return;

	auto& reinforcement = self->monsterinfo.reinforcements.reinforcements[self->monsterinfo.chosen_reinforcements[0]];

	if (FindSpawnPoint(startpoint, reinforcement.mins, reinforcement.maxs, spawnpoint, 32, false))
	{
		ent = CreateFlyMonster(spawnpoint, self->s.angles, reinforcement.mins, reinforcement.maxs, reinforcement.classname);

		if (!ent)
			return;

		gi.sound(self, CHAN_BODY, sound_spawn, 1, ATTN_NONE, 0);

		ent->nextthink = level.time;
		ent->think(ent);

		ent->monsterinfo.aiflags |= AI_SPAWNED_COMMANDER | AI_DO_NOT_COUNT | AI_IGNORE_SHOTS;
		ent->monsterinfo.commander = self;
		ent->monsterinfo.slots_from_commander = reinforcement.strength;
		self->monsterinfo.monster_used += reinforcement.strength;

		if ((self->enemy->inuse) && (self->enemy->health > 0))
		{
			ent->enemy = self->enemy;
			FoundTarget(ent);
		}
	}
}

void oldone_prep_spawn(edict_t* self)
{
	oldoneCoopCheck(self);
	self->monsterinfo.aiflags |= AI_MANUAL_STEERING;
	self->timestamp = level.time;
	self->yaw_speed = 10;
}

void oldone_spawn_check(edict_t* self)
{
	oldoneCoopCheck(self);
	oldoneSpawn(self);

	if (level.time > (self->timestamp + 2.0_sec)) // 0.5 seconds per flyer.  this gets three
	{
		self->monsterinfo.aiflags &= ~AI_MANUAL_STEERING;
	}
	else
		self->monsterinfo.nextframe = FRAME_old8;
}

void oldone_ready_spawn(edict_t* self)
{
	float  current_yaw;
	vec3_t offset, f, r, startpoint, spawnpoint;

	oldoneCoopCheck(self);

	current_yaw = anglemod(self->s.angles[YAW]);

	if (fabsf(current_yaw - self->ideal_yaw) > 0.1f)
	{
		self->monsterinfo.aiflags |= AI_HOLD_FRAME;
		self->timestamp += FRAME_TIME_S;
		return;
	}

	self->monsterinfo.aiflags &= ~AI_HOLD_FRAME;

	int num_summoned;
	self->monsterinfo.chosen_reinforcements = M_PickReinforcements(self, num_summoned, 1);

	if (!num_summoned)
		return;

	auto& reinforcement = self->monsterinfo.reinforcements.reinforcements[self->monsterinfo.chosen_reinforcements[0]];

	offset = { 105, 0, -58 };
	AngleVectors(self->s.angles, f, r, nullptr);
	startpoint = M_ProjectFlashSource(self, offset, f, r);
	if (FindSpawnPoint(startpoint, reinforcement.mins, reinforcement.maxs, spawnpoint, 32, false))
	{
		float radius = (reinforcement.maxs - reinforcement.mins).length() * 0.5f;

		SpawnGrow_Spawn(spawnpoint + (reinforcement.mins + reinforcement.maxs), radius, radius * 2.f);
	}
}

void oldone_start_spawn(edict_t* self)
{
	int	   mytime;
	float  enemy_yaw;
	vec3_t temp;

	oldoneCoopCheck(self);

	if (!self->enemy)
		return;

	mytime = (int)((level.time - self->timestamp) / 0.5).seconds();

	temp = self->enemy->s.origin - self->s.origin;
	enemy_yaw = vectoyaw(temp);

	// note that the offsets are based on a forward of 105 from the end angle
	if (mytime == 0)
		self->ideal_yaw = anglemod(enemy_yaw - 30);
	else if (mytime == 1)
		self->ideal_yaw = anglemod(enemy_yaw);
	else if (mytime == 2)
		self->ideal_yaw = anglemod(enemy_yaw + 30);
}

mframe_t oldone_frames_spawn[] =
{
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, oldone_prep_spawn},
	{ai_charge, 0, oldone_start_spawn},
	{ai_charge, 0, oldone_ready_spawn},
	{ai_charge},  // 10

	{ai_charge},
	{ai_charge, 0, oldone_spawn_check},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge}
};
MMOVE_T(oldone_move_spawn) = { FRAME_old1, FRAME_old18, oldone_frames_spawn, nullptr };

MONSTERINFO_ATTACK(oldone_attack) (edict_t* self) -> void
{
	vec3_t vec;
	float  range;
	bool   enemy_inback, enemy_infront, enemy_below;

	self->monsterinfo.aiflags &= ~AI_HOLD_FRAME;

	if ((!self->enemy) || (!self->enemy->inuse))
		return;

	enemy_inback = inback(self, self->enemy);
	enemy_infront = infront(self, self->enemy);
	enemy_below = below(self, self->enemy);

	if (self->bad_area)
	{
		return;
	}

	if (self->monsterinfo.attack_state == AS_BLIND)
	{
		M_SetAnimation(self, &oldone_move_spawn);
		return;
	}

	if (!enemy_inback && !enemy_infront && !enemy_below) // to side and not under
	{
		return;
	}

	if (enemy_infront)
	{
		vec = self->enemy->s.origin - self->s.origin;
		range = vec.length();

		if (M_SlotsLeft(self) > 2)
		{
			if (frandom() <= 0.20f)
				M_SetAnimation(self, &oldone_move_spawn);
		}
	}
}

//
// PAIN
//

mframe_t oldone_frames_pain[] =
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
	{ai_move}
};
MMOVE_T(oldone_move_pain) = { FRAME_shake1, FRAME_shake10, oldone_frames_pain, oldone_run };

PAIN(oldone_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	self->health += damage;

	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 5_sec;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	if (damage < 30)
	{
		gi.sound(self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &oldone_move_pain);
	}
}

//
// DEATH
//

void oldone_dead(edict_t* self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, -8 };
	monster_dead(self);
}

mframe_t oldone_frames_death1[] =
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
	{ai_move}
};
MMOVE_T(oldone_move_death) = { FRAME_shake1, FRAME_shake20, oldone_frames_death1, oldone_dead };

DIE(oldone_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 3, "models/objects/gibs/sm_meat/tris.md2" },
			{ "models/objects/gibs/sm_meat/tris.md2", GIB_SKINNED | GIB_HEAD }
			});

		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	gi.sound(self, CHAN_VOICE, sound_death1, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	M_SetAnimation(self, &oldone_move_death);
}


void SP_monster_oldone(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_idle.assign("oldone/idle.wav");
	sound_sight.assign("oldone/sight.wav");
	sound_death1.assign("oldone/death.wav");
	sound_death2.assign("oldone/pop2.wav");
	sound_spawn.assign("");
	sound_pain.assign("");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;
	self->s.modelindex = gi.modelindex("models/monsters/oldone/tris.md2");

	self->mins = { -160, - 128, - 24 };
	self->maxs = { 160, 128, 256 };

	self->health = 40000 * st.health_multiplier;
	self->gib_health = -50;
	self->mass = 3000;

	self->pain = oldone_pain;
	self->die = oldone_die;

	self->monsterinfo.stand = oldone_stand;
	self->monsterinfo.walk = oldone_walk;
	self->monsterinfo.run = oldone_run;
	self->monsterinfo.dodge = nullptr;
	self->monsterinfo.attack = oldone_attack;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.sight = oldone_sight;
	self->monsterinfo.idle = oldone_idle;

	gi.linkentity(self);

	self->monsterinfo.scale = MODEL_SCALE;

	self->flags |= FL_NO_KNOCKBACK | FL_STATIONARY | FL_DEEPONE;
	
	stationarymonster_start(self, st);

	self->monsterinfo.attack_finished = 0_ms;

	const char* reinforcements = default_reinforcements;

	if (!st.was_key_specified("monster_slots"))
		self->monsterinfo.monster_slots = default_monster_slots_base;
	if (st.was_key_specified("reinforcements"))
		reinforcements = st.reinforcements;

	if (self->monsterinfo.monster_slots && reinforcements && *reinforcements)
	{
		if (skill->integer)
			self->monsterinfo.monster_slots += floor(self->monsterinfo.monster_slots * (skill->value / 2.f));
		/* KONIG - doubles reinforcement scaling in coop */
		if (coop->integer)
		{
			self->monsterinfo.monster_slots += floor(self->monsterinfo.monster_slots * (skill->value / 2.f));
		}
		M_SetupReinforcements(reinforcements, self->monsterinfo.reinforcements);
	}

}