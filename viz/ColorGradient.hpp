// Code available at: http://www.andrewnoske.com/wiki/Code_-_heatmaps_and_color_gradients

// TODO: this can be moved to vizkit3d

#pragma once

#include <vector>

namespace vizkit3d
{
    class ColorGradient
    {
    private:
        struct ColorPoint  // Internal class used to store colors at different points in the gradient.
        {
            float r,g,b;      // Red, green and blue values of our color.
            float val;        // Position of our color along the gradient (between 0 and 1).
            ColorPoint(float red, float green, float blue, float value)
                : r(red), 
                  g(green), 
                  b(blue), 
                  val(value) 
            {}
        };
    
        std::vector<ColorPoint> color;      // An array of color points in ascending value.

    public:
        
        //-- Default constructor:
        ColorGradient()  
        {  
            createDefaultHeatMapGradient();  
        }

        //-- Inserts a new color point into its correct position:
        void addColorPoint(float red, float green, float blue, float value)
        {
            for(int i = 0; i < static_cast<int>(color.size()); i++)  
            {
                if(value < color[i].val) 
                {
                    color.insert(color.begin() + i, ColorPoint(red,green,blue, value));
                    return;  
                }
            }
            color.push_back(ColorPoint(red,green,blue, value));
        }

        //-- Inserts a new color point into its correct position:
        void clearGradient() 
        { 
            color.clear(); 
        }

        //-- Places a 5 color heapmap gradient into the "color" vector:
        void createDefaultHeatMapGradient()
        {
            color.clear();
            color.push_back(ColorPoint(0, 0, 0,   0.0f));      // black.
            color.push_back(ColorPoint(0, 0, 1,   0.1428f));   // Blue.
            color.push_back(ColorPoint(0, 1, 1,   0.2856f));   // Cyan.
            color.push_back(ColorPoint(0, 1, 0,   0.4284f));   // Green.
            color.push_back(ColorPoint(1, 1, 0,   0.5712f));   // Yellow.
            color.push_back(ColorPoint(1, 0, 0,   0.7140f));   // Red.
            color.push_back(ColorPoint(1, 1, 1,   1.0f));      // White.
        }

        //-- Inputs a (value) between 0 and 1 and outputs the (red), (green) and (blue)
        //-- values representing that position in the gradient.
        void getColorAtValue(const float value, float &red, float &green, float &blue) const
        {
            if(color.size() == 0)
                return;

            for(int i = 0; i < static_cast<int>(color.size()); i++)
            {
                const ColorPoint &currC = color[i];
                if(value < currC.val)
                {
                    const ColorPoint &prevC  = color[ std::max(0, i - 1) ];
                    float valueDiff    = (prevC.val - currC.val);
                    float fractBetween = (valueDiff == 0) ? 0 : (value - currC.val) / valueDiff;
                    red   = (prevC.r - currC.r) * fractBetween + currC.r;
                    green = (prevC.g - currC.g) * fractBetween + currC.g;
                    blue  = (prevC.b - currC.b) * fractBetween + currC.b;
                    return;
                }
            }
            
            red   = color.back().r;
            green = color.back().g;
            blue  = color.back().b;

            return;
        }
    };
}