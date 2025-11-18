// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"


PRETHINK(CheckTeleportReturn) (edict_t* self) -> void
{
	gi.Com_PrintFmt("{}: checking to return 1\n", *self);
	if (!self->active)
		return;

	gi.Com_PrintFmt("{}: checking to return 2\n", *self);
	// Only check after timer
	if (level.time < self->timeout)
		return;

	gi.Com_PrintFmt("{}: checking to return 3\n", *self);
	// If the monster cannot see its enemy, return to pre-teleport position
	if (!self->enemy || !visible(self, self->enemy)) {
		// Teleport effect - departure
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_TELEPORT_EFFECT);
		gi.WritePosition(self->s.origin);
		gi.multicast(self->s.origin, MULTICAST_PVS, false);

		gi.Com_PrintFmt("{}: returning\n", *self);
		// Restore original position
		self->s.origin = self->monsterinfo.shottarget;
		gi.linkentity(self);

		// Teleport effect - arrival
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_TELEPORT_EFFECT);
		gi.WritePosition(self->s.origin);
		gi.multicast(self->s.origin, MULTICAST_PVS, false);
	}

	// Reset flag
	self->active = false;
}

bool TryRandomTeleportPosition(edict_t* self, float radius)
{

	for (int i = 0; i < 10; ++i) {
		vec3_t offset = {
			crandom() * radius,
			crandom() * radius,
			crandom() * (radius * 0.5f)  // vertical variation
		};
		vec3_t target = self->s.origin + offset;

		// Check if location is in solid
		trace_t tr = gi.trace(self->s.origin, self->mins, self->maxs, target, self, MASK_SOLID);
		if (tr.startsolid) {
			continue;
		}

		// Check if occupied by other entity
		trace_t occ = gi.trace(target, self->mins, self->maxs, target, self, MASK_MONSTERSOLID);
		if (occ.fraction < 1.0f) {
			continue;
		}

		// Teleport effect - departure
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_TELEPORT_EFFECT);
		gi.WritePosition(self->s.origin);
		gi.multicast(self->s.origin, MULTICAST_PVS, false);

		// Move monster
		self->monsterinfo.shottarget = self->s.origin;
		self->timeout = level.time + 1_sec;
		self->active = 1;
		self->postthink = CheckTeleportReturn;

		self->s.origin = target;
		gi.linkentity(self);

		// Teleport effect - arrival
		gi.WriteByte(svc_temp_entity);
		gi.WriteByte(TE_TELEPORT_EFFECT);
		gi.WritePosition(self->s.origin);
		gi.multicast(self->s.origin, MULTICAST_PVS, false);

		return true;
	}
	return false;
}

THINK(BossGibs_think) (edict_t* self) -> void
{
	// owner gone or changed
	if (!self->owner->inuse || self->owner->s.modelindex != self->style || self->count != self->owner->spawn_count)
	{
		G_FreeEdict(self);
		return;
	}

	vec3_t org = self->owner->s.origin + self->owner->mins;

	org.x += frandom() * self->owner->size.x;
	org.y += frandom() * self->owner->size.y;
	org.z += frandom() * self->owner->size.z;

	// Select random gib model
	static const char* gib_models[] = {
		"models/objects/gibs/bone/tris.md2",
		"models/objects/gibs/bone2/tris.md2",
		"models/objects/gibs/sm_meat/tris.md2",
		"models/objects/gibs/q1_meat/tris.md2"
	};

	// Throw two gibs per tick instead of one
	for (int i = 0; i < 2; i++)
	{
		const char* gib_model = gib_models[irandom(4)];

		// Calculate velocity away from owner center
		vec3_t dir = org - self->owner->s.origin;
		dir.normalize();

		// Add some randomness to direction
		dir.x += crandom() * 0.3f;
		dir.y += crandom() * 0.3f;
		dir.z += crandom() * 0.3f;
		dir.normalize();

		// Increased velocity (500-800 speed range, was 200-400)
		float speed = 1000.0f + frandom() * 600.0f;
		vec3_t velocity = dir * speed;
		velocity.z += 400.0f + frandom() * 400.0f; // Increased upward component (was 100-200)

		ThrowGib(self->owner, gib_model, velocity.length(), GIB_NONE, (self->s.scale ? self->s.scale : 1));
	}

	self->viewheight++;
	self->nextthink = level.time + random_time(50_ms, 200_ms);
}

void BossGibs(edict_t* self)
{
	// no gibs on dead spawn
	if (self->spawnflags.has(SPAWNFLAG_MONSTER_DEAD))
		return;

	edict_t* gibber = G_Spawn();
	gibber->owner = self;
	gibber->count = self->spawn_count;
	gibber->style = self->s.modelindex;
	gibber->think = BossGibs_think;
	gibber->nextthink = level.time + random_time(75_ms, 250_ms);
	gibber->viewheight = 0;
}