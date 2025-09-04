/*
==============================================================================

QUAKE ARMY

==============================================================================
*/

#include "../g_local.h"
#include "m_army.h"
#include "../m_flash.h"

constexpr spawnflags_t SPAWNFLAG_ARMY_NOJUMPING = 8_spawnflag;

static cached_soundindex	sound_idle;
static cached_soundindex	sound_sight;
static cached_soundindex	sound_pain1;
static cached_soundindex	sound_pain2;
static cached_soundindex	sound_death;
static cached_soundindex	sound_attack;

void army_run(edict_t* self);
void army_stand(edict_t* self);
void army_attack(edict_t* self);

MONSTERINFO_SEARCH(army_search) (edict_t* self) -> void
{
	gi.sound (self, CHAN_VOICE, sound_idle, 1, ATTN_STATIC, 0);
}

// STAND

mframe_t army_frames_stand [] =
{
	{ai_stand, 0},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand},
	{ai_stand}
};
MMOVE_T(army_move_stand) = {FRAME_stand1, FRAME_stand8, army_frames_stand, army_stand};

MONSTERINFO_STAND(army_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &army_move_stand);
}

//
// WALK
//

mframe_t army_frames_walk [] =
{
	{ai_walk, 1},
	{ai_walk, 1},
	{ai_walk, 1},
	{ai_walk, 1},
	{ai_walk, 2},
	{ai_walk, 3},
	{ai_walk, 4},
	{ai_walk, 4},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 1},
	{ai_walk},
	{ai_walk, 1},
	{ai_walk, 1},
	{ai_walk, 1},
	{ai_walk, 3},
	{ai_walk, 3},
	{ai_walk, 3},
	{ai_walk, 3},
	{ai_walk, 2},
	{ai_walk, 1},
	{ai_walk, 1},
	{ai_walk, 1}
};
MMOVE_T(army_move_walk) = {FRAME_prowl_1, FRAME_prowl_24, army_frames_walk, nullptr };

MONSTERINFO_WALK(army_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &army_move_walk);
}

//
// RUN
//

mframe_t army_frames_run [] =
{
	{ai_run, 11},
	{ai_run, 15},
	{ai_run, 10},
	{ai_run, 10},
	{ai_run, 8},
	{ai_run, 15},
	{ai_run, 10},
	{ai_run, 15}
};
MMOVE_T(army_move_run) = {FRAME_run1, FRAME_run8, army_frames_run, army_run};

MONSTERINFO_RUN(army_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
	{
		M_SetAnimation(self, &army_move_stand);
		return;
	}
	else
		M_SetAnimation(self, &army_move_run);
}

//
// PAIN
//

mframe_t army_frames_pain1 [] =
{
	{ai_move, -2},
	{ai_move, 3},
	{ai_move, 1},
	{ai_move, 1},
	{ai_move},
	{ai_move}
};
MMOVE_T(army_move_pain1) = {FRAME_pain1, FRAME_pain6, army_frames_pain1, army_run};

mframe_t army_frames_pain2 [] =
{
	{ai_move},
	{ai_move},
	{ai_move, 13},
	{ai_move, 9},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move, -2},
	{ai_move}
};
MMOVE_T(army_move_pain2) = {FRAME_painb1, FRAME_painb14, army_frames_pain2, army_run};

mframe_t army_frames_pain3 [] =
{
	{ai_move},
	{ai_move, -1},
	{ai_move},
	{ai_move},
	{ai_move, 1},
	{ai_move, 1},
	{ai_move},
	{ai_move, -1},
	{ai_move, 4},
	{ai_move, 3},
	{ai_move, 6},
	{ai_move, 8},
	{ai_move}
};
MMOVE_T(army_move_pain3) = {FRAME_painc1, FRAME_painc13, army_frames_pain3, army_run};

PAIN(army_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	float	r = frandom();
	
	if (level.time < self->pain_debounce_time)
		return;
	
	if (self->health <= 0)
		return;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	if (r < 0.2)
	{
		self->pain_debounce_time = level.time + 0.6_sec;
		gi.sound (self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &army_move_pain1);
	}
	else if (r < 0.6)
	{
		self->pain_debounce_time = level.time + 1.1_sec;
		gi.sound (self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &army_move_pain2);
	}
	else
	{
		self->pain_debounce_time = level.time + 1.1_sec;
		gi.sound (self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &army_move_pain3);
	}
}

MONSTERINFO_SETSKIN(army_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
// ATTACK
//

void army_fire (edict_t *self)
{
	vec3_t	start;
	vec3_t	forward, right, up;
	vec3_t	aim;
	vec3_t	dir;
	vec3_t	end;
	float	r, u;
	vec3_t					 aim_good;
		
	if (!self->enemy)
		return;

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, monster_flash_offset[MZ2_SOLDIER_SHOTGUN_1], forward, right);

	if ((!self->enemy) || (!self->enemy->inuse))
	{
		self->monsterinfo.aiflags &= ~AI_HOLD_FRAME;
		return;
	}

	// PMM
	if (self->monsterinfo.attack_state == AS_BLIND)
		end = self->monsterinfo.blind_fire_target;
	else
		end = self->enemy->s.origin;
	// pmm
	end[2] += self->enemy->viewheight;
	aim = end - start;
	aim_good = end;
	dir = vectoangles(aim);
	AngleVectors(dir, forward, right, up);

	r = crandom() * 1000;
	u = crandom() * 500;

	end = start + (forward * 8192);
	end += (right * r);
	end += (up * u);

	aim = end - start;
	aim.normalize();
	
	gi.sound (self, CHAN_VOICE, sound_attack, 1, ATTN_NORM, 0);
	monster_fire_shotgun (self, start, aim, 2, 1, DEFAULT_SHOTGUN_HSPREAD, DEFAULT_SHOTGUN_VSPREAD, DEFAULT_SHOTGUN_COUNT, MZ2_SOLDIER_SHOTGUN_1);
}

void army_attack_refire (edict_t *self)
{
	if (self->monsterinfo.aiflags & AI_MANUAL_STEERING)
	{
		self->monsterinfo.aiflags &= ~AI_MANUAL_STEERING;
		return;
	}

	if (!self->enemy)
		return;

	if (self->count > 1)
		return;

	if (self->enemy->health <= 0)
		return;

	if (((frandom() < 0.5f) && visible(self, self->enemy)) || (range_to(self, self->enemy) <= RANGE_MELEE))
		self->monsterinfo.nextframe = FRAME_shoot1;
	else
		self->monsterinfo.nextframe = FRAME_shoot8;
}

mframe_t army_frames_attack [] =
{
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, army_fire},
	{ai_charge},
	{ai_charge, 0, army_attack_refire},
	{ai_charge},
	{ai_charge}
};
MMOVE_T(army_move_attack) = {FRAME_shoot1, FRAME_shoot9, army_frames_attack, army_run};

MONSTERINFO_ATTACK(army_attack) (edict_t* self) -> void
{
	M_SetAnimation(self, &army_move_attack);
}

//
// SIGHT
//

MONSTERINFO_SIGHT(army_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound (self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);

	if (self->enemy && (range_to(self, self->enemy) >= RANGE_NEAR) && visible(self, self->enemy))
	{
		M_SetAnimation(self, &army_move_attack);
	}
}

//
// DEATH
//
mframe_t army_frames_death1 [] =
{
	{ai_move},
	{ai_move, -10},
	{ai_move, -10},
	{ai_move, -10},
	{ai_move, -5},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(army_move_death1) = {FRAME_death1, FRAME_death10, army_frames_death1, monster_dead };

mframe_t army_frames_death2 [] =
{
	{ai_move},
	{ai_move, -5},
	{ai_move, -4},
	{ai_move, -13},
	{ai_move, -3},
	{ai_move, -4},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(army_move_death2) = {FRAME_deathc1, FRAME_deathc11, army_frames_death2, monster_dead };

DIE(army_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	// check for gib
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 3, "models/objects/gibs/sm_meat/tris.md2" },
			//{ "models/monsters/army/gibs/chest.md2", GIB_SKINNED },
			//{ 2, "models/monsters/army/gibs/arm.md2", GIB_SKINNED | GIB_UPRIGHT },
			//{ "models/monsters/army/gibs/gun.md2", GIB_SKINNED | GIB_UPRIGHT },
			//{ 2, "models/monsters/army/gibs/leg.md2", GIB_SKINNED },
			{ "models/objects/gibs/chest/tris.md2" },
			{ "models/monsters/army/gibs/head.md2", GIB_SKINNED | GIB_HEAD }
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

	if (frandom() < 0.5)
		M_SetAnimation(self, &army_move_death1);
	else
		M_SetAnimation(self, &army_move_death2);
}

/*QUAKED monster_army (1 .5 0) (-16 -16 -24) (16 16 40) Ambush Trigger_Spawn Sight
model="models/monsters/army/tris.md2"
*/
void SP_monster_army (edict_t *self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}
	
	sound_idle.assign("army/idle.wav");
	sound_sight.assign("army/sight1.wav");
	sound_pain1.assign("army/pain1.wav");
	sound_pain2.assign("army/pain2.wav");
	sound_death.assign("army/death1.wav");
	sound_attack.assign("army/sattck1.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex ("models/monsters/army/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 40 };
	
	self->health = 45 * st.health_multiplier;
	self->gib_health = -35;
	self->mass = 100;

	self->pain = army_pain;
	self->die = army_die;



	self->monsterinfo.aiflags |= AI_STINKY;

	self->monsterinfo.stand = army_stand;
	self->monsterinfo.walk = army_walk;
	self->monsterinfo.run = army_run;
	self->monsterinfo.attack = army_attack;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.sight = army_sight;
	self->monsterinfo.search = army_search;
	self->monsterinfo.setskin = army_setskin;

	gi.linkentity (self);

	M_SetAnimation(self, &army_move_stand);

	self->monsterinfo.scale = MODEL_SCALE;
	self->monsterinfo.can_jump = !(self->spawnflags & SPAWNFLAG_ARMY_NOJUMPING);
	self->monsterinfo.drop_height = 256;
	self->monsterinfo.jump_height = 68;

	walkmonster_start (self);
}

void Pickup_ArmyPack(edict_t* self)
{
	self->model = "models/items/pack/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_AMMO_SHELLS), ED_GetSpawnTemp());
	self->count = 5;
}