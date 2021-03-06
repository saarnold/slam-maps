#define BOOST_TEST_MODULE VizTest
#include <boost/test/included/unit_test.hpp>

/** Vizkit 3D **/
#include <vizkit3d/Vizkit3DWidget.hpp>
#include <vizkit3d/QtThreadedWidget.hpp>
#include <vizkit3d/GridVisualization.hpp>
#include "ContourMapVisualization.hpp"

/** Maps library **/
#include <maps/geometric/Point.hpp>
#include <maps/geometric/ContourMap.hpp>

/** Std **/
#include <cmath>

using namespace ::maps::geometric;

static void showContourMap(const ContourMap &contour_map)
{
    // set up test environment
    QtThreadedWidget<vizkit3d::Vizkit3DWidget> app;
    app.start();

    //create vizkit3d plugin
    vizkit3d::ContourMapVisualization *contour_viz = new vizkit3d::ContourMapVisualization();
    contour_viz->updateData(contour_map);
    contour_viz->setLineWidth(5.0);
    contour_viz->setColor(0.0, 0.0, 0.0, 1.0);

    //create vizkit3d widget
    vizkit3d::Vizkit3DWidget *widget = app.getWidget();

    // grid plugin
    vizkit3d::GridVisualization *grid_viz = new vizkit3d::GridVisualization();
    widget->addPlugin(grid_viz);

    // add plugin
    widget->addPlugin(contour_viz);

    while (app.isRunning())
    {
        usleep(1000);
    }
}


BOOST_AUTO_TEST_CASE(countour_lines_map_test)
{
    ContourMap *contour = new ContourMap();

    double radius = 5.0;
    double step_size = 0.1;

    Point3d center;
    Point3d p_a;
    Point3d p_b;
    center << radius, radius, 1.00;
    p_b << 0.00, 0.00, 1.00;
    p_a = p_b;

    {
        LineSegment3d center_line(center, p_b);

        for (register unsigned int k=radius; k > 0.0 ; --k)
        {
            for (register unsigned int i=0; p_b.x() < 5.0 ; ++i)
            {
                p_b.y() = sqrt((k * k) - (p_b.x() * p_b.x()));

                if (k == radius)
                {
                    /** Center lines **/
                    center_line.psi_b(p_b);
                    contour->push_back(center_line);
                    //std::cout<<"p_b: "<<p_b.x()<<" "<<p_b.y()<<" "<<p_b.z()<<"\n";
                }

                if (i!=0)
                {
                    /** Circle lines **/
                    LineSegment3d circle_line (p_a, p_b);
                    contour->push_back(circle_line);
                }

                p_a = p_b;
                p_b.x() += step_size;
            }
            p_b.x() = 0.00;
            p_a.z()++;
            p_b.z()++;
        }
    }

    center << radius, -radius, 1.00;
    p_b << 0.00, 0.00, 1.00;
    p_a = p_b;

    {
        LineSegment3d center_line(center, p_b);

        for (register unsigned int k=radius; k > 0.0 ; --k)
        {
            for (register unsigned int i=0; p_b.x() < 5.0 ; ++i)
            {
                p_b.y() = -sqrt((k * k) - (p_b.x() * p_b.x()));

                if (k == radius)
                {
                    /** Center lines **/
                    center_line.psi_b(p_b);
                    contour->push_back(center_line);
                }

                if (i!=0)
                {
                    /** Circle lines **/
                    LineSegment3d circle_line (p_a, p_b);
                    contour->push_back(circle_line);
                }

                p_a = p_b;
                p_b.x() += step_size;
            }

            p_b.x() = 0.00;
            p_a.z()++;
            p_b.z()++;
        }
    }

    center << -radius, radius, 1.00;
    p_b << 0.00, 0.00, 1.00;
    p_a = p_b;

    {
        LineSegment3d center_line(center, p_b);

        for (register unsigned int k=radius; k > 0.0 ; --k)
        {
            for (register unsigned int i=0; p_b.x() > -5.0 ; ++i)
            {
                p_b.y() = sqrt((k * k) - (p_b.x() * p_b.x()));

                if (k == radius)
                {
                    /** Center lines **/
                    center_line.psi_b(p_b);
                    contour->push_back(center_line);
                }

                if (i!=0)
                {
                    /** Circle lines **/
                    LineSegment3d circle_line (p_a, p_b);
                    contour->push_back(circle_line);
                }

                p_a = p_b;
                p_b.x() -= step_size;
            }

            p_b.x() = 0.00;
            p_a.z()++;
            p_b.z()++;
        }
    }

    center << -radius, -radius, 1.00;
    p_b << 0.00, 0.00, 1.00;
    p_a = p_b;

    {
        LineSegment3d center_line(center, p_b);

        for (register unsigned int k=radius; k > 0.0 ; --k)
        {
            for (register unsigned int i=0; p_b.x() > -5.0 ; ++i)
            {
                p_b.y() = -sqrt((k * k) - (p_b.x() * p_b.x()));

                if (k == radius)
                {
                    /** Center lines **/
                    center_line.psi_b(p_b);
                    contour->push_back(center_line);
                }

                if (i!=0)
                {
                    /** Circle lines **/
                    LineSegment3d circle_line (p_a, p_b);
                    contour->push_back(circle_line);
                }

                p_a = p_b;
                p_b.x() -= step_size;
            }

            p_b.x() = 0.00;
            p_a.z()++;
            p_b.z()++;
        }
    }

    showContourMap(*contour);

    delete contour;
}
