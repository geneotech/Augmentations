#pragma once
#include <iosfwd>
#include "augs/templates/hash_templates.h"

namespace augs {
	struct raw_pool_id_data {
		// GEN INTROSPECTOR struct augs::raw_pool_id_data
		unsigned version;
		int indirection_index;
		// END GEN INTROSPECTOR
	};

	class raw_pool_id {
	public:
		// GEN INTROSPECTOR class augs::raw_pool_id
		raw_pool_id_data pool;
		// END GEN INTROSPECTOR

		raw_pool_id();

		void unset();

		bool operator==(const raw_pool_id& b) const;
		bool operator!=(const raw_pool_id& b) const;

		friend std::ostream& operator<<(std::ostream& out, const raw_pool_id &x);
	};

	template<class T>
	struct unversioned_id {
		struct {
			int indirection_index = -1;
		} pool;

		template<class B>
		bool operator==(const B& b) const {
			return pool.indirection_index == b.pool.indirection_index;
		}

		template<class B>
		bool operator!=(const B& b) const {
			return pool.indirection_index != b.pool.indirection_index;
		}
	};

	template <class T>
	class pool_id : public raw_pool_id {
	public:
		// GEN INTROSPECTOR class augs::pool_id class T
		// INTROSPECT BASE augs::raw_pool_id
		// END GEN INTROSPECTOR

		typedef T element_type;

		operator unversioned_id<T>() const {
			unversioned_id<T> un;
			un.pool.indirection_index = pool.indirection_index;
			return un;
		}

		using raw_pool_id::raw_pool_id;
		using raw_pool_id::operator==;
		using raw_pool_id::operator!=;
	};

	template<class T>
	struct make_pool_id { 
		typedef pool_id<T> type; 
	};
}

namespace std {
	template <class T>
	struct hash<augs::pool_id<T>> {
		std::size_t operator()(const augs::pool_id<T>& k) const {
			return augs::simple_two_hash(k.pool.indirection_index, k.pool.version);
		}
	};

	template <class T>
	struct hash<augs::unversioned_id<T>> {
		std::size_t operator()(const augs::unversioned_id<T> k) const {
			return std::hash<int>()(k.pool.indirection_index);
		}
	};
}