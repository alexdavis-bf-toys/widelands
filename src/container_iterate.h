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

#ifndef CONTAINER_ITERATE_H
#define CONTAINER_ITERATE_H

// Helper structure for representing an iterator range in
// for loops.
// Usage:
// std::vector<int> v(10);
// for (wl_range< std::vector<int> > i(v);i;++i)
//     std::cout << *i;
//
template<typename C>
struct wl_range
{
	wl_range
		(const typename C::iterator & first, const typename C::iterator & last) :
			current(first), end(last) {}
	wl_range(C & container) : current(container.begin()), end(container.end()) {}
	wl_range(const wl_range & r) : current(r.current), end(r.end) {}
	typename C::iterator current;
	wl_range & operator++() {++current; return *this;}
	wl_range<C> & advance() {++current; return *this;}
	bool empty() const {return current == end;}
	operator bool() const {return empty() ? false: true;}
	typename C::reference front() const {return *current;}
	typename C::reference operator*() const {return *current;}
	typename C::pointer operator->() const {return (&**this);}
	typename C::iterator get_end() {return end;}
private:
	typename C::iterator end;
};

// Helper structure for representing an iterator range in
// for loops with constant iterators.
// Usage:
// std::vector<int> const v(10);
// for (wl_const_range< std::vector<int> > i(v);i;++i)
//     std::cout << *i;
//
template<typename C>
struct wl_const_range
{
	wl_const_range
		(const typename  C::const_iterator & first,
		 const typename C::const_iterator & last)
		: current(first), end(last) {}
	wl_const_range(const C & container) : current(container.begin()), end(container.end()) {}
	wl_const_range(const wl_const_range & r) : current(r.current), end(r.end) {}
	typename C::const_iterator current;
	wl_const_range & operator++() {++current; return *this;}
	wl_const_range<C> & advance() {++current; return *this;}
	bool empty() const {return current == end;}
	operator bool() const {return empty() ? false: true;}
	typename C::const_reference front() const {return *current;}
	typename C::const_reference operator*() const {return *current;}
	typename C::const_pointer operator->() const {return (&**this);}
	typename C::const_iterator get_end() {return end;}
private:
	typename C::const_iterator end;
};

#define container_iterate_const(type, container, i)                           \
	for                                                                       \
	   (wl_const_range< type > i(container);                                  \
		i;                                                                    \
		++i)                                                                  \

#define container_iterate(type, container, i)                                 \
	for                                                                       \
	   (wl_range< type > i(container);                                        \
		i;                                                                    \
		++i)                                                                  \

#endif
