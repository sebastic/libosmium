#ifndef OSMIUM_DIFF_ITERATOR_HPP
#define OSMIUM_DIFF_ITERATOR_HPP

/*

This file is part of Osmium (http://osmcode.org/osmium).

Copyright 2013 Jochen Topf <jochen@topf.org> and others (see README).

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.

*/

#include <cassert>
#include <iterator>

#include <osmium/osm/diff_object.hpp>

namespace osmium {

    template <class TBasicIterator>
    class DiffIterator : public std::iterator<std::input_iterator_tag, const osmium::DiffObject> {

        TBasicIterator m_prev;
        TBasicIterator m_curr;
        TBasicIterator m_next;

        const TBasicIterator m_end;

        mutable osmium::DiffObject m_diff {};

        void set_diff() const {
            assert(m_curr != m_end);

            TBasicIterator prev = m_prev;
            if (prev->type() != m_curr->type() || prev->id() != m_curr->id()) {
                prev = m_curr;
            }

            TBasicIterator next = m_next;
            if (next == m_end || next->type() != m_curr->type() || next->id() != m_curr->id()) {
                next = m_curr;
            }

            m_diff = osmium::DiffObject(*prev, *m_curr, *next);
        }

    public:

        DiffIterator(TBasicIterator begin, TBasicIterator end) :
            m_prev(begin),
            m_curr(begin),
            m_next(begin == end ? begin : ++begin),
            m_end(end) {
        }

        DiffIterator(const DiffIterator& other) = default;
        DiffIterator& operator=(const DiffIterator& other) = default;

        DiffIterator(DiffIterator&& other) = default;
        DiffIterator& operator=(DiffIterator&& other) = default;

        DiffIterator& operator++() {
            m_prev = std::move(m_curr);
            m_curr = m_next;

            if (m_next != m_end) {
                ++m_next;
            }

            return *this;
        }

        DiffIterator operator++(int) {
            DiffIterator tmp(*this);
            operator++();
            return tmp;
        }

        bool operator==(const DiffIterator& rhs) const {
            return m_curr == rhs.m_curr && m_end == rhs.m_end;
        }

        bool operator!=(const DiffIterator& rhs) const {
            return !(*this == rhs);
        }

        reference operator*() const {
            set_diff();
            return m_diff;
        }

        pointer operator->() const {
            set_diff();
            return &m_diff;
        }

    }; // class DiffIterator

} // namespace osmium

#endif // OSMIUM_DIFF_ITERATOR_HPP