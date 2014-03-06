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

#include <cstdlib>
#include <limits>
#include <stdexcept>

/**
 * This template is used to have a typesafe maintainer for Bob_Descr,
 * Worker_Descr and so on.
 */
template <typename T> struct Descr_Maintainer {
	Descr_Maintainer() : capacity(0), nitemsw(0), items(nullptr) {}
	~Descr_Maintainer();

	static typename T::Index invalid_index() {
		return std::numeric_limits<typename T::Index>::max();
	}

	T * exists(char const * name) const;
	int32_t add(T *);
	typename T::Index get_nitems() const {return nitemsw;}

	struct Nonexistent {};

	int32_t get_index(const std::string & name) const
	{
		for (typename T::Index i = 0; i < nitemsw; ++i)
			if (name == items[i]->name())
				return i;
		return -1;
	}

	int32_t get_index(const char * const name) const
	{
		for (typename T::Index i = 0; i < nitemsw; ++i)
			if (name == items[i]->name())
				return i;
		return -1;
	}

	T * get(int32_t const idx) const {
		return idx >= 0 and idx < static_cast<int32_t>(nitemsw) ? items[idx] : nullptr;
	}

private:
	typename T::Index capacity;
	typename T::Index nitemsw;
	T * * items;

	void reserve(const typename T::Index n) {
		T * * const new_items =
			static_cast<T * *>(std::realloc(items, sizeof(T *) * n));
		if (not new_items)
			throw std::bad_alloc();
		items = new_items;
		capacity = n;
	}
};


template <typename T>
int32_t Descr_Maintainer<T>::add(T * const item) {
	int32_t const result = nitemsw;
	if (++nitemsw >= capacity)
		reserve(nitemsw);
	items[result] = item;
	return result;
}

/// Returns the element if it exists, 0 otherwise.
template <typename T>
T * Descr_Maintainer<T>::exists(char const * const name) const {
	for (typename T::Index i = 0; i < nitemsw; ++i)
		if (name == items[i]->name())
			return items[i];
	return nullptr;
}

template<typename T> Descr_Maintainer<T>::~Descr_Maintainer() {
	for (typename T::Index i = 0; i < nitemsw; ++i)
		delete items[i];
	std::free(items);
}
/// This template is used to have a typesafe maintainer for Bob_Descr,
/// Worker_Descr and so on. This version uses boxed Index type for indexing.
/// Usage: Indexed_Descr_Maintainer<Worker_Descr, Ware_Index> workers;
template <typename T, typename T_Index> struct Indexed_Descr_Maintainer :
private Descr_Maintainer<T>
{
	T * exists(char const * const name) {
		return Descr_Maintainer<T>::exists(name);
	}
	T_Index add(T * const t) {
		return
			T_Index
				(static_cast<typename T_Index::value_t>
				 	(Descr_Maintainer<T>::add(t)));
	}
	T_Index get_nitems() const {
		return T_Index(Descr_Maintainer<T>::get_nitems());
	}
	T_Index get_index(const std::string & name) const {
		int32_t idx = Descr_Maintainer<T>::get_index(name);
		return
			idx == -1 ? T_Index::Null() :
			T_Index(static_cast<typename T_Index::value_t>(idx));
	}
	T_Index get_index(const char * const name) const {
		int32_t idx = Descr_Maintainer<T>::get_index(name);
		return
			idx == -1 ? T_Index::Null() :
			T_Index(static_cast<typename T_Index::value_t>(idx));
	}
	T * get(T_Index const idx) const {
		return idx ? Descr_Maintainer<T>::get(idx.value()) : nullptr;
	}
};

#endif
