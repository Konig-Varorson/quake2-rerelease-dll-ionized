/*
==============================================================================

QUAKE OGRE

==============================================================================
*/

#include "../g_local.h"
#include "m_ogre.h"
#include "../m_flash.h"

constexpr spawnflags_t SPAWNFLAG_OGRE_NOJUMPING = 8_spawnflag;

static cached_soundindex	sound_pain;
static cached_soundindex	sound_death;
static cached_soundindex	sound_idle;
static cached_soundindex	sound_idle2;
static cached_soundindex	sound_sight;
static cached_soundindex	sound_saw;
static cached_soundindex	sound_drag;

void ogre_check_refire (edict_t *self);
void ogre_attack (edict_t *self);
void ogre_stand(edict_t* self);
void ogre_flak_fire(edict_t* self);
void ogre_grenade_fire(edict_t* self);

MONSTERINFO_SIGHT(ogre_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound (self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

MONSTERINFO_SEARCH(ogre_search) (edict_t* self) -> void
{
	if (frandom() < 0.5f)
		gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_idle2, 1, ATTN_IDLE, 0);
}

void ogre_drag_sound (edict_t *self)
{
	if (frandom() <= 0.5f)
		gi.sound (self, CHAN_VOICE, sound_drag, 1, ATTN_IDLE, 0);
}

mframe_t ogre_frames_stand [] = {
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
};
MMOVE_T(ogre_move_stand) = { FRAME_stand1, FRAME_stand9, ogre_frames_stand, ogre_stand };

MONSTERINFO_STAND(ogre_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &ogre_move_stand);
}

mframe_t ogre_frames_walk [] = {
	{ai_walk, 3},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 2},
	{ai_walk, 6, ogre_drag_sound},
	{ai_walk, 3},
	{ai_walk, 2},
	{ai_walk, 3},
	{ai_walk, 1},

	{ai_walk, 2},
	{ai_walk, 3},
	{ai_walk, 3},
	{ai_walk, 3},
	{ai_walk, 3},
	{ai_walk, 4}
};
MMOVE_T(ogre_move_walk) = { FRAME_walk1, FRAME_walk16, ogre_frames_walk, nullptr};

MONSTERINFO_WALK(ogre_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &ogre_move_walk);
}

mframe_t ogre_frames_run [] = {
	{ai_run, 9},
	{ai_run, 12},
	{ai_run, 8},
	{ai_run, 22},
	{ai_run, 16},
	{ai_run, 4},
	{ai_run, 13, ogre_attack},
	{ai_run, 24}
};
MMOVE_T(ogre_move_run) = { FRAME_run1, FRAME_run8, ogre_frames_run, nullptr};

MONSTERINFO_RUN(ogre_run) (edict_t* self) -> void
{
	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
		M_SetAnimation(self, &ogre_move_stand);
	else
		M_SetAnimation(self, &ogre_move_run);
}

mframe_t ogre_frames_pain1 [] = {
	{ai_move, -3},
	{ai_move, 1},
	{ai_move, 1},
	{ai_move},
	{ai_move, 1}
};
MMOVE_T(ogre_move_pain1) = { FRAME_pain1, FRAME_pain5, ogre_frames_pain1, ogre_run };

mframe_t ogre_frames_pain2 [] = {
	{ai_move, -1},
	{ai_move},
	{ai_move, 1}
};
MMOVE_T(ogre_move_pain2) = { FRAME_painb1, FRAME_painb3, ogre_frames_pain2, ogre_run };


mframe_t ogre_frames_pain3 [] = {
	{ai_move, -3},
	{ai_move, 1},
	{ai_move, 1},
	{ai_move},
	{ai_move},
	{ai_move, 1}
};
MMOVE_T(ogre_move_pain3) = { FRAME_painc1, FRAME_painc6, ogre_frames_pain3, ogre_run };


mframe_t ogre_frames_pain4 [] = {
	{ai_move, -3},
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
	{ai_move, 1},
	{ai_move, 1},
	{ai_move}
};
MMOVE_T(ogre_move_pain4) = { FRAME_paind1, FRAME_paind16, ogre_frames_pain4, ogre_run };

mframe_t ogre_frames_pain5 [] =
{
	{ai_move, -3},
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
	{ai_move, 1},
	{ai_move, 1},
	{ai_move},
};
MMOVE_T(ogre_move_pain5) = {FRAME_paine1, FRAME_paine15, ogre_frames_pain5, ogre_run};

PAIN(ogre_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	float r = frandom();

	if (level.time < self->pain_debounce_time)
		return;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	gi.sound (self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);	

	if (r < 0.20)
	{
		self->pain_debounce_time = level.time + 1_sec;
		M_SetAnimation(self, &ogre_move_pain1);
	}
	else if (r < 0.4)
	{
		self->pain_debounce_time = level.time + 1_sec;
		M_SetAnimation(self, &ogre_move_pain2);
	}
	else if (r < 0.6)
	{
		self->pain_debounce_time = level.time + 1_sec;
		M_SetAnimation(self, &ogre_move_pain3);
	}
	else if (r < 0.8)
	{
		self->pain_debounce_time = level.time + 2_sec;
		M_SetAnimation(self, &ogre_move_pain4);
	}
	else
	{
		self->pain_debounce_time = level.time + 2_sec;
		M_SetAnimation(self,  &ogre_move_pain5);
	}
}

MONSTERINFO_SETSKIN(ogre_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

void Pickup_OgrePack1(edict_t* self)
{
	self->model = "models/items/pack/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_AMMO_GRENADES), ED_GetSpawnTemp());
	self->count = 2;
}

void Pickup_OgrePack2(edict_t* self)
{
	self->model = "models/items/pack/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_AMMO_FLECHETTES), ED_GetSpawnTemp());
	self->count = 5;
}

void Pickup_OgrePack3(edict_t* self)
{
	self->model = "models/items/pack/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_AMMO_GRENADES), ED_GetSpawnTemp());
	self->count = 2;
}

void ogre_droprockets (edict_t *self)
{
	//Pickup_OgrePack(self);
	if (self->health <= self->gib_health)
		return;
}

void ogre_dead (edict_t *self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, -8 };
	monster_dead(self);
}

mframe_t ogre_frames_death1 [] = {
	{ai_move},
	{ai_move},
	{ai_move, 0, ogre_droprockets},
	{ai_move, -7},
	{ai_move, -3},
	{ai_move, -5},
	{ai_move, 8},
	{ai_move, 6},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move},
	{ai_move}
};
MMOVE_T(ogre_move_death1) = {FRAME_death1, FRAME_death14, ogre_frames_death1, ogre_dead};

mframe_t ogre_frames_death2 [] = {
	{ai_move},
	{ai_move},
	{ai_move, 0, ogre_droprockets},
	{ai_move, -7},
	{ai_move, -3},

	{ai_move, -5},
	{ai_move, 8},
	{ai_move, 6},
	{ai_move},
	{ai_move}
};
MMOVE_T(ogre_move_death2) = {FRAME_bdeath1, FRAME_bdeath10, ogre_frames_death2, ogre_dead};

DIE(ogre_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 3, "models/objects/gibs/sm_meat/tris.md2" },
			//{ "models/monsters/ogre/gibs/chest.md2", GIB_SKINNED },
			//{ "models/monsters/ogre/gibs/chainsaw.md2", GIB_SKINNED | GIB_UPRIGHT },
			//{ "models/monsters/ogre/gibs/gun.md2", GIB_SKINNED | GIB_UPRIGHT },
			//{ "models/monsters/ogre/gibs/foot.md2", GIB_SKINNED },
			{ "models/monsters/ogre/gibs/head.md2", GIB_SKINNED | GIB_HEAD }
			});

		self->deadflag = true;
		return;
	}

	if (self->deadflag)
		return;

	// regular death
	gi.sound (self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	if (frandom() < 0.5)
		M_SetAnimation(self, &ogre_move_death1);
	else
		M_SetAnimation(self, &ogre_move_death2);
}

void ogre_fire(edict_t* self)
{
	if (strcmp(self->classname, "monster_ogre_marksman") == 0)
		ogre_flak_fire(self);
	else
		ogre_grenade_fire(self);
}

void ogre_flak_fire(edict_t* self)
{
	vec3_t						start;
	vec3_t						forward, right;
	vec3_t						aim;
	monster_muzzleflash_id_t	flash_number;
	int							spread = 500;

	if (!self->enemy || !self->enemy->inuse) // PGM
		return;								 // PGM

	flash_number = static_cast<monster_muzzleflash_id_t>(MZ2_GUNCMDR_GRENADE_FRONT_1 + (self->s.frame - FRAME_shoot3));

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, monster_flash_offset[flash_number], forward, right);
	PredictAim(self, self->enemy, start, 0, true, -0.2f, &aim, nullptr);
	monster_fire_flakcannon(self, start, aim, 4, 800, spread, spread, 5, flash_number);
}

void ogre_grenade_fire (edict_t *self)
{
	vec3_t					 start;
	vec3_t					 forward, right, up;
	vec3_t					 aim;
	monster_muzzleflash_id_t flash_number;
	float					 pitch = 0;
	// PMM
	vec3_t target;
	bool   blindfire = false;

	if (!self->enemy || !self->enemy->inuse) // PGM
		return;								 // PGM

	flash_number = MZ2_GUNCMDR_GRENADE_FRONT_1;

	// pmm
	if (self->monsterinfo.aiflags & AI_MANUAL_STEERING)
		blindfire = true;

	if ((blindfire) && (!visible(self, self->enemy)))
	{
		if (!self->monsterinfo.blind_fire_target)
			return;

		target = self->monsterinfo.blind_fire_target;
	}
	else
		target = self->enemy->s.origin;

	AngleVectors(self->s.angles, forward, right, up); // PGM
	start = M_ProjectFlashSource(self, monster_flash_offset[flash_number], forward, right);

	if (self->enemy)
	{
		float dist;

		aim = target - self->s.origin;
		dist = aim.length();

		// aim up if they're on the same level as me and far away.
		if ((dist > 512) && (aim[2] < 64) && (aim[2] > -64))
		{
			aim[2] += (dist - 512);
		}

		aim.normalize();
		pitch = aim[2];
		if (pitch > 0.4f)
			pitch = 0.4f;
		else if (pitch < -0.5f)
			pitch = -0.5f;
	}

	// mortar fires farther
	float speed = 600.f;

	// try search for best pitch
	if (M_CalculatePitchToFire(self, target, start, aim, speed, 2.5f, (flash_number >= MZ2_GUNCMDR_GRENADE_MORTAR_1 && flash_number <= MZ2_GUNCMDR_GRENADE_MORTAR_3)))
		if (strcmp(self->classname, "monster_ogre_multigrenade") == 0)
			monster_fire_multigrenade(self, start, aim, 40, speed, flash_number, (crandom_open() * 10.0f), frandom() * 10.f);
		else
			monster_fire_grenade(self, start, aim, 40, speed, flash_number, (crandom_open() * 10.0f), frandom() * 10.f);
	else
		if (strcmp(self->classname, "monster_ogre_multigrenade") == 0)
			monster_fire_multigrenade(self, start, aim, 40, speed, flash_number, (crandom_open() * 10.0f), frandom() * 10.f);
		else
			monster_fire_grenade(self, start, aim, 40, speed, flash_number, (crandom_open() * 10.0f), 200.f + (crandom_open() * 10.0f));
}

void ogre_swing_left (edict_t *self)
{
	vec3_t aim = { MELEE_DISTANCE, self->maxs[0], 8 };
	if (!fire_hit(self, aim, ((frandom() + frandom() + frandom()) * 4), 100))
		self->monsterinfo.melee_debounce_time = level.time + 1_sec;
}

void ogre_swing_right (edict_t *self)
{
	vec3_t aim = { MELEE_DISTANCE, self->maxs[0], 8 };
	if (!fire_hit(self, aim, ((frandom() + frandom() + frandom()) * 4), 100))
		self->monsterinfo.melee_debounce_time = level.time + 1_sec;
}

void ogre_smash (edict_t *self)
{
	vec3_t aim = { MELEE_DISTANCE, self->maxs[0], 8 };
	if (!fire_hit (self, aim, (25 + (frandom() * 5)), 100))
		self->monsterinfo.melee_debounce_time = level.time + 1.2_sec;

	gi.sound(self, CHAN_WEAPON, sound_saw, 1, ATTN_NORM, 0);
}

MONSTERINFO_MELEE(ogre_check_refire) (edict_t* self) -> void
{
	if (!self->enemy || !self->enemy->inuse || self->enemy->health <= 0)
		return;

	if ( (skill->value == 3) || (range_to(self, self->enemy) == RANGE_MELEE))
	{
		if (frandom() > 0.5)
			self->monsterinfo.nextframe = FRAME_swing1;
		else
			self->monsterinfo.nextframe = FRAME_smash1;
	}
	else
		ogre_attack(self);
}

void ogre_sawswingsound (edict_t *self)
{
	gi.sound (self, CHAN_WEAPON, sound_saw, 1, ATTN_NORM, 0);
}

mframe_t ogre_frames_swing [] = {
	
	{ai_charge},
	{ai_charge, 0, ogre_sawswingsound},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, ogre_swing_right},
	{ai_charge},
	{ai_charge, 0, ogre_sawswingsound},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, ogre_swing_left},
	{ai_charge},
	{ai_charge, 0, ogre_check_refire}
};
MMOVE_T(ogre_move_swing_attack) = { FRAME_swing1, FRAME_swing14, ogre_frames_swing, ogre_run };

mframe_t ogre_frames_smash [] = {
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, ogre_smash},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, ogre_check_refire}
};
MMOVE_T(ogre_move_smash_attack) = { FRAME_smash1, FRAME_smash14, ogre_frames_smash, ogre_run };

mframe_t ogre_frames_attack_grenade [] = {
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, ogre_fire},
	{ai_charge},
	{ai_charge}
};
MMOVE_T(ogre_move_attack_grenade) = { FRAME_shoot1, FRAME_shoot6, ogre_frames_attack_grenade, ogre_run };

MONSTERINFO_ATTACK(ogre_attack) (edict_t* self) -> void
{
	float r = range_to(self, self->enemy);

	if (!self->bad_area && r <= RANGE_MELEE && self->monsterinfo.melee_debounce_time <= level.time)
	{
		M_SetAnimation(self, &ogre_move_swing_attack);
	}
	else if (visible(self,self->enemy) && infront(self, self->enemy)
		&& (r <= RANGE_MID) && !(self->monsterinfo.aiflags & AI_SOUND_TARGET))
	{
		M_SetAnimation(self, &ogre_move_attack_grenade);
	}
	else
		M_SetAnimation(self, &ogre_move_run);
}

/*QUAKED monster_ogre (1 .5 0) (-20 -20 -24) (20 20 32) Ambush Trigger_Spawn Sight GoodGuy NoGib
model="models/monsters/ogre/tris.md2"
*/
void SP_monster_ogre (edict_t *self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_pain.assign("monsters/ogre/pain1.wav");
	sound_death.assign("monsters/ogre/death.wav");
	sound_idle.assign("monsters/ogre/idle.wav");
	sound_idle2.assign("monsters/ogre/idle2.wav");
	sound_sight.assign("monsters/ogre/wake.wav");
	sound_saw.assign("monsters/ogre/sawatk.wav");
	sound_drag.assign("monsters/ogre/drag.wav");

	gi.modelindex ("models/items/pack/tris.md2");
	gi.modelindex("models/monsters/ogre/gibs/head.md2");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;
	self->s.modelindex = gi.modelindex ("models/monsters/ogre/tris.md2");
	self->mins = { -20, -20, -24 };
	self->maxs = { 20, 20, 32 };

	self->health = 300 * st.health_multiplier;
	self->gib_health = -80;
	self->mass = 250;

	self->pain = ogre_pain;
	self->die = ogre_die;



	self->monsterinfo.aiflags |= AI_STINKY;

	self->monsterinfo.stand = ogre_stand;
	self->monsterinfo.walk = ogre_walk;
	self->monsterinfo.run = ogre_run;
	self->monsterinfo.dodge = nullptr;
	self->monsterinfo.attack = ogre_attack;
	self->monsterinfo.melee = ogre_check_refire;
	self->monsterinfo.sight = ogre_sight;
	self->monsterinfo.search = ogre_search;
	self->monsterinfo.setskin = ogre_setskin;

	gi.linkentity (self);

	M_SetAnimation(self, &ogre_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;
	self->monsterinfo.can_jump = !(self->spawnflags & SPAWNFLAG_OGRE_NOJUMPING);
	self->monsterinfo.drop_height = 256;
	self->monsterinfo.jump_height = 68;

	walkmonster_start (self);
}

void SP_monster_ogre_marksman(edict_t* self)
{
	SP_monster_ogre(self);
	self->s.skinnum = 2;
}

void SP_monster_ogre_multigrenade(edict_t* self)
{
	SP_monster_ogre(self);
	self->s.skinnum = 4;
}