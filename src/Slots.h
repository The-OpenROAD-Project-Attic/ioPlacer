#ifndef __SLOTS_H
#define __SLOTS_H

#include <vector>

#include "Coordinate.h"
#include "Netlist.h"

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

/*! \struct _Section_t
 *  \brief Brief struct description
 *
 *  Detailed description
 */
typedef struct _Section_t {
        slotVector_t sv;
        Coordinate pos;
        Netlist net;
        unsigned cost;
} Section_t;

typedef std::vector<Section_t> sectionVector_t;

#endif /* __SLOTS_H */
