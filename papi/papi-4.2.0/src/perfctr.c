/* 
* File:    perfctr.c
* CVS:     $Id: perfctr.c,v 1.9 2011/09/30 17:55:18 vweaver1 Exp $
* Author:  Philip Mucci
*          mucci at cs.utk.edu
* Mods:    Kevin London
*          london at cs.utk.edu
* Mods:    Maynard Johnson
*          maynardj at us.ibm.com
* Mods:    Brian Sheely
*          bsheely at eecs.utk.edu
*/

#include "papi.h"
#include "papi_internal.h"
#include "papi_memory.h"

#include "linux-common.h"

extern papi_vector_t MY_VECTOR;

#ifdef PPC64
extern int setup_ppc64_presets( int cputype );
#else
extern int setup_x86_presets( int cputype );
#endif

/* This should be in a linux.h header file maybe. */
#define FOPEN_ERROR "fopen(%s) returned NULL"

#if defined(PERFCTR26)
#define PERFCTR_CPU_NAME(pi)    perfctr_info_cpu_name(pi)
#define PERFCTR_CPU_NRCTRS(pi)  perfctr_info_nrctrs(pi)
#else
#define PERFCTR_CPU_NAME        perfctr_cpu_name
#define PERFCTR_CPU_NRCTRS      perfctr_cpu_nrctrs
#endif

#if !defined(PPC64) 
static inline int
xlate_cpu_type_to_vendor( unsigned perfctr_cpu_type )
{
	switch ( perfctr_cpu_type ) {
	case PERFCTR_X86_INTEL_P5:
	case PERFCTR_X86_INTEL_P5MMX:
	case PERFCTR_X86_INTEL_P6:
	case PERFCTR_X86_INTEL_PII:
	case PERFCTR_X86_INTEL_PIII:
	case PERFCTR_X86_INTEL_P4:
	case PERFCTR_X86_INTEL_P4M2:
#ifdef PERFCTR_X86_INTEL_P4M3
	case PERFCTR_X86_INTEL_P4M3:
#endif
#ifdef PERFCTR_X86_INTEL_PENTM
	case PERFCTR_X86_INTEL_PENTM:
#endif
#ifdef PERFCTR_X86_INTEL_CORE
	case PERFCTR_X86_INTEL_CORE:
#endif
#ifdef PERFCTR_X86_INTEL_CORE2
	case PERFCTR_X86_INTEL_CORE2:
#endif
#ifdef PERFCTR_X86_INTEL_ATOM	/* family 6 model 28 */
	case PERFCTR_X86_INTEL_ATOM:
#endif
#ifdef PERFCTR_X86_INTEL_NHLM	/* family 6 model 26 */
	case PERFCTR_X86_INTEL_NHLM:
#endif
#ifdef PERFCTR_X86_INTEL_WSTMR
	case PERFCTR_X86_INTEL_WSTMR:
#endif
		return ( PAPI_VENDOR_INTEL );
#ifdef PERFCTR_X86_AMD_K8
	case PERFCTR_X86_AMD_K8:
#endif
#ifdef PERFCTR_X86_AMD_K8C
	case PERFCTR_X86_AMD_K8C:
#endif
#ifdef PERFCTR_X86_AMD_FAM10 /* this is defined in perfctr 2.6.29 */
	case PERFCTR_X86_AMD_FAM10:
#endif
	case PERFCTR_X86_AMD_K7:
		return ( PAPI_VENDOR_AMD );
	default:
		return ( PAPI_VENDOR_UNKNOWN );
	}
}
#endif

/* volatile uint32_t lock; */

volatile unsigned int lock[PAPI_MAX_LOCK];

long long tb_scale_factor = ( long long ) 1;	/* needed to scale get_cycles on PPC series */

static void
lock_init( void )
{
	int i;
	for ( i = 0; i < PAPI_MAX_LOCK; i++ ) {
		lock[i] = MUTEX_OPEN;
	}
}

int
_perfctr_init_substrate( int cidx )
{
	int retval;
	struct perfctr_info info;
	char abiv[PAPI_MIN_STR_LEN];

#if defined(PERFCTR26)
	int fd;
#else
	struct vperfctr *dev;
#endif

#if defined(PERFCTR26)
	/* Get info from the kernel */
	/* Use lower level calls per Mikael to get the perfctr info
	   without actually creating a new kernel-side state.
	   Also, close the fd immediately after retrieving the info.
	   This is much lighter weight and doesn't reserve the counter
	   resources. Also compatible with perfctr 2.6.14.
	 */
	fd = _vperfctr_open( 0 );
	if ( fd < 0 ) {
		PAPIERROR( VOPEN_ERROR );
		return ( PAPI_ESYS );
	}
	retval = perfctr_info( fd, &info );
	close( fd );
	if ( retval < 0 ) {
		PAPIERROR( VINFO_ERROR );
		return ( PAPI_ESYS );
	}

	/* copy tsc multiplier to local variable        */
	/* this field appears in perfctr 2.6 and higher */
	tb_scale_factor = ( long long ) info.tsc_to_cpu_mult;
#else
	/* Opened once for all threads. */
	if ( ( dev = vperfctr_open(  ) ) == NULL ) {
		PAPIERROR( VOPEN_ERROR );
		return ( PAPI_ESYS );
	}
	SUBDBG( "_perfctr_init_substrate vperfctr_open = %p\n", dev );

	/* Get info from the kernel */
	retval = vperfctr_info( dev, &info );
	if ( retval < 0 ) {
		PAPIERROR( VINFO_ERROR );
		return ( PAPI_ESYS );
	}
	vperfctr_close( dev );
#endif

	/* Fill in what we can of the papi_system_info. */
	retval = MY_VECTOR.get_system_info( &_papi_hwi_system_info );
	if ( retval != PAPI_OK )
		return ( retval );

	/* Setup memory info */
	retval =
		MY_VECTOR.get_memory_info( &_papi_hwi_system_info.hw_info,
								   ( int ) info.cpu_type );
	if ( retval )
		return ( retval );

	strcpy( MY_VECTOR.cmp_info.name,
			"$Id: perfctr.c,v 1.9 2011/09/30 17:55:18 vweaver1 Exp $" );
	strcpy( MY_VECTOR.cmp_info.version, "$Revision: 1.9 $" );
	sprintf( abiv, "0x%08X", info.abi_version );
	strcpy( MY_VECTOR.cmp_info.support_version, abiv );
	strcpy( MY_VECTOR.cmp_info.kernel_version, info.driver_version );
	MY_VECTOR.cmp_info.CmpIdx = cidx;
	MY_VECTOR.cmp_info.num_cntrs = ( int ) PERFCTR_CPU_NRCTRS( &info );
	if ( info.cpu_features & PERFCTR_FEATURE_RDPMC )
		MY_VECTOR.cmp_info.fast_counter_read = 1;
	else
		MY_VECTOR.cmp_info.fast_counter_read = 0;
	MY_VECTOR.cmp_info.fast_real_timer = 1;
	MY_VECTOR.cmp_info.fast_virtual_timer = 1;
	MY_VECTOR.cmp_info.attach = 1;
	MY_VECTOR.cmp_info.attach_must_ptrace = 1;
	MY_VECTOR.cmp_info.default_domain = PAPI_DOM_USER;
#if !defined(PPC64)
	/* AMD and Intel ia386 processors all support unit mask bits */
	MY_VECTOR.cmp_info.cntr_umasks = 1;
#endif
#if defined(PPC64)
	MY_VECTOR.cmp_info.available_domains =
		PAPI_DOM_USER | PAPI_DOM_KERNEL | PAPI_DOM_SUPERVISOR;
#else
	MY_VECTOR.cmp_info.available_domains = PAPI_DOM_USER | PAPI_DOM_KERNEL;
#endif
	MY_VECTOR.cmp_info.default_granularity = PAPI_GRN_THR;
	MY_VECTOR.cmp_info.available_granularities = PAPI_GRN_THR;
	if ( info.cpu_features & PERFCTR_FEATURE_PCINT )
		MY_VECTOR.cmp_info.hardware_intr = 1;
	else
		MY_VECTOR.cmp_info.hardware_intr = 0;
	SUBDBG( "Hardware/OS %s support counter generated interrupts\n",
			MY_VECTOR.cmp_info.hardware_intr ? "does" : "does not" );
	MY_VECTOR.cmp_info.itimer_ns = PAPI_INT_MPX_DEF_US * 1000;
	MY_VECTOR.cmp_info.clock_ticks = ( int ) sysconf( _SC_CLK_TCK );

	strcpy( _papi_hwi_system_info.hw_info.model_string,
			PERFCTR_CPU_NAME( &info ) );
	_papi_hwi_system_info.hw_info.model = ( int ) info.cpu_type;
#if defined(PPC64)
	_papi_hwi_system_info.hw_info.vendor = PAPI_VENDOR_IBM;
	if ( strlen( _papi_hwi_system_info.hw_info.vendor_string ) == 0 )
		strcpy( _papi_hwi_system_info.hw_info.vendor_string, "IBM" );
#else
	_papi_hwi_system_info.hw_info.vendor =
		xlate_cpu_type_to_vendor( info.cpu_type );
#endif

	/* Setup presets last. Some platforms depend on earlier info */
#if !defined(PPC64)
//     retval = setup_p3_vector_table(vtable);
		if ( !retval )
			retval = setup_x86_presets( ( int ) info.cpu_type );
#else
	/* Setup native and preset events */
//  retval = ppc64_setup_vector_table(vtable);
	if ( !retval )
		retval = perfctr_ppc64_setup_native_table(  );
	if ( !retval )
		retval = setup_ppc64_presets( info.cpu_type );
#endif
	if ( retval )
		return ( retval );

	lock_init(  );

	return ( PAPI_OK );
}

static int
attach( hwd_control_state_t * ctl, unsigned long tid )
{
	struct vperfctr_control tmp;

#ifdef VPERFCTR_CONTROL_CLOEXEC
	tmp.flags = VPERFCTR_CONTROL_CLOEXEC;
#endif

	ctl->rvperfctr = rvperfctr_open( ( int ) tid );
	if ( ctl->rvperfctr == NULL ) {
		PAPIERROR( VOPEN_ERROR );
		return ( PAPI_ESYS );
	}
	SUBDBG( "_papi_hwd_ctl rvperfctr_open() = %p\n", ctl->rvperfctr );

	/* Initialize the per thread/process virtualized TSC */
	memset( &tmp, 0x0, sizeof ( tmp ) );
	tmp.cpu_control.tsc_on = 1;

	/* Start the per thread/process virtualized TSC */
	if ( rvperfctr_control( ctl->rvperfctr, &tmp ) < 0 ) {
		PAPIERROR( RCNTRL_ERROR );
		return ( PAPI_ESYS );
	}

	return ( PAPI_OK );
}							 /* end attach() */

static int
detach( hwd_control_state_t * ctl )
{
	rvperfctr_close( ctl->rvperfctr );
	return ( PAPI_OK );
}							 /* end detach() */

static inline int
round_requested_ns( int ns )
{
	if ( ns < MY_VECTOR.cmp_info.itimer_res_ns ) {
		return MY_VECTOR.cmp_info.itimer_res_ns;
	} else {
		int leftover_ns = ns % MY_VECTOR.cmp_info.itimer_res_ns;
		return ns + leftover_ns;
	}
}

int
_perfctr_ctl( hwd_context_t * ctx, int code, _papi_int_option_t * option )
{
	( void ) ctx;			 /*unused */
	switch ( code ) {
	case PAPI_DOMAIN:
	case PAPI_DEFDOM:
#if defined(PPC64)
		return ( MY_VECTOR.
				 set_domain( option->domain.ESI, option->domain.domain ) );
#else
		return ( MY_VECTOR.
				 set_domain( option->domain.ESI->ctl_state,
							 option->domain.domain ) );
#endif
	case PAPI_GRANUL:
	case PAPI_DEFGRN:
		return ( PAPI_ESBSTR );
	case PAPI_ATTACH:
		return ( attach( option->attach.ESI->ctl_state, option->attach.tid ) );
	case PAPI_DETACH:
		return ( detach( option->attach.ESI->ctl_state ) );
	case PAPI_DEF_ITIMER:
	{
		/* flags are currently ignored, eventually the flags will be able
		   to specify whether or not we use POSIX itimers (clock_gettimer) */
		if ( ( option->itimer.itimer_num == ITIMER_REAL ) &&
			 ( option->itimer.itimer_sig != SIGALRM ) )
			return PAPI_EINVAL;
		if ( ( option->itimer.itimer_num == ITIMER_VIRTUAL ) &&
			 ( option->itimer.itimer_sig != SIGVTALRM ) )
			return PAPI_EINVAL;
		if ( ( option->itimer.itimer_num == ITIMER_PROF ) &&
			 ( option->itimer.itimer_sig != SIGPROF ) )
			return PAPI_EINVAL;
		if ( option->itimer.ns > 0 )
			option->itimer.ns = round_requested_ns( option->itimer.ns );
		/* At this point, we assume the user knows what he or
		   she is doing, they maybe doing something arch specific */
		return PAPI_OK;
	}
	case PAPI_DEF_MPX_NS:
	{
		option->multiplex.ns =
			( unsigned long ) round_requested_ns( ( int ) option->multiplex.
												  ns );
		return ( PAPI_OK );
	}
	case PAPI_DEF_ITIMER_NS:
	{
		option->itimer.ns = round_requested_ns( option->itimer.ns );
		return ( PAPI_OK );
	}
	default:
		return ( PAPI_ENOSUPP );
	}
}

void
_perfctr_dispatch_timer( int signal, siginfo_t * si, void *context )
{
	( void ) signal;		 /*unused */
	_papi_hwi_context_t ctx;
	ThreadInfo_t *master = NULL;
	int isHardware = 0;
	caddr_t address;
	int cidx = MY_VECTOR.cmp_info.CmpIdx;

	ctx.si = si;
	ctx.ucontext = ( ucontext_t * ) context;

#define OVERFLOW_MASK si->si_pmc_ovf_mask
#define GEN_OVERFLOW 0

	address = ( caddr_t ) GET_OVERFLOW_ADDRESS( ( ctx ) );
	_papi_hwi_dispatch_overflow_signal( ( void * ) &ctx, address, &isHardware,
										OVERFLOW_MASK, GEN_OVERFLOW, &master,
										MY_VECTOR.cmp_info.CmpIdx );

	/* We are done, resume interrupting counters */
	if ( isHardware ) {
		errno = vperfctr_iresume( master->context[cidx]->perfctr );
		if ( errno < 0 ) {
			PAPIERROR( "vperfctr_iresume errno %d", errno );
		}
	}
}


int
_perfctr_init( hwd_context_t * ctx )
{
	struct vperfctr_control tmp;
	int error;

	/* Initialize our thread/process pointer. */
	if ( ( ctx->perfctr = vperfctr_open(  ) ) == NULL ) {
#ifdef VPERFCTR_OPEN_CREAT_EXCL
		/* New versions of perfctr have this, which allows us to
		   get a previously created context, i.e. one created after
		   a fork and now we're inside a new process that has been exec'd */
		if ( errno ) {
			if ( ( ctx->perfctr = vperfctr_open_mode( 0 ) ) == NULL ) {
				PAPIERROR( VOPEN_ERROR );
				return ( PAPI_ESYS );
			}
		} else {
			PAPIERROR( VOPEN_ERROR );
			return ( PAPI_ESYS );
		}
#else
		PAPIERROR( VOPEN_ERROR );
		return ( PAPI_ESYS );
#endif
	}
	SUBDBG( "_papi_hwd_init vperfctr_open() = %p\n", ctx->perfctr );

	/* Initialize the per thread/process virtualized TSC */
	memset( &tmp, 0x0, sizeof ( tmp ) );
	tmp.cpu_control.tsc_on = 1;

#ifdef VPERFCTR_CONTROL_CLOEXEC
	tmp.flags = VPERFCTR_CONTROL_CLOEXEC;
	SUBDBG( "close on exec\t\t\t%u\n", tmp.flags );
#endif

	/* Start the per thread/process virtualized TSC */
	error = vperfctr_control( ctx->perfctr, &tmp );
	if ( error < 0 ) {
		SUBDBG( "starting virtualized TSC; vperfctr_control returns %d\n",
				error );
		PAPIERROR( VCNTRL_ERROR );
		return ( PAPI_ESYS );
	}

	return ( PAPI_OK );
}

/* This routine is for shutting down threads, including the
   master thread. */

int
_perfctr_shutdown( hwd_context_t * ctx )
{
#ifdef DEBUG
	int retval = vperfctr_unlink( ctx->perfctr );
	SUBDBG( "_papi_hwd_shutdown vperfctr_unlink(%p) = %d\n", ctx->perfctr,
			retval );
#else
	vperfctr_unlink( ctx->perfctr );
#endif
	vperfctr_close( ctx->perfctr );
	SUBDBG( "_perfctr_shutdown vperfctr_close(%p)\n", ctx->perfctr );
	memset( ctx, 0x0, sizeof ( hwd_context_t ) );
	return ( PAPI_OK );
}
