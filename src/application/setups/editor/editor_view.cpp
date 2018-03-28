#include "application/setups/editor/editor_view.h"

void editor_view::toggle_grid() {
	auto& f = show_grid;
	f = !f;
}

void editor_view::reset_zoom_at(vec2 pos) {
	if (panned_camera) {
		panned_camera->zoom = 1.f;
		panned_camera->transform.pos = pos.discard_fract();
	}
}

void editor_view::toggle_flavour_rect_selection() {
	switch (rect_select_mode) {
		case editor_rect_select_type::SAME_FLAVOUR: 
			rect_select_mode = editor_rect_select_type::EVERYTHING; 
		break;

		default: 
			rect_select_mode = editor_rect_select_type::SAME_FLAVOUR;
		break;
	}
}