//
//  factory.hpp
//  enki
//
//  Created by Sébastien POUTEAU on 09/03/2017.
//
//

#ifndef __FACTORY_H
#define __FACTORY_H

#include <stdio.h>
#include "PhysicalEngine.h"


class Factory
{
public:
    enum class TypeObject { PHYS_OBJ, THYMIO2, EPUCK, SBOT, MARXBOT, KHEPERA  };

    Enki::PhysicalObject* initObject(const std::vector<std::string> tabObj) const;
};

#endif /* factory.h */
