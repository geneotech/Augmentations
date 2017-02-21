#include "augs/math/vec2.h"
#include "movement_system.h"
#include "game/transcendental/cosmos.h"
#include "game/messages/intent_message.h"
#include "game/messages/movement_response.h"
#include "augs/log.h"

#include "game/components/gun_component.h"

#include "game/components/physics_component.h"
#include "game/components/movement_component.h"
#include "game/components/sentience_component.h"

#include "game/transcendental/entity_handle.h"
#include "game/transcendental/logic_step.h"

#include "game/systems_stateless/sentience_system.h"

#include "game/detail/physics/physics_scripts.h"

using namespace augs;

void movement_system::set_movement_flags_from_input(const logic_step step) {
	auto& cosmos = step.cosm;
	const auto& delta = step.get_delta();
	const auto& events = step.transient.messages.get_queue<messages::intent_message>();

	for (const auto& it : events) {
		auto* const movement = cosmos[it.subject].find<components::movement>();
		
		if (movement == nullptr) {
			continue;
		}

		switch (it.intent) {
		case intent_type::MOVE_FORWARD:
			movement->moving_forward = it.is_pressed;
			break;
		case intent_type::MOVE_BACKWARD:
			movement->moving_backward = it.is_pressed;
			break;
		case intent_type::MOVE_LEFT:
			movement->moving_left = it.is_pressed;
			break;
		case intent_type::MOVE_RIGHT:
			movement->moving_right = it.is_pressed;
			break;
		case intent_type::WALK:
			movement->walking_enabled = it.is_pressed;
			break;
		case intent_type::SPRINT:
			movement->sprint_enabled = it.is_pressed;
			break;
		default: break;
		}
	}
}

void movement_system::apply_movement_forces(cosmos& cosmos) {
	auto& physics_sys = cosmos.systems_temporary.get<physics_system>();
	const auto& delta = cosmos.get_fixed_delta();

	for (const auto& it : cosmos.get(processing_subjects::WITH_MOVEMENT)) {
		auto& movement = it.get<components::movement>();

		const auto& physics = it.get<components::physics>();

		if (!physics.is_constructed()) {
			continue;
		}

		auto movement_force_mult = 1.f;

		bool is_sprint_effective = movement.sprint_enabled;

		auto* const sentience = it.find<components::sentience>();
		const bool is_sentient = sentience != nullptr;

		if (is_sentient) {
			if (sentience->consciousness.value <= 0.f) {
				is_sprint_effective = false;
			}

			if (sentience->haste.is_enabled(cosmos.get_timestamp(), cosmos.get_fixed_delta())) {
				if (sentience->haste.is_greater) {
					movement_force_mult *= 1.45f;
				}
				else {
					movement_force_mult *= 1.3f;
				}
			}
		}

		const bool is_inert = movement.make_inert_for_ms > 0.f;

		if (is_inert) {
			movement.make_inert_for_ms -= static_cast<float>(delta.in_milliseconds());
		}

		const auto requested_by_input = movement.get_force_requested_by_input();

		if (requested_by_input.non_zero()) {
			if (is_sprint_effective) {
				movement_force_mult /= 2.f;

				if (is_sentient) {
					sentience->consciousness.value -= sentience->consciousness.calculate_damage_result(2 * delta.in_seconds()).effective;
				}
			}

			if (movement.walking_enabled) {
				movement_force_mult /= 2.f;
			}

			if (is_inert) {
				movement_force_mult /= 10.f;
			}

			if (is_sentient) {
				sentience->time_of_last_exertion = cosmos.get_timestamp();
			}

			auto applied_force = requested_by_input;

			if (movement.acceleration_length > 0) {
				applied_force.set_length(movement.acceleration_length);
			}

			applied_force *= movement_force_mult;
			applied_force *= physics.get_mass();

			physics.apply_force(
				applied_force, 
				movement.applied_force_offset
			);
		}
		
		resolve_dampings_of_body(it);
	}
}

void movement_system::generate_movement_responses(const logic_step step) {
	auto& cosmos = step.cosm;
	const auto& delta = step.get_delta();
	step.transient.messages.get_queue<messages::movement_response>().clear();

	for (const auto& it : cosmos.get(processing_subjects::WITH_MOVEMENT)) {
		const auto& movement = it.get<components::movement>();

		float32 speed = 0.0f;

		if (movement.enable_animation) {
			if (it.has<components::physics>()) {
				speed = it.get<components::physics>().velocity().length();
			}
		}

		messages::movement_response msg;

		if (movement.max_speed_for_movement_response == 0.f) msg.speed = 0.f;
		else msg.speed = speed / movement.max_speed_for_movement_response;
		
		for (const auto receiver : movement.response_receivers) {
			messages::movement_response copy(msg);
			copy.stop_response_at_zero_speed = receiver.stop_response_at_zero_speed;
			copy.subject = receiver.target;
			step.transient.messages.post(copy);
		}
	}
}
