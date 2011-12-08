/*
* File:    papi_libpfm4_events.c
* Author:  Vince Weaver vweaver1 @ eecs.utk.edu
*          based heavily on existing papi_libpfm3_events.c
*/

#include <string.h>

#include "papi.h"
#include "papi_internal.h"

#include "papi_libpfm_events.h"

#include "perfmon/pfmlib.h"
#include "perfmon/pfmlib_perf_event.h"

/* Whis is this here? */
volatile unsigned int _papi_hwd_lock_data[PAPI_MAX_LOCK];

#define NATIVE_EVENT_CHUNK 1024

struct native_event_t {
  int component;
  char *pmu;
  int papi_code;
  int libpfm4_idx;
  char *allocated_name;
  char *base_name;
  char *pmu_plus_name;
  int users;
  long long config;
  long long config1;
  int type;
};

static struct native_event_t *native_events;

static int num_native_events=0;
static int allocated_native_events=0;

static pfm_pmu_info_t default_pmu;


/** @class  find_existing_event
 *  @brief  looks up an event, returns it if it exists
 *
 *  @param[name] 
 *             -- name of the event
 *
 *  @returns returns a struct native_event_t *, or NULL if event not found
 *
 */

static struct native_event_t *find_existing_event(char *name) {

  int i;
  struct native_event_t *temp_event=NULL;

  SUBDBG("Looking for %s in %d events\n",name,num_native_events);

  _papi_hwi_lock( NAMELIB_LOCK );

  for(i=0;i<num_native_events;i++) {

    if (!strcmp(name,native_events[i].allocated_name)) {
      SUBDBG("Found %s (%x %x)\n",
	     native_events[i].allocated_name,
	     native_events[i].libpfm4_idx,
	     native_events[i].papi_code);
       temp_event=&native_events[i];
       break;
    }
  }
  _papi_hwi_unlock( NAMELIB_LOCK );

  if (!temp_event) SUBDBG("%s not allocated yet\n",name);
  return temp_event;
}

/** @class  find_existing_event_by_number
 *  @brief  looks up a native_event_t given its PAPI event code
 *
 *  @param[in] eventnum
 *             -- a PAPI event number
 *
 *  @returns returns a struct native_event_t *, or NULL if event not found
 *
 */

static struct native_event_t *find_existing_event_by_number(int eventnum) {

  int i;
  struct native_event_t *temp_event=NULL;

  _papi_hwi_lock( NAMELIB_LOCK );

  /* Simple linear search for now */
  for(i=0;i<num_native_events;i++) {
    if (eventnum==native_events[i].papi_code) {
       temp_event=&native_events[i];
       break;
    }
  }
  _papi_hwi_unlock( NAMELIB_LOCK );

  SUBDBG("Found %p for %x\n",temp_event,eventnum);

  return temp_event;
}


/** @class  find_event_no_aliases
 *  @brief  looks up an event, avoiding aliases, returns it if it exists
 *
 *  @param[name] 
 *             -- name of the event
 *
 *  @returns returns libpfm4 number of the event or PFM_ERR_NOTFOUND
 *
 */

static int find_event_no_aliases(char *name) {

    int j,i, ret;
    pfm_pmu_info_t pinfo;
    pfm_event_info_t event_info;
    char full_name[BUFSIZ];

    SUBDBG("Looking for %s\n",name);

    pfm_for_all_pmus(j) {

       memset(&pinfo,0,sizeof(pfm_pmu_info_t));
       pfm_get_pmu_info(j, &pinfo);
       if (!pinfo.is_present) {
          SUBDBG("PMU %d not present, skipping...\n",j);
          continue;
       }

       SUBDBG("Looking in pmu %d\n",j);   
       i = pinfo.first_event; 
       while(1) {
          memset(&event_info,0,sizeof(pfm_event_info_t));
          ret=pfm_get_event_info(i, PFM_OS_PERF_EVENT, &event_info);
	  if (ret<0) break;
	
	  sprintf(full_name,"%s::%s",pinfo.name,event_info.name);

	  if (!strcmp(name,full_name)) {
	     SUBDBG("FOUND %s %s %x\n",name,full_name,i);
	     return i;
	  }

	  if (!strcmp(name,event_info.name)) {
	     SUBDBG("FOUND %s %s %x\n",name,event_info.name,i);
	     return i;
	  }
	  i++;
       }
    }
    return PFM_ERR_NOTFOUND;
}

/** @class  find_next_no_aliases
 *  @brief  finds the event after this one, avoiding any event alias issues
 *
 *  @param[in] code
 *             -- a libpfm4 event number
 *
 *  @returns returns a libpfm4 event number for next event
 *           or a libpfm4 error code
 *
 */

static int find_next_no_aliases(int code) {

  int current_pmu=0,current_event=0;
  pfm_err_t ret;
  pfm_pmu_info_t pinfo;
  pfm_event_info_t event_info;

  /* Clear the structures, as libpfm4 requires it */
  memset(&event_info,0,sizeof(pfm_event_info_t));

  ret=pfm_get_event_info(code, PFM_OS_PERF_EVENT, &event_info);
  if (ret!=PFM_SUCCESS) {
     return ret;
  }

  current_pmu=event_info.pmu;
  current_event=code+1;

  SUBDBG("Current is %x guessing next is %x\n",code,current_event);

  while(1) {

     memset(&event_info,0,sizeof(pfm_event_info_t));
     ret=pfm_get_event_info(current_event, PFM_OS_PERF_EVENT, &event_info);
     if (ret==PFM_SUCCESS) {
        SUBDBG("Returning %x\n",current_event);
        return current_event;
     }

     /* next event not found, so try moving to next PMU */

     while(1) {

        current_pmu++;
        SUBDBG("Incrementing PMU: %x\n",current_pmu);

	/* Off the end, so done iterating */
        if (current_pmu>PFM_PMU_MAX) {
           return PFM_ERR_NOTFOUND;
        }
 
        memset(&pinfo,0,sizeof(pfm_pmu_info_t));
        pfm_get_pmu_info(current_pmu, &pinfo);
        if (pinfo.is_present) break;
     }

     current_event=pinfo.first_event;

  }

}


/** @class  allocate_native_event
 *  @brief  Allocates a native event
 *
 *  @param[in] name
 *             -- name of the event
 *  @param[in] event_idx
 *             -- libpfm4 identifier for the event
 *
 *  @returns returns a native_event_t or NULL
 *
 */

static struct native_event_t *allocate_native_event(char *name, 
						    int event_idx) {

  int new_event;

  pfm_err_t ret;
  unsigned int i;
  char *base_start;
  pfm_event_info_t info;
  pfm_pmu_info_t pinfo;
  char base[BUFSIZ],pmuplusbase[BUFSIZ];
  char fullname[BUFSIZ];

  pfm_perf_encode_arg_t perf_arg;

  /* get the event name from libpfm */
  memset(&info,0,sizeof(pfm_event_info_t));
  ret = pfm_get_event_info(event_idx, PFM_OS_PERF_EVENT, &info);
  if (ret!=PFM_SUCCESS) {
     return NULL;
  }

  /* get the PMU info */
  memset(&pinfo,0,sizeof(pfm_pmu_info_t));
  pfm_get_pmu_info(info.pmu, &pinfo);

  /* calculate the base name, meaning strip off pmu identifier */
  strncpy(base,name,BUFSIZ);
  i=0;
  base_start=base;
  while(i<strlen(base)) {
    if (base[i]==':') {
      if (base[i+1]==':') {
          i++;
	  base_start=&base[i+1];
      }
      else {
	base[i]=0;
      }
    }
    i++;
  }

  /* add the event */
  _papi_hwi_lock( NAMELIB_LOCK );

  new_event=num_native_events;

  native_events[new_event].base_name=strdup(base_start);

  sprintf(fullname,"%s::%s",pinfo.name,info.name);
  native_events[new_event].pmu_plus_name=strdup(fullname);

  sprintf(pmuplusbase,"%s::%s",pinfo.name,base_start);

  native_events[new_event].component=0;
  native_events[new_event].pmu=strdup(pinfo.name);
  native_events[new_event].papi_code=new_event | PAPI_NATIVE_MASK;
    
  native_events[new_event].libpfm4_idx=find_event_no_aliases(pmuplusbase);

  SUBDBG("Using %x as index instead of %x for %s\n",
	 native_events[new_event].libpfm4_idx,event_idx,pmuplusbase);

  native_events[new_event].allocated_name=strdup(name);

  /* is this needed? */
  native_events[new_event].users=0;


  /* use name of the event to get the perf_event encoding */

  /* clear the attribute structure */
  memset(&perf_arg,0,sizeof(pfm_perf_encode_arg_t));
  perf_arg.attr=calloc(1,sizeof(struct perf_event_attr));

  ret = pfm_get_os_event_encoding(name, 
  				  PFM_PLM0 | PFM_PLM3, 
                                  PFM_OS_PERF_EVENT_EXT, 
  				  &perf_arg);
  if (ret!=PFM_SUCCESS) {
     /* should do something! */
  }
  
  native_events[new_event].config=perf_arg.attr->config;
  native_events[new_event].config1=perf_arg.attr->config1;
  native_events[new_event].type=perf_arg.attr->type;

  SUBDBG( "pe_event: config 0x%"PRIx64" config1 0x%"PRIx64" type 0x%"PRIx32"\n", 
          perf_arg.attr->config, 
	  perf_arg.attr->config1,
	  perf_arg.attr->type);

  SUBDBG("Creating event %s with papi %x perfidx %x\n",
	 name,
	 native_events[new_event].papi_code,
	 native_events[new_event].libpfm4_idx);

  num_native_events++;

  /* If we've allocated too many native events, then allocate more room */
  if (num_native_events >= allocated_native_events) {

     SUBDBG("Allocating more room for native events (%d %ld)\n",
	    (allocated_native_events+NATIVE_EVENT_CHUNK),
	    (long)sizeof(struct native_event_t) *
	    (allocated_native_events+NATIVE_EVENT_CHUNK));

     native_events=realloc(native_events,
			   sizeof(struct native_event_t) * 
			   (allocated_native_events+NATIVE_EVENT_CHUNK));
     allocated_native_events+=NATIVE_EVENT_CHUNK;
  }

  _papi_hwi_unlock( NAMELIB_LOCK );

  if (native_events==NULL) {
     return NULL;
  }

  return &native_events[new_event];

}

/** @class  find_max_umask
 *  @brief  finds the highest-numbered umask found in an event
 *
 *  @param[in] *current_event
 *             -- a struct native_event_t for the event in question
 *
 *  @returns returns the highest-number umask 
 *           or a libpfm4 error code
 *  @retval PFM_ERR_UMASK -- event name has no umask
 *  @retval PFM_ERR_NOTFOUND -- event not found
 *  @retval PFM_ERR_ATTR -- attribute name not found by libpfm4
 *
 */

static int find_max_umask(struct native_event_t *current_event) {

  pfm_event_attr_info_t ainfo;
  char *b;
  int a, ret, max =0;
  pfm_event_info_t info;
  char event_string[BUFSIZ],*ptr;

  SUBDBG("Trying to find max umask in %s\n",current_event->allocated_name);

  strcpy(event_string,current_event->allocated_name);

  /* Skip leading :: delimited PMU name and point to first umask */
  if (strstr(event_string,"::")) {
     ptr=strstr(event_string,"::");
     ptr+=2;
     b=strtok(ptr,":");
  }
  else {
     b=strtok(event_string,":");
  }

  if (!b) {
     SUBDBG("No colon!\n");
     return PFM_ERR_UMASK; /* Must be this value!! */
  }

  memset(&info,0,sizeof(pfm_event_info_t));
  ret = pfm_get_event_info(current_event->libpfm4_idx, 
			   PFM_OS_PERF_EVENT, &info);
  if (ret!=PFM_SUCCESS) {
     SUBDBG("get_event_info failed\n");
     return PFM_ERR_NOTFOUND;
  }

  /* skip first */
  b=strtok(NULL,":");
  if (!b) {
     SUBDBG("Skipping first failed\n");
     return PFM_ERR_UMASK; /* Must be this value!! */
  }

  while(b) {
    a=0;
    while(1) {

      SUBDBG("get_event_attr %x %d %p\n",current_event->libpfm4_idx,a,&ainfo);

      memset(&ainfo,0,sizeof(pfm_event_attr_info_t));

      ret = pfm_get_event_attr_info(current_event->libpfm4_idx, a, 
				    PFM_OS_PERF_EVENT, &ainfo);

      if (ret != PFM_SUCCESS) {
	SUBDBG("get_event_attr failed %s\n",pfm_strerror(ret));
	return ret;
      }

      SUBDBG("Trying %s with %s\n",ainfo.name,b);

      if (!strcasecmp(ainfo.name, b)) {
	SUBDBG("Found %s %d\n",b,a);
	if (a>max) max=a;
	goto found_attr;
      }
      a++;
    }

    SUBDBG("attr=%s not found for event %s\n", b, info.name);

    return PFM_ERR_ATTR;

found_attr:

    b=strtok(NULL,":");
  }

  SUBDBG("Found max %d\n", max);

  return max;
}

/** @class  get_event_first_active
 *  @brief  return the first available event that's on an active PMU
 *
 *  @returns returns a libpfm event number
 *  @retval PAPI_ENOEVENT  Could not find an event
 *
 */

static int
get_event_first_active(void)
{
  int pidx, pmu_idx, ret;

  pfm_pmu_info_t pinfo;

  pmu_idx=0;

  while(pmu_idx<PFM_PMU_MAX) {

    /* clear the PMU structure (required by libpfm4) */
    memset(&pinfo,0,sizeof(pfm_pmu_info_t));
    ret=pfm_get_pmu_info(pmu_idx, &pinfo);

    if ((ret==PFM_SUCCESS) && pinfo.is_present) {

      pidx=pinfo.first_event;

      return pidx;

    }

    pmu_idx++;

  }

  return PAPI_ENOEVNT;
  
}



/** @class  convert_libpfm4_to_string
 *  @brief  convert a libpfm event value to an event name
 *
 *  @param[in] code
 *        -- libpfm4 code to convert
 *  @param[out] **event_name
 *        -- pointer to a string pointer that will be allocated
 *
 *  @returns returns a libpfm error condition
 *
 *  If in the default PMU, then no leading PMU indicator.
 *  Otherwise includes the PMU name.
 */

static int
convert_libpfm4_to_string( int code, char **event_name)
{

  int ret;
  pfm_event_info_t gete;
  pfm_pmu_info_t pinfo;
  char name[BUFSIZ];

  SUBDBG("ENTER %x\n",code);

  /* Clear structures, as wanted by libpfm4 */
  memset( &gete, 0, sizeof (pfm_event_info_t) );

  ret=pfm_get_event_info(code, PFM_OS_PERF_EVENT, &gete);
  if (ret!=PFM_SUCCESS) {
     return ret;
  }

  memset( &pinfo, 0, sizeof(pfm_pmu_info_t) );
  ret=pfm_get_pmu_info(gete.pmu, &pinfo);
  if (ret!=PFM_SUCCESS) {
     return ret;
  }

  /* Only prepend PMU name if not on the "default" PMU */

  if (pinfo.pmu == default_pmu.pmu) {
     *event_name=strdup(gete.name);
  }
  else {
     sprintf(name,"%s::%s",pinfo.name,gete.name);
     *event_name=strdup(name);
  }

  SUBDBG("Found name: %s\n",*event_name);

  return PFM_SUCCESS;

}


/** @class  convert_pfmidx_to_native
 *  @brief  convert a libpfm event value to a PAPI event value
 *
 *  @param[in] code
 *        -- libpfm4 code to convert
 *  @param[out] *PapiEventCode
 *        -- PAPI event code
 *
 *  @returns returns a PAPI error code
 *
 */

static int convert_pfmidx_to_native(int code, unsigned int *PapiEventCode) {

  int ret;
  char *name=NULL;

  ret=convert_libpfm4_to_string( code, &name);
  if (ret!=PFM_SUCCESS) {
     return _papi_libpfm_error(ret);
  }

  SUBDBG("Converted %x to %s\n",code,name);

  ret=_papi_libpfm_ntv_name_to_code(name,PapiEventCode);

  SUBDBG("Converted %s to event %x\n",name,*PapiEventCode);

  if (name) free(name);

  return ret;

}


/** @class  find_next_umask
 *  @brief  finds the next umask
 *
 *  @param[in] *current_event
 *             -- a struct native_event_t for the event in question
 *  @param[in] current
 *             -- number of current highest umask.  -1 indicates
 *                start from scratch.
 *  @parm[out] umask_name
 *             -- name of next umask
 *
 *  @returns returns next umask value or a libpfm4 error code
 *  @retval PFM_ERR_NOTFOUND -- event not found
 *  @retval PFM_ERR_ATTR -- attribute name not found by libpfm4
 *  @retval PFM_ERR_NOMEM -- out of memory when malloc()
 *
 */

static int find_next_umask(struct native_event_t *current_event,
                           int current,char *umask_name) {

  char temp_string[BUFSIZ];
  pfm_event_info_t event_info;
  pfm_event_attr_info_t *ainfo=NULL;
  int num_masks=0;
  pfm_err_t ret;
  int i;

  /* get number of attributes */

  memset(&event_info, 0, sizeof(event_info));
  ret=pfm_get_event_info(current_event->libpfm4_idx, 
			 PFM_OS_PERF_EVENT, &event_info);
  if (ret!=PFM_SUCCESS) {
     return ret;
  }
	
  SUBDBG("%d possible attributes for event %s\n",
	 event_info.nattrs,
	 event_info.name);

  ainfo = malloc(event_info.nattrs * sizeof(*ainfo));
  if (!ainfo) {
     return PFM_ERR_NOMEM;
  }

  pfm_for_each_event_attr(i, &event_info) {

     ainfo[i].size = sizeof(*ainfo);

     ret = pfm_get_event_attr_info(event_info.idx, i, PFM_OS_PERF_EVENT, 
				   &ainfo[i]);
     if (ret != PFM_SUCCESS) {
        SUBDBG("Not found\n");
        if (ainfo) free(ainfo);
	return PFM_ERR_NOTFOUND;
     }

     if (ainfo[i].type == PFM_ATTR_UMASK) {
	SUBDBG("nm %d looking for %d\n",num_masks,current);
	if (num_masks==current+1) {	  
	   SUBDBG("Found attribute %d: %s type: %d\n",
		  i,ainfo[i].name,ainfo[i].type);
	
           sprintf(temp_string,"%s",ainfo[i].name);
           strncpy(umask_name,temp_string,BUFSIZ);

	   if (ainfo) free(ainfo);
	   return current+1;
	}
	num_masks++;
     }
  }

  if (ainfo) free(ainfo);
  return PFM_ERR_ATTR;

}



/***********************************************************/
/* Exported functions                                      */
/***********************************************************/


/** @class  _papi_libpfm_error
 *  @brief  convert libpfm error codes to PAPI error codes
 *
 *  @param[in] pfm_error
 *             -- a libpfm4 error code
 *
 *  @returns returns a PAPI error code
 *
 */

int
_papi_libpfm_error( int pfm_error ) {

  switch ( pfm_error ) {
  case PFM_SUCCESS:      return PAPI_OK;       /* success */
  case PFM_ERR_NOTSUPP:  return PAPI_ENOSUPP;  /* function not supported */
  case PFM_ERR_INVAL:    return PAPI_EINVAL;   /* invalid parameters */
  case PFM_ERR_NOINIT:   return PAPI_ENOINIT;  /* library not initialized */
  case PFM_ERR_NOTFOUND: return PAPI_ENOEVNT;  /* event not found */
  case PFM_ERR_FEATCOMB: return PAPI_ECOMBO;   /* invalid combination of features */
  case PFM_ERR_UMASK:    return PAPI_EATTR;    /* invalid or missing unit mask */
  case PFM_ERR_NOMEM:    return PAPI_ENOMEM;   /* out of memory */
  case PFM_ERR_ATTR:     return PAPI_EATTR;    /* invalid event attribute */
  case PFM_ERR_ATTR_VAL: return PAPI_EATTR;    /* invalid event attribute value */
  case PFM_ERR_ATTR_SET: return PAPI_EATTR;    /* attribute value already set */
  case PFM_ERR_TOOMANY:  return PAPI_ECOUNT;   /* too many parameters */
  case PFM_ERR_TOOSMALL: return PAPI_ECOUNT;   /* parameter is too small */
  default: return PAPI_EINVAL;
  }
}

/** @class  _papi_libpfm_ntv_name_to_code
 *  @brief  Take an event name and convert it to an event code.
 *
 *  @param[in] *name
 *        -- name of event to convert
 *  @param[out] *event_code
 *        -- pointer to an integer to hold the event code
 *
 *  @retval PAPI_OK event was found and an event assigned
 *  @retval PAPI_ENOEVENT event was not found
 */

int
_papi_libpfm_ntv_name_to_code( char *name, unsigned int *event_code )
{

  int actual_idx;
  struct native_event_t *our_event;

  SUBDBG( "Converting %s\n", name);

  our_event=find_existing_event(name);

  if (our_event==NULL) {

     /* event currently doesn't exist, so try to find it */
     /* using libpfm4                                    */

     SUBDBG("Using pfm to look up event %s\n",name);
     actual_idx=pfm_find_event(name);
     if (actual_idx<0) {
        return _papi_libpfm_error(actual_idx);
     }

     SUBDBG("Using %x as the index\n",actual_idx);

     /* We were found in libpfm4, so allocate our copy of the event */

     our_event=allocate_native_event(name,actual_idx);
  }

  if (our_event!=NULL) {      
     *event_code=our_event->papi_code;
     SUBDBG("Found code: %x\n",*event_code);
     return PAPI_OK;
  }

  /* Failure here means allocate_native_event failed */

  SUBDBG("Event %s not found\n",name);

  return PAPI_ENOEVNT;   

}


/** @class  _papi_libpfm_ntv_code_to_name
 *  @brief  Take an event code and convert it to a name
 *
 *  @param[in] EventCode
 *        -- PAPI event code
 *  @param[out] *ntv_name
 *        -- pointer to a string to hold the name
 *  @param[in] len
 *        -- length of ntv_name string
 *
 *  @retval PAPI_OK       The event was found and converted to a name
 *  @retval PAPI_ENOEVENT The event does not exist
 *  @retval PAPI_EBUF     The event name was too big for ntv_name
 */

int
_papi_libpfm_ntv_code_to_name(unsigned int EventCode, char *ntv_name, int len )
{

        struct native_event_t *our_event;

        SUBDBG("ENTER %x\n",EventCode);

        our_event=find_existing_event_by_number(EventCode);
	if (our_event==NULL) {
	  return PAPI_ENOEVNT;
	}

	strncpy(ntv_name,our_event->allocated_name,len);

	if (strlen(our_event->allocated_name) > (unsigned)len) {
	   return PAPI_EBUF;
	}

	return PAPI_OK;
}


/** @class  _papi_libpfm_ntv_code_to_descr
 *  @brief  Take an event code and convert it to a description
 *
 *  @param[in] EventCode
 *        -- PAPI event code
 *  @param[out] *ntv_descr
 *        -- pointer to a string to hold the description
 *  @param[in] len
 *        -- length of ntv_descr string
 *
 *  @retval PAPI_OK       The event was found and converted to a description
 *  @retval PAPI_ENOEVENT The event does not exist
 *  @retval PAPI_EBUF     The event name was too big for ntv_descr
 *
 *  Return the event description.
 *  If the event has umasks, then include ", masks" and the
 *  umask descriptions follow, separated by commas.
 */


int
_papi_libpfm_ntv_code_to_descr( unsigned int EventCode, char *ntv_descr, int len )
{
  int ret,a,first_mask=1;
  char *eventd, *tmp=NULL;
  pfm_event_info_t gete;

  pfm_event_attr_info_t ainfo;
  char *b;
  char event_string[BUFSIZ],*ptr;

  struct native_event_t *our_event;

  SUBDBG("ENTER %x\n",EventCode);

  our_event=find_existing_event_by_number(EventCode);
  if (our_event==NULL) {
     return PAPI_ENOEVNT;
  }

  SUBDBG("Getting info on %x\n",our_event->libpfm4_idx);
	
  /* libpfm requires the structure be zeroed */
  memset( &gete, 0, sizeof ( gete ) );

  ret=pfm_get_event_info(our_event->libpfm4_idx, PFM_OS_PERF_EVENT, &gete);
  if (ret<0) {
     SUBDBG("Return=%d\n",ret);
     return _papi_libpfm_error(ret);
  }

  eventd=strdup(gete.desc);

  tmp = ( char * ) malloc( strlen( eventd ) + 1 );
  if ( tmp == NULL ) {
     free( eventd );
     return PAPI_ENOMEM;
  }
	
  tmp[0] = '\0';
  strcat( tmp, eventd );
  free( eventd );
	
  /* Handle Umasks */

  /* attributes concactinated onto end of descr separated by ", masks" */
  /* then comma separated */

  strcpy(event_string,our_event->allocated_name);

  /* Point to first umask */

  /* Skip the pmu name :: if one exists */
  if (strstr(event_string,"::")) {
     ptr=strstr(event_string,"::");
     ptr+=2;
     b=strtok(ptr,":");
  }
  else {
     b=strtok(event_string,":");
  }
  
  /* if no umask, then done */
  if (!b) {
     SUBDBG("No colon!\n"); /* no umask */
     goto descr_in_tmp;
  }

  /* skip first */
  b=strtok(NULL,":");
  if (!b) {
     SUBDBG("Skipping first failed\n");
     goto descr_in_tmp;
  }

  while(b) {
    a=0;
    while(1) {

      SUBDBG("get_event_attr %x %p\n",our_event->libpfm4_idx,&ainfo);

      memset(&ainfo,0,sizeof(pfm_event_attr_info_t));

      ret = pfm_get_event_attr_info(our_event->libpfm4_idx, a, 
				    PFM_OS_PERF_EVENT, &ainfo);
      if (ret != PFM_SUCCESS) {
	SUBDBG("get_event_attr failed %s\n",pfm_strerror(ret));
	return _papi_libpfm_error(ret);
      }

      SUBDBG("Trying %s with %s\n",ainfo.name,b);

      if (!strcasecmp(ainfo.name, b)) {
	int new_length;

	 SUBDBG("Found %s\n",b);
	 new_length=strlen(ainfo.desc);

	 if (first_mask) {
	    tmp=realloc(tmp,strlen(tmp)+new_length+1+strlen(", masks:"));
	    strcat(tmp,", masks:");
	    first_mask=0;
	 }
	 else {
	    tmp=realloc(tmp,strlen(tmp)+new_length+1+strlen(","));
	    strcat(tmp,",");
	 }
	 strcat(tmp,ainfo.desc);

	 goto found_attr;
      }
      a++;
    }

    SUBDBG("attr=%s not found for event %s\n", b, ainfo.name);

    return PAPI_EATTR;

found_attr:

    b=strtok(NULL,":");
  }

  /* We are done and the description to copy is in tmp */
descr_in_tmp:
	strncpy( ntv_descr, tmp, ( size_t ) len );
	if ( ( int ) strlen( tmp ) > len - 1 )
		ret = PAPI_EBUF;
	else
		ret = PAPI_OK;
	free( tmp );

	SUBDBG("PFM4 Code: %x %s\n",EventCode,ntv_descr);

	return ret;
}


/** @class  _papi_libpfm_ntv_enum_events
 *  @brief  Walk through all events in a pre-defined order
 *
 *  @param[in,out] *PapiEventCode
 *        -- PAPI event code to start with
 *  @param[in] modifier
 *        -- describe how to enumerate
 *
 *  @retval PAPI_OK       The event was found and converted to a description
 *  @retval PAPI_ENOEVENT The event does not exist
 *  @retval PAPI_ENOIMPL  The enumeration method requested in not implemented
 *
 */

int
_papi_libpfm_ntv_enum_events( unsigned int *PapiEventCode, int modifier )
{
	int code,ret;
	struct native_event_t *current_event;

        SUBDBG("ENTER\n");

	/* return first event if so specified */
	if ( modifier == PAPI_ENUM_FIRST ) {

	   unsigned int papi_event=0;

           SUBDBG("ENUM_FIRST\n");

	   code=get_event_first_active();
	   if (code < 0 ) {
	      return code;
	   }

	   /* convert the libpfm4 event to a PAPI event */
	   ret=convert_pfmidx_to_native(code, &papi_event);

	   *PapiEventCode=(unsigned int)papi_event;

           SUBDBG("FOUND %x (from %x) ret=%d\n",*PapiEventCode,code,ret);

	   return ret;
	}

	/* If we get this far, we're looking for a        */
	/* next-event.  So gather info on the current one */
	current_event=find_existing_event_by_number(*PapiEventCode);
	if (current_event==NULL) {
           SUBDBG("EVENTS %x not found\n",*PapiEventCode);
	   return PAPI_ENOEVNT;
	}


	/* Handle looking for the next event */

	if ( modifier == PAPI_ENUM_EVENTS ) {

	   unsigned int papi_event=0;

	   SUBDBG("PAPI_ENUM_EVENTS %x\n",*PapiEventCode);

	   code=current_event->libpfm4_idx;

	   ret=find_next_no_aliases(code);
	   SUBDBG("find_next_no_aliases() Returned %x\n",ret);
	   if (ret<0) {
	      return ret;
	   }

	   /* Convert libpfm4 event code to PAPI event code */
	   ret=convert_pfmidx_to_native(ret, &papi_event);
	   if (ret<0) {
	       SUBDBG("Couldn't convert to native %d %s\n",
		      ret,PAPI_strerror(ret));
	       return ret;
	   }

	   *PapiEventCode=(unsigned int)papi_event;

           SUBDBG("Returning PAPI_OK\n");

	   return ret;
	}

	/* We don't handle PAPI_NTV_ENUM_UMASK_COMBOS */
	if ( modifier == PAPI_NTV_ENUM_UMASK_COMBOS ) {
	   return PAPI_ENOIMPL;
	} 

	/* Enumerate PAPI_NTV_ENUM_UMASKS (umasks on an event) */
	if ( modifier == PAPI_NTV_ENUM_UMASKS ) {

	   int max_umask,next_umask;
	   char umask_string[BUFSIZ],new_name[BUFSIZ];

	   SUBDBG("Finding maximum mask in event %s\n",
		  		  current_event->allocated_name);

	   max_umask=find_max_umask(current_event);
	   SUBDBG("Found max %d\n",max_umask);

	   if (max_umask<0) {
	      if (max_umask==PFM_ERR_UMASK) {
	         max_umask=-1; /* needed for find_next_umask() to work */
		               /* indicates the event as passed had no */
		               /* umask in it.                         */
	      }
	      else {
	         return _papi_libpfm_error(max_umask);
	      }
	   }

	   next_umask=find_next_umask(current_event,max_umask,
				      umask_string);

	   SUBDBG("Found next %d\n",next_umask);
	   if (next_umask>=0) {

	      unsigned int papi_event;

	      sprintf(new_name,"%s:%s",current_event->base_name,
		     umask_string);
     
              ret=_papi_libpfm_ntv_name_to_code(new_name,&papi_event);
	      if (ret!=PAPI_OK) {
		 return PAPI_ENOEVNT;
	      }

	      *PapiEventCode=(unsigned int)papi_event;
	      SUBDBG("found code %x\n",*PapiEventCode);
	      return PAPI_OK;
	   }

	   SUBDBG("couldn't find umask\n");

	   return _papi_libpfm_error(next_umask);

	}
	
	/* An unknown enumeration method was indicated */

	return PAPI_ENOIMPL;
}



/** @class  _papi_libpfm_ntv_code_to_bits
 *  @brief  Convert an EventCode to HW specific event info
 *
 *  @param[in] EventCode
 *        -- PAPI event code
 *  @param[out] *bits
 *        -- an opaque structure that holds HW specific event info
 *
 *  @retval PAPI_OK       We always return PAPI_OK
 *
 */

int
_papi_libpfm_ntv_code_to_bits( unsigned int EventCode, hwd_register_t *bits )
{

  /* on libpfm4 we just use the PAPI EventCode as the value for "bits" */

  *(int *)bits=EventCode;

  return PAPI_OK;
}


/** @class  _papi_libpfm_ntv_bits_to_info
 *  @brief  Convert native bits to a descriptive string describing registers
 *
 *  @param[in] *bits
 *  @param[out] *names
 *  @param[out] *values
 *  @param[in] name_len
 *  @param[in] count
 *
 *  @retval PAPI_OK       We always return PAPI_OK
 *
 */

int
_papi_libpfm_ntv_bits_to_info( hwd_register_t *bits, char *names,
			       unsigned int *values, int name_len, int count )
{

  (void)bits;
  (void)names;
  (void)values;
  (void)name_len;
  (void)count;

  /* We do not support this on libpfm4 */

  return PAPI_OK;

}


/** @class  _papi_libpfm_shutdown
 *  @brief  Shutdown any initialization done by the libpfm4 code
 *
 *  @retval PAPI_OK       We always return PAPI_OK
 *
 */

int 
_papi_libpfm_shutdown(void) {

  SUBDBG("shutdown\n");

  /* clean out and free the native events structure */
  _papi_hwi_lock( NAMELIB_LOCK );
  memset(native_events,0,
	 sizeof(struct native_event_t)*allocated_native_events);
  num_native_events=0;
  allocated_native_events=0;
  free(native_events);
  _papi_hwi_unlock( NAMELIB_LOCK );

  return PAPI_OK;
}


/** @class  _papi_libpfm_init
 *  @brief  Initialize the libpfm4 code
 *
 *  @retval PAPI_OK       We initialized correctly
 *  @retval PAPI_ESBSTR   There was an error initializing the substrate
 *
 */

int
_papi_libpfm_init(papi_vector_t *MY_VECTOR) {

   int detected_pmus=0, found_default=0;
   int i, version;
   pfm_err_t retval;
   unsigned int ncnt;
   pfm_pmu_info_t pinfo;

   /* The following checks the version of the PFM library
      against the version PAPI linked to... */
   if ( ( retval = pfm_initialize(  ) ) != PFM_SUCCESS ) {
      PAPIERROR( "pfm_initialize(): %s", pfm_strerror( retval ) );
      return PAPI_ESBSTR;
   }

   /* get the libpfm4 version */
   SUBDBG( "pfm_get_version()\n");
   if ( (version=pfm_get_version( )) < 0 ) {
      PAPIERROR( "pfm_get_version(): %s", pfm_strerror( retval ) );
      return PAPI_ESBSTR;
   }

   /* Set the version */
   sprintf( MY_VECTOR->cmp_info.support_version, "%d.%d",
	    PFM_MAJ_VERSION( version ), PFM_MIN_VERSION( version ) );

   /* Complain if the compiled-against version doesn't match current version */
   if ( PFM_MAJ_VERSION( version ) != PFM_MAJ_VERSION( LIBPFM_VERSION ) ) {
      PAPIERROR( "Version mismatch of libpfm: compiled %x vs. installed %x\n",
				   PFM_MAJ_VERSION( LIBPFM_VERSION ),
				   PFM_MAJ_VERSION( version ) );
      return PAPI_ESBSTR;
   }

   /* allocate the native event structure */

   num_native_events=0;

   native_events=calloc(NATIVE_EVENT_CHUNK,sizeof(struct native_event_t));
   if (native_events==NULL) {
      return PAPI_ENOMEM;
   }
   allocated_native_events=NATIVE_EVENT_CHUNK;

   /* Count number of present PMUs */
   detected_pmus=0;
   ncnt=0;

   /* need to init pinfo or pfmlib might complain */
   memset(&default_pmu, 0, sizeof(pfm_pmu_info_t));
   /* init default pmu */
   retval=pfm_get_pmu_info(0, &default_pmu);
   
   SUBDBG("Detected pmus:\n");
   for(i=0;i<PFM_PMU_MAX;i++) {
      memset(&pinfo,0,sizeof(pfm_pmu_info_t));
      retval=pfm_get_pmu_info(i, &pinfo);
      if (retval!=PFM_SUCCESS) {
	 SUBDBG("%d\n",retval);
	 continue;
      }
      if (pinfo.is_present) {
	 SUBDBG("\t%d %s %s %d\n",i,pinfo.name,pinfo.desc,pinfo.type);

         detected_pmus++;
	 ncnt+=pinfo.nevents;

	 /* Choose default PMU */
	 if ( (pinfo.type==PFM_PMU_TYPE_CORE) &&
              strcmp(pinfo.name,"ix86arch")) {

	    SUBDBG("\t  %s is default\n",pinfo.name);
	    memcpy(&default_pmu,&pinfo,sizeof(pfm_pmu_info_t));
	    found_default++;
	 }
      }
   }
   SUBDBG("%d native events detected on %d pmus\n",ncnt,detected_pmus);

   if (!found_default) {
      PAPIERROR("Could not find default PMU\n");
      return PAPI_ESBSTR;
   }

   if (found_default>1) {
     PAPIERROR("Found too many default PMUs!\n");
     return PAPI_ESBSTR;
   }

   MY_VECTOR->cmp_info.num_native_events = ncnt;

   MY_VECTOR->cmp_info.num_cntrs = default_pmu.num_cntrs+
                                  default_pmu.num_fixed_cntrs;

   SUBDBG( "num_counters: %d\n", MY_VECTOR->cmp_info.num_cntrs );

   MY_VECTOR->cmp_info.num_mpx_cntrs = MAX_MPX_EVENTS;
   
   /* Setup presets */
   retval = _papi_libpfm_setup_presets( (char *)default_pmu.name, 
				     default_pmu.pmu );
   if ( retval ) {
      return retval;
   }	

   return PAPI_OK;
}


/** @class  _papi_libpfm_setup_counters
 *  @brief  Generate the events HW bits to be programmed into the counters
 *
 *  @param[out] *attr
 *        -- perf_event compatible attr structure
 *  @param[in] *ni_bits
 *        -- an opaque structure that holds HW specific event info
 *
 *  @retval PAPI_OK       We generated the event properly
 *
 */

int
_papi_libpfm_setup_counters( struct perf_event_attr *attr,
			   hwd_register_t *ni_bits ) {

  //  int ret;
  int our_idx;
  //char our_name[BUFSIZ];
   
  pfm_perf_encode_arg_t perf_arg;

  struct native_event_t *our_event;

  /* clear the attribute structure */
  memset(&perf_arg,0,sizeof(pfm_perf_encode_arg_t));
  perf_arg.attr=attr;

  our_idx=*(int *)(ni_bits);
  our_event=find_existing_event_by_number(our_idx);
  if (our_event==NULL) {
     return PAPI_ENOEVNT;
  }

  perf_arg.attr->config=our_event->config; 
  perf_arg.attr->config1=our_event->config1;
  perf_arg.attr->type=our_event->type;

  /* convert our code to a name */
  //  ret=_papi_libpfm_ntv_code_to_name( our_idx,our_name,BUFSIZ);
  //if (ret!=PAPI_OK) {
  //   return ret;
  //}

  //SUBDBG("trying \"%s\" %x\n",our_name,our_idx);

  /* use name of the event to get the perf_event encoding */
  //ret = pfm_get_os_event_encoding(our_name, 
  //				  PFM_PLM0 | PFM_PLM3, 
  //                              PFM_OS_PERF_EVENT_EXT, 
  //				  &perf_arg);
  //if (ret!=PFM_SUCCESS) {
  //   return _papi_libpfm_error(ret);
  //}
  
  SUBDBG( "pe_event: config 0x%"PRIx64" config1 0x%"PRIx64" type 0x%"PRIx32"\n", 
          perf_arg.attr->config, 
	  perf_arg.attr->config1,
	  perf_arg.attr->type);
	  

  return PAPI_OK;
}





