/*
 * Copyright (C) 2008 by the Widelands Development Team
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

#ifndef FILEWRITE_H
#define FILEWRITE_H

#include <cassert>
#include <cstdarg>
#include <limits>
#include <memory>

#include "io/streamwrite.h"

class FileSystem;
class RealFSImpl;

/// Mirror of \ref FileRead : all writes are first stored in a block of memory
/// and finally written out when Write() is called.
class FileWrite : public StreamWrite {
public:
	struct Pos {
		Pos(size_t const p = 0) : pos(p) {
		}

		/// Returns a special value indicating invalidity.
		static Pos Null() {
			return std::numeric_limits<size_t>::max();
		}

		bool isNull() const {
			return *this == Null();
		}
		operator size_t() const {
			return pos;
		}
		Pos operator++() {
			return ++pos;
		}
		Pos operator += (Pos const other) {
			return pos += other.pos;
		}

	private:
		size_t pos;
	};

	struct Exception {};
	struct Buffer_Overflow : public Exception {};

	/// Set the buffer to empty.
	FileWrite();

	/// Clear any remaining allocated data.
	~FileWrite() override;

	/// Clears the object's buffer.
	void Clear();

	/// Write the file out to disk. If successful, this clears the buffers.
	/// Otherwise, an exception is thrown but the buffer remains intact (don't
	/// worry, it will be cleared by the destructor).
	void Write(FileSystem& fs, char const* const filename);

	/// Same as above, just that the data is appended to the file
	/// NOTE RealFSImpl is used by purpose - zip filesystems do not support appending
	void WriteAppend(RealFSImpl& fs, char const* const filename);

	/// Get the position that will be written to in the next write operation that
	/// does not specify a position.
	Pos GetPos() const;

	/// Set the file pointer to a new location. The position can be beyond the
	/// current end of file.
	void SetPos(Pos pos);

	/// Write data at the given location.
	void Data(const void* src, size_t size, Pos pos);

	/// Write data at the current file pointer and advance it.
	void Data(void const* const src, size_t const size) override;

private:
	char* data_;
	size_t length_;
	size_t max_size_;
	Pos filepos_;
};

#endif
