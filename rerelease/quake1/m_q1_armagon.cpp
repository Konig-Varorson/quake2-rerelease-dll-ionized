
/*
==============================================================================

QUAKE ARMAGON

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_armagon.h"
#include "../m_flash.h"

static cached_soundindex sound_idle;
static cached_soundindex sound_idle1;
static cached_soundindex sound_idle2;
static cached_soundindex sound_search1;
static cached_soundindex sound_search2;
static cached_soundindex sound_walk;
static cached_soundindex sound_pain;
static cached_soundindex sound_death;
static cached_soundindex sound_sight1;
static cached_soundindex sound_sight2;
static cached_soundindex sound_repel;
static cached_soundindex sound_servo;


MONSTERINFO_SEARCH(armagon_search) (edict_t* self) -> void
{
	if (frandom() <= 0.5f)
		gi.sound(self, CHAN_VOICE, sound_search1, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_search2, 1, ATTN_NORM, 0);
}

MONSTERINFO_IDLE(armagon_idle) (edict_t* self) -> void
{
	if (frandom() <= 0.3f)
		gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_NORM, 0);
	else if (frandom() <= 0.6f)
		gi.sound(self, CHAN_VOICE, sound_idle1, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_idle2, 1, ATTN_NORM, 0);
}

MONSTERINFO_SIGHT(armagon_sight) (edict_t* self, edict_t* other) -> void
{
	if (frandom() <= 0.2f)
		gi.sound(self, CHAN_VOICE, sound_sight1, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_sight2, 1, ATTN_NORM, 0);
}

mframe_t armagon_frames_stand[] = {
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },

	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand },
	{ ai_stand }
};
MMOVE_T(armagon_move_stand) = { FRAME_stand1, FRAME_stand20, armagon_frames_stand, nullptr };

MONSTERINFO_STAND(armagon_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &armagon_move_stand);
}

//
// WALK / RUN
//

void armagon_servo(edict_t* self)
{
	gi.sound(self, CHAN_VOICE, sound_servo, 1, ATTN_NORM, 0);
}

void armagon_footfall(edict_t* self)
{
	gi.sound(self, CHAN_VOICE, sound_walk, 1, ATTN_NORM, 0);
}

mframe_t armagon_frames_walk[] = {
	{ ai_walk, 14 },
	{ ai_walk, 14 },
	{ ai_walk, 14, armagon_servo },
	{ ai_walk, 14 },
	{ ai_walk, 14, armagon_footfall },
	{ ai_walk, 14 },
	{ ai_walk, 14 },
	{ ai_walk, 14 },
	{ ai_walk, 14, armagon_servo },
	{ ai_walk, 14 },

	{ ai_walk, 14, armagon_footfall },
	{ ai_walk, 14 }
};
MMOVE_T(armagon_move_walk) = { FRAME_walk1, FRAME_walk12, armagon_frames_walk, nullptr };

MONSTERINFO_WALK(armagon_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &armagon_move_walk);
}

mframe_t armagon_frames_run[] = {
	{ ai_run, 14 },
	{ ai_run, 14 },
	{ ai_run, 14, armagon_servo },
	{ ai_run, 14 },
	{ ai_run, 14, armagon_footfall },
	{ ai_run, 14 },
	{ ai_run, 14 },
	{ ai_run, 14 },
	{ ai_run, 14, armagon_servo },
	{ ai_run, 14 },

	{ ai_run, 14, armagon_footfall },
	{ ai_run, 14 }
};
MMOVE_T(armagon_move_run) = { FRAME_walk1, FRAME_walk12, armagon_frames_run, nullptr };

MONSTERINFO_RUN(armagon_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &armagon_move_run);
}

//
// PAIN
//

mframe_t armagon_frames_pain[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(armagon_move_pain) = { FRAME_die1, FRAME_die6, armagon_frames_pain, armagon_run };

PAIN(armagon_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 1_sec;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	self->pain_debounce_time = level.time + 2_sec;
	if (damage > 25 && frandom() > 0.5f)
	{
		gi.sound(self, CHAN_AUTO, sound_pain, 1, ATTN_NORM, 0);
		M_SetAnimation(self, &armagon_move_pain);
	}
}

MONSTERINFO_SETSKIN(armagon_setskin) (edict_t* self) -> void
{
	/*KONIG - set painskin*/
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

//
// DEATH
//

void Armagonlegs_Spawn(const vec3_t& startpos, const vec3_t& angles);
void ThrowWidowGibSized(edict_t* self, const char* gibname, int damage, gib_type_t type, const vec3_t* startpos, int hitsound, bool fade);

static void armagon_throw_gib(edict_t* self, const vec3_t& offset)
{
	vec3_t f, r, u, point;
	AngleVectors(self->s.angles, f, r, u);
	point = G_ProjectSource2(self->s.origin, offset, f, r, u);

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_EXPLOSION1);
	gi.WritePosition(point);
	gi.multicast(point, MULTICAST_PVS, false);

	for (int i = 0; i < 3; i++)
	{
		ThrowWidowGibSized(self, "models/objects/gibs/sm_meat/tris.md2",
			100 + (int)frandom(50.0f), GIB_METALLIC, &point, 0, false);
	}
}

static void armagon_death_explode(edict_t* self)
{
	vec3_t offset;
	offset[0] = crandom() * 60.0f;
	offset[1] = crandom() * 60.0f;
	offset[2] = crandom() * 40.0f + 20.0f;

	armagon_throw_gib(self, offset);
}

static void armagon_gib(edict_t* self)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_EXPLOSION1_BIG);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	self->s.sound = 0;
	self->s.skinnum /= 2;

	ThrowGibs(self, 500, {
		{ 4, "models/objects/gibs/sm_meat/tris.md2" },
		{ 1, "models/objects/gibs/chest/tris.md2" },
		{ "models/objects/gibs/head2/tris.md2", GIB_HEAD }
		});
}

void armagon_dead(edict_t* self)
{
	if (self->spawnflags.has(SPAWNFLAG_MONSTER_DEAD))
	{
		self->deadflag = false;
		self->takedamage = true;
		return;
	}

	// Spawn legs entity
	Armagonlegs_Spawn(self->s.origin, self->s.angles);

	armagon_gib(self);
}

mframe_t armagon_frames_death[] = {
	{ ai_move, 0, armagon_death_explode },
	{ ai_move, 0, armagon_death_explode },
	{ ai_move },
	{ ai_move, 0, armagon_death_explode },
	{ ai_move },
	{ ai_move, 0, armagon_death_explode },
	{ ai_move },
	{ ai_move, 0, armagon_death_explode },
	{ ai_move },
	{ ai_move },

	{ ai_move, 0, armagon_death_explode },
	{ ai_move }
};
MMOVE_T(armagon_move_death) = { FRAME_die1, FRAME_die12, armagon_frames_death, armagon_dead };

DIE(armagon_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (self->spawnflags.has(SPAWNFLAG_MONSTER_DEAD))
	{
		if (M_CheckGib(self, mod))
		{
			armagon_gib(self);
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

	M_SetAnimation(self, &armagon_move_death);
}

MONSTERINFO_CHECKATTACK(armagon_checkattack) (edict_t* self) -> bool
{
	return M_CheckAttack_Base(self, 0.4f, 0.8f, 0.8f, 0.8f, 0.f, 0.f);
}

//
// ATTACKS
// 1 - right rocket, left rocket while walking
// 2 - right blaster, left bouncing blaster while walking
// 3 - triple volley of both rockets
// 4 - 5x volley of both bouncing blasters
// 5 - melee, no animation, launches player away FAR followed by #3 or #4
// Nightmare should have predictive aiming shots

void armagon_right_laser(edict_t* self)
{
	vec3_t forward, right, target;
	vec3_t start;
	vec3_t origin = { 40, 84, 66 };

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, origin, forward, right);
	target = self->enemy->s.origin;
	target[2] += self->enemy->viewheight;
	forward = target - start;
	forward.normalize();

	fire_ionripper(self, start, forward, 10, 1000, EF_IONRIPPER);
}

void armagon_left_laser(edict_t* self)
{
	vec3_t forward, right, target;
	vec3_t start;
	vec3_t origin = { -40, 84, 66 };

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, origin, forward, right);
	target = self->enemy->s.origin;
	target[2] += self->enemy->viewheight;
	forward = target - start;
	forward.normalize();

	fire_ionripper(self, start, forward, 10, 1000, EF_IONRIPPER);
}

void armagon_right_rocket(edict_t* self)
{
	vec3_t forward, right, target;
	vec3_t start;
	vec3_t origin = { 40, 84, 66 };

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, origin, forward, right);
	target = self->enemy->s.origin;
	target[2] += self->enemy->viewheight;
	forward = target - start;
	forward.normalize();

	if (skill->integer >= 3)
		fire_guardian_heat(self, start, forward, forward, 50, 450, 150, 35, 0.085f);
	else
		fire_heat(self, start, forward, 50, 450, 50, 50, 0.15f);
}

void armagon_left_rocket(edict_t* self)
{
	vec3_t forward, right, target;
	vec3_t start;
	vec3_t origin = { -40, 84, 66 };

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, origin, forward, right);
	target = self->enemy->s.origin;
	target[2] += self->enemy->viewheight;
	forward = target - start;
	forward.normalize();

	if (skill->integer >= 3)
		fire_guardian_heat(self, start, forward, forward, 50, 450, 150, 35, 0.085f);
	else
		fire_heat(self, start, forward, 50, 450, 50, 50, 0.15f);
}

void armagon_overright_attack(edict_t* self)
{
	vec3_t forward, right, target;
	vec3_t start;
	vec3_t origin = { 165, 84, 66 };

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, origin, forward, right);
	target = self->enemy->s.origin;
	target[2] += self->enemy->viewheight;
	forward = target - start;
	forward.normalize();

	fire_ionripper(self, start, forward, 10, 1000, EF_IONRIPPER);
}

void armagon_overleft_attack(edict_t* self)
{
	vec3_t forward, right, target;
	vec3_t start;
	vec3_t origin = { -165, 84, 66 };

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, origin, forward, right);
	target = self->enemy->s.origin;
	target[2] += self->enemy->viewheight;
	forward = target - start;
	forward.normalize();

	fire_ionripper(self, start, forward, 10, 1000, EF_IONRIPPER);
}

void armagon_rocket_volley(edict_t* self)
{
	armagon_right_rocket(self);
	armagon_left_rocket(self);
}

void armagon_laser_volley(edict_t* self)
{
	armagon_right_laser(self);
	armagon_left_laser(self);
}

mframe_t armagon_frames_walk_attack1[] = {
	{ ai_charge, 14 },
	{ ai_charge, 14, armagon_servo },
	{ ai_charge, 14 },
	{ ai_charge, 14, armagon_footfall },
	{ ai_charge, 14 },
	{ ai_charge, 14, armagon_right_laser },
	{ ai_charge, 14 },
	{ ai_charge, 14, armagon_servo },
	{ ai_charge, 14 },
	{ ai_charge, 14, armagon_footfall },

	{ ai_charge, 14, armagon_left_laser },
	{ ai_charge, 14 },
	{ ai_charge, 14 }
};
MMOVE_T(armagon_move_walk_attack1) = { FRAME_w0fir1, FRAME_w0fir13, armagon_frames_walk_attack1, armagon_run };

//attack to the left
mframe_t armagon_frames_walk_attack2[] = {
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, armagon_servo },
	{ ai_charge },
	{ ai_charge, 0, armagon_footfall },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, armagon_servo },
	{ ai_charge },
	{ ai_charge, 0, armagon_footfall },

	{ ai_charge, 0, armagon_overleft_attack },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(armagon_move_walk_attack2) = { FRAME_w1fir1, FRAME_w1fir15, armagon_frames_walk_attack2, armagon_run };

//attack to the right
mframe_t armagon_frames_walk_attack3[] = {
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, armagon_servo },
	{ ai_charge },
	{ ai_charge, 0, armagon_footfall },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, armagon_servo },
	{ ai_charge },
	{ ai_charge, 0, armagon_footfall },

	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(armagon_move_walk_attack3) = { FRAME_w2fir1, FRAME_w2fir12, armagon_frames_walk_attack3, armagon_run };

//attack rocket barrage
mframe_t armagon_frames_attack1[] = {
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, armagon_footfall },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, armagon_rocket_volley },
	{ ai_charge },

	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(armagon_move_attack1) = { FRAME_sfire1, FRAME_sfire12, armagon_frames_attack1, armagon_run };

//attack laser barrage
mframe_t armagon_frames_attack2[] = {
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, armagon_footfall },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge, 0, armagon_laser_volley },
	{ ai_charge },

	{ ai_charge },
	{ ai_charge }
};
MMOVE_T(armagon_move_attack2) = { FRAME_sfire1, FRAME_sfire12, armagon_frames_attack2, armagon_run };


MONSTERINFO_ATTACK(armagon_attack) (edict_t* self) -> void
{
	float range = range_to(self, self->enemy);


	if (range <= RANGE_MELEE && self->monsterinfo.melee_debounce_time <= level.time)
		return;

	// Check enemy position relative to armagon
	vec3_t vec;
	vec = self->enemy->s.origin - self->s.origin;
	vec[2] = 0;
	vec.normalize();

	vec3_t forward, right;
	AngleVectors(self->s.angles, forward, right, nullptr);
	forward[2] = 0;
	right[2] = 0;
	forward.normalize();
	right.normalize();
	
	float dot_right = vec[0] * right[0] + vec[1] * right[1] + vec[2] * right[2];
	float dot_forward = vec[0] * forward[0] + vec[1] * forward[1] + vec[2] * forward[2]; 

	// If enemy significantly to the side (not in front)
	if (fabs(dot_right) > 0.5f && dot_forward < 0.7f)
	{
		if (dot_right > 0)
		{
			// Enemy to right
			self->ideal_yaw = vectoyaw(vec);
			M_SetAnimation(self, &armagon_move_walk_attack3);
		}
		else
		{
			// Enemy to left
			self->ideal_yaw = vectoyaw(vec);
			M_SetAnimation(self, &armagon_move_walk_attack2);
		}
	}
	// Enemy in front - use barrage if ready
	else if (self->count > 5 && frandom() > 0.33f)
	{
		self->count = 0;
		M_SetAnimation(self, &armagon_move_attack1);
	}
	else if (self->count > 3 && frandom() > 0.33f)
	{
		self->count++;
		M_SetAnimation(self, &armagon_move_attack2);
	}
	else
	{
		self->count++;
		M_SetAnimation(self, &armagon_move_walk_attack1);
	}

}

//
// MELLE
//
//repulse attack

void T_SlamRadiusDamage(vec3_t point, edict_t* inflictor, edict_t* attacker, float damage, float kick, edict_t* ignore, float radius, mod_t mod);

void armagon_repulse(edict_t* self)
{
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_BERSERK_SLAM);
	vec3_t f, r, start;
	AngleVectors(self->s.angles, f, r, nullptr);
	start = M_ProjectFlashSource(self, { 20.f, 0.f, 14.f }, f, r);
	trace_t tr = gi.traceline(self->s.origin, start, self, MASK_SOLID);
	gi.WritePosition(tr.endpos);
	gi.WriteDir(f);
	gi.multicast(tr.endpos, MULTICAST_PHS, false);

	T_SlamRadiusDamage(tr.endpos, self, self, 50, 1000.f, self, 200.f, MOD_UNKNOWN);
	gi.sound(self, CHAN_VOICE, sound_repel, 1, ATTN_NORM, 0);
}

mframe_t armagon_frames_melee[] = {
	{ ai_charge },
	{ ai_charge, 0, armagon_repulse }
};
MMOVE_T(armagon_move_melee) = { FRAME_tran1, FRAME_tran2, armagon_frames_melee, armagon_run };


MONSTERINFO_MELEE(armagon_melee) (edict_t* self) -> void
{
	M_SetAnimation(self, &armagon_move_melee);
}


/*QUAKED monster_armagon (1 0 0) (-32 -32 -24) (32 32 64) Ambush
*/
void SP_monster_armagon(edict_t* self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_idle.assign("armagon/idle.wav");
	sound_idle1.assign("armagon/idle1.wav");
	sound_idle2.assign("armagon/idle2.wav");
	sound_search1.assign("armagon/idle3.wav");
	sound_search2.assign("armagon/idle4.wav");
	sound_walk.assign("armagon/footfall.wav");
	sound_pain.assign("armagon/pain.wav");
	sound_death.assign("armagon/death.wav");
	sound_sight1.assign("armagon/sight.wav");
	sound_sight2.assign("armagon/sight2.wav");
	sound_repel.assign("armagon/repel.wav");
	sound_servo.assign("armagon/servo.wav");

	self->movetype = MOVETYPE_NONE;

	self->solid = SOLID_BBOX;
	self->s.modelindex = gi.modelindex("models/monsters/armagon/tris.md2");
	self->s.modelindex2 = gi.modelindex("models/monsters/armagon/legs/tris.md2");

	self->mins = { -32, -32, -24 };
	self->maxs = { 32, 32, 64 };

	self->health = max(4000, 4000 + 1250 * (skill->integer - 1)) * st.health_multiplier;
	if (!st.was_key_specified("armor_type"))
		self->monsterinfo.armor_type = IT_ARMOR_BODY;
	if (!st.was_key_specified("armor_power"))
		self->monsterinfo.armor_power = max(500, 500 + 150 * (skill->integer - 1));
	if (coop->integer)
	{
		self->health += (500 * skill->integer) + (500 * (CountPlayers() - 1));
		self->monsterinfo.armor_power += (250 * skill->integer) + (250 * (CountPlayers() - 1));
	}

	self->gib_health = -1000;
	self->mass = 800;

	self->pain = armagon_pain;
	self->die = armagon_die;

	self->monsterinfo.stand = armagon_stand;
	self->monsterinfo.walk = armagon_walk;
	self->monsterinfo.run = armagon_run;
	self->monsterinfo.dodge = nullptr;
	self->monsterinfo.attack = armagon_attack;
	self->monsterinfo.melee = armagon_melee;
	self->monsterinfo.sight = armagon_sight;
	self->monsterinfo.search = armagon_search;
	self->monsterinfo.idle = armagon_idle;
	self->monsterinfo.checkattack = armagon_checkattack;
	self->monsterinfo.setskin = armagon_setskin;

	self->s.renderfx |= RF_FULLBRIGHT;

	gi.linkentity(self);

	self->monsterinfo.scale = MODEL_SCALE;
	M_SetAnimation(self, &armagon_move_stand);

	walkmonster_start(self);
}

// ****************************
// ArmagonLeg stuff
// ****************************
constexpr int32_t MAX_LEGSFRAME = 23;
constexpr gtime_t LEG_WAIT_TIME = 1_sec;

void ThrowMoreStuff(edict_t* self, const vec3_t& point);
void ThrowSmallStuff(edict_t* self, const vec3_t& point);

THINK(armagonlegs_think) (edict_t* self) -> void
{
	vec3_t offset;
	vec3_t point;
	vec3_t f, r, u;

	if (level.time >= gtime_t::from_sec(self->wait)) 
	{
		offset = { 11.77f, -7.24f, 23.31f };
		AngleVectors(self->s.angles, f, r, u);
		point = G_ProjectSource2(self->s.origin, offset, f, r, u);
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_EXPLOSION1);
		gi.WritePosition(point);
		gi.multicast(point, MULTICAST_ALL, false);
		ThrowSmallStuff(self, point);

		ThrowWidowGibSized(self, "models/objects/gibs/sm_metal/tris.md2", 80 + (int)frandom(20.0f), GIB_METALLIC, &point, 0, true);
		ThrowWidowGibSized(self, "models/objects/gibs/gear/tris.md2", 80 + (int)frandom(20.0f), GIB_METALLIC, &point, 0, true);
		ThrowWidowGibSized(self, "models/monsters/blackwidow/gib2/tris.md2", 80 + (int)frandom(20.0f), GIB_METALLIC, &point, 0, true);
		ThrowWidowGibSized(self, "models/monsters/blackwidow/gib4/tris.md2", 80 + (int)frandom(20.0f), GIB_METALLIC, &point, 0, true);

		G_FreeEdict(self);
		return;
	}
	if ((level.time > gtime_t::from_sec(self->wait - 0.5f)) && (self->count == 0))
	{
		self->count = 1;
		AngleVectors(self->s.angles, f, r, u);

		offset = { 31, -88.7f, 10.96f };
		point = G_ProjectSource2(self->s.origin, offset, f, r, u);
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_EXPLOSION1);
		gi.WritePosition(point);
		gi.multicast(point, MULTICAST_ALL, false);

		offset = { -12.67f, -4.39f, 15.68f };
		point = G_ProjectSource2(self->s.origin, offset, f, r, u);
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_EXPLOSION1);
		gi.WritePosition(point);
		gi.multicast(point, MULTICAST_ALL, false);

		self->nextthink = level.time + 10_hz;
		return;
	}
	self->nextthink = level.time + 10_hz;
}

void Armagonlegs_Spawn(const vec3_t& startpos, const vec3_t& angles)
{
	edict_t* ent;

	ent = G_Spawn();
	ent->s.origin = startpos;
	ent->s.angles = angles;
	ent->solid = SOLID_NOT;
	ent->s.renderfx = RF_IR_VISIBLE;
	ent->movetype = MOVETYPE_NONE;
	ent->classname = "armagonlegs";

	ent->s.modelindex = gi.modelindex("models/monsters/armagon/legs/tris.md2");
	ent->think = armagonlegs_think;

	ent->nextthink = level.time + 10_hz;
	gi.linkentity(ent);
}