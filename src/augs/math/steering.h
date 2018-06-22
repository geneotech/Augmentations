#pragma once
#include <optional>
#include "augs/math/vec2.h"
#include "augs/math/arithmetical.h"
#include "augs/log.h"

namespace augs {
	inline bool isLeft(const vec2 a, const vec2 b, const vec2 c) {
		return ((b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x)) > 0;
	}

	inline auto seek(
		const vec2 current_vel,
		const vec2 current_pos,
		const vec2 target_pos
	) {
		const auto desired_vel = target_pos - current_pos;
		return desired_vel - current_vel;
	}

	inline auto seek(
		const vec2 current_vel,
		const vec2 current_pos,
		const vec2 hostile_pos,
		const real32 desired_speed
	) {
		const auto desired_vel = (hostile_pos - current_pos).set_length(desired_speed);
		return desired_vel - current_vel;
	}

	inline auto arrive(
		const vec2 current_vel,
		const vec2 current_pos,
		const vec2 arrival_pos,
		const real32 max_desired_speed,
		const real32 easing_zone
	) {
		const auto target_vector = arrival_pos - current_pos;
		const auto dist_from_target = target_vector.length();

		const auto desired_speed = std::min(1.f, dist_from_target / easing_zone) * max_desired_speed;
		const auto target_dir = target_vector / dist_from_target;
		const auto desired_vel = target_dir * desired_speed;

		return desired_vel - current_vel;
	}

	inline auto furthest_perpendicular(
		const vec2 current_vel,
		const vec2 target_vector
	) {
		const auto right_hand = current_vel.perpendicular_cw();
		const auto left_hand = -current_vel.perpendicular_cw();

		if (right_hand.dot(target_vector) < left_hand.dot(target_vector)) {
			return right_hand;
		}

		return left_hand;
	}

	inline auto closest_perpendicular(
		const vec2 current_vel,
		const vec2 target_vector
	) {
		const auto right_hand = current_vel.perpendicular_cw();
		const auto left_hand = -current_vel.perpendicular_cw();

		if (right_hand.dot(target_vector) > left_hand.dot(target_vector)) {
			return right_hand;
		}

		return left_hand;
	}

	inline auto immediate_avoidance(
		const vec2 victim_pos,
		const vec2 victim_vel,
		const vec2 danger_pos,
		const vec2,
		const real32 comfort_zone,
		const real32 desired_speed
	) {
		const auto target_vector = danger_pos - victim_pos;
		const auto danger_dist = target_vector.length();

		const auto desired_vel = furthest_perpendicular(victim_vel, target_vector).set_length(desired_speed);

		return (desired_vel - victim_vel) * disturbance(danger_dist, comfort_zone);
	}

	inline auto danger_avoidance(
		const vec2 victim_pos,
		const vec2 danger_pos,
		const vec2 danger_vel
	) {
		const auto danger_speed = danger_vel.length();
		const auto danger_dir = (danger_pos - victim_pos);

		if (danger_speed > 10) {
			return isLeft(danger_pos, danger_pos + danger_vel, victim_pos) ? danger_vel.perpendicular_cw() : -danger_vel.perpendicular_cw();
		}

		return -danger_dir;
	}

	inline auto danger_avoidance_in_comfort_zone(
		const vec2 victim_pos,
		const vec2 danger_pos,
		const vec2 danger_vel,
		const real32 comfort_zone,
		const real32 avoidance_force
	) {
		const auto avoidance = danger_avoidance(victim_pos, danger_pos, danger_vel);
		const auto danger_dist = (victim_pos - danger_pos).length();

		return vec2(avoidance).set_length(avoidance_force * std::max(0.f, 1.f - (danger_dist / comfort_zone)));
	}

	inline auto steer_to_avoid_bounds(
		const vec2 velocity,
		const vec2 position,
		const xywh& bound,
		const real32 max_dist_to_avoid,
		const real32 force_multiplier
	) {
		const auto edges = bound.make_edges();
		auto dist_to_closest = std::numeric_limits<real32>::max();

		for (const auto& e : edges) {
			const auto dist = position.distance_from_segment_sq(e[0], e[1]);

			if (dist < dist_to_closest) {
				dist_to_closest = dist;
			}
		}

		dist_to_closest = std::sqrt(dist_to_closest);

		/* Finally, correct velocities against the walls */

		auto dir_mult = std::max(0.f, 1.f - dist_to_closest / max_dist_to_avoid) * force_multiplier;

		if (!bound.hover(position)) {
			/* Protect from going outside */
			dir_mult = 1.f;
		}

		return augs::seek(
			velocity,
			position,
			bound.get_center(),
			velocity.length()
		) * dir_mult;
	}

}
