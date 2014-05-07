#ifndef OSMIUM_DIFF_VISITOR_HPP
#define OSMIUM_DIFF_VISITOR_HPP

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

#include <stdexcept>
#include <type_traits>

#include <osmium/diff_iterator.hpp>
#include <osmium/io/input_iterator.hpp>
#include <osmium/memory/buffer.hpp>
#include <osmium/osm/diff_object.hpp>
#include <osmium/osm/item_type.hpp>

namespace osmium {

    class Object;

    namespace diff_handler {
        class DiffHandler;
    }

    namespace diff_visitor {

        namespace detail {

            template <class TVisitor>
            inline void switch_on_type(TVisitor& visitor, const osmium::DiffObject& diff, std::false_type) {
                switch (diff.type()) {
                    case osmium::item_type::node:
                        visitor(static_cast<const osmium::DiffNode&>(diff));
                        break;
                    case osmium::item_type::way:
                        visitor(static_cast<const osmium::DiffWay&>(diff));
                        break;
                    case osmium::item_type::relation:
                        visitor(static_cast<const osmium::DiffRelation&>(diff));
                        break;
                    default:
                        throw std::runtime_error("unknown type");
                }
            }

            template <class TVisitor>
            inline void switch_on_type(TVisitor& visitor, const osmium::DiffObject& diff, std::true_type) {
                switch (diff.type()) {
                    case osmium::item_type::node:
                        visitor.node(static_cast<const osmium::DiffNode&>(diff));
                        break;
                    case osmium::item_type::way:
                        visitor.way(static_cast<const osmium::DiffWay&>(diff));
                        break;
                    case osmium::item_type::relation:
                        visitor.relation(static_cast<const osmium::DiffRelation&>(diff));
                        break;
                    default:
                        throw std::runtime_error("unknown type");
                }
            }

            template <class TVisitor>
            inline void apply_diff_iterator_recurse(const osmium::DiffObject& diff, TVisitor& visitor) {
                switch_on_type(visitor, diff, std::is_base_of<osmium::diff_handler::DiffHandler, TVisitor>());
            }

            template <class TVisitor, class ...TRest>
            inline void apply_diff_iterator_recurse(const osmium::DiffObject& diff, TVisitor& visitor, TRest&... more) {
                apply_diff_iterator_recurse(diff, visitor);
                apply_diff_iterator_recurse(diff, more...);
            }

            template <class TVisitor>
            inline void switch_on_type_before_after(osmium::item_type /*last*/, osmium::item_type /*current*/, TVisitor& /*visitor*/, std::false_type) {
                // intentionally left blank
            }

            template <class TVisitor>
            inline void switch_on_type_before_after(osmium::item_type last, osmium::item_type current, TVisitor& visitor, std::true_type) {
                switch (last) {
                    case osmium::item_type::undefined:
                        visitor.init();
                        break;
                    case osmium::item_type::node:
                        visitor.after_nodes();
                        break;
                    case osmium::item_type::way:
                        visitor.after_ways();
                        break;
                    case osmium::item_type::relation:
                        visitor.after_relations();
                        break;
                    default:
                        break;
                }
                switch (current) {
                    case osmium::item_type::undefined:
                        visitor.done();
                        break;
                    case osmium::item_type::node:
                        visitor.before_nodes();
                        break;
                    case osmium::item_type::way:
                        visitor.before_ways();
                        break;
                    case osmium::item_type::relation:
                        visitor.before_relations();
                        break;
                    default:
                        break;
                }
            }

            template <class TVisitor>
            inline void apply_before_and_after_recurse(osmium::item_type last, osmium::item_type current, TVisitor& visitor) {
                switch_on_type_before_after(last, current, visitor, std::is_base_of<osmium::diff_handler::DiffHandler, TVisitor>());
            }

            template <class TVisitor, class ...TRest>
            inline void apply_before_and_after_recurse(osmium::item_type last, osmium::item_type current, TVisitor& visitor, TRest&... more) {
                apply_before_and_after_recurse(last, current, visitor);
                apply_before_and_after_recurse(last, current, more...);
            }

        } // namespace detail

    } // namespace diff_visitor

    template <class TIterator, class ...TVisitors>
    inline void apply_diff(TIterator it, TIterator end, TVisitors&... visitors) {
        typedef osmium::DiffIterator<TIterator> diff_iterator;

        diff_iterator dit(it, end);
        diff_iterator dend(end, end);

        osmium::item_type last_type = osmium::item_type::undefined;

        for (; dit != dend; ++dit) {
            if (last_type != dit->type()) {
                osmium::diff_visitor::detail::apply_before_and_after_recurse(last_type, dit->type(), visitors...);
                last_type = dit->type();
            }
            osmium::diff_visitor::detail::apply_diff_iterator_recurse(*dit, visitors...);
        }

        osmium::diff_visitor::detail::apply_before_and_after_recurse(last_type, osmium::item_type::undefined, visitors...);
    }

    template <class TSource, class ...TVisitors>
    inline void apply_diff(TSource& source, TVisitors&... visitors) {
        apply_diff(osmium::io::InputIterator<TSource, osmium::Object> {source},
                   osmium::io::InputIterator<TSource, osmium::Object> {},
                   visitors...);
    }

    template <class ...TVisitors>
    inline void apply_diff(osmium::memory::Buffer& buffer, TVisitors&... visitors) {
        apply_diff(buffer.begin(), buffer.end(), visitors...);
    }

    template <class ...TVisitors>
    inline void apply_diff(const osmium::memory::Buffer& buffer, TVisitors&... visitors) {
        apply_diff(buffer.cbegin(), buffer.cend(), visitors...);
    }

} // namespace osmium

#endif // OSMIUM_DIFF_VISITOR_HPP