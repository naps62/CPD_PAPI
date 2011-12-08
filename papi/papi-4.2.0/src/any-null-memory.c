/* 
* File:    any-null-memory.c
* CVS:     $Id: any-null-memory.c,v 1.11 2010/05/07 19:06:22 bsheely Exp $
* Author:  
* Mods:    <your name here>
*          <your email address>
*/

#include "papi.h"
#include "papi_internal.h"
#include <unistd.h>

int
_any_get_memory_info( PAPI_hw_info_t * hw, int id )
{
	( void ) hw;			 /*unused */
	( void ) id;			 /*unused */
	return PAPI_OK;
}

int
_any_get_dmem_info( PAPI_dmem_info_t * d )
{
	d->size = 1;
	d->resident = 2;
	d->high_water_mark = 3;
	d->shared = 4;
	d->text = 5;
	d->library = 6;
	d->heap = 7;
	d->locked = 8;
	d->stack = 9;
	d->pagesize = getpagesize(  );
	return ( PAPI_OK );
}
