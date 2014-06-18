#ifndef OSMIUM_OSM_NODE_HPP
#define OSMIUM_OSM_NODE_HPP

/*

This file is part of Osmium (http://osmcode.org/libosmium).

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

#include <osmium/osm/item_type.hpp>
#include <osmium/osm/location.hpp>
#include <osmium/osm/object.hpp>

namespace osmium {

    namespace memory {
        template <class T> class ObjectBuilder;
    }

    class Node : public Object {

        friend class osmium::memory::ObjectBuilder<osmium::Node>;

        osmium::Location m_location {};

        Node() :
            Object(sizeof(Node), osmium::item_type::node) {
        }

    public:

        static constexpr osmium::item_type itemtype = osmium::item_type::node;

        const osmium::Location location() const {
            return m_location;
        }

        Node& location(const osmium::Location& location) {
            m_location = location;
            return *this;
        }

        void lon(double x) {
            m_location.lon(x);
        }

        void lat(double y) {
            m_location.lat(y);
        }

        double lon() const {
            return m_location.lon();
        }

        double lat() const {
            return m_location.lat();
        }

    }; // class Node

} // namespace osmium

#endif // OSMIUM_OSM_NODE_HPP