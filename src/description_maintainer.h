/*
 * Copyright (C) 2002, 2006-2008 by the Widelands Development Team
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

#ifndef DESCR_MAINTAINER_H
#define DESCR_MAINTAINER_H

#include <cassert>
#include <map>
#include <memory>
#include <vector>

// Used to have a typesafe maintainer for description classes.
template <typename T> struct DescriptionMaintainer {
	// Adds the 'entry', will assert() if it is already registered. Returns the
	// index of the entry. Ownership is taken.
	int32_t add(T* entry);

	// Returns the number of entries in the container.
	size_t get_nitems() const {return items_.size();}

	// Returns the entry with the given 'name' if it exists or nullptr.
	T* exists(const std::string& name) const;

	// Returns the index of the entry with the given 'name' or -1 if the entry
	// is not in the container.
	int32_t get_index(const std::string& name) const {
		// TODO(sirver): this should return INVALID_INDEX
		NameToIndexMap::const_iterator i = name_to_index_.find(name);
		if (i == name_to_index_.end())
			return -1;
		return i->second;
	}

	// Returns the entry with the given 'idx' or nullptr if 'idx' is out of
	// bound. Ownership is retained.
	T* get(const int32_t idx) const {
		return (idx >= 0 && idx < static_cast<int32_t>(items_.size())) ? items_[idx].get() : nullptr;
	}

private:
	typedef std::map<std::string, int> NameToIndexMap;
	std::vector<std::unique_ptr<T>> items_;
	NameToIndexMap name_to_index_;
};


template <typename T>
int32_t DescriptionMaintainer<T>::add(T * const item) {
	assert(exists(item->name()) == nullptr);
	int32_t index = items_.size();
	items_.emplace_back(item);
	name_to_index_[item->name()] = index;
	return index;;
}

template <typename T> T* DescriptionMaintainer<T>::exists(const std::string& name) const {
	int32_t index = get_index(name);
	if (index < 0) return nullptr;
	return items_[index].get();
}

#endif
