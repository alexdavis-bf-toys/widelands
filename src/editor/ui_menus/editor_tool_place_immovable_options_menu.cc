/*
 * Copyright (C) 2002-2004, 2006-2008, 2010 by the Widelands Development Team
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "editor/ui_menus/editor_tool_place_immovable_options_menu.h"

#include <SDL_keysym.h>

#include "editor/editorinteractive.h"
#include "editor/tools/editor_place_immovable_tool.h"
#include "graphic/graphic.h"
#include "i18n.h"
#include "logic/map.h"
#include "logic/world/world.h"
#include "ui_basic/box.h"
#include "ui_basic/button.h"
#include "ui_basic/checkbox.h"
#include "ui_basic/textarea.h"
#include "wlapplication.h"


namespace {

using namespace Widelands;

UI::Checkbox* create_immovable_checkbox(UI::Panel* parent, const Immovable_Descr& immovable_descr) {
	const Image& pic =
	   g_gr->animations().get_animation(immovable_descr.main_animation()).representative_image(
	      RGBColor(0, 0, 0));
	UI::Checkbox* cb = new UI::Checkbox(parent, Point(0, 0), &pic, immovable_descr.descname());
	const int kMinClickableArea = 24;
	cb->set_desired_size(std::max<int>(pic.width(), kMinClickableArea),
	                     std::max<int>(pic.height(), kMinClickableArea));
	return cb;
}

}  // namespace

Editor_Tool_Place_Immovable_Options_Menu::Editor_Tool_Place_Immovable_Options_Menu(
   Editor_Interactive& parent,
   Editor_Place_Immovable_Tool& tool,
   UI::UniqueWindow::Registry& registry)
   : Editor_Tool_Options_Menu(parent, registry, 0, 0, _("Immovable Select")) {
	const Widelands::World& world = parent.egbase().world();
	multi_select_menu_.reset(
	   new CategorizedItemSelectionMenu<Widelands::Immovable_Descr, Editor_Place_Immovable_Tool>(
	      this,
	      world.editor_immovable_categories(),
	      world.immovables(),
	      [this](UI::Panel* cb_parent, const Immovable_Descr& immovable_descr) {
		      return create_immovable_checkbox(cb_parent, immovable_descr);
		   },
	      [this] {select_correct_tool();},
	      &tool));
	set_center_panel(multi_select_menu_.get());
}

Editor_Tool_Place_Immovable_Options_Menu::~Editor_Tool_Place_Immovable_Options_Menu() {
}
