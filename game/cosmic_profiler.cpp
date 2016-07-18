#include "cosmic_profiler.h"
#include <algorithm>

cosmic_profiler::cosmic_profiler() {
	meters[(int)meter_type::LOGIC].title = L"Logic";
	meters[(int)meter_type::RENDERING].title = L"Rendering";
	meters[(int)meter_type::CAMERA_QUERY].title = L"Camera query";
	meters[(int)meter_type::INTERPOLATION].title = L"Interpolation";
	meters[(int)meter_type::PHYSICS].title = L"Physics";
	meters[(int)meter_type::VISIBILITY].title = L"Visibility";
	meters[(int)meter_type::AI].title = L"AI";
	meters[(int)meter_type::PATHFINDING].title = L"Pathfinding";
	meters[(int)meter_type::PARTICLES].title = L"Particles";

	set_count_of_tracked_measurements(20);
}

void cosmic_profiler::set_count_of_tracked_measurements(size_t count) {
	for (auto& m : meters) {
		m.tracked.resize(count, 0);
	}
}

std::wstring cosmic_profiler::sorted_summary() const {
	std::vector<augs::measurements> sorted_meters;

	for (auto& m : meters)
		if (m.was_measured())
			sorted_meters.push_back(m);

	std::sort(sorted_meters.begin(), sorted_meters.end());
	std::reverse(sorted_meters.begin(), sorted_meters.end());

	std::wstring summary = raycasts.summary();

	summary += entropy_length.summary();

	for (auto& m : sorted_meters)
		summary += m.summary();

	return summary;
}

void cosmic_profiler::start(meter_type m) {
	meters[(int)m].new_measurement();
}

void cosmic_profiler::stop(meter_type m) {
	meters[(int)m].end_measurement();
}