/*
 * Fimex, OrthographicProjection.h
 *
 * (C) Copyright 2011, met.no
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
 *  Created on: Mar 01, 2011
 *      Author: Heiko Klein
 */

#ifndef ORTHOGRAPHICPROJECTION_H_
#define ORTHOGRAPHICPROJECTION_H_

/**
 * @headerfile "fimex/coordSys/OrthographicProjection.h"
 */
#include "fimex/coordSys/ProjectionImpl.h"

namespace MetNoFimex
{

class OrthographicProjection: public MetNoFimex::ProjectionImpl
{

public:
    OrthographicProjection();
    virtual ~OrthographicProjection() {}
    static bool acceptsProj4(const std::string& proj4Str);
    static std::vector<CDMAttribute> parametersFromProj4(const std::string& proj4);
protected:
    OrthographicProjection(std::string name) : ProjectionImpl(name, false) {}
    virtual std::ostream& getProj4ProjectionPart(std::ostream& oproj) const;
};

}


#endif /* ORTHOGRAPHICPROJECTION_H_ */
