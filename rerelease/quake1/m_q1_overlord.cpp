/*
==============================================================================

OVERLORD

==============================================================================
*/

#include "../g_local.h"
#include "m_q1_overlord.h"

static cached_soundindex	sound_sight;
static cached_soundindex	sound_attack;
static cached_soundindex	sound_melee;
static cached_soundindex	sound_die;
static cached_soundindex	sound_pain;

static cached_soundindex sound_attack2;

constexpr spawnflags_t SPAWNFLAG_OVERLORD_NO_TELEPORT = 8_spawnflag;

// sqrt(64*64*2) + sqrt(16*16*2) => 113.1
constexpr vec3_t spawnpoints[] = {
	{ 30, 128, 0 },
	{ 30, -128, 0 }
};

constexpr vec3_t wrath_mins = { -16, -16, -24 };
constexpr vec3_t wrath_maxs = { 16, 16, 32 };

//TODO
// DUDE TELEPORTS! Forgot about that. Boss2-Titan will use that code.

//
// SOUNDS
//

MONSTERINFO_SIGHT(overlord_sight) (edict_t* self, edict_t* other) -> void
{
	gi.sound (self, CHAN_VOICE, sound_sight, 1, ATTN_NORM, 0);
}

void overlord_attack_sound (edict_t *self)
{
	gi.sound (self, CHAN_VOICE, sound_attack, 1, ATTN_NORM, 0);
}

//
// STAND
//

mframe_t overlord_frames_stand [] =
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
	{ai_stand},
	{ai_stand},
	{ai_stand}
};
MMOVE_T(overlord_move_stand) = { FRAME_s_wtwk01, FRAME_s_wtwk15, overlord_frames_stand, nullptr };

MONSTERINFO_STAND(overlord_stand) (edict_t* self) -> void
{
	M_SetAnimation(self, &overlord_move_stand);
}

//
// WALK
//

mframe_t overlord_frames_walk [] =
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
	{ai_walk, 5},
	{ai_walk, 5},
	{ai_walk, 5}
};
MMOVE_T(overlord_move_walk) = { FRAME_s_wtwk01, FRAME_s_wtwk15, overlord_frames_walk, nullptr };

MONSTERINFO_WALK(overlord_walk) (edict_t* self) -> void
{
	M_SetAnimation(self, &overlord_move_walk);
}

//
// RUN
//

mframe_t overlord_frames_run [] =
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
	{ai_run, 10},
	{ai_run, 10},
	{ai_run, 10}
};
MMOVE_T(overlord_move_run) = { FRAME_s_wtwk01, FRAME_s_wtwk15, overlord_frames_run, nullptr };

MONSTERINFO_RUN(overlord_run) (edict_t* self) -> void
{
	M_SetAnimation(self, &overlord_move_run);
}

//
// TELEPORT
//

void OverlordTeleport(edict_t* self)
{
	float chance;

	if (self->spawnflags.has(SPAWNFLAG_OVERLORD_NO_TELEPORT))
		return;

	switch (skill->integer)
	{
	case 0:
	case 1:
		chance = frandom() * 2;
		break;
	case 2:
		chance = frandom() * 3;
		break;
	case 3:
		chance = frandom() * 4;
		break;
	default:
		chance = frandom() * 2;
		break;
	}

	if (chance <= 1)
		return;

	if (!TryRandomTeleportPosition(self, 128.0f)) {
		return;
	}
}

void overlord_surprise(edict_t* self)
{
	if (!self->enemy || !self->enemy->inuse || self->enemy->health <= 0)
		return;

	edict_t* target = self->enemy;

	vec3_t forward, target_angles, behind_pos;
	target_angles = target->s.angles;
	target_angles[PITCH] = 0; // Ignore pitch to stay horizontal
	AngleVectors(target_angles, forward, nullptr, nullptr);

	// Adjust this distance for melee range
	float back_distance = 64.0f;
	behind_pos = target->s.origin + (forward * -back_distance);

	// Raise the position if needed (since enemy is flying)
	behind_pos[2] += 32;

	// Check if the spot is clear
	trace_t tr = gi.trace(
		behind_pos,
		self->mins,
		self->maxs,
		behind_pos,
		self,
		MASK_MONSTERSOLID
	);

	if (tr.startsolid || tr.allsolid || tr.fraction < 1.0f)
		return; // Can't teleport — blocked

	// Teleport effect - departure
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_TELEPORT_EFFECT);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PVS, false);

	// Teleport to the cleared spot
	self->s.origin = behind_pos;
	gi.linkentity(self);

	// Face the player
	vec3_t dir;
	dir = target->s.origin - self->s.origin;
	self->s.angles = vectoangles(dir);

	// Teleport effect - arrival
	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_TELEPORT_EFFECT);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PVS, false);
}

//
// ATTACK
//

void overlord_spawn(edict_t* self)
{
	vec3_t	 f, r, u, offset, startpoint, spawnpoint;
	edict_t* ent, * designated_enemy;
	int		 i;

	AngleVectors(self->s.angles, f, r, u);

	for (i = 0; i < 2; i++)
	{
		offset = spawnpoints[i];

		startpoint = G_ProjectSource2(self->s.origin, offset, f, r, u);

		if (FindSpawnPoint(startpoint, wrath_mins, wrath_maxs, spawnpoint, 64))
		{
			ent = CreateFlyMonster(spawnpoint, self->s.angles, wrath_mins, wrath_maxs, "monster_wrath");

			if (!ent)
				continue;

			self->monsterinfo.monster_used++;
			ent->monsterinfo.commander = self;
			ent->monsterinfo.slots_from_commander = 1;

			ent->nextthink = level.time;
			ent->think(ent);

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

void overlord_ready_spawn(edict_t* self)
{
	vec3_t f, r, u, offset, startpoint, spawnpoint;
	int	   i;

	AngleVectors(self->s.angles, f, r, u);

	for (i = 0; i < 2; i++)
	{
		offset = spawnpoints[i];
		startpoint = G_ProjectSource2(self->s.origin, offset, f, r, u);
		if (FindSpawnPoint(startpoint, wrath_mins, wrath_maxs, spawnpoint, false))
		{
			float radius = (wrath_maxs - wrath_mins).length() * 0.5f;

			SpawnGrow_Spawn(spawnpoint + (wrath_mins + wrath_maxs), radius, radius * 2.f);
		}
	}
}

void OverlordCalcSlots(edict_t* self)
{
	switch (skill->integer)
	{
	case 0:
	case 1:
		self->monsterinfo.monster_slots = 2;
		break;
	case 2:
		self->monsterinfo.monster_slots = 3;
		break;
	case 3:
		self->monsterinfo.monster_slots = 4;
		break;
	default:
		self->monsterinfo.monster_slots = 2;
		break;
	}
	if (coop->integer)
	{
		self->monsterinfo.monster_slots = min(6, self->monsterinfo.monster_slots + (skill->integer * (CountPlayers() - 1)));
	}
}

void overlord_fire(edict_t* self)
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

	rocketSpeed = 450;

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
			fire_vorepod(self, start, dir, damage, rocketSpeed, (float)damage, damage, 0.015f, 1);
	}

	gi.sound(self, CHAN_WEAPON | CHAN_RELIABLE, sound_attack2, 1, ATTN_NORM, 0);
}

void overlord_hit_left(edict_t* self)
{
	vec3_t aim = { MELEE_DISTANCE, self->mins[0], 8 };
	if (fire_hit(self, aim, irandom(20, 30), 400))
		gi.sound(self, CHAN_WEAPON, sound_melee, 1, ATTN_NORM, 0);
	else
	{
		gi.sound(self, CHAN_WEAPON, sound_melee, 1, ATTN_NORM, 0);
		self->monsterinfo.melee_debounce_time = level.time + 1.5_sec;
	}
}

void overlord_hit_right(edict_t* self)
{
	vec3_t aim = { MELEE_DISTANCE, self->maxs[0], 8 };
	if (fire_hit(self, aim, irandom(20, 30), 400))
		gi.sound(self, CHAN_WEAPON, sound_melee, 1, ATTN_NORM, 0);
	else
	{
		gi.sound(self, CHAN_WEAPON, sound_melee, 1, ATTN_NORM, 0);
		self->monsterinfo.melee_debounce_time = level.time + 1.5_sec;
	}
}

void overlord_hit_right_hard(edict_t* self)
{
	vec3_t aim = { MELEE_DISTANCE, self->maxs[0], 8 };
	if (fire_hit(self, aim, irandom(40, 60), 400))
		gi.sound(self, CHAN_WEAPON, sound_melee, 1, ATTN_NORM, 0);
	else
	{
		gi.sound(self, CHAN_WEAPON, sound_melee, 1, ATTN_NORM, 0);
		self->monsterinfo.melee_debounce_time = level.time + 1.5_sec;
	}
}

mframe_t overlord_frames_melee1[] = {
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, overlord_attack_sound},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, overlord_hit_left},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, OverlordTeleport}
};
MMOVE_T(overlord_move_melee1) = { FRAME_s_wtaa01, FRAME_s_wtaa10, overlord_frames_melee1, overlord_run };

mframe_t overlord_frames_melee2[] = {
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, overlord_attack_sound},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, overlord_hit_right},
	{ai_charge},
	{ai_charge},

	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, OverlordTeleport}
};
MMOVE_T(overlord_move_melee2) = { FRAME_s_wtab01, FRAME_s_wtab14, overlord_frames_melee2, overlord_run };

mframe_t overlord_frames_melee3[] = {
	{ai_charge, 0, overlord_surprise},
	{ai_charge},
	{ai_charge, 0, overlord_attack_sound},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, overlord_hit_right_hard},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge},

	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, OverlordTeleport}
};
MMOVE_T(overlord_move_melee3) = { FRAME_s_wtac01, FRAME_s_wtac14, overlord_frames_melee3, overlord_run };

MONSTERINFO_MELEE(overlord_melee) (edict_t* self) -> void
{
	if (frandom() <= 0.20f && skill->integer >= 3)
		M_SetAnimation(self, &overlord_move_melee3);
	else if (frandom() >= 0.60f)
		M_SetAnimation(self, &overlord_move_melee2);
	else
		M_SetAnimation(self, &overlord_move_melee1);
}

mframe_t overlord_frames_missile[] = {
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, overlord_attack_sound},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, -1},
	{ai_charge, -2},
	{ai_charge, -3},
	{ai_charge, -2, overlord_fire},

	{ai_charge, -1},
	{ai_charge, 0, OverlordTeleport}
};
MMOVE_T(overlord_move_missile) = { FRAME_s_wtba01, FRAME_s_wtba12, overlord_frames_missile, overlord_run };

mframe_t overlord_frames_spawn[] = {
	{ai_charge},
	{ai_charge},
	{ai_charge, 0, overlord_attack_sound},
	{ai_charge},
	{ai_charge},
	{ai_charge},
	{ai_charge, -1, overlord_ready_spawn },
	{ai_charge, -2},
	{ai_charge, -3},
	{ai_charge, -2, overlord_spawn },

	{ai_charge, -1},
	{ai_charge, 0, OverlordTeleport}
};
MMOVE_T(overlord_move_spawn) = { FRAME_s_wtba01, FRAME_s_wtba12, overlord_frames_spawn, overlord_run };

MONSTERINFO_ATTACK(overlord_attack) (edict_t* self) -> void
{
	vec3_t offset = { 0, 0, 10 };
	bool  blocked = false;

	if (self->monsterinfo.aiflags & AI_BLOCKED)
	{
		blocked = true;
		self->monsterinfo.aiflags &= ~AI_BLOCKED;
	}

	if (!M_CheckClearShot(self, offset))
		return;

	float r, chance;

	OverlordCalcSlots(self);

	// if we can't see the target, spawn stuff
	if ((self->monsterinfo.attack_state == AS_BLIND) && (M_SlotsLeft(self) >= 2))
	{
		M_SetAnimation(self, &overlord_move_spawn);
		return;
	}

	// accept bias towards spawning
	if (blocked && (M_SlotsLeft(self) >= 2))
	{
		M_SetAnimation(self, &overlord_move_spawn);
		return;
	}


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

		if (M_SlotsLeft(self) >= 2)
		{
			if (frandom() <= 0.40f)
				M_SetAnimation(self, &overlord_move_missile);
			else
				M_SetAnimation(self, &overlord_move_spawn);
		}
		else
			M_SetAnimation(self, &overlord_move_missile);

		self->monsterinfo.attack_finished = level.time + random_time(4_sec);
		return;
	}

	float shindeiru = frandom();

	if (M_SlotsLeft(self) >= 2)
	{
		if (shindeiru <= 0.20f && skill->integer >= 3)
			M_SetAnimation(self, &overlord_move_melee3);
		else if (shindeiru <= 0.60f)
			M_SetAnimation(self, &overlord_move_missile);
		else
			M_SetAnimation(self, &overlord_move_spawn);
	}
	else
	{
	if (shindeiru <= 0.20f && skill->integer >= 3)
		M_SetAnimation(self, &overlord_move_melee3);
	else
		M_SetAnimation(self, &overlord_move_missile);
	}
}

//
// PAIN
//

mframe_t overlord_frames_pain1 [] =
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
		{ai_move}
};
MMOVE_T(overlord_move_pain1) = { FRAME_s_wtpa01, FRAME_s_wtpa14, overlord_frames_pain1, overlord_run };

mframe_t overlord_frames_pain2[] =
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
MMOVE_T(overlord_move_pain2) = { FRAME_s_wtpb01, FRAME_s_wtpb11, overlord_frames_pain2, overlord_run };

PAIN(overlord_pain) (edict_t* self, edict_t* other, float kick, int damage, const mod_t& mod) -> void
{
	if (level.time < self->pain_debounce_time)
		return;

	self->pain_debounce_time = level.time + 2_sec;
	
	if (!M_ShouldReactToPain(self, mod))
		return; // no pain anims in nightmare
	
	gi.sound (self, CHAN_VOICE, sound_pain, 1, ATTN_NORM, 0);

	if (frandom() >= 0.4f)
		M_SetAnimation(self, &overlord_move_pain1);
	else
		M_SetAnimation(self, &overlord_move_pain2);
}

//
// DEATH
//

void overlord_dead (edict_t * self)
{
	T_RadiusDamage(self, self, 60, nullptr, 105, DAMAGE_NONE, MOD_BARREL);

	gi.WriteByte(svc_temp_entity);
	gi.WriteByte(TE_EXPLOSION1);
	gi.WritePosition(self->s.origin);
	gi.multicast(self->s.origin, MULTICAST_PHS, false);

	self->s.skinnum /= 2;

	ThrowGibs(self, 55, {
		{ 2, "models/objects/gibs/bone/tris.md2" },
		{ 2, "models/monsters/overlord/gibs/claw.md2" },
		{ 2, "models/monsters/overlord/gibs/arm.md2" },
		{ "models/monsters/overlord/gibs/ribs.md2" },
		{ "models/monsters/overlord/gibs/bone.md2", GIB_HEAD }
		});

	self->touch = nullptr;
}

mframe_t overlord_frames_die [] =
{	
	{ai_move, 0, BossGibs},
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
MMOVE_T(overlord_move_die) = { FRAME_s_wtdt01, FRAME_s_wtdt17, overlord_frames_die, overlord_dead };

void KillWraiths(edict_t* self)
{
	edict_t* ent = nullptr;

	while (1)
	{
		ent = G_FindByString<&edict_t::classname>(ent, "monster_wrath");
		if (!ent)
			return;

		if ((ent->inuse) && (ent->health > 0))
			T_Damage(ent, self, self, vec3_origin, self->enemy->s.origin, vec3_origin, (ent->health + 1), 0, DAMAGE_NO_KNOCKBACK, MOD_UNKNOWN);
	}
}


DIE(overlord_die) (edict_t* self, edict_t* inflictor, edict_t* attacker, int damage, const vec3_t& point, const mod_t& mod) -> void
{
	if (self->deadflag)
		return;

	gi.sound(self, CHAN_VOICE, sound_die, 1, ATTN_NORM, 0);
	self->deadflag = true;
	self->takedamage = true;
	KillWraiths(self);

	M_SetAnimation(self, &overlord_move_die);
}

MONSTERINFO_CHECKATTACK(overlord_checkattack) (edict_t* self) -> bool
{
	if (!self->enemy)
		return false;

	return M_CheckAttack_Base(self, 0.4f, 0.8f, 0.8f, 0.8f, 0.f, 0.f);
}

static void overlord_set_fly_parameters(edict_t* self)
{
	self->monsterinfo.fly_thrusters = false;
	self->monsterinfo.fly_acceleration = 20.f;
	self->monsterinfo.fly_speed = 120.f;
	self->monsterinfo.fly_min_distance = 200.f;
	self->monsterinfo.fly_max_distance = 400.f;
}

/*QUAKED monster_overlord (1 .5 0) (-16 -16 -24) (16 16 32) Ambush Trigger_Spawn Sight
model="models/monsters/overlord/tris.md2"
*/
void SP_monster_overlord (edict_t *self)
{
	const spawn_temp_t& st = ED_GetSpawnTemp();

	if (!M_AllowSpawn(self)) {
		G_FreeEdict(self);
		return;
	}

	sound_melee.assign("overlord/smash.wav");
	sound_sight.assign("wrath/wsee.wav");
	sound_attack.assign("wrath/watt.wav");
	sound_die.assign("wrath/wdthc.wav");
	sound_pain.assign("wrath/wpain.wav");


	sound_attack2.assign("vore/attack2.wav");

	self->movetype = MOVETYPE_STEP;
	self->solid = SOLID_BBOX;

	self->s.modelindex = gi.modelindex ("models/monsters/overlord/tris.md2");
	self->mins = { -16, -16, -24 };
	self->maxs = { 16, 16, 32 };

	self->health = max(3000, 3000 + 1250 * (skill->integer - 1)) * st.health_multiplier;
	if (!st.was_key_specified("armor_type"))
		self->monsterinfo.armor_type = IT_ARMOR_BODY;
	if (!st.was_key_specified("armor_power"))
		self->monsterinfo.armor_power = max(500, 500 + 150 * (skill->integer - 1));
	self->mass = 750;
	if (coop->integer)
	{
		self->health += (500 * skill->integer) + (500 * (CountPlayers() - 1));
		self->monsterinfo.armor_power += (250 * skill->integer) + (250 * (CountPlayers() - 1));
	}
	self->pain = overlord_pain;
	self->die = overlord_die;

	self->monsterinfo.stand = overlord_stand;
	self->monsterinfo.walk = overlord_walk;
	self->monsterinfo.run = overlord_run;
	self->monsterinfo.attack = overlord_attack;
	self->monsterinfo.melee = overlord_melee;
	self->monsterinfo.sight = overlord_sight;
	self->monsterinfo.search = nullptr;
	self->monsterinfo.checkattack = overlord_checkattack;

	gi.linkentity (self);

	M_SetAnimation(self, &overlord_move_stand);
	self->monsterinfo.scale = MODEL_SCALE;
	
	self->flags |= FL_FLY | FL_DEEPONE;
	if (!self->yaw_speed)
		self->yaw_speed = 10;
	self->viewheight = 10;

	flymonster_start (self);

	self->monsterinfo.aiflags |= AI_ALTERNATE_FLY;

	overlord_set_fly_parameters(self);
}
