#include "tile_layer.h"
#include "game/resources/manager.h"
#include "augs/templates/container_templates.h"

namespace resources {
	tile_layer::tile::tile(const unsigned type) : type_id(type) {}
	tile_layer::tile_type::tile_type(const assets::texture_id tile_texture) : tile_texture(tile_texture) {}

	size_t tile_layer::get_tile_side() const {
		return (*tileset[0].tile_texture).get_size().x;
	}

	vec2u tile_layer::get_size() const {
		return size;
	}

	tile_layer::tile& tile_layer::tile_at(const vec2u p) {
		return tiles[p.y * size.x + p.x];
	}

	const tile_layer::tile& tile_layer::tile_at(const vec2u p) const {
		return tiles[p.y * size.x + p.x];
	}
	
	const tile_layer::tile_type& tile_layer::get_tile_type(const tile& t) const {
		return tileset[t.type_id - 1];
	}

	void tile_layer::expand(const vec2u new_area) {
		std::vector<tile> new_tiles;
		new_tiles.resize(new_area.area());

		for (size_t x = 0; x < size.x; ++x) {
			for (size_t y = 0; y < size.y; ++y) {
				new_tiles[y * new_area.x + x] = tiles[y * size.x + x];
			}
		}

		tiles = std::move(new_tiles);
		size = new_area;
	}

	void tile_layer::expand_to_position(const vec2u pos) {
		expand({ std::max(pos.x, size.x), std::max(pos.y, size.y) });
	}

	void tile_layer::expand_to_rect(const ltrbu rc) {
		expand_to_position( rc.right_bottom() );
	}

	void tile_layer::set_tile(const vec2u pos, const assets::texture_id id) {
		expand_to_position(pos);

		tile_at(pos).type_id = get_tile_index_for_texture(id);
	}

	void tile_layer::set_tiles(const ltrbu rc, const assets::texture_id id) {
		expand_to_rect(rc);
		const auto id_index = get_tile_index_for_texture(id);

		for (size_t x = rc.l; x < rc.r; ++x) {
			for (size_t y = rc.t; y < rc.b; ++y) {
				tile_at({ x, y }).type_id = id_index;
			}
		}
	}

	void tile_layer::set_tiles_alternating(const ltrbu rc, const assets::texture_id id_1, const assets::texture_id id_2) {
		expand_to_rect(rc);
		const auto id_1_index = get_tile_index_for_texture(id_1);
		const auto id_2_index = get_tile_index_for_texture(id_2);

		for (size_t x = rc.l; x < rc.r; ++x) {
			if ((x - rc.l) % 2 == 0) {
				for (size_t y = rc.t; y < rc.b; ++y) {
					tile_at({ x, y }).type_id = (y - rc.t) % 2 == 0 ? id_1_index : id_2_index;
				}
			}
			else {
				for (size_t y = rc.t; y < rc.b; ++y) {
					tile_at({ x, y }).type_id = (y - rc.t) % 2 == 1 ? id_1_index : id_2_index;
				}
			}
		}
	}

	void tile_layer::set_tiles(const ltrbu rc, const tile_rectangular_filling id) {
		ensure(rc.w() > 2 && rc.h() > 2)
		
		expand_to_rect(rc);
		
		const unsigned fill = get_tile_index_for_texture(id.fill);

		const unsigned left_border = get_tile_index_for_texture(id.left_border);
		const unsigned top_border = get_tile_index_for_texture(id.top_border);
		const unsigned right_border = get_tile_index_for_texture(id.right_border);
		const unsigned bottom_border = get_tile_index_for_texture(id.bottom_border);

		const unsigned lt_corner = get_tile_index_for_texture(id.lt_corner);
		const unsigned rt_corner = get_tile_index_for_texture(id.rt_corner);
		const unsigned rb_corner = get_tile_index_for_texture(id.rb_corner);
		const unsigned lb_corner = get_tile_index_for_texture(id.lb_corner);

		tile_at(rc.left_top()).type_id = lt_corner;
		tile_at(rc.right_top() - vec2u(1, 0)).type_id = rt_corner;
		tile_at(rc.right_bottom() - vec2u(1, 1)).type_id = rb_corner;
		tile_at(rc.left_bottom() - vec2u(0, 1)).type_id = lb_corner;

		for (size_t x = rc.l + 1; x < rc.r - 1; ++x) {
			tile_at({ x, rc.t }).type_id = top_border;
			tile_at({ x, rc.b - 1 }).type_id = bottom_border;
		}

		for (size_t y = rc.t + 1; y < rc.b - 1; ++y) {
			tile_at({ rc.l, y }).type_id = left_border;
			tile_at({ rc.r - 1, y }).type_id = right_border;
		}

		for (size_t x = rc.l + 1; x < rc.r - 1; ++x) {
			for (size_t y = rc.t + 1; y < rc.b - 1; ++y) {
				tile_at({ x, y }).type_id = fill;
			}
		}
	}

	unsigned tile_layer::get_tile_index_for_texture(const assets::texture_id new_id) {
		const auto it = find_in(tileset, new_id);
		
		if (it == tileset.end()) {
			tileset.push_back(new_id);

			const bool is_square = (*new_id).get_size().x == (*new_id).get_size().y;

			ensure(is_square);

			if (tileset.size() > 1) {
				const bool is_same_size = (*new_id).get_size() == (*tileset[tileset.size() - 2].tile_texture).get_size();

				ensure(is_same_size);
			}

			return tileset.size();
		}
		else {
			return (it - tileset.begin()) + 1;
		}
	}

	tile_layer::visible_tiles_by_type tile_layer::get_visible_tiles_by_type(const ltrbu visible_tiles) const {
		tile_layer::visible_tiles_by_type result;
		auto& indices_by_type = result.tiles_by_type;
		
		for (unsigned y = visible_tiles.t; y < visible_tiles.b; ++y) {
			for (unsigned x = visible_tiles.l; x < visible_tiles.r; ++x) {
				const auto i = y * size.x + x;
				const auto type = tiles[i].type_id;
				
				if (type != 0) {
					if (indices_by_type.size() < type + 1) {
						indices_by_type.resize(type + 1);
					}

					indices_by_type[type].push_back(vec2u(x, y));
				}
			}
		}

		return std::move(result);
	}
}