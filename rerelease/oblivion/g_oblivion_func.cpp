// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

#if 0

#define STATE_TOP                       0
#define STATE_BOTTOM            1
#define STATE_UP                        2
#define STATE_DOWN                      3

#define RTRAIN_START_ON         1
#define RTRAIN_TOGGLE           2
#define RTRAIN_BLOCK_STOPS      4

void Move_Calc(edict_t* ent, vec3_t dest, void(*func)(edict_t*));

struct rotate_train_state_s {
    vec3_t  final_angles;
    qboolean        has_final;
};

static void rotate_train_next(edict_t* self);
static void rotate_train_wait(edict_t* self);
static void rotate_train_resume(edict_t* self);

static float RotateTrain_ComputeMoveTime(edict_t* self, edict_t* corner, float distance)
{
    if (corner && corner->duration > 0)
        return corner->duration;
    if (self->duration > 0)
        return self->duration;
    float speed = self->speed;
    if (corner && corner->speed > 0)
        speed = corner->speed;
    if (speed <= 0)
        return 0;
    return distance / speed;
}

static void RotateTrain_SetAngles(edict_t* self, edict_t* corner, float move_time)
{
    rotate_train_state_t* rt = self->rotate_train;
    if (!rt)
        return;

    rt->has_final = false;
    VectorClear(self->avelocity);

    vec3_t goal;
    qboolean has_goal = false;

    if (corner && !VectorCompare(corner->rotate, vec3_origin)) {
        VectorAdd(self->s.angles, corner->rotate, goal);
        if (move_time > 0)
            VectorScale(corner->rotate, 1.0f / move_time, self->avelocity);
        has_goal = true;
    }
    else if (corner && !VectorCompare(corner->rotate_speed, vec3_origin)) {
        VectorCopy(self->s.angles, goal);
        VectorMA(goal, move_time, corner->rotate_speed, goal);
        if (move_time > 0)
            VectorCopy(corner->rotate_speed, self->avelocity);
        has_goal = true;
    }
    else if (!VectorCompare(self->rotate, vec3_origin)) {
        VectorAdd(self->s.angles, self->rotate, goal);
        if (move_time > 0)
            VectorScale(self->rotate, 1.0f / move_time, self->avelocity);
        has_goal = true;
    }
    else if (!VectorCompare(self->rotate_speed, vec3_origin)) {
        VectorCopy(self->s.angles, goal);
        VectorMA(goal, move_time, self->rotate_speed, goal);
        if (move_time > 0)
            VectorCopy(self->rotate_speed, self->avelocity);
        has_goal = true;
    }

    if (!has_goal)
        return;

    for (int i = 0; i < 3; i++)
        rt->final_angles[i] = anglemod(goal[i]);
    rt->has_final = true;

    if (move_time <= 0)
        VectorCopy(rt->final_angles, self->s.angles);
}

static void rotate_train_blocked(edict_t* self, edict_t* other)
{
    if (!(other->svflags & SVF_MONSTER) && (!other->client)) {
        T_Damage(other, self, self, vec3_origin, other->s.origin, vec3_origin, 100000, 1, 0, MOD_CRUSH);
        if (other)
            BecomeExplosion1(other);
        return;
    }

    if (level.time < self->touch_debounce_time)
        return;

    if (!self->dmg)
        return;
    self->touch_debounce_time = level.time + 0.5f;
    T_Damage(other, self, self, vec3_origin, other->s.origin, vec3_origin, self->dmg, 1, 0, MOD_CRUSH);
}

static void rotate_train_wait(edict_t* self)
{
    rotate_train_state_t* rt = self->rotate_train;
    VectorClear(self->avelocity);
    if (rt && rt->has_final)
        VectorCopy(rt->final_angles, self->s.angles);
    gi.linkentity(self);

    if (self->target_ent && self->target_ent->pathtarget) {
        char* savetarget = self->target_ent->target;
        edict_t* ent = self->target_ent;
        ent->target = ent->pathtarget;
        G_UseTargets(ent, self->activator);
        ent->target = savetarget;
        if (!self->inuse)
            return;
    }

    if (self->moveinfo.wait) {
        if (self->moveinfo.wait > 0) {
            self->nextthink = level.time + self->moveinfo.wait;
            self->think = rotate_train_next;
        }
        else if (self->spawnflags & RTRAIN_TOGGLE) {
            rotate_train_next(self);
            self->spawnflags &= ~RTRAIN_START_ON;
            VectorClear(self->velocity);
            self->nextthink = 0;
        }

        if (!(self->flags & FL_TEAMSLAVE)) {
            if (self->moveinfo.sound_end)
                gi.sound(self, CHAN_NO_PHS_ADD + CHAN_VOICE, self->moveinfo.sound_end, 1, ATTN_STATIC, 0);
            self->s.sound = 0;
        }
    }
    else {
        rotate_train_next(self);
    }
}

static void rotate_train_next(edict_t* self)
{
    edict_t* ent;
    vec3_t dest;
    qboolean first = true;

again:
    if (!self->target)
        return;

    ent = G_PickTarget(self->target);
    if (!ent) {
        gi.dprintf("rotate_train_next: bad target %s\n", self->target);
        return;
    }

    self->target = ent->target;

    if (ent->spawnflags & 1) {
        if (!first) {
            gi.dprintf("connected teleport path_corners, see %s at %s\n", ent->classname, vtos(ent->s.origin));
            return;
        }
        first = false;
        VectorSubtract(ent->s.origin, self->mins, self->s.origin);
        VectorCopy(self->s.origin, self->s.old_origin);
        self->s.event = EV_OTHER_TELEPORT;
        gi.linkentity(self);
        goto again;
    }

    self->moveinfo.wait = ent->wait;
    self->target_ent = ent;

    if (!(self->flags & FL_TEAMSLAVE)) {
        if (self->moveinfo.sound_start)
            gi.sound(self, CHAN_NO_PHS_ADD + CHAN_VOICE, self->moveinfo.sound_start, 1, ATTN_STATIC, 0);
        self->s.sound = self->moveinfo.sound_middle;
    }

    VectorSubtract(ent->s.origin, self->mins, dest);
    self->moveinfo.state = STATE_TOP;
    VectorCopy(self->s.origin, self->moveinfo.start_origin);
    VectorCopy(dest, self->moveinfo.end_origin);

    vec3_t delta;
    VectorSubtract(dest, self->s.origin, delta);
    float distance = VectorLength(delta);
    float move_time = RotateTrain_ComputeMoveTime(self, ent, distance);

    if (move_time <= 0 || distance <= 1.0f) {
        VectorCopy(dest, self->s.origin);
        gi.linkentity(self);
        RotateTrain_SetAngles(self, ent, 0);
        rotate_train_wait(self);
        return;
    }

    RotateTrain_SetAngles(self, ent, move_time);
    self->moveinfo.speed = distance / move_time;
    self->moveinfo.accel = self->moveinfo.decel = self->moveinfo.speed;

    Move_Calc(self, dest, rotate_train_wait);
    self->spawnflags |= RTRAIN_START_ON;
}

static void rotate_train_resume(edict_t* self)
{
    edict_t* ent = self->target_ent;
    if (!ent)
        return;

    vec3_t dest;
    VectorSubtract(ent->s.origin, self->mins, dest);
    vec3_t delta;
    VectorSubtract(dest, self->s.origin, delta);
    float distance = VectorLength(delta);
    float move_time = RotateTrain_ComputeMoveTime(self, ent, distance);

    if (move_time <= 0 || distance <= 1.0f) {
        VectorCopy(dest, self->s.origin);
        gi.linkentity(self);
        RotateTrain_SetAngles(self, ent, 0);
        rotate_train_wait(self);
        return;
    }

    RotateTrain_SetAngles(self, ent, move_time);
    self->moveinfo.speed = distance / move_time;
    self->moveinfo.accel = self->moveinfo.decel = self->moveinfo.speed;
    Move_Calc(self, dest, rotate_train_wait);
    self->spawnflags |= RTRAIN_START_ON;
}

static void rotate_train_find(edict_t* self)
{
    edict_t* ent;

    if (!self->target) {
        gi.dprintf("rotate_train_find: no target\n");
        return;
    }
    ent = G_PickTarget(self->target);
    if (!ent) {
        gi.dprintf("rotate_train_find: target %s not found\n", self->target);
        return;
    }
    self->target = ent->target;

    VectorSubtract(ent->s.origin, self->mins, self->s.origin);
    gi.linkentity(self);

    if (!self->targetname)
        self->spawnflags |= RTRAIN_START_ON;

    if (self->spawnflags & RTRAIN_START_ON) {
        self->nextthink = level.time + FRAMETIME;
        self->think = rotate_train_next;
        self->activator = self;
    }
}

static void rotate_train_use(edict_t* self, edict_t* other, edict_t* activator)
{
    self->activator = activator;

    if (self->spawnflags & RTRAIN_START_ON) {
        if (!(self->spawnflags & RTRAIN_TOGGLE))
            return;
        self->spawnflags &= ~RTRAIN_START_ON;
        VectorClear(self->velocity);
        VectorClear(self->avelocity);
        self->nextthink = 0;
    }
    else {
        if (self->target_ent)
            rotate_train_resume(self);
        else
            rotate_train_next(self);
    }
}

void SP_func_rotate_train(edict_t* self)
{
    self->movetype = MOVETYPE_PUSH;
    VectorClear(self->s.angles);
    self->blocked = rotate_train_blocked;

    if (self->spawnflags & RTRAIN_BLOCK_STOPS)
        self->dmg = 0;
    else if (!self->dmg)
        self->dmg = 100;

    self->solid = SOLID_BSP;
    gi.setmodel(self, self->model);

    if (st.noise)
        self->moveinfo.sound_middle = gi.soundindex(st.noise);

    if (!self->speed)
        self->speed = 100;

    self->moveinfo.speed = self->speed;
    self->moveinfo.accel = self->moveinfo.decel = self->moveinfo.speed;

    self->use = rotate_train_use;

    rotate_train_state_t* rt = gi.TagMalloc(sizeof(*rt), TAG_LEVEL);
    memset(rt, 0, sizeof(*rt));
    self->rotate_train = rt;

    gi.linkentity(self);

    if (self->target) {
        self->nextthink = level.time + FRAMETIME;
        self->think = rotate_train_find;
    }
    else {
        gi.dprintf("func_rotate_train without a target at %s\n", vtos(self->absmin));
    }
}
#endif