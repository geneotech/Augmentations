#pragma once

#include "augs/math/vec2.h"

#include "augs/graphics/pixel.h"
#include "augs/graphics/vertex.h"
#include "game/assets/texture_id.h"
#include "transform_component.h"

namespace augs {
	class texture;
}

namespace components {
	struct polygon {
		enum uv_mapping_mode {
			OVERLAY,
			STRETCH
		};

		struct drawing_input : vertex_triangle_buffer_reference {
			using vertex_triangle_buffer_reference::vertex_triangle_buffer_reference;

			components::transform renderable_transform;
			components::transform camera_transform;
			vec2 visible_world_area;

			augs::rgba colorize = augs::white;
		};

		void automatically_map_uv(assets::texture_id, unsigned uv_mapping_mode);

		/* the polygon as it was originally, so possibly concave
		it is later triangulated for rendering and divided into convex polygons for physics */
		std::vector<vec2> original_polygon;

		/* triangulated version of original_polygon, ready to be rendered triangle-by-triangle */
		std::vector<vertex> triangulated_polygon;

		/* indices used in glDrawElements */
		std::vector<int> indices;

		/* construct a set of convex polygons from a potentially concave polygon */
		void add_polygon_vertices(std::vector<vertex>);

		void set_color(rgba col);

		int get_vertex_count() const {
			return triangulated_polygon.size();
		}

		vertex& get_vertex(int i) {
			return triangulated_polygon[i];
		}

		void draw(const drawing_input&) const;

		std::vector<vec2> get_vertices() const;
		rects::ltrb<float> get_aabb(components::transform) const;
	};
}