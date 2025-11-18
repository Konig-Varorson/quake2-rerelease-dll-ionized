// Licensed under the GNU General Public License 2.0.
/*
==============================================================================

QUAKE CENTROID

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_centroid.h"


static cached_soundindex sound_idle;
static cached_soundindex sound_pain1;
static cached_soundindex sound_pain2;
static cached_soundindex sound_sight;
static cached_soundindex sound_search;
static cached_soundindex sound_melee1;
static cached_soundindex sound_melee2;
static cached_soundindex sound_walk;
static cached_soundindex sound_fire1;
static cached_soundindex sound_fire2;
static cached_soundindex sound_death;

//
// SOUNDS
//

MONSTERINFO_IDLE(centroid_idle) (edict_t* self) -> void
{
	if (frandom() <= 0.5f)
		gi.sound(self, CHAN_VOICE, sound_idle, 1, ATTN_NORM, 0);
}

MONSTERINFO_SEARCH(centroid_search) (edict_t* self) -> void
{
	if (frandom() <= 0.5f)
		gi.sound(self, CHAN_VOICE, sound_search, 1, ATTN_NORM, 0);
}

MONSTERINFO_SIGHT(centroid_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound(self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

//
// STAND
//

mframe_t centroid_frames_stand[] = {
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
MMOVE_T(centroid_move_stand) = { FRAME_stand1, FRAME_stand12, centroid_frames_stand, nullptr };

MONSTERINFO_STAND(centroid_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &centroid_move_stand);
}

// 
// WALK
// 

mframe_t centroid_frames_walk[] = {
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },
	{ ai_walk, 8 },

	{ ai_walk, 8 }
};
MMOVE_T(centroid_move_walk) = { FRAME_walk1, FRAME_walk6, centroid_frames_walk, nullptr };

MONSTERINFO_WALK(centroid_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &centroid_move_walk);
}

// 
// RUN
// 

mframe_t centroid_frames_run[] = {
	{ ai_run, 14 },
	{ ai_run, 14 },
	{ ai_run, 14 },
	{ ai_run, 14 },
	{ ai_run, 14 },
	{ ai_run, 14, monster_done_dodge }
};
MMOVE_T(centroid_move_run) = { FRAME_walk1, FRAME_walk6, centroid_frames_run, nullptr };

mframe_t centroid_frames_run2[] = {
	{ ai_run, 28 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 24 },
	{ ai_run, 22 },
	{ ai_run, 19, monster_done_dodge }
};
MMOVE_T(centroid_move_run2) = { FRAME_walk1, FRAME_walk6, centroid_frames_run2, nullptr };

MONSTERINFO_RUN(centroid_run) (edict_t* self) -> void
{
	monster_done_dodge(self);

	if (self->monsterinfo.aiflags & AI_STAND_GROUND)
	{
		M_SetAnimation(self, &centroid_move_stand);
		return;
	}

	M_SetAnimation(self, &centroid_move_run);
}

// 
// ATTACKS
// 

void centroid_attack2(edict_t* self);
void centroid_reattack(edict_t* self);

void centroid_fire_inner(edict_t* self)
{
	vec3_t					start;
	vec3_t					forward, right, up;
	vec3_t					aim;
	gvec3_t					offset_right, offset_left;

	AngleVectors(self->s.angles, forward, right, up);

	offset_right = { 19, 26 , -14 };
	offset_left = { 19, -26, -14 };

	start = M_ProjectFlashSource(self, offset_right, forward, right);
	PredictAim(self, self->enemy, start, 600, false, frandom() * 0.3f, &aim, nullptr);
	fire_flechette(self, start, aim, 9, 600, 9 / 2);

	start = M_ProjectFlashSource(self, offset_left, forward, right);
	PredictAim(self, self->enemy, start, 600, false, frandom() * 0.3f, &aim, nullptr);
	fire_flechette(self, start, aim, 9, 600, 9 / 2);

	gi.sound(self, CHAN_WEAPON, sound_fire1, 1, ATTN_NORM, 0);
}

void centroid_fire_outer(edict_t* self)
{
	vec3_t					start;
	vec3_t					forward, right, up;
	vec3_t					aim;
	gvec3_t					offset_right, offset_left;

	AngleVectors(self->s.angles, forward, right, up);

	offset_right = { 19, 38 , -14 };
	offset_left = { 19, -38, -14 };

	start = M_ProjectFlashSource(self, offset_right, forward, right);
	PredictAim(self, self->enemy, start, 600, false, frandom() * 0.3f, &aim, nullptr);
	fire_flechette(self, start, aim, 9, 600, 9 / 2);

	start = M_ProjectFlashSource(self, offset_left, forward, right);
	PredictAim(self, self->enemy, start, 600, false, frandom() * 0.3f, &aim, nullptr);
	fire_flechette(self, start, aim, 9, 600, 9 / 2);

	gi.sound(self, CHAN_WEAPON, sound_fire1, 1, ATTN_NORM, 0);
}

void centroid_fire_attack(edict_t* self)
{
	if (frandom() <= 0.25f)
		centroid_fire_outer(self);
	else if (frandom() <= 0.5f)
		centroid_fire_inner(self);
}

mframe_t centroid_frames_attack1[] = {
	{ ai_charge, 14, centroid_fire_inner },
	{ ai_charge, 14, centroid_fire_outer }
};
MMOVE_T(centroid_move_attack1) = { FRAME_shoot1, FRAME_shoot2, centroid_frames_attack1, centroid_attack2 };

mframe_t centroid_frames_attack2[] = {
	{ ai_charge, 14, centroid_fire_inner },
	{ ai_charge, 14, [](edict_t* self) { centroid_fire_outer(self); centroid_reattack(self); } },
};
MMOVE_T(centroid_move_attack2) = { FRAME_shoot1, FRAME_shoot2, centroid_frames_attack2, nullptr };

void centroid_attack2(edict_t* self)
{
	M_SetAnimation(self, &centroid_move_attack2);
}

void centroid_reattack(edict_t* self)
{
	if (self->enemy->health > 0 && (visible(self, self->enemy)) && frandom() <= 0.6f)
	{
		if (frandom() <= 0.3f)
			M_SetAnimation(self, &centroid_move_attack1);
		else
			M_SetAnimation(self, &centroid_move_run);
	}
	else
		M_SetAnimation(self, &centroid_move_run);
}

MONSTERINFO_ATTACK(centroid_attack) (edict_t* self) -> void
{
	M_SetAnimation(self, &centroid_move_attack1);
}

void centroid_swing(edict_t* self)
{
	gi.sound(self, CHAN_WEAPON, sound_melee1, 1, ATTN_NORM, 0);
}

void centroid_stinger(edict_t* self)
{
	vec3_t aim = { MELEE_DISTANCE, self->mins[0], 10 };
	gi.sound(self, CHAN_WEAPON, sound_melee2, 1, ATTN_NORM, 0);
	fire_hit(self, aim, irandom(10, 40), 100);
}

mframe_t centroid_frames_melee[] = {
	{ ai_charge, 3 },
	{ ai_charge, 3 },
	{ ai_charge, 2 },
	{ ai_charge, 2 },
	{ ai_charge, 3, centroid_swing },

	{ ai_charge, 1 },
	{ ai_charge, 0, centroid_stinger },
	{ ai_charge },
	{ ai_charge },
	{ ai_charge },

	{ ai_charge }
};
MMOVE_T(centroid_move_melee) = { FRAME_sting1, FRAME_sting11, centroid_frames_melee, centroid_run };

MONSTERINFO_MELEE(centroid_melee) (edict_t* self) -> void
{
	M_SetAnimation(self, &centroid_move_melee);
}

// 
// DODGE
// 

void centroid_jump_straightup(edict_t* self)
{
	if (self->deadflag)
		return;
	if (self->groundentity)
	{
		self->velocity[0] += crandom() * 5;
		self->velocity[1] += crandom() * 5;
		self->velocity[2] += -400 * self->gravityVector[2];
	}
}

void centroid_jump_wait_land(edict_t* self)
{
	if ((frandom() < 0.4f) && (level.time >= self->monsterinfo.attack_finished))
	{
		self->monsterinfo.attack_finished = level.time + 300_ms;
		centroid_fire_attack(self);
	}

	if (self->groundentity == nullptr)
	{
		self->gravity = 1.3f;
		self->monsterinfo.nextframe = self->s.frame;

		if (monster_jump_finished(self))
		{
			self->gravity = 1;
			self->monsterinfo.nextframe = self->s.frame + 1;
		}
	}
	else
	{
		self->gravity = 1;
		self->monsterinfo.nextframe = self->s.frame + 1;
	}
}

mframe_t centroid_frames_jump[] = {
	{ ai_move, 1, centroid_jump_straightup },
	{ ai_move, 1, centroid_jump_wait_land },
	{ ai_move, 1 },
	{ ai_move, -1 },
	{ ai_move, -1 },
	{ ai_move, -1 }
};
MMOVE_T(centroid_move_jump) = { FRAME_walk1, FRAME_walk6, centroid_frames_jump, centroid_run };

void centroid_dodge_jump(edict_t* self)
{
	M_SetAnimation(self, &centroid_move_jump);
}

bool centroid_sidestep(edict_t* self);

MONSTERINFO_DODGE(centroid_dodge) (edict_t* self, edict_t* attacker, gtime_t eta, trace_t* tr, bool gravity) -> void
{
	if (!self->groundentity || self->health <= 0)
		return;

	if (frandom() <= 0.66)
	{
		centroid_sidestep(self);
		return;
	}

	if (!self->enemy)
	{
		self->enemy = attacker;
		FoundTarget(self);
		return;
	}

	if ((eta < FRAME_TIME_MS) || (eta > 5_sec))
		return;

	if (self->timestamp > level.time)
		return;

	self->timestamp = level.time + random_time(1_sec, 5_sec);
	centroid_dodge_jump(self);
}

MONSTERINFO_SIDESTEP(centroid_sidestep) (edict_t* self) -> bool
{
	if ((self->monsterinfo.active_move == &centroid_move_jump))
		return false;

	if (self->monsterinfo.active_move != &centroid_move_run || self->monsterinfo.active_move != &centroid_move_run2)
		M_SetAnimation(self, &centroid_move_run2);

	return true;
}

MONSTERINFO_BLOCKED(centroid_blocked) (edict_t *self, float dist) -> bool
{
	if (blocked_checkplat(self, dist))
		return true;

	return false;
}

// 
// PAIN
// 

mframe_t centroid_frames_pain1[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move },
	{ ai_move }
};
MMOVE_T(centroid_move_pain) = { FRAME_pain1, FRAME_pain5, centroid_frames_pain1, centroid_run };

PAIN(centroid_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	monster_done_dodge(self);

	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 3_sec;

	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare

	float r = frandom();
	if (r < 0.5f)
		gi.sound(self, CHAN_VOICE, sound_pain1, 1, ATTN_NORM, 0);
	else
		gi.sound(self, CHAN_VOICE, sound_pain2, 1, ATTN_NORM, 0);

	M_SetAnimation(self, &centroid_move_pain);
}

MONSTERINFO_SETSKIN(centroid_setskin) (edict_t* self) -> void
{
	if (self->health < (self->max_health / 2))
		self->s.skinnum |= 1;
	else
		self->s.skinnum &= ~1;
}

// 
// DEATH
// 

void centroid_dead(edict_t* self)
{
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 8 };
	monster_dead(self);
}

static void centroid_shrink(edict_t* self)
{
	self->maxs[2] = 12;
	self->svflags |= SVF_DEADMONSTER;
	gi.linkentity(self);
}

mframe_t centroid_frames_death[] = {
	{ ai_move },
	{ ai_move },
	{ ai_move, 0, centroid_shrink },
	{ ai_move },
	{ ai_move, 0, monster_footstep }
};
MMOVE_T(centroid_move_death) = { FRAME_death1, FRAME_death5, centroid_frames_death, centroid_dead };

DIE(centroid_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (M_CheckGib(self, mod))
	{
		gi.sound(self, CHAN_VOICE, gi.soundindex("misc/udeath.wav"), 1, ATTN_NORM, 0);

		self->s.skinnum /= 2;

		ThrowGibs(self, damage, {
			{ 2, "models/objects/gibs/bone/tris.md2" },
			{ 3, "models/objects/gibs/sm_meat/tris.md2" },
			{ 2, "models/objects/gibs/sm_metal/tris.md2" },
			{ "models/monsters/centroid/gibs/head.md2", GIB_HEAD | GIB_SKINNED }
			});
		self->deadflag = true;

		return;
	}

	if (self->deadflag)
		return;

	self->deadflag = true;
	self->takedamage = true;

	M_SetAnimation(self, &centroid_move_death);
	gi.sound(self, CHAN_VOICE, sound_death, 1, ATTN_NORM, 0);
}


/*QUAKED monster_centroid (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight NoJumping
 */
void SP_monster_centroid(edict_t *self)
{
	const spawn_temp_t &st = ED_GetSpawnTemp();

	if ( !M_AllowSpawn( self ) ) {
		G_FreeEdict( self );
		return;
	}

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;
	self->s.modelindex = gi.modelindex("models/monsters/centroid/tris.md2");

	sound_idle.assign("centroid/idle1.wav");
	sound_pain1.assign("centroid/pain.wav");
	sound_pain2.assign("centroid/pain2.wav");
	sound_sight.assign("centroid/sight.wav");
	sound_search.assign("centroid/sight.wav");
	sound_melee1.assign("centroid/tailswing.wav");
	sound_melee2.assign("centroid/tailswing.wav");
	sound_walk.assign("centroid/walk.wav");
	sound_fire1.assign("weapons/nail1.wav");
	sound_fire2.assign("guncmdr/gcdratck2.wav");
	sound_death.assign("centroid/pain2.wav");

	gi.modelindex("models/monsters/centroid/gibs/head.md2");
	gi.modelindex("models/monsters/centroid/gibs/chest.md2");
	gi.modelindex("models/monsters/centroid/gibs/gun.md2");
	gi.modelindex("models/monsters/centroid/gibs/arm.md2");
	gi.modelindex("models/monsters/centroid/gibs/foot.md2");

	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };

	self->health = 250 * st.health_multiplier;
	if (!st.was_key_specified("power_type"))
		self->monsterinfo.armor_type = IT_ARMOR_COMBAT;
	if (!st.was_key_specified("power_power"))
		self->monsterinfo.armor_power = 100;
	self->gib_health = -65;
	self->mass = 400;

	self->pain = centroid_pain;
	self->die = centroid_die;

	self->monsterinfo.combat_style = COMBAT_MIXED;

	self->monsterinfo.search = centroid_search;
	self->monsterinfo.sight = centroid_sight;
	self->monsterinfo.idle = centroid_idle;

	self->monsterinfo.stand = centroid_stand;
	self->monsterinfo.walk = centroid_walk;
	self->monsterinfo.run = centroid_run;

	self->monsterinfo.dodge = M_MonsterDodge;
	self->monsterinfo.unduck = monster_duck_up;
	self->monsterinfo.sidestep = centroid_sidestep;
	self->monsterinfo.blocked = centroid_blocked;

	self->monsterinfo.attack = centroid_attack;
	self->monsterinfo.melee = centroid_melee;
	self->monsterinfo.setskin = centroid_setskin;

	gi.linkentity(self);
	self->flags |= FL_DEEPONE;

	M_SetAnimation(self, &centroid_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;

	walkmonster_start(self);
}