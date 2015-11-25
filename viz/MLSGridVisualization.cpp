#include <iostream>

#include "MLSGridVisualization.hpp"

#include "PatchesGeode.hpp"

#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/Material>

using namespace vizkit3d;
using namespace envire::maps;

template <class T>
osg::Vec3 Vec3( const Eigen::Matrix<T,3,1>& v )
{
    return osg::Vec3( v.x(), v.y(), v.z() );
}

struct MLSGridVisualization::Data {
    // Copy of the value given to updateDataIntern.
    //
    // Making a copy is required because of how OSG works
    envire::maps::MLSGrid data;
};


MLSGridVisualization::MLSGridVisualization()
    : p(new Data),
    horizontalCellColor(osg::Vec4(0.1,0.5,0.9,1.0)), 
    verticalCellColor(osg::Vec4(0.8,0.9,0.5,1.0)), 
    negativeCellColor(osg::Vec4(0.1,0.5,0.9,0.2)), 
    uncertaintyColor(osg::Vec4(0.5,0.1,0.1,0.3)), 
    showUncertainty(false),
    showNegative(false),
    estimateNormals(false),
    cycleHeightColor(false),
    cycleColorInterval(1.0),
    showExtents(true)
{
}

MLSGridVisualization::~MLSGridVisualization()
{
    delete p;
}

osg::ref_ptr<osg::Node> MLSGridVisualization::createMainNode()
{
    // Geode is a common node used for vizkit3d plugins. It allows to display
    // "arbitrary" geometries
    osg::ref_ptr<osg::Group> group = new osg::Group();
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    group->addChild(geode.get());

    return group.release();
}

void MLSGridVisualization::updateMainNode ( osg::Node* node )
{
    osg::Group* group = static_cast<osg::Group*>(node);    

    osg::ref_ptr<PatchesGeode> geode = new PatchesGeode();
    group->setChild( 0, geode );

    MLSGrid &mls = p->data;

    /*// draw the extents of the mls
    group->removeChild( 1 );
    if( showExtents )
    {
        // get the color as a function of the environmentitem pointer
        float scale = ((long)item%1000)/1000.0;
        osg::Vec4 col(0,0,0,1);
        vizkit3d::hslToRgb( scale, 1.0, 0.6, col.x(), col.y(), col.z() );

        group->addChild( 
            new ExtentsRectangle( mls->getExtents(), col ) );
    }    */

    const double xs = mls.getScaleX();
    const double ys = mls.getScaleY();

    const double xo = mls.getOffsetX();
    const double yo = mls.getOffsetY();

    osg::ref_ptr<osg::Vec3Array> var_vertices = new osg::Vec3Array;

    for (size_t x = 0; x < mls.getCellSizeX(); x++)
    {
        for (size_t y = 0; y < mls.getCellSizeY(); y++)
        {
            const SPList &list = mls.at(x, y);

            for (SPList::const_iterator it = list.begin(); it != list.end(); it++)
            {

                const SurfacePatch &p(*it);
                double xp = (x+0.5) * xs + xo;
                double yp = (y+0.5) * ys + yo; 

                // setup the color for the next geometry
                if(mls.getConfig().useColor == true)
                {
                    geode->showCycleColor(false);
                    base::Vector3d c = p.getColor();
                    osg::Vec4 col = osg::Vec4(c.x(), c.y(), c.z(), 1.0);
                    geode->setColor(col);
                }
                else if(cycleHeightColor)
                {
                    geode->showCycleColor(true);
                    geode->setCycleColorInterval(cycleColorInterval);
                    double hue = (p.getMean() - std::floor(p.getMean() / cycleColorInterval) * cycleColorInterval) / cycleColorInterval;
                    double sat = 1.0;
                    double lum = 0.6;
                    double alpha = std::max(0.0, 1.0 - p.getStdev());
                    geode->setColorHSVA(hue, sat, lum, alpha);
                }
                else
                    geode->setColor(horizontalCellColor);

                // slopes need to be handled differently
                if( mls.getConfig().updateModel == MLSConfig::SLOPE )
                {
                    if( !p.isNegative() )
                    {
                        osg::Vec4 heights(0,0,0,0);
                        heights[0] = p.getHeight( Eigen::Vector2f( 0, 0 ) );
                        heights[1] = p.getHeight( Eigen::Vector2f( xs, 0 ) );
                        heights[2] = p.getHeight( Eigen::Vector2f( xs, ys ) );
                        heights[3] = p.getHeight( Eigen::Vector2f( 0, ys ) );

                        geode->drawPlane(  
                                osg::Vec3( xp, yp, p.getMean() ), 
                                heights, 
                                osg::Vec3( xs, ys, 0.0 ), 
                                Vec3( p.getNormal() ),
                                p.getMinZ(), p.getMaxZ() 
                        );
                    }
                }
                else
                {
                    if( p.isHorizontal() )
                    {
                        geode->drawBox( 
                                osg::Vec3( xp, yp, p.getMean() ), 
                                osg::Vec3( xs, ys, 0.0 ), 
                                estimateNormals ? 
                                    estimateNormal(mls, p, GridBase::Index(x,y)) :
                                    osg::Vec3( 0, 0, 1.0 ) );
                    }
                    else
                    {
                        if( p.isVertical() || showNegative )
                        {   
                            geode->setColor( 
                                    p.isVertical() ? verticalCellColor : negativeCellColor );
                            geode->drawBox( 
                                    osg::Vec3( xp, yp, p.getMean()-p.getHeight()*.5 ), 
                                    osg::Vec3( xs, ys, p.getHeight() ), 
                                    osg::Vec3(0, 0, 1.0) );
                        }
                    }
                }
            }
        }
    } 
}


void MLSGridVisualization::updateDataIntern(envire::maps::MLSGrid const& value)
{
    p->data = value;
}

osg::Vec3 MLSGridVisualization::estimateNormal(const MLSGrid &grid, const SurfacePatch &patch, const GridBase::Index &patch_idx) const 
{
   /* Eigen::Vector2d patch_pos;
    grid.fromGrid(patch_idx, patch_pos);

    Eigen::Vector3d patch_center(patch_pos.x(), patch_pos.y(), patch.getMean());

    Eigen::Vector3d d[2] = { Eigen::Vector3d::Zero(), Eigen::Vector3d::Zero() };

    for( int n = 0; n < 2; n++ )
    {
        for( int i = -1; i < 2; i += 2 )
        {
            GridBase::Index idx( idx.x + n * i, idx.y + (n - 1) * i );
            if (grid.inGrid(idx))
            {
                SPList::iterator it = grid.at(idx).getPatchByZ(patch.getMean(), grid.getScaleX() * 2);
                if( it != grid.at(idx).end() )
                {
                    Eigen::Vector2d pos;
                    grid.fromGrid(idx, pos);

                    Eigen::Vector3d v(pos.x(), pos.y(), it->getMean());
                    d[n] += (v - patch_center)*i;
                }
            }
        }
    }

    Eigen::Vector3d n = d[0].cross( d[1] );
    if( n.norm() > 0.0 )
    {
        n.normalize();
        return osg::Vec3(n.x(), n.y(), n.z());
    }
    else*/
        return osg::Vec3(0,0,1.0);
}

bool MLSGridVisualization::isUncertaintyShown() const
{
    return showUncertainty;
}

void MLSGridVisualization::setShowUncertainty(bool enabled)
{
    showUncertainty = enabled;
    emit propertyChanged("show_uncertainty");
    setDirty();
}

bool MLSGridVisualization::isNegativeShown() const
{
    return showNegative;
}

void MLSGridVisualization::setShowNegative(bool enabled)
{
    showNegative = enabled;
    emit propertyChanged("show_negative");
    setDirty();
}

bool MLSGridVisualization::areNormalsEstimated() const
{
    return estimateNormals;
}

void MLSGridVisualization::setEstimateNormals(bool enabled)
{
    estimateNormals = enabled;
    emit propertyChanged("estimate_normals");
    setDirty();
}

bool MLSGridVisualization::isHeightColorCycled() const
{
    return cycleHeightColor;
}

void MLSGridVisualization::setCycleHeightColor(bool enabled)
{
    cycleHeightColor = enabled;
    emit propertyChanged("cycle_height_color");
    setDirty();
}

double MLSGridVisualization::getCycleColorInterval() const
{
    return cycleColorInterval;
}

void MLSGridVisualization::setCycleColorInterval(double interval)
{
    if(interval == 0.0)
        cycleColorInterval = 1.0;
    else
        cycleColorInterval = interval;
    emit propertyChanged("cycle_color_interval");
    setDirty();
}

QColor MLSGridVisualization::getHorizontalCellColor() const
{
    QColor color;
    color.setRgbF(horizontalCellColor.x(), horizontalCellColor.y(), horizontalCellColor.z(), horizontalCellColor.w());
    return color;
}

void MLSGridVisualization::setHorizontalCellColor(QColor color)
{
    horizontalCellColor.x() = color.redF();
    horizontalCellColor.y() = color.greenF();
    horizontalCellColor.z() = color.blueF();
    horizontalCellColor.w() = color.alphaF();
    emit propertyChanged("horizontal_cell_color");
    setDirty();
}

QColor MLSGridVisualization::getVerticalCellColor() const
{
    QColor color;
    color.setRgbF(verticalCellColor.x(), verticalCellColor.y(), verticalCellColor.z(), verticalCellColor.w());
    return color;
}

void MLSGridVisualization::setVerticalCellColor(QColor color)
{
    verticalCellColor.x() = color.redF();
    verticalCellColor.y() = color.greenF();
    verticalCellColor.z() = color.blueF();
    verticalCellColor.w() = color.alphaF();
    emit propertyChanged("vertical_cell_color");
    setDirty();
}

QColor MLSGridVisualization::getNegativeCellColor() const
{
    QColor color;
    color.setRgbF(negativeCellColor.x(), negativeCellColor.y(), negativeCellColor.z(), negativeCellColor.w());
    return color;
}

void MLSGridVisualization::setNegativeCellColor(QColor color)
{
    negativeCellColor.x() = color.redF();
    negativeCellColor.y() = color.greenF();
    negativeCellColor.z() = color.blueF();
    negativeCellColor.w() = color.alphaF();
    emit propertyChanged("negative_cell_color");
    setDirty();
}

QColor MLSGridVisualization::getUncertaintyColor() const
{
    QColor color;
    color.setRgbF(uncertaintyColor.x(), uncertaintyColor.y(), uncertaintyColor.z(), uncertaintyColor.w());
    return color;
}

void MLSGridVisualization::setUncertaintyColor(QColor color)
{
    uncertaintyColor.x() = color.redF();
    uncertaintyColor.y() = color.greenF();
    uncertaintyColor.z() = color.blueF();
    uncertaintyColor.w() = color.alphaF();
    emit propertyChanged("uncertainty_color");
    setDirty();
}

void MLSGridVisualization::setShowExtents( bool value ) 
{
    showExtents = value;
    emit propertyChanged("show_extents");
    setDirty();
}

bool MLSGridVisualization::areExtentsShown() const
{
    return showExtents;
}

//Macro that makes this plugin loadable in ruby, this is optional.
//VizkitQtPlugin(MLSGridVisualization)