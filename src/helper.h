/*
 * Copyright (C) 2006-2009 by the Widelands Development Team
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

#ifndef HELPER_H
#define HELPER_H

#include <cassert>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>

#include <SDL_keyboard.h>
#include <boost/utility.hpp>

#include "wexception.h"

/// Matches the string that candidate points to against the string that
/// template points to. Stops at when reaching a null character or the
/// character terminator. If a match is found, candidate is moved beyond the
/// matched part.
///
/// example:
///    char const * candidate = "return   75";
///    bool const result = match(candidate, "return");
/// now candidate points to "   75" and result is true
bool match(char* & candidate, const char* pattern);

/// Returns the word starting at the character that p points to and ending
/// before the first terminator character. Replaces the terminator with null.
char* next_word(char* & p, bool& reached_end, char terminator = ' ');

/// Skips a sequence of consecutive characters with the value c, starting at p.
/// Returns whether any characters were skipped.
bool skip(char* & p, char c = ' ');

/// Skips a sequence of consecutive characters with the value c, starting at p.
/// Throws _wexception if no characters were skipped.
void force_skip(char* & p, char c = ' ');

/// Combines match and force_skip.
///
/// example:
///    char const * candidate = "return   75";
///    bool const result = match_force_skip(candidate, "return");
/// now candidate points to "75" and result is true
///
/// example:
///   char const * candidate = "return75";
///    bool const result = match_force_skip(candidate, "return");
/// throws _wexception
bool match_force_skip(char* & candidate, const char* pattern);

/**
 * Convert std::string to any sstream-compatible type
 *
 * \see http://www.experts-exchange.com/Programming/
 *    Programming_Languages/Cplusplus/Q_20670737.html
 * \author AssafLavie on http://www.experts-exchange.com
 */
template<typename T> T stringTo(const std::string & s) {
	std::istringstream iss(s);
	T x;
	iss >> x;
	return x;
}

std::vector<std::string> split_string
	(const std::string &, char const * separators);
void remove_spaces(std::string &);
void log(char * const fmt, ...);

bool is_printable(SDL_keysym k);

/// A class that makes iteration over filename_?.png templates easy.
class NumberGlob : boost::noncopyable {
public:
	typedef uint32_t type;
	NumberGlob(const std::string& pictmp);

	/// If there is a next filename, puts it in 's' and returns true.
	bool next(std::string* s);

private:
	std::string templ_;
	std::string fmtstr_;
	std::string replstr_;
	uint32_t cur_;
	uint32_t max_;
};

/// Generate a random string of given size out of the given alphabet.
std::string random_string(const std::string& chars, int nlen);

/// A functional container filtering (by copying the values). Returns a new
//ContainerType that / contains all values where 'test' returned true.
template <typename ContainerType, class UnaryPredicate>
ContainerType filter(const ContainerType& container, UnaryPredicate test) {
	ContainerType filtered;
	for (const auto& entry : container) {
		if (!test(entry)) {
			continue;
		}
		filtered.insert(entry);
	}
	return filtered;
}
#endif
