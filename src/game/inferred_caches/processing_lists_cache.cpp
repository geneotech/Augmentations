#include "augs/templates/container_templates.h"
#include "processing_lists_cache.h"
#include "augs/templates/enum_introspect.h"
#include "game/components/force_joint_component.h"

#include "game/transcendental/cosmos.h"
#include "game/transcendental/entity_handle.h"

void processing_lists_cache::destroy_cache_of(const const_entity_handle handle) {
	const auto index = linear_cache_key(handle);

	if (per_entity_cache[index].is_constructed) {
		for (auto& list : lists) {
			erase_element(list, handle.get_id());
		}

		per_entity_cache[index] = cache();
	}
}

void processing_lists_cache::infer_cache_for(const const_entity_handle handle) {
	if (!handle.has<components::processing>()) {
		return;
	}

	const auto index = linear_cache_key(handle);
	
	ensure(!per_entity_cache[index].is_constructed);

	const auto& processing = handle.get<components::processing>();
	
	if (processing.is_activated()) {
		augs::for_each_enum_except_bounds([&](const processing_subjects key) {
			if (processing.is_in(key)) {
				lists[key].push_back(handle.get_id());
			}
		});
			
		per_entity_cache[index].is_constructed = true;
	}
}

void processing_lists_cache::reserve_caches_for_entities(size_t n) {
	per_entity_cache.resize(n);
}

const std::vector<entity_id>& processing_lists_cache::get(const processing_subjects list) const {
	return lists[list];
}