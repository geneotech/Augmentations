#include "tree_of_npo_system.h"
#include "game/transcendental/logic_step.h"
#include "game/transcendental/cosmos.h"
#include "game/transcendental/entity_handle.h"
#include "game/components/tree_of_npo_node_component.h"
#include "game/enums/filters.h"

bool tree_of_npo_system::cache::is_constructed() const {
	return constructed;
}

tree_of_npo_system::cache& tree_of_npo_system::get_cache(const unversioned_entity_id id) {
	return per_entity_cache[linear_cache_key(id)];
}

tree_of_npo_system::tree& tree_of_npo_system::get_tree(const cache& c) {
	return trees[static_cast<size_t>(c.type)];
}

void tree_of_npo_system::destroy_inferred_state_of(const const_entity_handle handle) {
	auto& cache = get_cache(handle.get_id());

	if (cache.is_constructed()) {
		if (cache.tree_proxy_id != -1) {
			get_tree(cache).nodes.DestroyProxy(cache.tree_proxy_id);
		}

		cache = tree_of_npo_system::cache();
	}
}

void tree_of_npo_system::create_inferred_state_for(const const_entity_handle handle) {
	auto& cache = get_cache(handle.get_id());

	ensure(!cache.is_constructed());

	const auto tree_node = handle.find<components::tree_of_npo_node>();

	if (tree_node != nullptr && tree_node.is_activated()) {
		const auto data = tree_node.get_raw_component();

		cache.type = data.type;

		b2AABB input;
		input.lowerBound = b2Vec2(data.aabb.left_top());
		input.upperBound = b2Vec2(data.aabb.right_bottom());
		
		tree_of_npo_node node;
		node.payload = handle.get_id().operator unversioned_entity_id();

		cache.tree_proxy_id = get_tree(cache).nodes.CreateProxy(input, node.bytes);
		
		cache.constructed = true;
	}
}

void tree_of_npo_system::reserve_caches_for_entities(const size_t n) {
	per_entity_cache.resize(n);
}