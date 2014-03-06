/*
 * Copyright (C) 2006, 2008-2009 by the Widelands Development Team
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

#ifndef JOURNAL_EXCEPTIONS_H
#define JOURNAL_EXCEPTIONS_H

#include <stdexcept>
#include <string>

#include <stdint.h>

///
/// Thrown for IO-errors occurring with a journal file (unable to open file
/// for any reason, out of space, etc.) that a) are unrecoverable and b)
/// have already been dealt with.
///
/// This is a purely informational exception. Do not throw it unless it can
/// safely be ignored.
///
/// \todo add offset into journal file if applicable
/// \todo Rework as proposed by Erik, see filesystem_exceptions.h. Before that:
/// Replace with File*_error where appropriate, migrate from runtime_error to
/// logic_error (?)
struct Journalfile_error : public std::runtime_error {
	explicit Journalfile_error(const std::string & filename);
	virtual ~Journalfile_error() throw () {}

	virtual char const * what() const throw () override {return text.c_str();}

	std::string text;
	std::string filename;
};

/**
 * Thrown if the journal file contains a bad magic number
 * \todo add offset into journal file
 */
struct BadMagic_error : public Journalfile_error {
	explicit BadMagic_error(const std::string & filename);
	virtual ~BadMagic_error() throw () {}
};

/**
 * Thrown if the journal file contains a record with an unknown type number
 * \todo add offset into journal file
 */
struct BadRecord_error : public Journalfile_error {
	explicit BadRecord_error
		(const std::string & filename,
		 const uint8_t     code,
		 const uint8_t     expectedcode)
	;
	virtual ~BadRecord_error() throw () {}

	std::streamoff offset;
	uint8_t code;
	uint8_t expectedcode;
};

/**
 * Thrown if the journal file contains an event record with an unknown
 * event type
 * \todo add offset into journal file
 */
struct BadEvent_error : public Journalfile_error {
	explicit BadEvent_error(const std::string & filename, uint8_t const type)
	;
	virtual ~BadEvent_error() throw () {}

	std::streamoff offset;
	uint8_t type;
};

#endif
