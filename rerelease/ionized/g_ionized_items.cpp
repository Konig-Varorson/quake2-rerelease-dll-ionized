// Licensed under the GNU General Public License 2.0.

#include "../g_local.h"

/* KONIG - small ammos*/
void SP_ammo_shells_small(edict_t* self)
{
	self->model = "models/vault/items/ammo/shells/small/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_AMMO_SHELLS), ED_GetSpawnTemp());
	self->count = 5;
}

void SP_ammo_bullets_small(edict_t* self)
{
	self->model = "models/vault/items/ammo/bullets/small/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_AMMO_BULLETS), ED_GetSpawnTemp());
	self->count = 25;
}

void SP_ammo_rockets_small(edict_t* self)
{
	self->model = "models/vault/items/ammo/rockets/small/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_AMMO_ROCKETS), ED_GetSpawnTemp());
	self->count = 2;
}

void SP_ammo_cells_small(edict_t* self)
{
	self->model = "models/vault/items/ammo/cells/small/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_AMMO_CELLS), ED_GetSpawnTemp());
	self->count = 25;
}

void SP_ammo_slugs_small(edict_t* self)
{
	self->model = "models/vault/items/ammo/slugs/small/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_AMMO_SLUGS), ED_GetSpawnTemp());
	self->count = 3;
}

void SP_ammo_flechettes_small(edict_t* self)
{
	self->model = "models/items/ammo/flechette/small/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_AMMO_FLECHETTES), ED_GetSpawnTemp());
	self->count = 3;
}

void SP_ammo_prox_small(edict_t* self)
{
	self->model = "models/items/ammo/prox/small/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_AMMO_PROX), ED_GetSpawnTemp());
	self->count = 3;
}

void SP_ammo_magslugs_small(edict_t* self)
{
	self->model = "models/items/ammo/magslugs/small/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_AMMO_MAGSLUG), ED_GetSpawnTemp());
	self->count = 3;
}

void SP_ammo_rounds_small(edict_t* self)
{
	self->model = "models/items/ammo/rounds/small/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_AMMO_ROUNDS), ED_GetSpawnTemp());
	self->count = 3;
}

//ZAERO KEY COMPATIBILITY
void SP_key_landingarea(edict_t* self)
{
	self->model = "models/items/keys/blue_gearkey/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_KEY_BLUE_GEAR), ED_GetSpawnTemp());
	self->count = 1;
}

void SP_key_lab(edict_t* self)
{
	self->model = "models/items/keys/yellow_gearkey/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_KEY_YELLOW_GEAR), ED_GetSpawnTemp());
	self->count = 1;
}

void SP_key_clearancepass(edict_t* self)
{
	self->model = "models/items/keys/red_gearkey/tris.md2";
	SpawnItem(self, GetItemByIndex(IT_KEY_RED_GEAR), ED_GetSpawnTemp());
	self->count = 1;
}

void SP_item_sigil(edict_t* self)
{
	if (self->spawnflags.has(16_spawnflag))
	{
		self->model = "models/items/keys/sigil2/tris.md2";
		SpawnItem(self, GetItemByIndex(IT_TECH_STRENGTH), ED_GetSpawnTemp());
	}
	else if (self->spawnflags.has(32_spawnflag))
	{
		self->model = "models/items/keys/sigil3/tris.md2";
		SpawnItem(self, GetItemByIndex(IT_TECH_HASTE), ED_GetSpawnTemp());
	}
	else if (self->spawnflags.has(64_spawnflag))
	{
		self->model = "models/items/keys/sigil4/tris.md2";
		SpawnItem(self, GetItemByIndex(IT_TECH_REGENERATION), ED_GetSpawnTemp());
	}
	else
	{
		self->model = "models/items/keys/sigil1/tris.md2";
		SpawnItem(self, GetItemByIndex(IT_TECH_RESISTANCE), ED_GetSpawnTemp());
	}
	self->count = 1;
}
