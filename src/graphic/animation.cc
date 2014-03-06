/*
 * Copyright (C) 2002, 2006-2010 by the Widelands Development Team
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

#include "graphic/animation.h"

#include <cstdio>

#include "constants.h"
#include "graphic/diranimations.h"
#include "helper.h"
#include "i18n.h"
#include "logic/bob.h"
#include "profile/profile.h"
#include "sound/sound_handler.h"
#include "wexception.h"

/// Find out if there is a sound effect registered for the animation's frame
/// and try to play it. This is used to have sound effects that are tightly
/// synchronized to an animation, for example when a geologist is shown
/// hammering on rocks.
///
/// \par framenumber  The framenumber currently on display.
///
/// \note uint32_t animation is an ID number that starts at 1, not a vector
///       index that starts at 0 !
///
/// \sa RenderTarget::drawanim
void AnimationData::trigger_soundfx
	(uint32_t framenumber, uint32_t stereo_position) const
{
	std::map<uint32_t, std::string>::const_iterator const sfx_cue =
		sfx_cues.find(framenumber);
	if (sfx_cue != sfx_cues.end())
		g_sound_handler.play_fx(sfx_cue->second, stereo_position, 1);
}


/*
==============================================================================

AnimationManager IMPLEMENTATION

==============================================================================
*/

AnimationManager g_anim;

/*
===============
Remove all animations
===============
*/
void AnimationManager::flush()
{
	m_animations.clear();
}

/**
 * Read in basic information about the animation.
 * The graphics are loaded later by the graphics subsystem.
 *
 * Read in sound effects associated with the animation as well as the
 * framenumber on which the effect should be played
 *
 * The animation resides in the given directory and is described by the given
 * section.
 *
 * The sound effects reside in the given directory and are described by
 * the given section.
 *
 * This function looks for image files in this order:
 *    key 'pics', if present
 *    picnametempl, if not null
 *    \<sectionname\>_??.bmp
 *
 * \param directory     which directory to look in for image and sound files
 * \param s             conffile section to search for data on this animation
 * \param picnametempl  a template for the image names
*/
uint32_t AnimationManager::get
	(char       const * const directory,
	 Section          &       s,
	 char       const *       picnametempl)
{
	m_animations.push_back(AnimationData());
	uint32_t const id = m_animations.size();
	AnimationData & ad = m_animations[id - 1];
	ad.frametime = FRAME_LENGTH;
	ad.hotspot.x = 0;
	ad.hotspot.y = 0;
	ad.picnametempl = "";

	// Determine image name template

	char templbuf[256];
	if (char const * const pics = s.get_string("pics"))
		picnametempl = pics;
	else if (!picnametempl) {
		snprintf(templbuf, sizeof(templbuf), "%s.png", s.get_name());
		picnametempl = templbuf;
	}
	{
		char pictempl[256];
		snprintf(pictempl, sizeof(pictempl), "%s%s", directory, picnametempl);
		assert(4 <= strlen(pictempl));
		size_t const len = strlen(pictempl) - 4;
		if (pictempl[len] == '.')
			pictempl[len] = '\0'; // delete extension
		ad.picnametempl = pictempl;
	}

	// Read mapping from frame numbers to sound effect names and load effects
	while (Section::Value * const v = s.get_next_val("sfx")) {
		char * parameters = v->get_string(), * endp;
		std::string fx_name;
		unsigned long long int const value = strtoull(parameters, &endp, 0);
		uint32_t const frame_number = value;
		try {
			if (endp == parameters or frame_number != value)
				throw wexception
					("expected %s but found \"%s\"",
					 "frame number", parameters);
			parameters = endp;
			force_skip(parameters);
			fx_name = std::string(directory) + "/" + std::string(parameters);
			g_sound_handler.load_fx_if_needed(directory, parameters, fx_name);
			std::map<uint32_t, std::string>::const_iterator const it =
				ad.sfx_cues.find(frame_number);
			if (it != ad.sfx_cues.end())
				throw wexception
					("redefinition for frame %u to \"%s\" (previously defined to "
					 "\"%s\")",
					 frame_number, parameters, it->second.c_str());
		} catch (const _wexception & e) {
			throw wexception("sfx: %s", e.what());
		}
		ad.sfx_cues[frame_number] = fx_name;
	}

	ad.hasplrclrs = s.get_bool("playercolor", false);

	int32_t const fps = s.get_int("fps");
	if (fps < 0)
		throw wexception("fps is %i, must be non-negative", fps);
	if (fps > 0)
		ad.frametime = 1000 / fps;

	// TODO: Frames of varying size / hotspot?
	ad.hotspot = s.get_Point("hotspot");

	return id;
}


/*
===============
Return the number of animations.
===============
*/
uint32_t AnimationManager::get_nranimations() const
{
	return m_animations.size();
}


/*
===============
Return AnimationData for this animation or throws if this id is unknown.
===============
*/
const AnimationData& AnimationManager::get_animation(uint32_t id) const
{
	if (!id || id > m_animations.size())
		throw wexception("Requested unknown animation with id: %i", id);

	return m_animations[id - 1];
}


/*
==============================================================================

DirAnimations IMPLEMENTAION

==============================================================================
*/

DirAnimations::DirAnimations
	(uint32_t const dir1,
	 uint32_t const dir2,
	 uint32_t const dir3,
	 uint32_t const dir4,
	 uint32_t const dir5,
	 uint32_t const dir6)
{
	m_animations[0] = dir1;
	m_animations[1] = dir2;
	m_animations[2] = dir3;
	m_animations[3] = dir4;
	m_animations[4] = dir5;
	m_animations[5] = dir6;
}


/*
===============
Parse an animation from the given directory and config.
sectnametempl is of the form "foowalk_??", where ?? will be replaced with
nw, ne, e, se, sw and w to get the section names for the animations.

If defaults is not zero, the additional sections are not actually necessary.
If they don't exist, the data is taken from defaults and the bitmaps
foowalk_??_nn.bmp are used.
===============
*/
void DirAnimations::parse
	(Widelands::Map_Object_Descr &       b,
	 const std::string           &       directory,
	 Profile                     &       prof,
	 char                  const * const sectnametempl,
	 Section                     * const defaults)
{
	char dirpictempl[256];
	char sectnamebase[256];
	char * repl;

	if (strchr(sectnametempl, '%'))
		throw wexception("sectnametempl %s contains %%", sectnametempl);

	snprintf(sectnamebase, sizeof(sectnamebase), "%s", sectnametempl);
	repl = strstr(sectnamebase, "??");
	if (!repl)
		throw wexception
			("DirAnimations section name template %s does not contain %%s",
			 sectnametempl);

	strncpy(repl, "%s", 2);

	if
		(char const * const string =
		 defaults ? defaults->get_string("dirpics", nullptr) : nullptr)
	{
		snprintf(dirpictempl, sizeof(dirpictempl), "%s", string);
		repl = strstr(dirpictempl, "!!");
		if (!repl)
			throw wexception
				("DirAnimations dirpics name templates %s does not contain !!",
				 dirpictempl);

		strncpy(repl, "%s", 2);
	} else {
		snprintf(dirpictempl, sizeof(dirpictempl), "%s_??.png", sectnamebase);
	}

	for (int32_t dir = 0; dir < 6; ++dir) {
		static char const * const dirstrings[6] =
			{"ne", "e", "se", "sw", "w", "nw"};
		char sectname[300];

		snprintf(sectname, sizeof(sectname), sectnamebase, dirstrings[dir]);

		std::string const anim_name = sectname;

		Section * s = prof.get_section(sectname);
		if (!s) {
			if (!defaults)
				throw wexception
					("Section [%s] missing and no default supplied",
					 sectname);

			s = defaults;
		}

		snprintf(sectname, sizeof(sectname), dirpictempl, dirstrings[dir]);
		m_animations[dir] = g_anim.get(directory, *s, sectname);
		b.add_animation(anim_name.c_str(), m_animations[dir]);
	}
}
