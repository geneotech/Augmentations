#pragma once
#include <type_traits>

#include "game/detail/inventory_slot_handle_declaration.h"
#include "game/entity_handle_declaration.h"

#include "entity_system/component_aggregate.h"
#include "entity_system/aggregate_mixins.h"
#include "misc/object_pool_handle.h"
#include "game/entity_id.h"

#include "game/components/processing_component.h"
#include "game/detail/entity/inventory_getters.h"
#include "game/detail/entity/relations_component_helpers.h"
#include "game/detail/entity/physics_getters.h"

class cosmos;

namespace augs {
	template <bool is_const>
	class basic_handle<is_const, cosmos, put_all_components_into<component_aggregate>::type> :
		public basic_handle_base<is_const, cosmos, put_all_components_into<component_aggregate>::type>,

		private augs::component_allocators<is_const, basic_entity_handle<is_const>>,
		public augs::component_setters<is_const, basic_entity_handle<is_const>>,
		public inventory_getters<is_const>,
		public physics_getters<is_const>,
		public relations_component_helpers<is_const>
	{
		typedef augs::component_allocators<is_const, basic_entity_handle<is_const>> allocator;

		friend class augs::component_allocators<is_const, basic_entity_handle<is_const>>;

		template <class T, typename = void>
		struct component_or_synchronizer {
			basic_entity_handle<is_const> h;

			decltype(auto) get() const {
				return h.allocator::get<T>();
			}

			decltype(auto) add(const T& t) const {
				return h.allocator::add(t);
			}

			decltype(auto) remove() const {
				return h.allocator::remove<T>();
			}
		};

		template <class T>
		struct component_or_synchronizer<T, typename std::enable_if<is_component_synchronized<T>::value>::type> {
			basic_entity_handle<is_const> h;

			auto get() const {
				return component_synchronizer<is_const, T>(h.allocator::get<T>(), h);
			}

			auto add(const T& t) const {
				ensure(!h.has<T>());

				return component_synchronizer<is_const, T>(h.allocator::add(t), h);
			}

			void remove() const {
				ensure(h.has<T>());
				component_synchronizer<is_const, T> sync(h.allocator::get<T>(), h);


				h.allocator::remove<T>();
			}
		};

		using basic_handle_base::get;

	public:
		using basic_handle_base::basic_handle_base;

		basic_entity_handle<is_const> make_handle(entity_id) const;

		decltype(auto) get_cosmos() const {
			return owner;
		}

		bool operator==(entity_id b) const;
		bool operator!=(entity_id b) const;

		template <class = typename std::enable_if<!is_const>::type>
		operator basic_entity_handle<true>() const;
		operator entity_id() const;

		template <class component>
		bool has() const {
			return allocator::has<component>();
		}

		template<class component>
		decltype(auto) get() const {
			return component_or_synchronizer<component>({ *this }).get();
		}

		template<class component>
		typename std::enable_if<!is_const, component_or_synchronizer<component>>::type add(const component& c = component()) const {
			return component_or_synchronizer<component>({ *this }).add(c);
		}

		template<class component>
		typename std::enable_if<!is_const, component_or_synchronizer<component>>::type add(const component_or_synchronizer<component>& c = component()) const {
			return component_or_synchronizer<component>({ *this }).add(c.get_data());
		}

		template<class component>
		decltype(auto) find() const {
			static_assert(!is_component_synchronized<component>::value, "Cannot return a pointer to synchronized component!");
			return allocator::find<component>();
		}

		template<class component>
		typename std::enable_if<!is_const, void>::type remove() const {
			return component_or_synchronizer<component>({ *this }).remove();
		}

		template<>
		typename std::enable_if<!is_const, component_or_synchronizer<components::substance>>::type
			add(const components::substance& c = components::substance()) const {
			auto result = component_or_synchronizer<components::substance>({ *this }).add(c);
			get_cosmos().complete_resubstantialization(*this);
			return result;
		}

		template<>
		typename std::enable_if<!is_const, void>::type remove<components::substance>() const {
			allocator::remove<components::substance>();
			get_cosmos().complete_resubstantialization(*this);
		}

		template<class = typename std::enable_if<!is_const>::type>
		void add_standard_components();
	};
}

template <bool is_const>
std::vector<entity_id> to_id_vector(std::vector<basic_entity_handle<is_const>> vec) {
	return std::vector<entity_id>(vec.begin(), vec.end());
}