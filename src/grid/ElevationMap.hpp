#ifndef __MAPS_ELEVATION_MAP_HPP__
#define __MAPS_ELEVATION_MAP_HPP__

#include "GridMap.hpp"

namespace maps { namespace grid
{

    /**@brief ElevationMap class
     * It extends the typedef GridMapF with some convinient methods
     */
    class ElevationMap : public GridMapF
    {
    public:
        typedef boost::shared_ptr<float> Ptr;
        static const float  ELEVATION_DEFAULT;
    public:
        ElevationMap();
        ElevationMap(const ElevationMap &elevation_map);
        ElevationMap(const GridMapF &grid_map);
        ElevationMap(const Vector2ui &num_cells, const Vector2d &resolution);
        ElevationMap(const Vector2ui &num_cells, const Vector2d &resolution, const float &default_value);

        ~ElevationMap();

        Vector3d getNormal(const Index& pos) const;

        /** @brief get the normal vector at the given position
        */
        Vector3d getNormal(const Vector3d& pos) const;

        /** @brief get the elevation at the given point 
        *
        * The underlying model assumes the height value to be at
        * the center of the cell, and a surface is approximated
        * using the getNormal. The Height value is the value of the
        * plane at that point.
        */
        float getMeanElevation(const Vector3d& pos) const;

        std::pair<float, float> getElevationRange() const;   
    };
}}


#endif // __MAPS_ELEVATION_MAP_HPP__
