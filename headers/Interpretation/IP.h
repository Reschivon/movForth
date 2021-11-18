//
// Created by reschivon on 11/17/21.
//

#ifndef MOVFORTH_IP_H
#define MOVFORTH_IP_H

#include <list>
#include "iData.h"

namespace mov {
    /**
     * Iterator over a word's definition
     */

    typedef std::list<iData>::iterator& IP;

}

#endif //MOVFORTH_IP_H
