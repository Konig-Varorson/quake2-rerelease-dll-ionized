// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"
#include "../m_player.h"

/*
======================================================================

FLAREGUN

======================================================================
*/

void weapon_flaregun_fire(edict_t *ent)
{
	vec3_t start, aimdir;
	// limit upwards angle so you don't fire behind you
	P_ProjectSource(ent, { max(-62.5f, ent->client->v_angle[0]), ent->client->v_angle[1], ent->client->v_angle[2] }, { 8, 8, -8 }, start, aimdir);
	fire_flare(ent, start, aimdir, 1, 600, 1, 1, (crandom_open() * 10.0f), (200 + crandom_open() * 10.0f));

	P_AddWeaponKick(ent, ent->client->v_forward * -2, { -1.f, 0.f, 0.f });

	gi.WriteByte(svc_muzzleflash);
	gi.WriteEntity(ent);
	// use the blaster muzzleflash since we don't have a flare gun muzzleflash
	gi.WriteByte(MZ_BLASTER | is_silenced);
	gi.multicast(ent->s.origin, MULTICAST_PVS, false);

	gi.sound(ent, CHAN_AUTO, gi.soundindex("weapons/flare/shoot.wav"), !is_silenced, ATTN_NORM, 0);
	PlayerNoise(ent, start, PNOISE_WEAPON);
	
	G_RemoveAmmo(ent);
}

void Weapon_Flaregun(edict_t *ent)
{
	constexpr int pause_frames[] = { 15, 25, 35, 0 };
	constexpr int fire_frames[] = { 8, 0 };

	Weapon_Generic(ent, 5, 14, 44, 48, pause_frames, fire_frames, weapon_flaregun_fire);
}
#if 0
/*
======================================================================

LASER TRIPBOMB / IRED

======================================================================
*/

void weapon_lasertripbomb_fire (edict_t *ent)
{
	if (ent->client->ps.gunframe == 10)
	{
		vec3_t	offset;
		vec3_t	forward;
		vec3_t	start;
		int damage = TBOMB_DAMAGE;
		int radius = TBOMB_RADIUS_DAMAGE;
		if (is_quad)
			damage *= 4;

		// place the trip bomb
		offset = { 0, 0, ent->viewheight * 0.75f };
		AngleVectors(ent->client->v_angle, forward, nullptr, nullptr);
		start = ent->s.origin + offset;

		if (fire_lasertripbomb(ent, start, forward, damage, radius, is_quad))
		{
			G_RemoveAmmo(ent, 1);
			Weapon_PowerupSound(ent);

			// switch models
			ent->client->ps.gunindex = gi.modelindex("models/weapons/v_ired/hand.md2");

			if (!ent->client->pers.inventory[ent->client->pers.weapon->ammo])
			{
				// Skip directly to FRAME_IDLE_FIRST (16) to avoid showing the "grab another tripbomb" part
				ent->client->ps.gunframe = 16;
				return;
			}
		}
	}
	else if (ent->client->ps.gunframe == 15)
	{
		// switch models back
		int mi = gi.modelindex("models/weapons/v_ired/tris.md2");
		if (ent->client->ps.gunindex != mi)
		{
			ent->client->ps.gunindex = mi;
			// go back to get another trip bomb
			ent->client->ps.gunframe = 0;
			return;
		}
	}
	else if (ent->client->ps.gunframe == 6)
	{
		ent->client->ps.gunframe = 16;
		return;
	}

	ent->client->ps.gunframe++;
}

void Weapon_LaserTripBomb(edict_t *ent)
{
	static int	pause_frames[]	= {24, 33, 43, 0};
	static int	fire_frames[]	= {6, 10, 15, 0};

	Place_Generic(ent, 6, 15, 43, 48, pause_frames, fire_frames, weapon_lasertripbomb_fire);
}
#endif
/*
======================================================================

EMPNUKE

======================================================================
*/

void weapon_EMPNuke_fire (edict_t *ent)
{
	fire_empnuke(ent, ent->s.origin, 1024);

	G_RemoveAmmo(ent);

	if(ent->client->pers.inventory[ent->client->pers.weapon->ammo])
	{
		ent->client->weaponstate = WEAPON_ACTIVATING;
		ent->client->ps.gunframe = 0;
	}
	else
		NoAmmoWeaponChange(ent, true);
}

void Weapon_EMPNuke (edict_t *ent)
{
	static int pause_frames[] = {25, 34, 43, 0};
	static int fire_frames[]  = {16, 0};

	// These sounds are too obnoxious, commented out for now.
	// if (deathmatch->value)
	// {
		// if (ent->client->ps.gunframe == 0)
		// {
		// 	gi.sound(ent, CHAN_AUTO, gi.soundindex("items/empnuke/emp_act.wav"), 1, ATTN_NORM, 0);
		// }
		// else if (ent->client->ps.gunframe == 11)
		// {
		// 	gi.sound(ent, CHAN_AUTO, gi.soundindex("items/empnuke/emp_spin.wav"), 1, ATTN_NORM, 0);
		// }
		// else if (ent->client->ps.gunframe == 35)
		// {
		// 	gi.sound(ent, CHAN_AUTO, gi.soundindex("items/empnuke/emp_idle.wav"), 1, ATTN_NORM, 0);
		// }
	// }

	Weapon_Generic (ent, 9, 16, 43, 47, pause_frames, fire_frames, weapon_EMPNuke_fire);
}
#if 0
/*
======================================================================

SONIC CANNON

======================================================================
*/

gtime_t 	  SC_FIRE_DELAY     = 400_ms;	 // emulates the slight firing delay from original Zaero
gtime_t 	  SC_MAXFIRETIME    = 5_sec;     // in seconds...
constexpr int SC_MAXCELLS       = 100;       // maximum number of cells

void weapon_sc_fire (edict_t *ent)
{
	vec3_t start;

	if (!(ent->client->buttons & BUTTON_ATTACK))
	{
		ent->client->ps.gunframe++;

    	if (ent->client->weapon_sound && ent->client->ps.gunframe < 18)
			ent->client->ps.gunframe = 18;

		ent->client->scannon_ready_time = gtime_t::from_sec(0);
	}

	else
	{
		if (EMPNukeCheck(ent, ent->s.origin))
		{
			gi.sound (ent, CHAN_AUTO, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
			
			ent->client->ps.gunframe = 18;
			ent->client->weapon_sound = 0;
		 	ent->client->scannon_sound_time = gtime_t::from_sec(0);

	    	ent->dmg_radius = 0;
		  	ent->client->scannon_start_fire_time = gtime_t::from_sec(0);
			return;
		}

		if (ent->client->scannon_ready_time.milliseconds() == 0)
        {
            ent->client->scannon_ready_time = level.time;
            // Don't proceed with firing yet
            return;
        }

        else if ((level.time - ent->client->scannon_ready_time).milliseconds() < SC_FIRE_DELAY.milliseconds())
            // Still in delay period, do nothing
            return;

		if (ent->client->scannon_start_fire_time.milliseconds() == 0)
			ent->client->scannon_start_fire_time = level.time;

		else if (level.time.seconds() - ent->client->scannon_start_fire_time.seconds() >= SC_MAXFIRETIME.seconds())
			ent->client->ps.gunframe = 17;

		else
		{
			int old_cells = (int)ent->dmg_radius;
			ent->dmg_radius = ((level.time - ent->client->scannon_start_fire_time).seconds() /  SC_MAXFIRETIME.seconds()) * SC_MAXCELLS;

			if (old_cells < (int)ent->dmg_radius)
			{
				old_cells = (int)ent->dmg_radius - old_cells;

				if (ent->client->pers.inventory[ent->client->pers.weapon->ammo] < old_cells)
				{
					ent->dmg_radius -= (old_cells - ent->client->pers.inventory[ent->client->pers.weapon->ammo]);
					ent->client->pers.inventory[ent->client->pers.weapon->ammo] = 0;
				}

				else
					ent->client->pers.inventory[ent->client->pers.weapon->ammo] -= old_cells;
			}
		}

		if (ent->client->pers.inventory[ent->client->pers.weapon->ammo] < 1)
		{
			ent->client->ps.gunframe = 17;
			NoAmmoWeaponChange(ent, true);
		}
		else
		{
			if (ent->client->scannon_sound_time < level.time && (ent->client->buttons & BUTTON_ATTACK))
			{
				ent->client->weapon_sound = gi.soundindex("weapons/sonic/sc_fire.wav");
				PlayerNoise(ent, start, PNOISE_WEAPON);
			}
		}

		fire_scannon_effects(ent);

		ent->client->ps.gunframe++;
	}

	if (ent->client->ps.gunframe >= 18)
	{
		if (((level.time.seconds() - ent->client->scannon_start_fire_time.seconds()) < SC_MAXFIRETIME.seconds()) && 
			ent->client->pers.inventory[ent->client->pers.weapon->ammo] && 
			(ent->client->buttons & BUTTON_ATTACK))
		{
			ent->client->ps.gunframe = 12;
		}

		else 
		{
			ent->client->weapon_sound = 0;
			ent->client->scannon_sound_time = gtime_t::from_sec(0);
	
			if(EMPNukeCheck(ent, ent->s.origin))
				gi.sound (ent, CHAN_WEAPON, gi.soundindex("items/empnuke/emp_missfire.wav"), 1, ATTN_NORM, 0);
	
			else
			{
				gi.sound(ent, CHAN_WEAPON, gi.soundindex("weapons/sonic/sc_cool.wav"), !is_silenced ? 1 : 0.4, ATTN_NORM, 0);

				if (ent->dmg_radius > 0)
					fire_scannon(ent, SC_MAXCELLS);
			}
			ent->dmg_radius = 0;
			ent->client->scannon_start_fire_time = gtime_t::from_sec(0);
			ent->client->ps.gunframe = 23;
			return;
		}
	}
}

void Weapon_SonicCannon (edict_t *ent)
{
	constexpr int pause_frames[] = {32, 42, 52, 0};
	constexpr int activate_start = 0;
	constexpr int deactivate_start = 53;

	if (ent->client->ps.gunframe == activate_start)
	{
		gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/sonic/sc_act.wav"), !is_silenced ? 1 : 0.4, ATTN_NORM, 0);
    	ent->client->scannon_sound_time = gtime_t::from_ms(0);
    	ent->client->scannon_start_fire_time = gtime_t::from_ms(0);
    	ent->dmg_radius = 0;
	}

	else if (ent->client->ps.gunframe == deactivate_start)
		gi.sound(ent, CHAN_VOICE, gi.soundindex("weapons/sonic/sc_dact.wav"), !is_silenced ? 1 : 0.4, ATTN_NORM, 0);

  	else if ((ent->client->buttons & BUTTON_ATTACK) && ent->client->scannon_sound_time.seconds() == 0)
  	{
		bool request_firing = ent->client->weapon_fire_buffered || ((ent->client->latched_buttons | ent->client->buttons) & BUTTON_ATTACK);

		if (ent->client->zaero_camera_tracking)
		{
			Visor_Check(ent, request_firing);
			return;
		}

    	ent->client->scannon_sound_time = level.time + 400_ms;
		gi.sound(ent, CHAN_WEAPON, gi.soundindex("weapons/sonic/sc_warm.wav"), !is_silenced ? 1 : 0.4, ATTN_NORM, 0);
  	}

  	Weapon_Repeating(ent, 6, 22, 52, 57, pause_frames, weapon_sc_fire);
}

/*
======================================================================

SNIPER RIFLE

======================================================================
*/

void weapon_sniperrifle_fire (edict_t *ent)
{
	int damage;
	int kick;

	if (deathmatch->value)
	{	// normal damage is too extreme in dm
		damage = 150;
		kick = 300;
	}

	else
	{
		damage = 250; // in rerelease this is twice the damage of a railgun shot
		kick = 400;
	}

	if (is_quad)
	{
		damage *= damage_multiplier;
		kick *= damage_multiplier;
	}

	vec3_t start, dir;
	P_ProjectSource(ent, ent->client->v_angle, { 0, 0, -8 }, start, dir, true);
	G_LagCompensate(ent, start, dir);
	fire_sniper_bullet(ent, start, dir, damage, kick);
	G_UnLagCompensate();
	P_AddWeaponKick(ent, ent->client->v_forward * -2, { -2.f, 0.f, 0.f });

	gi.sound(ent, CHAN_AUTO, gi.soundindex("weapons/sniper/fire.wav"), !is_silenced ? 1 : 0.4, ATTN_NORM, 0);
	PlayerNoise(ent, start, PNOISE_WEAPON);

	G_RemoveAmmo(ent);
}

void Weapon_SniperRifle(edict_t *ent)
{
	/*
		Activate/Deactivate
		0 - 8	: Activate
		9 - 18	: Fire
		19 - 27 : Idle 1
		28 - 36	: Idle 2
		37 - 41	: Deactivate

		Zoom
		0 - 1 Zoom
		Hold 1 while zoomed
	*/

	constexpr int activate_start = 0;
	constexpr int activate_end = 8;
	constexpr int deactivate_start = 37;
	constexpr int deactivate_end = 41;
	constexpr int sp_fov = 15;
	constexpr int dm_fov = 30;
	char val[MAX_INFO_VALUE];

	if (ent->client->weaponstate == WEAPON_DROPPING)
	{
		ent->client->sniper_ready_time = gtime_t::from_ms(0);
		if (ent->client->ps.gunframe == deactivate_start)
		{
			gi.Info_ValueForKey(ent->client->pers.userinfo, "fov", val, sizeof(val));
			ent->client->ps.fov = clamp((float) atoi(val), 1.f, 160.f);
			gi.sound(ent, CHAN_AUTO, gi.soundindex("weapons/sniper/snip_bye.wav"), 1, ATTN_NORM, 0);
		}
		else if (ent->client->ps.gunframe == deactivate_end)
		{
			ChangeWeapon(ent);
			return;
		}

		ent->client->ps.gunframe++;
		return;
	}

	if (ent->client->weaponstate == WEAPON_ACTIVATING)
	{
		if (ent->client->ps.gunframe == activate_start)
			gi.sound(ent, CHAN_AUTO, gi.soundindex("weapons/sniper/snip_act.wav"), 1, ATTN_NORM, 0);
		else if (ent->client->ps.gunframe == activate_end)
		{
			ent->client->weaponstate = WEAPON_READY;
			ent->client->ps.gunindex = (deathmatch->value ?
				gi.modelindex("models/weapons/v_sniper/dmscope/tris.md2") :
				gi.modelindex("models/weapons/v_sniper/scope/tris.md2") );
			ent->client->ps.gunframe = 0;
			ent->client->ps.fov = (deathmatch->value ? dm_fov : sp_fov);
			ent->client->sniper_ready_time = level.time + 3_sec;
			return;
		}
		ent->client->ps.gunframe++;
		return;
	}

	if ((ent->client->newweapon) && (ent->client->weaponstate != WEAPON_FIRING))
	{
		// back to other gun model
		ent->client->ps.gunindex = gi.modelindex("models/weapons/v_sniper/tris.md2");
		ent->client->weaponstate = WEAPON_DROPPING;
		ent->client->ps.gunframe = deactivate_start;
		return;
	}

	if (ent->client->weaponstate == WEAPON_READY)
	{
		ent->client->ps.gunindex = (deathmatch->value ? 
			gi.modelindex("models/weapons/v_sniper/dmscope/tris.md2") :
			gi.modelindex("models/weapons/v_sniper/scope/tris.md2") );
		
		ent->client->ps.fov = (deathmatch->value ? dm_fov : sp_fov);

		if (ent->client->sniper_ready_time >= level.time)
		{
			if ((ent->client->sniper_ready_time.milliseconds() - level.time.milliseconds()) % 1000 < 20)
				gi.sound(ent, CHAN_AUTO, gi.soundindex("weapons/sniper/beep.wav"), 1, ATTN_NORM, 0);
		}

		if (((ent->client->latched_buttons|ent->client->buttons) & BUTTON_ATTACK))
		{
			if (ent->client->zaero_camera_tracking)
			{
				bool request_firing = ent->client->weapon_fire_buffered || ((ent->client->latched_buttons | ent->client->buttons) & BUTTON_ATTACK);
				Visor_Check(ent, request_firing);
			}
			else if ((level.time >= ent->client->sniper_ready_time) && (!ent->client->zaero_camera_tracking))
			{
				ent->client->latched_buttons &= ~BUTTON_ATTACK;
				if ((G_CheckInfiniteAmmo(ent->client->pers.weapon)) || 
					(ent->client->pers.inventory[ent->client->pers.weapon->ammo] >= ent->client->pers.weapon->quantity))
				{
					ent->client->weaponstate = WEAPON_FIRING;

					// start the animation
					ent->client->anim_priority = ANIM_ATTACK;
					if (ent->client->ps.pmove.pm_flags & PMF_DUCKED)
					{
						ent->s.frame = FRAME_crattak1-1;
						ent->client->anim_end = FRAME_crattak9;
					}
					else
					{
						ent->s.frame = FRAME_attack1-1;
						ent->client->anim_end = FRAME_attack8;
					}
				}
				else
					NoAmmoWeaponChange(ent, true);
			}
		}
	}

	if (ent->client->weaponstate == WEAPON_FIRING)
	{
		ent->client->ps.gunindex = (deathmatch->value ? 
				gi.modelindex("models/weapons/v_sniper/dmscope/tris.md2") :
				gi.modelindex("models/weapons/v_sniper/scope/tris.md2") );
			
		ent->client->ps.fov = (deathmatch->value ? dm_fov : sp_fov);

		// fire
		weapon_sniperrifle_fire(ent);
		
		// start recharge
		ent->client->weaponstate = WEAPON_READY;
		ent->client->sniper_ready_time = level.time + 3_sec;
	}
}

/*
=================
weapon_a2k_exp_think
=================
*/

THINK(weapon_a2k_exp_think) (edict_t *self) -> void
{
	self->s.frame++;
	self->s.skinnum++;

	if (self->s.frame == 6)
	{
		G_FreeEdict(self);
		return;
	}

	self->nextthink = level.time + 10_hz;
}

void weapon_a2k_fire (edict_t *ent)
{
	if (ent->client->ps.gunframe < 14 || ent->client->ps.gunframe > 19)
		ent->client->ps.gunframe = 14;

	if (ent->client->ps.gunframe == 14)
	{
		ent->client->a2k_time = level.time + 5_sec;
		gi.sound(ent, CHAN_WEAPON, gi.soundindex("weapons/a2k/countdn.wav"), 1, ATTN_NORM, 0);
		G_RemoveAmmo(ent);
	}
		
	if (ent->client->ps.gunframe >= 14 && ent->client->ps.gunframe <= 18)
		ent->client->ps.gunframe++;

	else if (level.time >= ent->client->a2k_time)
	{
		// boom
		edict_t *exp = nullptr;
		float damage = 2500;
		float dmg_radius = 512;
		Weapon_PowerupSound(ent);
		if (is_quad)
		{
			damage *= 4;
			dmg_radius *= 4;
		}
		// Original Zaero uses a custom radius damage function, but all it does is multiplies dmg_radius by 2?
		T_RadiusDamage(ent, ent, damage, nullptr, dmg_radius * 2, DAMAGE_NONE, MOD_A2K);

		exp = G_Spawn();
		exp->classname = "A2K_Explosion";
		exp->solid = SOLID_NOT;
		exp->movetype = MOVETYPE_NONE;
		exp->s.origin = ent->s.origin;
		exp->s.modelindex = gi.modelindex("models/objects/b_explode/tris.md2");
		exp->s.frame = 0;
		exp->s.skinnum = 6;
		exp->s.effects |= EF_ANIM_ALLFAST;
		exp->think = weapon_a2k_exp_think;
		exp->nextthink = level.time + 10_hz;
		gi.linkentity(exp);
		gi.positioned_sound(exp->s.origin, exp, CHAN_AUTO, gi.soundindex("weapons/a2k/ak_exp01.wav"), 1, ATTN_NORM, 0);
		ent->client->a2k_time = gtime_t::from_sec(0);
		ent->client->ps.gunframe++;
		ent->client->weapon_sound = 0;
		return;
	}

	else if (ent->client->ps.gunframe == 19)
		// don't increase the gunframe
		return;
}

void Weapon_A2k (edict_t *ent)
{
	constexpr int pause_frames[] = {20, 30, 40, 0};

	Weapon_Repeating(ent, 9, 19, 49, 55, pause_frames, weapon_a2k_fire);
}

/*
======================================================================

SHOVE / PUSH

======================================================================
*/

void Action_Push (edict_t *ent)
{
	if (ent->client->ps.gunframe == 0)
		ent->client->ps.gunframe++;

	else if (ent->client->ps.gunframe == 4)
	{
		vec3_t forward;
		vec3_t offset;
		vec3_t start;
		
		// contact
		AngleVectors(ent->client->v_angle, forward, nullptr, nullptr);
		offset = { 0, 0, ent->viewheight * 0.5f };
		start = ent->s.origin + offset;
		push_hit(ent, start, forward, 2, 512);
		ent->client->ps.gunframe++;
	}
	else if (ent->client->ps.gunframe == 8)
	{
		// go back to old weapon
		ent->client->newweapon = ent->client->pers.lastweapon;
		ChangeWeapon(ent);
	}
	else
		ent->client->ps.gunframe++;
}
#endif