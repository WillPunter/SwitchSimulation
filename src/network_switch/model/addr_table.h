/*  addr_table.h

    An address table for address to port mapping for use in any type of switch.
    
    This is simply just a wrapper around a hash table. */

#ifndef ADDRESS_TABLE
#define ADDRESS_TABLE

#include "./../network_common.h"

struct addr_table;

typedef struct addr_table *addr_table_t;

/*  Address table API. */
addr_table_t addr_table_create();

addr_table_t addr_table_free();

#endif