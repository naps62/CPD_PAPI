#ifndef _LINUX_COMMON_H
#define _LINUX_COMMON_H

#define LINUX_VERSION(a,b,c) ( ((a&0xff)<<24) | ((b&0xff)<<16) | ((c&0xff) << 8))

#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })

static inline pid_t
mygettid( void )
{
#ifdef SYS_gettid
	return syscall( SYS_gettid );
#elif defined(__NR_gettid)
	return syscall( __NR_gettid );
#else
#error "cannot find gettid"
#endif
}

#ifndef F_SETOWN_EX
   #define F_SETOWN_EX     15
   #define F_GETOWN_EX     16
   
   #define F_OWNER_TID     0
   #define F_OWNER_PID     1
   #define F_OWNER_PGRP    2
   
   struct f_owner_ex {
              int     type;
              pid_t   pid;
   };
#endif

int _linux_get_version();
int _linux_detect_nmi_watchdog();
int _linux_get_cpu_info( PAPI_hw_info_t * hwinfo );
int _linux_get_system_info( papi_mdi_t * mdi );

#endif
