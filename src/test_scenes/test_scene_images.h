#pragma once
#include "test_scenes/test_id_to_pool_id.h"

enum class test_scene_image_id {
	// GEN INTROSPECTOR enum class test_scene_image_id
	BLANK = 0,

	CRATE,

	TRUCK_INSIDE,
	TRUCK_FRONT,

	JMIX114,

	TEST_CROSSHAIR,

	SMOKE_1,
	SMOKE_2,
	SMOKE_3,
	SMOKE_4,
	SMOKE_5,
	SMOKE_6,

	PIXEL_THUNDER_1,
	PIXEL_THUNDER_2,
	PIXEL_THUNDER_3,
	PIXEL_THUNDER_4,
	PIXEL_THUNDER_5,

	ASSAULT_RIFLE,
	BILMER2000,
	KEK9,
	SUBMACHINE,
	URBAN_CYAN_MACHETE,
	ROCKET_LAUNCHER,

	TEST_BACKGROUND,
	FLOOR,

	SAMPLE_MAGAZINE,
	SMALL_MAGAZINE,
	ROUND_TRACE,
	ELECTRIC_MISSILE,
	PINK_CHARGE,
	PINK_SHELL,
	CYAN_CHARGE,
	CYAN_SHELL,
	RED_CHARGE,
	RED_SHELL,
	GREEN_CHARGE,
	GREEN_SHELL,
	BACKPACK,

	HAVE_A_PLEASANT,
	AWAKENING,
	METROPOLIS,

	BRICK_WALL,
	ROAD,
	ROAD_FRONT_DIRT,

	CAST_BLINK_1,
	CAST_BLINK_2,
	CAST_BLINK_3,
	CAST_BLINK_4,
	CAST_BLINK_5,
	CAST_BLINK_6,
	CAST_BLINK_7,
	CAST_BLINK_8,
	CAST_BLINK_9,
	CAST_BLINK_10,
	CAST_BLINK_11,
	CAST_BLINK_12,
	CAST_BLINK_13,
	CAST_BLINK_14,
	CAST_BLINK_15,
	CAST_BLINK_16,
	CAST_BLINK_17,
	CAST_BLINK_18,
	CAST_BLINK_19,

	SILVER_TROUSERS_1,
	SILVER_TROUSERS_2,
	SILVER_TROUSERS_3,
	SILVER_TROUSERS_4,
	SILVER_TROUSERS_5,
	SILVER_TROUSERS_6,
	SILVER_TROUSERS_7,
	SILVER_TROUSERS_8,
	SILVER_TROUSERS_9,
	SILVER_TROUSERS_10,
	SILVER_TROUSERS_11,
	SILVER_TROUSERS_12,

	SILVER_TROUSERS_STRAFE_1,
	SILVER_TROUSERS_STRAFE_2,
	SILVER_TROUSERS_STRAFE_3,
	SILVER_TROUSERS_STRAFE_4,
	SILVER_TROUSERS_STRAFE_5,
	SILVER_TROUSERS_STRAFE_6,
	SILVER_TROUSERS_STRAFE_7,
	SILVER_TROUSERS_STRAFE_8,

	METROPOLIS_CHARACTER_BARE_1,
	METROPOLIS_CHARACTER_BARE_2,
	METROPOLIS_CHARACTER_BARE_3,
	METROPOLIS_CHARACTER_BARE_4,
	METROPOLIS_CHARACTER_BARE_5,

	TRUCK_ENGINE,

	HEALTH_ICON,
	PERSONAL_ELECTRICITY_ICON,
	CONSCIOUSNESS_ICON,

	AMPLIFIER_ARM,

	SPELL_HASTE_ICON,
	SPELL_ELECTRIC_SHIELD_ICON,
	SPELL_ELECTRIC_TRIAD_ICON,
	SPELL_FURY_OF_THE_AEONS_ICON,
	SPELL_ULTIMATE_WRATH_OF_THE_AEONS_ICON,
	SPELL_EXALTATION_ICON,
	SPELL_ECHOES_OF_THE_HIGHER_REALMS_ICON,

	PERK_HASTE_ICON,
	PERK_ELECTRIC_SHIELD_ICON,

	FORCE_GRENADE,
	PED_GRENADE,
	INTERFERENCE_GRENADE,

	FORCE_GRENADE_RELEASED,
	PED_GRENADE_RELEASED,
	INTERFERENCE_GRENADE_RELEASED,
	FORCE_ROCKET,

	COUNT
	// END GEN INTROSPECTOR
};

inline auto to_image_id(const test_scene_image_id id) {
	return to_pool_id<assets::image_id>(id);
}
