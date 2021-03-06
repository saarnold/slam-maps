#pragma once

#include "LevelList.hpp"
#include "GridMap.hpp"
#include "../tools/Overlap.hpp"

namespace maps { namespace grid
{

    template <class P>
    class MultiLevelGridMap : public GridMap<LevelList<P> >
    {
    public:
        typedef LevelList<P> CellType; 
        
        typedef P PatchType;
        MultiLevelGridMap(const Vector2ui &num_cells,
                    const Eigen::Vector2d &resolution,
                    const boost::shared_ptr<LocalMapData> &data) : GridMap<LevelList<P> >(num_cells, resolution, LevelList<P>(), data)
        {}

        MultiLevelGridMap(const Vector2ui &num_cells,
                    const Eigen::Vector2d &resolution) : GridMap<LevelList<P> >(num_cells, resolution, LevelList<P>())
        {}
        
        MultiLevelGridMap() {}
        
        template<class Q>
        MultiLevelGridMap(const MultiLevelGridMap<Q> &other) : GridMap<CellType>(other, other)
        {
        }

        class View : public GridMap<LevelList<const P *> >
        {
        public:
            View(const Vector2ui &num_cells,
                const Eigen::Vector2d &resolution) : GridMap<LevelList<const P *> >(num_cells, resolution, LevelList<const P *>())
            {
            };
            
            View() : GridMap<LevelList<const P *> >()
            {
            };
        };
    
        View intersectCuboid(const Eigen::AlignedBox3d& box) const
        {
            size_t ignore;
            return intersectCuboid(box, ignore);
        }
        
        typedef std::vector<std::pair<Index, const P*>> PatchVector;

        /** Intersects @p box with the mls map.
         * The box is in local grid coordinates (i.e., starting at (0,0))
         * @return A list of patches and their grid indices that intersect the @p box.
         * @throw std::runtime_error if any part of @p box is outside the grid*/
        PatchVector intersectAABB(const Eigen::AlignedBox3d& box) const
        {
            PatchVector ret;
            intersectAABB_callback(box,
                    [&ret](Index idx, const P& p)
                    { 
                        ret.emplace_back(idx, &p);
                        return false;
                     });
            return ret;
        }

        // TODO compiling this can get rather expensive
        /** @param cb A callback that is executed for each patch that intersects.
         *            Prototype: bool f(const maps::grid::Index&, const P&)
         *            The return value of the the callback indicates whether
         *            the intersection test should abort or not.
         *            I.e. if the callback returns true, the intersection test
         *            will be aborted.*/
        template<class CallBack>
        void intersectAABB_callback(const Eigen::AlignedBox3d& box, CallBack&& cb) const
        {
            double minHeight = box.min().z();
            double maxHeight = box.max().z();

            Index minIdx = (box.min().head<2>().cwiseQuotient(this->getResolution())).template cast<int>();
            Index maxIdx = (box.max().head<2>().cwiseQuotient(this->getResolution())).template cast<int>();

            minIdx = minIdx.cwiseMax(0);
            maxIdx = maxIdx.cwiseMin(this->getNumCells().template cast<int>());

            for(int y = minIdx.y(); y < maxIdx.y(); y++)
            {
                for(int x = minIdx.x();x < maxIdx.x(); x++)
                {
                    const Index curIdx(x,y);
                    for(const P &p: this->at(curIdx))
                    {
                        if(::maps::tools::overlap(p.getMin(), p.getMax(), minHeight, maxHeight))
                        {
                            if(cb(curIdx, p))
                                return;
                        }
                    }
                }
            }
        }
        



        /** @param outNumIntersections contains the number of mls patches that
                                       intersected the @p box*/
        View intersectCuboid(const Eigen::AlignedBox3d& box, size_t& outNumIntersections) const
        {
            double minHeight = box.min().z();
            double maxHeight = box.max().z();
            outNumIntersections = 0;
            
            Index minIdx;
            Index maxIdx;
            
            if(!this->toGrid(box.min(), minIdx))
            {
                return View();
            }

            if(!this->toGrid(box.max(), maxIdx))
            {
                return View();
            }
            
            Index newSize(maxIdx-minIdx);
            
            View ret(Vector2ui(newSize.x(), newSize.y()) , this->getResolution());
            
            for(Index::Scalar y = minIdx.y(); y < maxIdx.y(); y++)
            {
                for(Index::Scalar x = minIdx.x();x < maxIdx.x(); x++)
                {
                    Index curIdx(x,y);
                    
                    LevelList<const P *> &retList(ret.at(Index(curIdx - minIdx)));
                    
                    for(const P &p: this->at(curIdx))
                    {
                        if(::maps::tools::overlap(p.getMin(), p.getMax(), minHeight, maxHeight))
                        {
                            retList.insert(&p);
                            ++outNumIntersections;
                        }
                    }
                }
            }
            
            return ret;
        }
        
    };

}}
