#pragma once
#include "application/setups/editor/editor_command_structs.h"
#include "application/setups/editor/property_editor/property_editor_structs.h"

struct editor_all_entities_gui {
	// GEN INTROSPECTOR struct editor_all_entities_gui
	bool show = false;
	// END GEN INTROSPECTOR

	entity_guid hovered_guid;

	void open();
	void perform(editor_command_input);
	void interrupt_tweakers();

private:
	property_editor_gui properties_gui;
	bool acquire_once = false;
};