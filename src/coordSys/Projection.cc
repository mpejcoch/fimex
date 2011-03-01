/*
 * Fimex, Projection.cc
 *
 * (C) Copyright 2009, met.no
 *
 * Project Info:  https://wiki.met.no/fimex/start
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 *  Created on: Apr 27, 2010
 *      Author: Heiko Klein
 */

#include "fimex/coordSys/Projection.h"
#include <boost/regex.hpp>
#include <functional>
#include <algorithm>

#include "fimex/interpolation.h"

// list over supported projections
#include "fimex/coordSys/AlbersConicalEqualAreaProjection.h"
#include "fimex/coordSys/AzimuthalEquidistantProjection.h"
#include "fimex/coordSys/LambertAzimuthalEqualAreaProjection.h"
#include "fimex/coordSys/LambertConformalConicProjection.h"
#include "fimex/coordSys/LambertCylindricalEqualAreaProjection.h"
#include "fimex/coordSys/LatitudeLongitudeProjection.h"
#include "fimex/coordSys/MercatorProjection.h"
#include "fimex/coordSys/OrthographicProjection.h"
#include "fimex/coordSys/PolarStereographicProjection.h"
#include "fimex/coordSys/RotatedLatitudeLongitudeProjection.h"
#include "fimex/coordSys/StereographicProjection.h"
#include "fimex/coordSys/TransverseMercatorProjection.h"
#include "fimex/coordSys/VerticalPerspectiveProjection.h"

#include <proj_api.h>


namespace MetNoFimex
{

bool Projection::operator==(const  Projection& b) const
{
    return toString() == b.toString();
}


std::ostream& operator<<(std::ostream& out, const Projection& proj)
{
    out << proj.toString();
    return out;
}

void Projection::convertToLonLat(std::vector<double>& xVals, std::vector<double>& yVals) const throw(CDMException)
{
    // check input
    if (xVals.size() == 0) return;
    if (xVals.size() != yVals.size()) throw CDMException("convertToLonLat: xVals.size() != yVals.size()");

    // convert to radian if required
    if (isDegree()) {
        std::transform(xVals.begin(),
                       xVals.end(),
                       xVals.begin(),
                       std::bind1st(std::multiplies<double>(), DEG_TO_RAD));
        std::transform(yVals.begin(),
                       yVals.end(),
                       yVals.begin(),
                       std::bind1st(std::multiplies<double>(), DEG_TO_RAD));
    }

    // run projection
    std::string fromProj = getProj4String();
    std::string toProj = "+proj=latlong " + getProj4EarthString();
    if (fromProj == toProj) return;
    if (MIFI_OK != mifi_project_values(fromProj.c_str(), toProj.c_str(), &xVals[0], &yVals[0], static_cast<int>(xVals.size()))) {
        throw CDMException("convertToLonLat: unable to convert from '" +fromProj + "' to '"+toProj+"'");
    }

    // convert to degree
    std::transform(xVals.begin(),
                   xVals.end(),
                   xVals.begin(),
                   std::bind1st(std::multiplies<double>(), RAD_TO_DEG));
    std::transform(yVals.begin(),
                   yVals.end(),
                   yVals.begin(),
                   std::bind1st(std::multiplies<double>(), RAD_TO_DEG));

}
void Projection::convertFromLonLat(std::vector<double>& xVals, std::vector<double>& yVals) const throw(CDMException)
{
    // check input
    if (xVals.size() == 0) return;
    if (xVals.size() != yVals.size()) throw CDMException("convertToLonLat: xVals.size() != yVals.size()");

    // convert to radian
    std::transform(xVals.begin(),
                   xVals.end(),
                   xVals.begin(),
                   std::bind1st(std::multiplies<double>(), DEG_TO_RAD));
    std::transform(yVals.begin(),
                   yVals.end(),
                   yVals.begin(),
                   std::bind1st(std::multiplies<double>(), DEG_TO_RAD));

    // run projection
    std::string fromProj = "+proj=latlong " + getProj4EarthString();
    std::string toProj = getProj4String();
    if (fromProj == toProj) return;
    if (MIFI_OK != mifi_project_values(fromProj.c_str(), toProj.c_str(), &xVals[0], &yVals[0], static_cast<int>(xVals.size()))) {
        throw CDMException("convertFromLonLat: unable to convert from '" +fromProj + "' to '"+toProj+"'");
    }

    // convert to degree if required
    if (isDegree()) {
        std::transform(xVals.begin(),
                       xVals.end(),
                       xVals.begin(),
                       std::bind1st(std::multiplies<double>(), RAD_TO_DEG));
        std::transform(yVals.begin(),
                       yVals.end(),
                       yVals.begin(),
                       std::bind1st(std::multiplies<double>(), RAD_TO_DEG));
    }

}


boost::shared_ptr<Projection> Projection::create(std::vector<CDMAttribute> attrs)
{
    std::vector<CDMAttribute>::const_iterator projAttr = std::find_if(attrs.begin(), attrs.end(), CDMNameEqual("grid_mapping_name"));
    boost::shared_ptr<Projection> proj;
    if (projAttr == attrs.end()) {
        proj = boost::shared_ptr<Projection>(new LatitudeLongitudeProjection());
    } else {
        std::string projName(projAttr->getStringValue());
        if (projName == "albers_conical_equal_area") {
            proj = boost::shared_ptr<Projection>(new AlbersConicalEqualAreaProjection());
        } else if (projName == "azimuthal_equidistant") {
                    proj =  boost::shared_ptr<Projection>(new AzimuthalEquidistantProjection());
        } else if (projName == "lambert_azimuthal_equal_area") {
            proj =  boost::shared_ptr<Projection>(new LambertAzimuthalEqualAreaProjection());
        } else if (projName == "lambert_conformal_conic") {
                    proj =  boost::shared_ptr<Projection>(new LambertConformalConicProjection());
        } else if (projName == "lambert_cylindrical_equal_area") {
                    proj =  boost::shared_ptr<Projection>(new LambertCylindricalEqualAreaProjection());
        } else if (projName == "latitude_longitude") {
            proj =  boost::shared_ptr<Projection>(new LatitudeLongitudeProjection());
        } else if (projName == "mercator") {
            proj =  boost::shared_ptr<Projection>(new MercatorProjection());
        } else if (projName == "orthographic") {
            proj =  boost::shared_ptr<Projection>(new OrthographicProjection());
        } else if (projName == "polar_stereographic") {
            proj =  boost::shared_ptr<Projection>(new PolarStereographicProjection());
        } else if (projName == "rotated_latitude_longitude") {
            proj =  boost::shared_ptr<Projection>(new RotatedLatitudeLongitudeProjection());
        } else if (projName == "stereographic") {
            proj =  boost::shared_ptr<Projection>(new StereographicProjection());
        } else if (projName == "transverse_mercator") {
            proj =  boost::shared_ptr<Projection>(new TransverseMercatorProjection());
        } else if (projName == "vertical_perspective") {
            proj =  boost::shared_ptr<Projection>(new VerticalPerspectiveProjection());
        } else {
            throw CDMException("unsupported projection: " + projName);
        }
    }
    proj->addParameters(attrs);
    return proj;
}

boost::shared_ptr<Projection> Projection::createByProj4(const std::string& projStr)
{
    std::vector<CDMAttribute> attrs;
    if (LatitudeLongitudeProjection::acceptsProj4(projStr)) {
        attrs = LatitudeLongitudeProjection::parametersFromProj4(projStr);
    } else if (RotatedLatitudeLongitudeProjection::acceptsProj4(projStr)) {
        attrs = RotatedLatitudeLongitudeProjection::parametersFromProj4(projStr);
    } else if (PolarStereographicProjection::acceptsProj4(projStr)) {
        attrs = PolarStereographicProjection::parametersFromProj4(projStr);
    } else if (StereographicProjection::acceptsProj4(projStr)) {
        attrs = StereographicProjection::parametersFromProj4(projStr);
    } else if (LambertConformalConicProjection::acceptsProj4(projStr)) {
        attrs = LambertConformalConicProjection::parametersFromProj4(projStr);
    } else if (MercatorProjection::acceptsProj4(projStr)) {
        attrs = MercatorProjection::parametersFromProj4(projStr);
    } else if (TransverseMercatorProjection::acceptsProj4(projStr)) {
        attrs = TransverseMercatorProjection::parametersFromProj4(projStr);
    }

    if (attrs.size() == 0) {
        std::cerr << "translation of proj4 '" << projStr << "' to FGDC/CF not supported" << std::endl;
        throw CDMException("proj-string "+projStr+" to FGDC/CF not supported");
    }
    return Projection::create(attrs);
}

}
