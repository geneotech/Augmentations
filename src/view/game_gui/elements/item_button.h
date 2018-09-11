#pragma once
#include "augs/pad_bytes.h"

#include "augs/gui/rect.h"
#include "augs/gui/appearance_detector.h"

#include "game/cosmos/entity_handle.h"
#include "game/detail/inventory/inventory_slot_id.h"

#include "view/viewables/all_viewables_declaration.h"
#include "view/game_gui/game_gui_context.h"

struct item_button : game_gui_rect_node {
	using base = game_gui_rect_node;
	using gui_entropy = base::gui_entropy;
	
	using this_in_item = dereferenced_location<item_button_in_item>;
	using const_this_in_item = const_dereferenced_location<item_button_in_item>;

	augs::gui::appearance_detector detector;

	bool is_container_open = false;
	bool started_drag = false;
	pad_bytes<2> pad;

	vec2i drag_offset_in_item_deposit;

	static ltrb calc_button_layout(
		const const_entity_handle component_owner,
		const image_definitions_map&,
		const bool include_attachments = true
	);

	struct drawing_settings {
		bool draw_background = false;
		bool draw_item = false;
		bool draw_attachments_even_if_open = false;
		bool draw_border = false;
		bool draw_connector = false;
		bool decrease_alpha = false;
		bool decrease_border_alpha = false;
		bool draw_container_opened_mark = false;
		bool draw_charges = true;
		bool always_draw_charges_as_closed = false;
		bool expand_size_to_grid = true;
		bool always_full_item_alpha = false;
		std::optional<vec2> overridden_charges_pos;
		vec2 absolute_xy_offset;
	};

	item_button(xywh rc = xywh());

	template <class C, class gui_element_id, class L>
	static void for_each_child(const C context, const gui_element_id this_id, L generic_call) {
		const auto& cosmos = context.get_cosmos();
		const auto container_entity = cosmos[this_id.get_location().item_id];

		if (const auto container = container_entity.template find<invariants::container>()) {
			for (const auto& s : container->slots) {
				{
					slot_button_in_container child_slot_location;
					child_slot_location.slot_id.type = s.first;
					child_slot_location.slot_id.container_entity = container_entity;
					generic_call(context.dereference_location(child_slot_location));
				}

				for (const auto in : ::get_items_inside(container_entity, s.first)) {
					item_button_in_item child_item_location;
					child_item_location.item_id = in;
					generic_call(context.dereference_location(child_item_location));
				}
			}
		}
	}

	static vec2 griddify_size(const vec2 size, const vec2 expander);

	static bool is_being_wholely_dragged_or_pending_finish(const const_game_gui_context, const const_this_in_item this_id);

	static void respond_to_events(const game_gui_context, const this_in_item this_id, const gui_entropy& entropies);
	static void rebuild_layouts(const game_gui_context, const this_in_item this_id);

	static bool is_inventory_root(const const_game_gui_context, const const_this_in_item this_id);
	
	static void draw(
		const viewing_game_gui_context, 
		const const_this_in_item this_id
	);

	static void draw_proc(
		const viewing_game_gui_context, 
		const const_this_in_item, 
		const drawing_settings&
	);

	static void draw_dragged_ghost_inside(
		const viewing_game_gui_context context, 
		const const_this_in_item this_id, 
		const vec2 absolute_xy_offset
	);

	static void draw_grid_border_ghost(
		const viewing_game_gui_context, 
		const const_this_in_item, 
		const vec2 absolute_xy_offset
	);

	static void draw_complete_dragged_ghost(
		const viewing_game_gui_context, 
		const const_this_in_item, 
		const vec2 absolute_xy_offset
	);

	static void draw_complete_with_children(
		const viewing_game_gui_context, 
		const const_this_in_item this_id
	);
};