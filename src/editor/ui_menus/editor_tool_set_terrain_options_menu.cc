/*
 * Copyright (C) 2002-2004, 2006-2011 by the Widelands Development Team
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

#include "editor/ui_menus/editor_tool_set_terrain_options_menu.h"

#include <SDL_keysym.h>

#include "editor/editorinteractive.h"
#include "editor/tools/editor_set_terrain_tool.h"
#include "graphic/graphic.h"
#include "graphic/in_memory_image.h"
#include "graphic/rendertarget.h"
#include "graphic/surface.h"
#include "graphic/texture.h"
#include "i18n.h"
#include "logic/map.h"
#include "logic/world/editor_category.h"
#include "logic/world/terrain_description.h"
#include "logic/world/world.h"
#include "ui_basic/checkbox.h"
#include "ui_basic/panel.h"
#include "ui_basic/tabpanel.h"

namespace {

using namespace Widelands;

static const int32_t check[] = {
   TerrainDescription::GREEN,                                 //  "green"
   TerrainDescription::DRY,                                   //  "dry"
   TerrainDescription::DRY | TerrainDescription::MOUNTAIN,    //  "mountain"
   TerrainDescription::DRY | TerrainDescription::UNPASSABLE,  //  "unpassable"
   TerrainDescription::ACID | TerrainDescription::DRY |
      TerrainDescription::UNPASSABLE,  //  "dead" or "acid"
   TerrainDescription::UNPASSABLE | TerrainDescription::DRY | TerrainDescription::WATER,
   -1,  // end marker
};

UI::Checkbox* create_terrain_checkbox(UI::Panel* parent,
                                      const TerrainDescription& terrain_descr,
                                      std::vector<std::unique_ptr<const Image>>* offscreen_images) {
	const Image* green = g_gr->images().get("pics/terrain_green.png");
	const Image* water = g_gr->images().get("pics/terrain_water.png");
	const Image* mountain = g_gr->images().get("pics/terrain_mountain.png");
	const Image* dead = g_gr->images().get("pics/terrain_dead.png");
	const Image* unpassable = g_gr->images().get("pics/terrain_unpassable.png");
	const Image* dry = g_gr->images().get("pics/terrain_dry.png");

	constexpr int kSmallPicHeight = 20;
	constexpr int kSmallPicWidth = 20;
	for (size_t checkfor = 0; check[checkfor] >= 0; ++checkfor) {
		const TerrainDescription::Type ter_is = terrain_descr.get_is();
		if (ter_is != check[checkfor])
			continue;

		Surface* surf = Surface::create(TEXTURE_WIDTH, TEXTURE_HEIGHT);
		const Image* tex = g_gr->images().get(
		   g_gr->get_maptexture_data(terrain_descr.get_texture())->get_texture_image());
		surf->blit(Point(0, 0), tex->surface(), Rect(0, 0, tex->width(), tex->height()), CM_Solid);
		Point pt(1, TEXTURE_HEIGHT - kSmallPicHeight - 1);

		if (ter_is == TerrainDescription::GREEN) {
			surf->blit(pt, green->surface(), Rect(0, 0, green->width(), green->height()));
			pt.x += kSmallPicWidth + 1;
		} else {
			if (ter_is & TerrainDescription::WATER) {
				surf->blit(pt, water->surface(), Rect(0, 0, water->width(), water->height()));
				pt.x += kSmallPicWidth + 1;
			}
			if (ter_is & TerrainDescription::MOUNTAIN) {
				surf->blit(pt, mountain->surface(), Rect(0, 0, mountain->width(), mountain->height()));
				pt.x += kSmallPicWidth + 1;
			}
			if (ter_is & TerrainDescription::ACID) {
				surf->blit(pt, dead->surface(), Rect(0, 0, dead->width(), dead->height()));
				pt.x += kSmallPicWidth + 1;
			}
			if (ter_is & TerrainDescription::UNPASSABLE) {
				surf->blit(
				   pt, unpassable->surface(), Rect(0, 0, unpassable->width(), unpassable->height()));
				pt.x += kSmallPicWidth + 1;
			}
			if (ter_is & TerrainDescription::DRY)
				surf->blit(pt, dry->surface(), Rect(0, 0, dry->width(), dry->height()));
		}
		// Make sure we delete this later on.
		offscreen_images->emplace_back(new_in_memory_image("dummy_hash", surf));
		break;
	}

	UI::Checkbox* cb = new UI::Checkbox(parent, Point(0, 0), offscreen_images->back().get());
	cb->set_desired_size(TEXTURE_WIDTH + 1, TEXTURE_HEIGHT + 1);
	return cb;
}

}  // namespace

Editor_Tool_Set_Terrain_Options_Menu::Editor_Tool_Set_Terrain_Options_Menu(
   Editor_Interactive& parent, Editor_Set_Terrain_Tool& tool, UI::UniqueWindow::Registry& registry)
   : Editor_Tool_Options_Menu(parent, registry, 0, 0, _("Terrain Select")) {
	const Widelands::World& world = parent.egbase().world();
	multi_select_menu_.reset(
	   new CategorizedItemSelectionMenu<Widelands::TerrainDescription, Editor_Set_Terrain_Tool>(
	      this,
	      world.editor_terrain_categories(),
	      world.terrains(),
	      [this](UI::Panel* cb_parent, const TerrainDescription& terrain_descr) {
		      return create_terrain_checkbox(cb_parent, terrain_descr, &offscreen_images_);
		   },
	      [this] { select_correct_tool(); },
	      &tool));
	set_center_panel(multi_select_menu_.get());
}

Editor_Tool_Set_Terrain_Options_Menu::~Editor_Tool_Set_Terrain_Options_Menu() {
}
