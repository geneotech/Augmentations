#pragma once
#include <type_traits>
#include "augs/misc/pool/pooled_object_id.h"
#include "augs/build_settings/platform_defines.h"

#include "game/cosmos/entity_id_declaration.h"

#include "game/organization/all_entity_types.h"

struct entity_guid {
	using guid_value_type = unsigned;
	// GEN INTROSPECTOR struct entity_guid
	guid_value_type value = 0u;
	// END GEN INTROSPECTOR

	static auto dead() {
		return entity_guid(0);
	}

	static auto first() {
		return entity_guid(1);
	}

	entity_guid() = default;
	entity_guid(const guid_value_type b) : value(b) {}
	
	entity_guid& operator=(const entity_guid& b) = default;

	bool operator==(const entity_guid& b) const {
		return value == b.value;
	}

	bool operator!=(const entity_guid& b) const {
		return value != b.value;
	}

	operator guid_value_type() const {
		return value;
	}

	bool is_set() const {
		return *this != entity_guid();
	}

	void unset() {
		*this = {};
	}
};

struct unversioned_entity_id {
	// GEN INTROSPECTOR struct unversioned_entity_id
	unversioned_entity_id_base raw;
	entity_type_id type_id;
	// END GEN INTROSPECTOR

	unversioned_entity_id() = default;

	unversioned_entity_id(
		const unversioned_entity_id_base id,
		const entity_type_id type_id
	) : 
		raw(id),
		type_id(type_id)
	{}

	bool is_set() const {
		return raw.is_set() && type_id.is_set();
	}

	void unset() {
		*this = unversioned_entity_id();
	}

	bool operator==(const unversioned_entity_id b) const {
		return type_id == b.type_id && raw == b.raw;
	}

	bool operator!=(const unversioned_entity_id b) const {
		return !operator==(b);
	}
};

struct child_entity_id;

struct entity_id {
	// GEN INTROSPECTOR struct entity_id
	entity_id_base raw;
	entity_type_id type_id;
	// END GEN INTROSPECTOR

	entity_id() = default;

	entity_id(
		const entity_id_base id,
		entity_type_id type_id
	) : 
		raw(id),
		type_id(type_id)
	{}

	entity_id(
		const child_entity_id& id
	);

	bool operator==(const entity_id b) const {
		return type_id == b.type_id && raw == b.raw;
	}

	bool operator!=(const entity_id b) const {
		return !operator==(b);
	}

	void unset() {
		*this = entity_id();
	}

	bool is_set() const {
		return raw.is_set() && type_id.is_set();
	}

	unversioned_entity_id to_unversioned() const {
		return { raw, type_id };
	}

	operator unversioned_entity_id() const {
		return { raw, type_id };
	}

	friend std::ostream& operator<<(std::ostream& out, const entity_id x);
}; 

template <class E>
struct typed_entity_id {
	using raw_type = entity_id_base;

	// GEN INTROSPECTOR struct typed_entity_id class E
	raw_type raw;
	// END GEN INTROSPECTOR

	typed_entity_id() = default;
	typed_entity_id(const entity_id b) = delete;
	explicit typed_entity_id(const raw_type b) : raw(b) {}

	operator entity_id() const {
		return { raw, entity_type_id::of<E>() };
	}

	unversioned_entity_id to_unversioned() const {
		return { raw, entity_type_id::of<E>() };
	}

	bool operator==(const typed_entity_id<E> b) const {
		return raw == b.raw;
	}

	bool operator!=(const typed_entity_id<E> b) const {
		return !operator==(b);
	}
}; 

struct child_entity_id : entity_id {
	using base = entity_id;
	using introspect_base = base;

	child_entity_id(const entity_id id = entity_id()) : entity_id(id) {}
	using base::operator unversioned_entity_id;
};

inline entity_id::entity_id(
	const child_entity_id& id
) : entity_id(static_cast<const entity_id&>(id)) {}

namespace std {
	template <>
	struct hash<entity_guid> {
		std::size_t operator()(const entity_guid v) const {
			return hash<entity_guid::guid_value_type>()(v.value);
		}
	};

	template <>
	struct hash<entity_id> {
		std::size_t operator()(const entity_id v) const {
			return augs::simple_two_hash(v.raw, v.type_id);
		}
	};

	template <class E>
	struct hash<typed_entity_id<E>> {
		std::size_t operator()(const typed_entity_id<E> v) const {
			return hash<entity_id_base>()(v.raw);
		}
	};

	template <>
	struct hash<unversioned_entity_id> {
		std::size_t operator()(const unversioned_entity_id v) const {
			return augs::simple_two_hash(v.raw, v.type_id);
		}
	};
}