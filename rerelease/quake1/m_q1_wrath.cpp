/*
==============================================================================

QUAKE WRATH

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_wrath.h"

static cached_soundindex	sound_sight;
static cached_soundindex	sound_attack;
static cached_soundindex	sound_die;
static cached_soundindex	sound_pain;

static cached_soundindex sound_attack2;

MONSTERINFO_SIGHT(wrath_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound (self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

void wrath_attack_sound (edict_t *self)
{
	gi.sound (self, CHAN_VOICE, sound_attack, 1, ATTN_NORM, 0);
}

mframe_t wrath_frames_stand [] =
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
	{ai_stand}
};
MMOVE_T(wrath_move_stand) = { FRAME_wrthwk01, FRAME_wrthwk12, wrath_frames_stand, nullptr };

MONSTERINFO_STAND(wrath_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &wrath_move_stand);
}

mframe_t wrath_frames_walk [] =
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
	{ai_walk, 5}
};
MMOVE_T(wrath_move_walk) = { FRAME_wrthwk01, FRAME_wrthwk12, wrath_frames_walk, nullptr };

MONSTERINFO_WALK(wrath_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &wrath_move_walk);
}

mframe_t wrath_frames_run [] =
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
	{ai_run, 10}
};
MMOVE_T(wrath_move_run) = { FRAME_wrthwk01, FRAME_wrthwk12, wrath_frames_run, nullptr };

MONSTERINFO_RUN(wrath_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &wrath_move_run);
}

mframe_t wrath_frames_pain1 [] =
{	
		{ai_move},
		{ai_move},
		{ai_move},
		{ai_move},
		{ai_move},

		{ai_move}
};
MMOVE_T(wrath_move_pain1) = { FRAME_wrthpa01, FRAME_wrthpa06, wrath_frames_pain1, wrath_run };

mframe_t wrath_frames_pain2[] =
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

		{ai_move}
};
MMOVE_T(wrath_move_pain2) = { FRAME_wrthpb01, FRAME_wrthpb11, wrath_frames_pain2, wrath_run };

PAIN(wrath_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 2_sec;
	
	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare
	
	gi.sound (self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

	if (frandom() >= 0.4f)
		M_SetAnimation(self, &wrath_move_pain1);
	else
		M_SetAnimation(self, &wrath_move_pain2);
}

void wrath_dead (edict_t * self)
{
	T_RadiusDamage(self, self, 60, nullptr, 105, DAMAGE_NONE, MOD_BARREL);

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_EXPLOSION1);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	self->s.skinnum /= 2;

	ThrowGibs(self, 55, {
		{ 2, "models/objects/gibs/bone/tris.md2" },
		{ 4, "models/monsters/wrath/gibs/claw.md2" },
		{ 4, "models/monsters/wrath/gibs/arm.md2" },
		{ "models/monsters/overlord/gibs/ribs.md2" },
		{ "models/monsters/wrath/gibs/bone.md2", GIB_HEAD }
		});

	self->touch = nullptr;
}

mframe_t wrath_frames_die [] =
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
	{ai_move}
};
MMOVE_T(wrath_move_die) = { FRAME_wrthdt01, FRAME_wrthdt15, wrath_frames_die, wrath_dead };

DIE(wrath_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (self->deadflag)
		return;

	gi.sound(self, CHAN_VOICE, sound_die, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;

	M_SetAnimation(self, &wrath_move_die);
}

void wrath_fire (edict_t *self)
{
	vec3_t	forward, right;
	vec3_t	start;
	vec3_t	dir;
	vec3_t	vec;
	trace_t trace;
	int		rocketSpeed;
	vec3_t target;
	bool   blindfire = false;
	vec3_t offset = { 0, 0, 10 };
	int damage = 20;

	if (self->monsterinfo.aiflags & AI_MANUAL_STEERING)
		blindfire = true;
	else
		blindfire = false;

	if (!self->enemy || !self->enemy->inuse)
		return;

	AngleVectors(self->s.angles, forward, right, nullptr);
	start = M_ProjectFlashSource(self, offset, forward, right);

	rocketSpeed = 400;

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
			fire_vorepod(self, start, dir, damage, rocketSpeed, (float)damage, damage, 0.075f, 1);
		else
		{
			vec = target;
			vec += (right * -10);
			dir = vec - start;
			dir.normalize();
			trace = gi.traceline(start, vec, self, MASK_PROJECTILE);
			if (!(trace.startsolid || trace.allsolid || (trace.fraction < 0.5f)))
				fire_vorepod(self, start, dir, damage, rocketSpeed, (float)damage, damage, 0.075f, 1);
			else
			{
				vec = target;
				vec += (right * 10);
				dir = vec - start;
				dir.normalize();
				trace = gi.traceline(start, vec, self, MASK_PROJECTILE);
				if (!(trace.startsolid || trace.allsolid || (trace.fraction < 0.5f)))
					fire_vorepod(self, start, dir, damage, rocketSpeed, (float)damage, damage, 0.075f, 1);
			}
		}
	}
	else
	{
		if (trace.fraction > 0.5f || trace.ent->solid != SOLID_BSP)
			fire_vorepod(self, start, dir, damage, rocketSpeed, (float)damage, damage, 0.15f, 1);
	}

	gi.sound(self, CHAN_WEAPON | CHAN_RELIABLE, sound_attack2, 1, ATTN_NORM, 0);
}

mframe_t wrath_frames_attack1 [] = {
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, wrath_attack_sound},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, -1},
	{ai_charge, -2},

	{ai_charge, -3},
	{ai_charge, -2, wrath_fire},
	{ai_charge, -1},
	{ai_charge}
};
MMOVE_T(wrath_move_attack1) = { FRAME_wrthaa01, FRAME_wrthaa14, wrath_frames_attack1, wrath_run };

mframe_t wrath_frames_attack2[] = {
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, wrath_attack_sound},
	{ai_charge},
	{ai_charge},
	{ai_charge, -1},
	{ai_charge, -2},
	{ai_charge, -3},
	{ai_charge, -2, wrath_fire},
	{ai_charge, -1},

	{ai_charge},
	{ai_charge},
	{ai_charge}
};
MMOVE_T(wrath_move_attack2) = { FRAME_wrthab01, FRAME_wrthab13, wrath_frames_attack2, wrath_run };

mframe_t wrath_frames_attack3[] = {
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, wrath_attack_sound},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, -1},
	{ai_charge, -2},
	{ai_charge, -3},
	{ai_charge, -2, wrath_fire},

	{ai_charge, -1},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge}
};
MMOVE_T(wrath_move_attack3) = { FRAME_wrthac01, FRAME_wrthac15, wrath_frames_attack3, wrath_run };

MONSTERINFO_ATTACK(wrath_attack) (edict_t* self) -> void
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

		if (frandom() > 0.33f)
			M_SetAnimation(self, &wrath_move_attack1);
		else if (frandom() > 0.66f)
			M_SetAnimation(self, &wrath_move_attack2);
		else
			M_SetAnimation(self, &wrath_move_attack3);	

		self->monsterinfo.attack_finished = level.time + random_time(2_sec);
		return;
	}

	if (frandom() > 0.33f)
		M_SetAnimation(self, &wrath_move_attack1);
	else if (frandom() > 0.66f)
		M_SetAnimation(self, &wrath_move_attack2);
	else
		M_SetAnimation(self, &wrath_move_attack3);		
}

static void wrath_set_fly_parameters(edict_t* self)
{
	self->monsterinfo.fly_thrusters = false;
	self->monsterinfo.fly_acceleration = 20.f;
	self->monsterinfo.fly_speed = 120.f;
	self->monsterinfo.fly_min_distance = 200.f;
	self->monsterinfo.fly_max_distance = 400.f;
}

/*QUAKED monster_wrath (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
model="models/monsters/wrath/tris.md2"
*/
void SP_monster_wrath (edict_t *self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_sight.assign("wrath/wsee.wav");
	sound_attack.assign("wrath/watt.wav");
	sound_die.assign("wrath/wdthc.wav");
	sound_pain.assign("wrath/wpain.wav");


	sound_attack2.assign("vore/attack2.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex ("models/monsters/wrath/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };

	self->health = 400 * st.health_multiplier;
	self->mass = 400;

	self->pain = wrath_pain;
	self->die = wrath_die;



	self->monsterinfo.stand = wrath_stand;
	self->monsterinfo.walk = wrath_walk;
	self->monsterinfo.run = wrath_run;
	self->monsterinfo.attack = wrath_attack;
	self->monsterinfo.melee = nullptr;
	self->monsterinfo.sight = wrath_sight;
	self->monsterinfo.search = nullptr;

	gi.linkentity (self);
	self->flags |= FL_DEEPONE;

	M_SetAnimation(self, &wrath_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;
	
	self->flags |= FL_FLY;
	if (!self->yaw_speed)
		self->yaw_speed = 10;
	self->viewheight = 10;

	flymonster_start (self);

	self->monsterinfo.aiflags |= AI_ALTERNATE_FLY;
	wrath_set_fly_parameters(self);
}
