#ifndef __SLOTS_H
#define __SLOTS_H

#include <vector>

#include "Coordinate.h"

/*! \struct _Slot_t
 *  \brief Brief struct description
 *
 *  Detailed description
 */
struct _Slot_t {
        bool current;
        bool visited;
        Coordinate pos;
        /* IOPin* pin; // TODO: do we need this? */
} typedef Slot_t;

typedef std::vector<Slot_t> slotVector_t;


#endif /* __SLOTS_H */
