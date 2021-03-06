#pragma once
#include "augs/templates/identity_templates.h"

#include "augs/templates/introspection_utils/on_dynamic_content.h"
#include "augs/templates/introspection_utils/field_name_tracker.h"
#include "augs/templates/introspection_utils/types_in.h"

template <bool allow_conversion>
struct detail_same_or_convertible;

template <>
struct detail_same_or_convertible<false> {
	template <class A, class B>
	static constexpr bool value = can_type_contain_v<A, B>;
};

template <>
struct detail_same_or_convertible<true> {
	template <class A, class B>
	static constexpr bool value = can_type_contain_constructible_from_v<A, B>;
};

template <
	template <class> class IgnorePredicate, 
	bool allow_conversion
>
class object_in_object {
	template <
		class Searched, 
		class Candidate, 
		class F,
		class... Args
	>
	static void find_detail(
		augs::field_name_tracker& fields,
		const Searched& searched_object,
		const Candidate& field,
		F&& location_callback,
		Args... label
	) {
		using contains = detail_same_or_convertible<allow_conversion>;
		static constexpr bool has_label = sizeof...(Args) > 0;

		((void)label, ...); (void)field; (void)location_callback; (void)has_label;

		if constexpr(contains::template value<Candidate, Searched>) {
			if constexpr(IgnorePredicate<Candidate>::value) {
				/* Apparently, this has a special logic of finding */
			}
			else if constexpr(std::is_same_v<Candidate, Searched>) {
				if (searched_object == field) {
					location_callback(fields.get_full_name(label...));
				}
			}
			else if constexpr(allow_conversion && std::is_constructible_v<Candidate, Searched>) {
				if (Candidate(searched_object) == field) {
					location_callback(fields.get_full_name(label...));
				}
			}
			else if constexpr(is_introspective_leaf_v<Candidate>) {
				return;
			}
			else if constexpr(augs::has_dynamic_content_v<Candidate>) {
				augs::on_dynamic_content(
					[&](auto& dyn, auto... args) {
						if constexpr(has_label) {
							fields.track_no_scope(args...);
						}

						find_detail(
							fields, searched_object, dyn, std::forward<F>(location_callback)
						);

						if constexpr(has_label) {
							fields.pop();
						}
					},
					field
				);
			}
			else {
				if constexpr(has_label) {
					fields.track_no_scope(label...);
				}

				augs::introspect(
					[&](const auto& label, auto& member) {
						find_detail(
							fields, searched_object, member, std::forward<F>(location_callback), label
						);
					},
					field
				);

				if constexpr(has_label) {
					fields.pop();
				}
			}
		}
	}

public:
	template <
		class Searched, 
		class O, 
		class F
	>
	static void find(
		const Searched& searched_object,
		const O& in_object,
		F&& location_callback
	) {
		using contains = detail_same_or_convertible<allow_conversion>;
		static_assert(contains::template value<O, Searched>, "This search will never find anything.");

		thread_local augs::field_name_tracker fields;
		fields.clear();

		augs::introspect(
			[&](const auto& label, const auto& member) {
				find_detail(
					fields, searched_object, member, std::forward<F>(location_callback), label
				);
			},
			in_object
		);
	}
};

