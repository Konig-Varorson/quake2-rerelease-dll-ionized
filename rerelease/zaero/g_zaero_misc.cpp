// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

void barrel_touch(edict_t* self, edict_t* other, const trace_t& tr, bool other_touching_self);
void barrel_start(edict_t* self);

void setupCrate(edict_t* self)
{
	if (deathmatch->integer)
	{
		G_FreeEdict(self);
		return;
	}

	self->solid = SOLID_BBOX;
	self->movetype = MOVETYPE_FALLFLOAT;

	if (!self->mass)
		self->mass = 400;

	self->touch = barrel_touch;
	self->think = barrel_start;
	self->nextthink = level.time + FRAME_TIME_S;

	gi.linkentity(self);
}

void SP_misc_crate(edict_t* self)
{
	if (!self->mass)
		self->mass = 400;

	self->model = "models/objects/crate/crate64.md2";
	self->s.modelindex = gi.modelindex(self->model);
	self->mins = { -32, -32, 0 };
	self->maxs = { 32, 32, 64 };

	setupCrate(self);
}

void SP_misc_crate_medium(edict_t* self)
{
	if (!self->mass)
		self->mass = 150;

	self->model = "models/objects/crate/crate48.md2";
	self->s.modelindex = gi.modelindex(self->model);
	self->mins = { -24, -24, 0 };
	self->maxs = { 24, 24, 48 };

	setupCrate(self);
}

void SP_misc_crate_small(edict_t* self)
{
	if (!self->mass)
		self->mass = 50;

	self->model = "models/objects/crate/crate32.md2";
	self->s.modelindex = gi.modelindex(self->model);
	self->mins = { -16, -16, 0 };
	self->maxs = { 16, 16, 32 };

	setupCrate(self);
}

void SP_misc_seat(edict_t* self)
{
	if (!self->mass)
		self->mass = 50;

	self->model = "models/objects/seat/tris.md2";
	self->s.modelindex = gi.modelindex(self->model);
	self->mins = { -16, -16, 0 };
	self->maxs = { 16, 16, 40 };

	setupCrate(self);
}

/*QUAKED misc_commdish (0 .5 .8) (-16 -16 0) (16 16 40)
*/
THINK(commdish_think) (edict_t* ent) -> void
{
	ent->s.frame++;

	if (ent->s.frame >= 98)
	{
		ent->s.frame = 98;
	}
	else
	{
		ent->nextthink = level.time + FRAME_TIME_S;
	}
}

THINK(commdish_start) (edict_t* self) -> void
{
	M_droptofloor(self);
	self->nextthink = level.time + FRAME_TIME_S;
}

USE(commdish_use) (edict_t* self, edict_t* other, edict_t* activator) -> void
{
	self->think = commdish_think;
	self->use = nullptr;
	gi.sound(self, CHAN_AUTO, gi.soundindex("misc/commdish.wav"), 1, ATTN_NORM, 0);
	self->nextthink = level.time + FRAME_TIME_S;
}

void SP_misc_commdish(edict_t* ent)
{
	if (deathmatch->value)
	{	// auto-remove for deathmatch
		G_FreeEdict(ent);
		return;
	}

	ent->solid = SOLID_BBOX;
	ent->movetype = MOVETYPE_STEP;

	ent->mins = { -100, -100, 0 };
	ent->maxs = { 100, 100, 275 };
	ent->s.modelindex = gi.modelindex("models/objects/satdish/tris.md2");

	ent->monsterinfo.aiflags = AI_NOSTEP;

	ent->think = commdish_start;
	ent->nextthink = level.time + 20_hz;
	ent->use = commdish_use;

	gi.linkentity(ent);
}

//ZAERO CUT

/*QUAKED misc_bulldog_s (1 .5 0) (-16 -16 0) (16 16 32)
This is a ship from ZAERO for the flybys.
It is trigger_spawned, so you must have something use it for it to show up.
There must be a path for it to follow once it is activated.

"speed"		How fast it should fly
*/

constexpr spawnflags_t SPAWNFLAG_SOLID = 1_spawnflag;

USE(misc_bulldog_use) (edict_t* self, edict_t* other, edict_t* activator) -> void
{
	self->svflags &= ~SVF_NOCLIENT;
	self->use = train_use;
	train_use(self, other, activator);
}

void SP_misc_bulldog_s(edict_t* ent)
{
	if (!ent->target)
	{
		gi.Com_PrintFmt("{} without a target\n", *ent);
		G_FreeEdict(ent);
		return;
	}

	if (!ent->speed)
		ent->speed = 300;

	ent->movetype = MOVETYPE_PUSH;
	ent->solid = SOLID_NOT;
	ent->s.modelindex = gi.modelindex("models/ships/bulldog_s/main/tris.md2");
	ent->s.modelindex2 = gi.modelindex("models/ships/bulldog_s/left/tris.md2");
	ent->s.modelindex3 = gi.modelindex("models/ships/bulldog_s/right/tris.md2");
	ent->mins = { -16, -16, 0 };
	ent->maxs = { 16, 16, 32 };

	if (ent->spawnflags.has(SPAWNFLAG_SOLID))
	{
		ent->solid = SOLID_BBOX;
		ent->spawnflags &= ~SPAWNFLAG_SOLID;
	}

	ent->think = func_train_find;
	ent->nextthink = level.time + 10_hz;
	ent->use = misc_bulldog_use;
	ent->svflags |= SVF_NOCLIENT;
	ent->moveinfo.accel = ent->moveinfo.decel = ent->moveinfo.speed = ent->speed;

	gi.linkentity(ent);
}

/*QUAKED misc_bulldog_, (1 .5 0) (-96 -96 -12) (96 96 52)
This is a larger ship from ZAERO for the flybys.
It is trigger_spawned, so you must have something use it for it to show up.
There must be a path for it to follow once it is activated.

"speed"		How fast it should fly
*/
void SP_misc_bulldog_m(edict_t* ent)
{
	if (!ent->target)
	{
		gi.Com_PrintFmt("{} without a target\n", *ent);
		G_FreeEdict(ent);
		return;
	}

	if (!ent->speed)
		ent->speed = 300;

	ent->movetype = MOVETYPE_PUSH;
	ent->solid = SOLID_NOT;
	ent->s.modelindex = gi.modelindex("models/ships/bulldog_m/main/tris.md2");
	ent->s.modelindex2 = gi.modelindex("models/ships/bulldog_m/left/tris.md2");
	ent->s.modelindex3 = gi.modelindex("models/ships/bulldog_m/right/tris.md2");
	ent->mins = { -96, -96, -12 };
	ent->maxs = { 96, 96, 52 };

	if (ent->spawnflags.has(SPAWNFLAG_SOLID))
	{
		ent->solid = SOLID_BBOX;
		ent->spawnflags &= ~SPAWNFLAG_SOLID;
	}
	
	ent->think = func_train_find;
	ent->nextthink = level.time + 10_hz;
	ent->use = misc_bulldog_use;
	ent->svflags |= SVF_NOCLIENT;
	ent->moveinfo.accel = ent->moveinfo.decel = ent->moveinfo.speed = ent->speed;

	gi.linkentity(ent);
}


/*QUAKED misc_bulldog_l (1 .5 0) (-176 -120 -24) (176 120 72)
This is a large stationary bulldog
*/
void SP_misc_bulldog_l(edict_t* ent)
{
	ent->movetype = MOVETYPE_NONE;
	ent->solid = SOLID_BBOX;
	ent->mins = { -432, -120, -32 };
	ent->maxs = { 144, 120, 96 };
	ent->s.modelindex = gi.modelindex("models/ships/bulldog_l/main/tris.md2");
	ent->s.modelindex2 = gi.modelindex("models/ships/bulldog_l/left/tris.md2");
	ent->s.modelindex3 = gi.modelindex("models/ships/bulldog_l/right/tris.md2");
	gi.linkentity(ent);
}

#if 0
/*QUAKED misc_ired (1 0 0) (-16 -16 -16) (16 16 16)
'Native' Laser tripbomb
*/

TOUCH(shrapnel_touch) (edict_t *ent, edict_t *other, const trace_t &tr, bool other_touching_self) -> void
{
	// do damage if we can
	if (!other->takedamage)
		return;

	if (ent->velocity != vec3_origin)
		return;

	T_Damage(other, ent, ent->owner, ent->velocity, ent->s.origin, 
			tr.plane.normal, TBOMB_SHRAPNEL_DMG, 8, DAMAGE_NONE, MOD_TRIPBOMB);
	G_FreeEdict(ent);
}

THINK(TripBomb_Explode) (edict_t *ent) -> void
{
	vec3_t origin;
	int i = 0;

	T_RadiusDamage(ent, ent->owner ? ent->owner : ent, ent->dmg, ent, ent->dmg_radius, DAMAGE_NONE, MOD_TRIPBOMB);

	origin = ent->s.origin + (ent->velocity * -0.02f);

	gi.WriteByte (svc_temp_entity);
	if (ent->waterlevel)
	{
		if (ent->groundentity)
			gi.WriteByte (TE_GRENADE_EXPLOSION_WATER);
		else
			gi.WriteByte (TE_ROCKET_EXPLOSION_WATER);
	}
	else
	{
		if (ent->groundentity)
			gi.WriteByte (TE_GRENADE_EXPLOSION);
		else
			gi.WriteByte (TE_ROCKET_EXPLOSION);
	}
	gi.WritePosition(origin);
	gi.multicast(ent->s.origin, MULTICAST_PHS, false);

	// throw off some debris
	for (i = 0; i < TBOMB_SHRAPNEL; i++)
	{
		edict_t *sh = G_Spawn();
		vec3_t forward, right, up;
		sh->classname = "shrapnel";
		sh->movetype = MOVETYPE_BOUNCE;
		sh->solid = SOLID_BBOX;
		sh->s.effects |= EF_GRENADE;
		sh->s.modelindex = gi.modelindex("models/objects/shrapnel/tris.md2");
		sh->owner = ent->owner;
		sh->avelocity = { 300, 300, 300 };
		sh->s.origin = ent->s.origin;
		AngleVectors (ent->s.angles, forward, right, up);
		forward = forward * 500;
		forward += forward + (right * (crandom() * 500));
		forward += forward + (up * (crandom() * 500));
		sh->velocity = forward;
		sh->touch = shrapnel_touch;
		sh->think = G_FreeEdict;
		sh->nextthink = level.time + gtime_t::from_sec(3.0f + (crandom() * 1.5));
	}

	G_FreeEdict(ent);
}

THINK(tripbomb_laser_think) (edict_t *self) -> void
{
	vec3_t  start;
	vec3_t  end;
	vec3_t  delta;
	trace_t	tr;
	int		count = 8;
	self->nextthink = level.time + FRAME_TIME_S;

	if (level.time > self->timeout)
	{
		// blow up
		self->chain->think = TripBomb_Explode;
		self->chain->nextthink = level.time + FRAME_TIME_S;
		G_FreeEdict(self);
		return;
	}

	// randomly phase out or EMPNuke is in effect
	if (EMPNukeCheck(self, self->s.origin) || frandom() < 0.1f)
	{
		self->svflags |= SVF_NOCLIENT;
		return;
	}

	self->svflags &= ~SVF_NOCLIENT;
	start = self->s.origin;
	end = start + (self->movedir * 2048);
	tr = gi.traceline (start, end, self, MASK_SHOT);

	delta = tr.endpos - self->move_origin;
	if (self->s.origin == self->move_origin)
	{
		// we haven't done anything yet
		self->move_origin = tr.endpos;
		if (self->spawnflags.has(SPAWNFLAG_LASER_ZAP))
		{
			self->spawnflags &= SPAWNFLAG_LASER_ZAP;
			gi.WriteByte(svc_temp_entity);
			gi.WriteByte(TE_LASER_SPARKS);
			gi.WriteByte(count);
			gi.WritePosition(tr.endpos);
			gi.WriteDir(tr.plane.normal);
			gi.WriteByte(self->s.skinnum);
			gi.multicast(tr.endpos, MULTICAST_PVS, false);
		}
	}

	else if ((delta.length() > 1.0) || (tr.ent->svflags & SVF_MONSTER) || (tr.ent->client))
	{
		// blow up
		self->chain->think = TripBomb_Explode;
		self->chain->nextthink = level.time + FRAME_TIME_S;
		G_FreeEdict(self);
		return;
	}
	self->s.old_origin = self->move_origin;
}

THINK(tripbomb_laser_on) (edict_t *self) -> void
{
	self->svflags &= ~SVF_NOCLIENT;
	self->think = tripbomb_laser_think;
	self->flags |= FL_TRAP_LASER_FIELD;

	// play a sound
	gi.sound(self, CHAN_VOICE, gi.soundindex("weapons/ired/las_arm.wav"), 1, ATTN_NORM, 0);
	tripbomb_laser_think(self);
}

THINK(create_tripbomb_laser) (edict_t *bomb) -> void
{
	// create the laser
	edict_t *laser = G_Spawn();
	bomb->chain = laser;
	laser->classname = "laser trip bomb laser";
	laser->s.origin = bomb->s.origin;
	laser->move_origin = bomb->s.origin;
	laser->s.angles = bomb->s.angles;
	G_SetMovedir(laser->s.angles, laser->movedir);
	laser->owner = bomb;
	laser->s.skinnum = 0xb0b1b2b3; // <- faint purple  0xf3f3f1f1 <-blue  red-> 0xf2f2f0f0;
	laser->s.frame = 2; // Beam diameter
	laser->movetype = MOVETYPE_NONE;
	laser->solid = SOLID_NOT;
	laser->s.renderfx |= RF_BEAM|RF_TRANSLUCENT;
	laser->s.modelindex = MODELINDEX_WORLD;
	laser->chain = bomb;
	laser->spawnflags |= SPAWNFLAG_LASER_ZAP | SPAWNFLAG_LASER_ON;
	laser->think = tripbomb_laser_on;
	laser->nextthink = level.time + FRAME_TIME_S;
	laser->svflags |= SVF_NOCLIENT;
	// Set misc_ired to not time out in a reasonable time
	laser->timeout = (Q_strcasecmp(bomb->classname, "misc_ired") == 0) ? 
						gtime_t::from_min(99999) : level.time + TBOMB_TIMEOUT;
	gi.linkentity(laser);
}

USE(use_tripbomb) (edict_t *self, edict_t *other, edict_t *activator) -> void
{
	if (self->chain)
	{
		// we already have a laser, remove it
		G_FreeEdict(self->chain);
		self->chain = nullptr;
	}
	else
		// create the laser
		create_tripbomb_laser(self);
}

void turnOffGlow(edict_t *self)
{
	self->s.effects &= ~EF_COLOR_SHELL;
	self->s.renderfx &= ~RF_SHELL_GREEN;
	self->think = nullptr;
	self->nextthink = 0_ms;
}

PAIN(tripbomb_pain) (edict_t *self, edict_t *other, float kick, int damage, const mod_t &mod) -> void
{
	// turn on the glow
	self->damage_debounce_time = level.time + 10_hz;

	// if we don't have a think function, then turn this thing on
	if (!self->think)
	{
		self->s.effects |= EF_COLOR_SHELL;
		self->s.renderfx |= RF_SHELL_GREEN;
		self->nextthink = self->damage_debounce_time;
		self->think = turnOffGlow;
	}
}

THINK(tripbomb_think) (edict_t *self) -> void
{
	if (self->chain == nullptr)
	{
		// check whether we need to create the laser
		if (self->timeout < level.time)
		{
			create_tripbomb_laser(self);
		}
	}

	// do we need to show damage?
	if (self->damage_debounce_time > level.time)
	{
		self->s.effects |= EF_COLOR_SHELL;
		self->s.renderfx |= RF_SHELL_GREEN;
	}
	else
	{
		self->s.effects &= ~EF_COLOR_SHELL;
		self->s.renderfx &= ~RF_SHELL_GREEN;
	}

	self->nextthink = level.time + FRAME_TIME_S;
}

void Setup_Bomb(edict_t *bomb, const char *classname, int damage, int damage_radius)
{
	bomb->classname = classname;
	bomb->mins = { -8, -8, -8 };
	bomb->maxs = { 8, 8, 8 };
	bomb->solid = SOLID_BBOX;
	bomb->movetype = MOVETYPE_NONE;
	bomb->s.modelindex = gi.modelindex("models/objects/ired/tris.md2");
	bomb->radius_dmg = damage;
	bomb->dmg = damage;
	bomb->dmg_radius = damage_radius;
	bomb->health = 1;
	bomb->takedamage = true; // health will not be deducted
	bomb->flags |= FL_IMMORTAL;
	bomb->pain = tripbomb_pain;
	bomb->timeout = level.time + TBOMB_DELAY;
	bomb->think = tripbomb_think;
	bomb->nextthink = level.time + FRAME_TIME_S;
}

void SP_misc_lasertripbomb(edict_t *bomb)
{
	// precache
	gi.soundindex("weapons/ired/las_set.wav");
	gi.soundindex("weapons/ired/las_arm.wav");
	gi.modelindex("models/objects/shrapnel/tris.md2");
	gi.modelindex("models/objects/ired/tris.md2");

	if (bomb->spawnflags.has(SPAWNFLAG_CHECK_BACK_WALL))
	{
		vec3_t forward, endPos;
		trace_t tr;
		// look backwards toward a wall
		AngleVectors(bomb->s.angles, forward, nullptr, nullptr);
        endPos = bomb->s.origin + (forward * -64.0f);

		tr = gi.traceline(bomb->s.origin, endPos, bomb, MASK_SOLID);
        bomb->s.origin = tr.endpos;
        bomb->s.angles = vectoangles(tr.plane.normal);
	}

	// set up ourself
	Setup_Bomb(bomb, "misc_ired", TBOMB_DAMAGE, TBOMB_RADIUS_DAMAGE);

	bomb->owner = bomb;
	
	if (bomb->targetname)
	{
		bomb->use = use_tripbomb;
	}
	else
	{
		bomb->think = create_tripbomb_laser;
		bomb->nextthink = level.time + TBOMB_DELAY;
	}
	gi.linkentity(bomb);
}

/*QUAKED misc_securitycamera (1 0 0) (-16 -16 -16) (16 16 16)
A security camera that can be viewed through the Visor item.
"message" Display name for the camera (required)
"mangle" Pitch Yaw Roll angles for camera orientation
*/

USE(use_securitycamera) (edict_t *self, edict_t *other, edict_t *activator) -> void
{
	self->active = !self->active;
}

constexpr int CAMERA_FRAME_FIRST = 0;
constexpr int CAMERA_FRAME_LAST = 59;
THINK(securitycamera_think) (edict_t *self) -> void
{
	if (self->active)
	{
		self->s.frame++;
		if (self->s.frame > CAMERA_FRAME_LAST)
			self->s.frame = CAMERA_FRAME_FIRST;
	}

	if (self->timeout > level.time)
	{
		self->s.effects |= EF_COLOR_SHELL;
		self->s.renderfx |= RF_SHELL_GREEN;
	}
	else
	{
		self->s.effects &= ~EF_COLOR_SHELL;
		self->s.renderfx &= ~RF_SHELL_GREEN;
	}

	self->nextthink = level.time + 100_ms;
}

PAIN(camera_pain) (edict_t *self, edict_t *other, float kick, int damage, const mod_t &mod) -> void
{
	self->timeout = level.time + 10_hz;
}

void SP_misc_securitycamera(edict_t *self)
{
	vec3_t offset, forward, up;

	const spawn_temp_t &st = ED_GetSpawnTemp();

	// Security camera without a message
	if (!self->message)
	{
		gi.Com_PrintFmt("{} without a message\n", *self);
		G_FreeEdict(self);
		return;
	}

	self->solid = SOLID_BBOX;
	self->movetype = MOVETYPE_NONE;
	self->s.modelindex = gi.modelindex("models/objects/camera/tris.md2");

	// set the bounding box
	self->mins = { -16, -16, -32 };
	self->maxs = { 16, 16, 0 };

	// set the angle of direction
	self->move_angles = st.mangle;
	self->s.angles = { 0, st.mangle[YAW], 0 };
	
	// get an offset
	AngleVectors(self->s.angles, forward, nullptr, up);
	offset = { 0, 0, 0 };
	offset += (forward * 8);
	offset += (up * -32);
	self->move_origin = self->s.origin + offset;
    
	if (self->targetname)
	{
		self->use = use_securitycamera;
		self->active = false;
	}
	else
	{
		self->active = true;
	}
	self->think = securitycamera_think;
	self->nextthink = level.time + 100_ms;

	self->health = 1;
	self->takedamage = true; // health will not be deducted
	self->flags |= FL_IMMORTAL;
	self->pain = camera_pain;

	gi.linkentity(self);
}
#endif