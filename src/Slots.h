#ifndef __SLOTS_H
#define __SLOTS_H

#include <vector>

#include "Coordinate.h"

struct slot {
        bool current;
        bool visited;
        Coordinate pos;
} typedef slot_t;
typedef std::vector<slot_t> slotVector_t;

#endif /* __SLOTS_H */
