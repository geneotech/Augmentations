#include "crosshair_system.h"
#include "game/cosmos/cosmos.h"

#include "game/messages/intent_message.h"
#include "game/messages/motion_message.h"

#include "game/components/sprite_component.h"

#include "game/components/crosshair_component.h"
#include "game/components/transform_component.h"
#include "game/messages/intent_message.h"

#include "game/cosmos/entity_handle.h"
#include "game/cosmos/logic_step.h"
#include "game/cosmos/data_living_one_step.h"
#include "game/cosmos/for_each_entity.h"

void crosshair_system::handle_crosshair_intents(const logic_step step) {
	auto& cosm = step.get_cosmos();

	const auto& events = step.get_queue<messages::intent_message>();

	for (const auto& it : events) {
		const auto subject = cosm[it.subject];
		
		if (const auto crosshair = subject.find_crosshair()) {
			if (it.intent == game_intent_type::SWITCH_LOOK && it.was_pressed()) {
				auto& mode = crosshair->orbit_mode;

				if (mode == crosshair_orbit_type::LOOK) {
					mode = crosshair_orbit_type::ANGLED;
				}
				else {
					mode = crosshair_orbit_type::LOOK;
				}
			}
		}
	}
}

void crosshair_system::update_base_offsets(const logic_step step) {
	auto& cosm = step.get_cosmos();

	const auto& events = step.get_queue<messages::motion_message>();
	
	for (const auto& motion : events) {
		const auto subject = cosm[motion.subject];

		if (const auto crosshair = subject.find_crosshair()) {
			const auto delta = vec2(motion.offset) * crosshair->sensitivity;

			auto& base_offset = crosshair->base_offset;
			base_offset += delta;
		}
	}
}

void crosshair_system::integrate_crosshair_recoils(const logic_step step) {
	const auto delta = step.get_delta();
	const auto secs = delta.in_seconds();
	auto& cosm = step.get_cosmos();

	cosm.for_each_having<components::crosshair>(
		[&](const auto subject) {
			auto& crosshair = subject.template get<components::crosshair>();
			auto& crosshair_def = subject.template get<invariants::crosshair>();
			auto& recoil = crosshair.recoil;

			recoil.integrate(secs);
			recoil.damp(secs, crosshair_def.recoil_damping);

			recoil.position.damp(secs, vec2::square(60.f));
			recoil.rotation = augs::damp(recoil.rotation, secs, 60.f);
		}
	);
}