#pragma once

#include "AccessIterator.hpp"
#include "Index.hpp"

namespace maps { namespace grid
{
    template <typename CellBaseT>
    class GridAccessInterface
    {
    public:

        typedef AccessIterator<CellBaseT> iterator;
        typedef ConstAccessIterator<CellBaseT> const_iterator;

        GridAccessInterface() {}

        virtual ~GridAccessInterface() {}

        virtual const CellBaseT &getDefaultValue() const = 0;

        virtual iterator begin() = 0;

        virtual iterator end() = 0;

        virtual const_iterator begin() const = 0;

        virtual const_iterator end() const = 0;

        virtual void resize(Vector2ui new_number_cells) = 0;

        virtual void moveBy(Index idx) = 0;

        virtual const CellBaseT& at(Index idx) const = 0;

        virtual CellBaseT& at(Index idx) = 0;

        virtual const Vector2ui &getNumCells() const = 0;

        virtual void clear() = 0;
    };
}}
