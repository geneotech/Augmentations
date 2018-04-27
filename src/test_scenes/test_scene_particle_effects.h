#pragma once
#include "test_scenes/test_id_to_pool_id.h"

enum class test_scene_particle_effect_id {
	// GEN INTROSPECTOR enum class test_scene_particle_effect_id
	WANDERING_SMOKE,
	ENGINE_PARTICLES,
	MUZZLE_SMOKE,
	EXHAUSTED_SMOKE,
	CAST_CHARGING,
	HEALTH_DAMAGE_SPARKLES,
	CAST_SPARKLES,
	EXPLODING_RING_SMOKE,
	EXPLODING_RING_SPARKLES,
	PIXEL_MUZZLE_LEAVE_EXPLOSION,
	ELECTRIC_PROJECTILE_DESTRUCTION,
	WANDERING_PIXELS_DIRECTED,
	WANDERING_PIXELS_SPREAD,
	CONCENTRATED_WANDERING_PIXELS,
	ROUND_ROTATING_BLOOD_STREAM,
	THUNDER_REMNANTS,
	MISSILE_SMOKE_TRAIL,

	COUNT
	// END GEN INTROSPECTOR
};


inline auto to_particle_effect_id(const test_scene_particle_effect_id id) {
	return to_pool_id<assets::particle_effect_id>(id);
}