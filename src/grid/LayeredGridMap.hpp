#ifndef __MAPS_LAYEREDGRIDMAP_HPP__
#define __MAPS_LAYEREDGRIDMAP_HPP__

#include <map>
#include <iostream>
#include <string>

#include <boost/multi_array.hpp>
#include <boost/shared_ptr.hpp>

#include "GridMap.hpp"

namespace maps { namespace grid
{
    class LayeredGridMap : public LocalMap
    {
    public:
        LayeredGridMap()
        : LocalMap(maps::LocalMapType::GRID_MAP),
          num_cells(0,0),
          resolution(0.0,0.0)
        {}
        /**
         * @brief creat an empty grid of specific size and resolution
         * @details Create an empty grid of specific size and resolution. 
         * No layer will be created in this case. Call <createLayer>"()"
         * to add layers to the grid.
         * 
         * @param config [description]
         */
        // initialize the structure of grid
        // no layers will be created
        // call createLayer to add some layers to grid
        LayeredGridMap(const Vector2ui &num_cells, const Vector2d &resolution)
            : LocalMap(maps::LocalMapType::GRID_MAP),
              num_cells(num_cells), 
              resolution(resolution)
        {}

        virtual ~LayeredGridMap() 
        {
            removeAllLayers();
        }

        /**
         * @brief create the layer with the specific key and default value
         * @details create new layer with the specific key and initialize 
         * the grid of this layer with the default value. this function can be failed
         * in case if the layer with the same key already exists
         * 
         * The first layer will be set as basis layer
         * 
         * @param key the unique identification key of the layer
         * @param default_value the default initialisation value for the layer
         * @return true if the layer could be created successfully, otherwise false
         */
        template <typename T>
        GridMap<T>& addLayer(const std::string &key, const T &default_value)
        {

            if (hasLayer(key) == true)
            {
                throw std::out_of_range("LayeredGridMap::addLayer: The grid with the key '" + key + "' exists already.");
            }

            GridMap<T> *new_grid = new GridMap<T>(num_cells, resolution, default_value, this->getLocalMapData());
            layers[key] = new_grid;

            return *new_grid;
        }

        /**
         * @brief [brief description]
         * @details [long description]
         * 
         * @param key [description]
         * @return [description]
         */
        bool hasLayer(const std::string &key) const
        { 
            typename LayerType::const_iterator it = layers.find(key);
            if (it == layers.end())
                return false;
            else 
                return true;
        }

        bool removeLayer(const std::string &key)
        {
            if(hasLayer(key) == false)
            {
                return false;
            }

            delete layers[key];
            layers.erase(key);

            return true;
        }

        void removeAllLayers() 
        {
            // TODO: check if the 
            typename LayerType::iterator it;
            for (it = layers.begin(); it != layers.end(); ++it)
            {
                delete it->second;
            }
            layers.clear();
        }

        template <typename T>
        const GridMap<T>& getLayer(const std::string &key) const
        {
            return *getGridMapPtr<T>(key);
        }           

        template <typename T>
        GridMap<T>& getLayer(const std::string &key)
        {
            return *getGridMapPtr<T>(key);
        }

        std::vector<std::string> getAllLayerKeys() const
        {
            std::vector<std::string> keys;
            typename LayerType::const_iterator it;
            for (it = layers.begin(); it != layers.end(); ++it)
            {
                keys.push_back(it->first);
            }
            return keys;
        }

    private:
        Vector2ui num_cells;
        Vector2d resolution;

        typedef std::map<std::string, LocalMap*> LayerType;
        LayerType layers;

        template <typename T>
        GridMap<T>* getGridMapPtr(const std::string &key) const
        {
            GridMap<T>* grid = NULL;

            if (hasLayer(key) == false)
                throw std::out_of_range("The map does not contain the grid with the key '" + key + "'.");

            grid = dynamic_cast<GridMap<T>*>(layers.at(key));   
                
            if (grid == NULL)
                throw std::runtime_error("The grid with the key '" + key + "' is not of required type.");

            return grid;
        }


    };
}}

#endif // __MAPS_LAYEREDGRIDMAP_HPP__
