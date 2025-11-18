// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

#if 0
void InitTrigger (edict_t *self);

#define CAMERA_SPAWN_FREEZE     1

static void Camera_AttachAll(edict_t *self);
static void Camera_DetachAll(edict_t *self);
static void Camera_Stop(edict_t *self);
static void Camera_StartPath(edict_t *self, edict_t *corner);
static void Camera_Think(edict_t *self);
static void Camera_UpdatePosition(edict_t *self);
static void Camera_HandleCorner(edict_t *self);
static void Camera_UpdateOrientation(edict_t *self);
static edict_t *Camera_FindNextCorner(edict_t *corner);
static void Camera_TargetUse(edict_t *self, edict_t *other, edict_t *activator);
static void TriggerCamera_Reset(edict_t *self);
static void TriggerCamera_Use(edict_t *self, edict_t *other, edict_t *activator);
static void TriggerCamera_Touch(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf);

static qboolean Camera_IsActive(edict_t *self)
{
        if (!self || !self->camera_state)
                return false;
        return self->camera_state->active && self->inuse;
}

static void Camera_UpdateClients(edict_t *self)
{
        if (!self->camera_state->freeze_players)
                return;

        for (int i = 1; i <= maxclients->value; i++) {
                edict_t *cl = &g_edicts[i];
                if (!cl->inuse || !cl->client)
                        continue;
                if (cl->client->camera == self)
                        cl->client->camera_freeze = true;
        }
}

static void Camera_AttachAll(edict_t *self)
{
        camera_state_t *cam = self->camera_state;

        for (int i = 1; i <= maxclients->value; i++) {
                edict_t *cl = &g_edicts[i];
                if (!cl->inuse || !cl->client)
                        continue;
                cl->client->camera = self;
                cl->client->camera_freeze = cam->freeze_players;
                cl->client->camera_endtime = cam->stop_time;
        }
}

static void Camera_DetachAll(edict_t *self)
{
        for (int i = 1; i <= maxclients->value; i++) {
                edict_t *cl = &g_edicts[i];
                if (!cl->inuse || !cl->client)
                        continue;
                if (cl->client->camera == self) {
                        cl->client->camera = NULL;
                        cl->client->camera_freeze = false;
                        cl->client->camera_endtime = 0;
                }
        }
}

static void Camera_StartSounds(edict_t *self)
{
        if (self->camera_state->sound_loop)
                self->s.sound = self->camera_state->sound_loop;
}

static void Camera_StopSounds(edict_t *self)
{
        self->s.sound = 0;
}

static void Camera_Stop(edict_t *self)
{
        camera_state_t *cam = self->camera_state;
        if (!cam)
                return;

        cam->active = false;
        cam->move_duration = 0;
        cam->target_corner = NULL;
        cam->wait_override = 0;
        Camera_StopSounds(self);
        Camera_DetachAll(self);
        self->nextthink = 0;
        self->think = NULL;
}

static void Camera_UpdateOrientation(edict_t *self)
{
        camera_state_t *cam = self->camera_state;
        edict_t *focus = NULL;

        if (cam->track && cam->track->inuse)
                focus = cam->track;
        else if (cam->focus && cam->focus->inuse)
                focus = cam->focus;

        if (focus) {
                vec3_t target;
                VectorCopy(focus->s.origin, target);
                if (focus->client)
                        target[2] += focus->viewheight;
                vec3_t dir;
                VectorSubtract(target, self->s.origin, dir);
                if (!VectorCompare(dir, vec3_origin))
                        vectoangles(dir, self->s.angles);
                return;
        }

        if (cam->has_angle_goal && cam->move_duration > 0) {
                float t = 0;
                if (cam->move_duration > 0)
                        t = (level.time - cam->move_start_time) / cam->move_duration;
                if (t < 0)
                        t = 0;
                if (t > 1)
                        t = 1;
                for (int i = 0; i < 3; i++) {
                        float a0 = cam->start_angles[i];
                        float a1 = cam->end_angles[i];
                        float result = LerpAngle(a0, a1, t);
                        self->s.angles[i] = anglemod(result);
                }
        }
}

static void Camera_UpdatePosition(edict_t *self)
{
        camera_state_t *cam = self->camera_state;
        if (cam->move_duration <= 0)
                return;

        float t = (level.time - cam->move_start_time) / cam->move_duration;
        if (t >= 1.0f) {
                VectorCopy(cam->move_end, self->s.origin);
                if (cam->has_angle_goal)
                        VectorCopy(cam->end_angles, self->s.angles);
                cam->move_duration = 0;
                gi.linkentity(self);
                Camera_HandleCorner(self);
                return;
        }

        if (t < 0)
                t = 0;

        vec3_t delta;
        VectorSubtract(cam->move_end, cam->move_start, delta);
        vec3_t pos;
        VectorMA(cam->move_start, t, delta, pos);
        VectorCopy(pos, self->s.origin);
        gi.linkentity(self);
}

static edict_t *Camera_FindInitialCorner(edict_t *self)
{
        camera_state_t *cam = self->camera_state;
        if (cam->initial_corner)
                return cam->initial_corner;
        if (!self->pathtarget)
                return NULL;
        cam->initial_corner = G_PickTarget(self->pathtarget);
        return cam->initial_corner;
}

static edict_t *Camera_FindNextCorner(edict_t *corner)
{
        if (!corner || !corner->target)
                return NULL;
        return G_PickTarget(corner->target);
}

static void Camera_SetAngleGoal(edict_t *self, edict_t *corner, float move_time)
{
        camera_state_t *cam = self->camera_state;
        cam->has_angle_goal = false;
        if (move_time <= 0)
                return;

        if (cam->track && cam->track->inuse)
                return; // tracking overrides explicit rotations
        if (cam->focus && cam->focus->inuse)
                return;

        VectorCopy(self->s.angles, cam->start_angles);

        if (!VectorCompare(corner->rotate, vec3_origin)) {
                VectorAdd(self->s.angles, corner->rotate, cam->end_angles);
                cam->has_angle_goal = true;
                goto finalize_angles;
        }

        vec3_t rate;
        if (!VectorCompare(corner->rotate_speed, vec3_origin))
                VectorCopy(corner->rotate_speed, rate);
        else if (!VectorCompare(self->rotate_speed, vec3_origin))
                VectorCopy(self->rotate_speed, rate);
        else
                VectorClear(rate);

        if (!VectorCompare(rate, vec3_origin)) {
                vec3_t delta;
                VectorScale(rate, move_time, delta);
                VectorAdd(self->s.angles, delta, cam->end_angles);
                cam->has_angle_goal = true;
                goto finalize_angles;
        }

        if (!VectorCompare(self->rotate, vec3_origin)) {
                VectorAdd(self->s.angles, self->rotate, cam->end_angles);
                cam->has_angle_goal = true;
                goto finalize_angles;
        }

        return;

finalize_angles:
        for (int i = 0; i < 3; i++) {
                cam->start_angles[i] = anglemod(cam->start_angles[i]);
                cam->end_angles[i] = anglemod(cam->end_angles[i]);
        }
}

static float Camera_ComputeMoveTime(edict_t *self, edict_t *corner, float distance)
{
        camera_state_t *cam = self->camera_state;
        if (corner && corner->duration > 0)
                return corner->duration;
        if (cam->duration > 0)
                return cam->duration;
        float speed = cam->speed;
        if (corner && corner->speed > 0)
                speed = corner->speed;
        else if (self->speed > 0)
                speed = self->speed;

        if (speed <= 0)
                return 0;
        return distance / speed;
}

static void Camera_StartPath(edict_t *self, edict_t *corner)
{
        camera_state_t *cam = self->camera_state;
        cam->target_corner = corner;
        if (!corner) {
                cam->move_duration = 0;
                return;
        }

        vec3_t dest;
        VectorCopy(corner->s.origin, dest);

        VectorCopy(self->s.origin, cam->move_start);
        VectorCopy(dest, cam->move_end);

        vec3_t delta;
        VectorSubtract(dest, self->s.origin, delta);
        float dist = VectorLength(delta);
        float move_time = Camera_ComputeMoveTime(self, corner, dist);
        if (move_time <= 0 || dist <= 1.0f) {
                VectorCopy(dest, self->s.origin);
                gi.linkentity(self);
                cam->move_duration = 0;
                cam->current_corner = corner;
                Camera_HandleCorner(self);
                return;
        }

        cam->move_start_time = level.time;
        cam->move_duration = move_time;
        Camera_SetAngleGoal(self, corner, move_time);
        Camera_UpdateClients(self);
}

static void Camera_HandleCorner(edict_t *self)
{
        camera_state_t *cam = self->camera_state;
        edict_t *corner = cam->target_corner;
        if (!corner)
                return;

        cam->current_corner = corner;
        cam->target_corner = NULL;

        if (corner->pathtarget) {
                char *save = corner->target;
                edict_t *old_owner = corner->owner;
                corner->target = corner->pathtarget;
                corner->owner = self;
                G_UseTargets(corner, cam->activator ? cam->activator : self);
                corner->target = save;
                corner->owner = old_owner;
                if (!self->inuse)
                        return;
        }

        if (corner->wait > 0) {
                self->think = Camera_Think;
                self->nextthink = level.time + corner->wait;
                cam->move_duration = 0;
                return;
        }

        edict_t *next = Camera_FindNextCorner(corner);
        if (next)
                Camera_StartPath(self, next);
}

static void Camera_Think(edict_t *self)
{
        camera_state_t *cam = self->camera_state;
        if (!cam)
                return;

        if (!cam->active) {
                self->think = NULL;
                self->nextthink = 0;
                return;
        }

        if (cam->stop_time > 0 && level.time >= cam->stop_time) {
                Camera_Stop(self);
                return;
        }

        Camera_UpdatePosition(self);
        Camera_UpdateOrientation(self);

        if (cam->move_duration <= 0 && !cam->target_corner) {
                edict_t *next = NULL;
                if (cam->current_corner)
                        next = Camera_FindNextCorner(cam->current_corner);
                else
                        next = Camera_FindInitialCorner(self);
                if (next)
                        Camera_StartPath(self, next);
        }

        self->think = Camera_Think;
        self->nextthink = level.time + FRAMETIME;
}

static edict_t *Camera_SelectTrack(edict_t *self, edict_t *requested, edict_t *activator)
{
        if (requested && requested->inuse)
                return requested;
        if (activator && activator->inuse && activator->client)
                return activator;
        return NULL;
}

static void Camera_Start(edict_t *self, edict_t *activator, edict_t *track, float wait_override)
{
        camera_state_t *cam = self->camera_state;
        if (!cam)
                return;

        cam->activator = activator;
        cam->track = Camera_SelectTrack(self, track, activator);
        if (cam->track)
                cam->focus = cam->track;

        cam->wait_override = wait_override;
        if (wait_override < 0 || cam->default_wait < 0)
                cam->stop_time = 0;
        else if (wait_override > 0)
                cam->stop_time = level.time + wait_override;
        else if (cam->default_wait > 0)
                cam->stop_time = level.time + cam->default_wait;
        else
                cam->stop_time = 0;

        cam->active = true;
        Camera_AttachAll(self);
        Camera_StartSounds(self);

        if (!cam->current_corner && !cam->target_corner)
                Camera_StartPath(self, Camera_FindInitialCorner(self));

        self->think = Camera_Think;
        self->nextthink = level.time + FRAMETIME;
}

static void camera_use(edict_t *self, edict_t *other, edict_t *activator)
{
        Camera_Start(self, activator, activator, 0);
}

void SP_misc_camera(edict_t *self)
{
        camera_state_t *cam;

        cam = gi.TagMalloc(sizeof(*cam), TAG_LEVEL);
        memset(cam, 0, sizeof(*cam));
        self->camera_state = cam;

        self->movetype = MOVETYPE_NONE;
        self->solid = SOLID_NOT;
        self->svflags |= SVF_NOCLIENT;

        cam->freeze_players = (self->spawnflags & CAMERA_SPAWN_FREEZE) != 0;
        cam->default_wait = (self->wait != 0) ? self->wait : 3.0f;
        cam->speed = (self->speed > 0) ? self->speed : 200.0f;
        cam->duration = self->duration;
        if (cam->default_wait < 0)
                cam->stop_time = 0;

        if (self->target)
                cam->focus = G_PickTarget(self->target);

        if (st.noise)
                cam->sound_loop = gi.soundindex(st.noise);

        self->use = camera_use;
        self->think = NULL;
        gi.linkentity(self);
}

void Camera_ClientPreFrame(edict_t *ent)
{
        if (!ent->client)
                return;

        edict_t *cam_ent = ent->client->camera;
        if (!cam_ent || !Camera_IsActive(cam_ent)) {
                ent->client->camera = NULL;
                ent->client->camera_freeze = false;
                return;
        }

        VectorCopy(cam_ent->s.angles, ent->client->v_angle);
}

void Camera_ClientPostFrame(edict_t *ent)
{
        if (!ent->client)
                return;

        edict_t *cam_ent = ent->client->camera;
        if (!cam_ent || !Camera_IsActive(cam_ent)) {
                ent->client->camera = NULL;
                ent->client->camera_freeze = false;
                return;
        }

        camera_state_t *cam = cam_ent->camera_state;
        if (!cam) {
                ent->client->camera = NULL;
                ent->client->camera_freeze = false;
                return;
        }

        for (int i = 0; i < 3; i++) {
                ent->client->ps.pmove.origin[i] = cam_ent->s.origin[i] * 8.0f;
                ent->client->ps.pmove.velocity[i] = 0;
                ent->client->ps.viewangles[i] = cam_ent->s.angles[i];
        }
        VectorCopy(cam_ent->s.angles, ent->client->v_angle);
        ent->client->ps.pmove.pm_type = cam->freeze_players ? PM_FREEZE : ent->client->ps.pmove.pm_type;
        ent->client->ps.gunindex = 0;
}

static edict_t *TriggerCamera_FindTarget(edict_t *self)
{
        if (!self->target)
                return NULL;
        edict_t *ent = G_PickTarget(self->target);
        if (!ent)
                gi.dprintf("trigger_misc_camera without valid camera target\n");
        return ent;
}

static edict_t *TriggerCamera_FindPathtarget(edict_t *self)
{
        if (!self->pathtarget)
                return NULL;
        return G_PickTarget(self->pathtarget);
}

static void TriggerCamera_Fire(edict_t *self, edict_t *activator)
{
        if (self->nextthink && self->nextthink > level.time)
                return;

        edict_t *camera = TriggerCamera_FindTarget(self);
        if (!camera || !camera->camera_state) {
                gi.dprintf("Illegal target for trigger_misc_camera\n");
                return;
        }

        edict_t *track = TriggerCamera_FindPathtarget(self);
        Camera_Start(camera, activator, track, self->wait);

        if (self->message && activator && activator->client)
                gi.centerprintf(activator, "%s", self->message);

        if (self->noise_index)
                gi.sound(activator, CHAN_AUTO, self->noise_index, 1.0f, ATTN_NORM, 0);

        if (self->delay <= 0)
                self->delay = 1.0f;
        self->think = TriggerCamera_Reset;
        self->nextthink = level.time + self->delay;
}

static void TriggerCamera_Reset(edict_t *self)
{
        self->nextthink = 0;
        self->think = NULL;
}

static void TriggerCamera_Use(edict_t *self, edict_t *other, edict_t *activator)
{
        TriggerCamera_Fire(self, activator);
}

static void TriggerCamera_Touch(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf)
{
        if (!other->client) {
                if (other->svflags & SVF_MONSTER) {
                        if (!(self->spawnflags & 1))
                                return;
                } else {
                        return;
                }
        } else {
                if (self->spawnflags & 2)
                        return;
        }

        TriggerCamera_Fire(self, other);
}

void SP_trigger_misc_camera(edict_t *self)
{
        if (!self->delay)
                self->delay = 1.0f;

        if (self->sounds == 1)
                self->noise_index = gi.soundindex("misc/secret.wav");
        else if (self->sounds == 2)
                self->noise_index = gi.soundindex("misc/talk.wav");
        else if (self->sounds == 3)
                self->noise_index = gi.soundindex("misc/trigger1.wav");

        InitTrigger(self);

        if (self->spawnflags & 4) {
                self->solid = SOLID_NOT;
                self->use = TriggerCamera_Use;
        } else {
                self->touch = TriggerCamera_Touch;
                self->use = TriggerCamera_Use;
        }

        gi.linkentity(self);
}

void SP_misc_camera_target(edict_t *self)
{
        self->svflags |= SVF_NOCLIENT;
        self->solid = SOLID_NOT;
        self->use = Camera_TargetUse;
        gi.linkentity(self);
}

static void Camera_TargetUse(edict_t *self, edict_t *other, edict_t *activator)
{
        edict_t *camera = NULL;
        if (other && other->owner && other->owner->camera_state)
                camera = other->owner;
        else if (activator && activator->camera_state)
                camera = activator;

        if (!camera || !camera->camera_state)
                return;

        camera_state_t *cam = camera->camera_state;
        if (self->target) {
                edict_t *target = G_PickTarget(self->target);
                if (target)
                        cam->track = target;
                cam->focus = target;
        } else {
                cam->focus = self;
        }

        if (self->speed > 0)
                cam->speed = self->speed;
        if (self->duration > 0)
                cam->duration = self->duration;
        if (!VectorCompare(self->rotate, vec3_origin))
                VectorCopy(self->rotate, camera->rotate);
        if (!VectorCompare(self->rotate_speed, vec3_origin))
                VectorCopy(self->rotate_speed, camera->rotate_speed);
}
#endif