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

// TO DO: Replace explosions with gibs

THINK(Q1BossExplode_think) (edict_t* self) -> void
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

    gi.WriteByte(svc_temp_entity);
    gi.WriteByte(!(self->viewheight % 3) ? TE_EXPLOSION1 : TE_EXPLOSION1_NL);
    gi.WritePosition(org);
    gi.multicast(org, MULTICAST_PVS, false);

    self->viewheight++;

    self->nextthink = level.time + random_time(50_ms, 200_ms);
}

void Q1BossExplode(edict_t* self)
{
    // no blowy on deady
    if (self->spawnflags.has(SPAWNFLAG_MONSTER_DEAD))
        return;

    edict_t* exploder = G_Spawn();
    exploder->owner = self;
    exploder->count = self->spawn_count;
    exploder->style = self->s.modelindex;
    exploder->think = Q1BossExplode_think;
    exploder->nextthink = level.time + random_time(75_ms, 250_ms);
    exploder->viewheight = 0;
}