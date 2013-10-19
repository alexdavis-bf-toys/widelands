/*
 * Copyright (C) 2006-2010 by the Widelands Development Team
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

#include "scripting/lua_globals.h"

#include "build_info.h"
#include "i18n.h"
#include "logic/game.h"
#include "scripting/c_utils.h"
#include "scripting/scripting.h"


namespace LuaGlobals {

/* RST
Global functions
======================

The following functions are imported into the global namespace
of all scripts that are running inside widelands. They provide convenient
access to other scripts in other locations, localisation features and more.

*/

/*
 * ========================================================================
 *                         MODULE CLASSES
 * ========================================================================
 */


/*
 * ========================================================================
 *                         MODULE FUNCTIONS
 * ========================================================================
 */
/* RST
	.. function:: set_textdomain(domain)

		Sets the textdomain for all further calls to :func:`_`.

		:arg domain: The textdomain
		:type domain: :class:`string`
		:returns: :const:`nil`
*/
static int L_set_textdomain(lua_State * L) {
	luaL_checkstring(L, -1);
	lua_setglobal(L, "__TEXTDOMAIN");
	return 0;
}

/* RST
	.. function:: _(str)

		This peculiar function is used to translate texts in your scenario into
		another language. The function takes a single string, grabs the
		textdomain of your map (which is usually the maps name) and returns the
		translated string. Make sure that you separate translatable and untranslatable
		stuff:

		.. code-block:: lua

			s = "<p><br>" .. _ "Only this should be translated" .. "<br></p>"

		:arg str: text to translate.
		:type str: :class:`string`
		:returns: :const:`nil`
*/
static int L__(lua_State * L) {
	lua_getglobal(L, "__TEXTDOMAIN");

	if (not lua_isnil(L, -1)) {
		i18n::Textdomain dom(luaL_checkstring(L, -1));
		lua_pushstring(L, i18n::translate(luaL_checkstring(L, 1)));
	} else {
		lua_pushstring(L, i18n::translate(luaL_checkstring(L, 1)));
	}
	return 1;
}

/* RST
	.. function:: use(ns, script)

		Includes the script referenced at the caller location. Use this
		to factor your scripts into smaller parts.

		:arg ns:
			The namespace were the imported script resides. Can be any of
				:const:`map`
					The script is in the ``scripting/`` directory of the current map.
				:const:`aux`
					The script is one of the auxiliary scripts that come bundled in
					the ``scripting/`` directory of Widelands itself.

		:type ns: :class:`string`
		:arg script: The filename of the string without the extension ``.lua``.
		:type script: :class:`string`
		:returns: :const:`nil`
*/
static int L_use(lua_State * L) {
	const char * ns = luaL_checkstring(L, -2);
	const char * script = luaL_checkstring(L, -1);

	// remove our argument so that the executed script gets a clear stack
	lua_pop(L, 2);

	try {
		lua_getfield(L, LUA_REGISTRYINDEX, "lua_interface");
		LuaInterface * lua = static_cast<LuaInterface *>(lua_touserdata(L, -1));
		lua_pop(L, 1); // pop this userdata

		lua->run_script(ns, script);
	} catch (LuaError & e) {
		report_error(L, "%s", e.what());
	}
	return 0;
}

/* RST
.. function:: get_build_id()

	returns the version string of this widelands executable.  Something like
	"build-16[debug]".
*/
static int L_get_build_id(lua_State * L) {
	lua_pushstring(L, build_id());
	return 1;
}

const static struct luaL_Reg globals [] = {
	{"set_textdomain", &L_set_textdomain},
	{"use", &L_use},
	{"get_build_id", &L_get_build_id},
	{"_", &L__},
	{0, 0}
};

void luaopen_globals(lua_State * L) {
	lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
	luaL_setfuncs(L, globals, 0);
	lua_pop(L, 1);
}


};
