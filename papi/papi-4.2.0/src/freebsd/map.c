/****************************/
/* THIS IS OPEN SOURCE CODE */
/****************************/

/* 
* File:    freebsd-map.c
* CVS:     $Id: map.c,v 1.6 2010/04/15 18:32:41 bsheely Exp $
* Author:  Harald Servat
*          redcrash@gmail.com
*/

#include SUBSTRATE
#include "papiStdEventDefs.h"
#include "map.h"

/** See other freebsd-map*.* for more details! **/

Native_Event_Info_t _papi_hwd_native_info[CPU_LAST+1];

void init_freebsd_libpmc_mappings (void)
{
	_papi_hwd_native_info[CPU_UNKNOWN].map = UnkProcessor_map;
	_papi_hwd_native_info[CPU_UNKNOWN].info = UnkProcessor_info;

	_papi_hwd_native_info[CPU_P6].map = P6Processor_map;
	_papi_hwd_native_info[CPU_P6].info = P6Processor_info;

	_papi_hwd_native_info[CPU_P6_C].map = P6_C_Processor_map;
	_papi_hwd_native_info[CPU_P6_C].info = P6_C_Processor_info;

	_papi_hwd_native_info[CPU_P6_2].map = P6_2_Processor_map;
	_papi_hwd_native_info[CPU_P6_2].info = P6_2_Processor_info;

	_papi_hwd_native_info[CPU_P6_3].map = P6_3_Processor_map;
	_papi_hwd_native_info[CPU_P6_3].info = P6_3_Processor_info;

	_papi_hwd_native_info[CPU_P6_M].map = P6_M_Processor_map;
	_papi_hwd_native_info[CPU_P6_M].info = P6_M_Processor_info;

	_papi_hwd_native_info[CPU_P4].map = P4Processor_map;
	_papi_hwd_native_info[CPU_P4].info = P4Processor_info;

	_papi_hwd_native_info[CPU_K7].map = K7Processor_map;
	_papi_hwd_native_info[CPU_K7].info = K7Processor_info;

	_papi_hwd_native_info[CPU_K8].map = K8Processor_map;
	_papi_hwd_native_info[CPU_K8].info = K8Processor_info;

	_papi_hwd_native_info[CPU_ATOM].map = AtomProcessor_map;
	_papi_hwd_native_info[CPU_ATOM].info = AtomProcessor_info;

	_papi_hwd_native_info[CPU_CORE].map = CoreProcessor_map;
	_papi_hwd_native_info[CPU_CORE].info = CoreProcessor_info;

	_papi_hwd_native_info[CPU_CORE2].map = Core2Processor_map;
	_papi_hwd_native_info[CPU_CORE2].info = Core2Processor_info;

	_papi_hwd_native_info[CPU_CORE2EXTREME].map = Core2ExtremeProcessor_map;
	_papi_hwd_native_info[CPU_CORE2EXTREME].info = Core2ExtremeProcessor_info;

	_papi_hwd_native_info[CPU_COREI7].map = i7Processor_map;
	_papi_hwd_native_info[CPU_COREI7].info = i7Processor_info;

	_papi_hwd_native_info[CPU_LAST].map = NULL;
	_papi_hwd_native_info[CPU_LAST].info = NULL;
}

int freebsd_substrate_number_of_events (int processortype)
{
	int counter = 0;

	while (_papi_hwd_native_info[processortype].info[counter].name != NULL)
		counter++;

	return counter;
}
