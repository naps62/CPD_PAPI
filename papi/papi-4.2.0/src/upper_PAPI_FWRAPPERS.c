# 1 "papi_fwrappers.c"
# 1 "<built-in>"
# 1 "<command-line>"
# 1 "papi_fwrappers.c"
# 22 "papi_fwrappers.c"
# 1 "/usr/include/stdio.h" 1 3 4
# 28 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/features.h" 1 3 4
# 323 "/usr/include/features.h" 3 4
# 1 "/usr/include/bits/predefs.h" 1 3 4
# 324 "/usr/include/features.h" 2 3 4
# 356 "/usr/include/features.h" 3 4
# 1 "/usr/include/sys/cdefs.h" 1 3 4
# 353 "/usr/include/sys/cdefs.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 354 "/usr/include/sys/cdefs.h" 2 3 4
# 357 "/usr/include/features.h" 2 3 4
# 388 "/usr/include/features.h" 3 4
# 1 "/usr/include/gnu/stubs.h" 1 3 4



# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 5 "/usr/include/gnu/stubs.h" 2 3 4




# 1 "/usr/include/gnu/stubs-64.h" 1 3 4
# 10 "/usr/include/gnu/stubs.h" 2 3 4
# 389 "/usr/include/features.h" 2 3 4
# 29 "/usr/include/stdio.h" 2 3 4





# 1 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include/stddef.h" 1 3 4
# 211 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include/stddef.h" 3 4
typedef long unsigned int size_t;
# 35 "/usr/include/stdio.h" 2 3 4

# 1 "/usr/include/bits/types.h" 1 3 4
# 28 "/usr/include/bits/types.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 29 "/usr/include/bits/types.h" 2 3 4


typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;


typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;

typedef signed long int __int64_t;
typedef unsigned long int __uint64_t;







typedef long int __quad_t;
typedef unsigned long int __u_quad_t;
# 131 "/usr/include/bits/types.h" 3 4
# 1 "/usr/include/bits/typesizes.h" 1 3 4
# 132 "/usr/include/bits/types.h" 2 3 4


typedef unsigned long int __dev_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef unsigned long int __ino_t;
typedef unsigned long int __ino64_t;
typedef unsigned int __mode_t;
typedef unsigned long int __nlink_t;
typedef long int __off_t;
typedef long int __off64_t;
typedef int __pid_t;
typedef struct { int __val[2]; } __fsid_t;
typedef long int __clock_t;
typedef unsigned long int __rlim_t;
typedef unsigned long int __rlim64_t;
typedef unsigned int __id_t;
typedef long int __time_t;
typedef unsigned int __useconds_t;
typedef long int __suseconds_t;

typedef int __daddr_t;
typedef long int __swblk_t;
typedef int __key_t;


typedef int __clockid_t;


typedef void * __timer_t;


typedef long int __blksize_t;




typedef long int __blkcnt_t;
typedef long int __blkcnt64_t;


typedef unsigned long int __fsblkcnt_t;
typedef unsigned long int __fsblkcnt64_t;


typedef unsigned long int __fsfilcnt_t;
typedef unsigned long int __fsfilcnt64_t;

typedef long int __ssize_t;



typedef __off64_t __loff_t;
typedef __quad_t *__qaddr_t;
typedef char *__caddr_t;


typedef long int __intptr_t;


typedef unsigned int __socklen_t;
# 37 "/usr/include/stdio.h" 2 3 4
# 45 "/usr/include/stdio.h" 3 4
struct _IO_FILE;



typedef struct _IO_FILE FILE;





# 65 "/usr/include/stdio.h" 3 4
typedef struct _IO_FILE __FILE;
# 75 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/libio.h" 1 3 4
# 32 "/usr/include/libio.h" 3 4
# 1 "/usr/include/_G_config.h" 1 3 4
# 15 "/usr/include/_G_config.h" 3 4
# 1 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include/stddef.h" 1 3 4
# 16 "/usr/include/_G_config.h" 2 3 4




# 1 "/usr/include/wchar.h" 1 3 4
# 83 "/usr/include/wchar.h" 3 4
typedef struct
{
  int __count;
  union
  {

    unsigned int __wch;



    char __wchb[4];
  } __value;
} __mbstate_t;
# 21 "/usr/include/_G_config.h" 2 3 4

typedef struct
{
  __off_t __pos;
  __mbstate_t __state;
} _G_fpos_t;
typedef struct
{
  __off64_t __pos;
  __mbstate_t __state;
} _G_fpos64_t;
# 53 "/usr/include/_G_config.h" 3 4
typedef int _G_int16_t __attribute__ ((__mode__ (__HI__)));
typedef int _G_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int _G_uint16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int _G_uint32_t __attribute__ ((__mode__ (__SI__)));
# 33 "/usr/include/libio.h" 2 3 4
# 53 "/usr/include/libio.h" 3 4
# 1 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include/stdarg.h" 1 3 4
# 40 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include/stdarg.h" 3 4
typedef __builtin_va_list __gnuc_va_list;
# 54 "/usr/include/libio.h" 2 3 4
# 170 "/usr/include/libio.h" 3 4
struct _IO_jump_t; struct _IO_FILE;
# 180 "/usr/include/libio.h" 3 4
typedef void _IO_lock_t;





struct _IO_marker {
  struct _IO_marker *_next;
  struct _IO_FILE *_sbuf;



  int _pos;
# 203 "/usr/include/libio.h" 3 4
};


enum __codecvt_result
{
  __codecvt_ok,
  __codecvt_partial,
  __codecvt_error,
  __codecvt_noconv
};
# 271 "/usr/include/libio.h" 3 4
struct _IO_FILE {
  int _flags;




  char* _IO_read_ptr;
  char* _IO_read_end;
  char* _IO_read_base;
  char* _IO_write_base;
  char* _IO_write_ptr;
  char* _IO_write_end;
  char* _IO_buf_base;
  char* _IO_buf_end;

  char *_IO_save_base;
  char *_IO_backup_base;
  char *_IO_save_end;

  struct _IO_marker *_markers;

  struct _IO_FILE *_chain;

  int _fileno;



  int _flags2;

  __off_t _old_offset;



  unsigned short _cur_column;
  signed char _vtable_offset;
  char _shortbuf[1];



  _IO_lock_t *_lock;
# 319 "/usr/include/libio.h" 3 4
  __off64_t _offset;
# 328 "/usr/include/libio.h" 3 4
  void *__pad1;
  void *__pad2;
  void *__pad3;
  void *__pad4;
  size_t __pad5;

  int _mode;

  char _unused2[15 * sizeof (int) - 4 * sizeof (void *) - sizeof (size_t)];

};


typedef struct _IO_FILE _IO_FILE;


struct _IO_FILE_plus;

extern struct _IO_FILE_plus _IO_2_1_stdin_;
extern struct _IO_FILE_plus _IO_2_1_stdout_;
extern struct _IO_FILE_plus _IO_2_1_stderr_;
# 364 "/usr/include/libio.h" 3 4
typedef __ssize_t __io_read_fn (void *__cookie, char *__buf, size_t __nbytes);







typedef __ssize_t __io_write_fn (void *__cookie, __const char *__buf,
     size_t __n);







typedef int __io_seek_fn (void *__cookie, __off64_t *__pos, int __w);


typedef int __io_close_fn (void *__cookie);
# 416 "/usr/include/libio.h" 3 4
extern int __underflow (_IO_FILE *);
extern int __uflow (_IO_FILE *);
extern int __overflow (_IO_FILE *, int);
# 460 "/usr/include/libio.h" 3 4
extern int _IO_getc (_IO_FILE *__fp);
extern int _IO_putc (int __c, _IO_FILE *__fp);
extern int _IO_feof (_IO_FILE *__fp) __attribute__ ((__nothrow__));
extern int _IO_ferror (_IO_FILE *__fp) __attribute__ ((__nothrow__));

extern int _IO_peekc_locked (_IO_FILE *__fp);





extern void _IO_flockfile (_IO_FILE *) __attribute__ ((__nothrow__));
extern void _IO_funlockfile (_IO_FILE *) __attribute__ ((__nothrow__));
extern int _IO_ftrylockfile (_IO_FILE *) __attribute__ ((__nothrow__));
# 490 "/usr/include/libio.h" 3 4
extern int _IO_vfscanf (_IO_FILE * __restrict, const char * __restrict,
   __gnuc_va_list, int *__restrict);
extern int _IO_vfprintf (_IO_FILE *__restrict, const char *__restrict,
    __gnuc_va_list);
extern __ssize_t _IO_padn (_IO_FILE *, int, __ssize_t);
extern size_t _IO_sgetn (_IO_FILE *, void *, size_t);

extern __off64_t _IO_seekoff (_IO_FILE *, __off64_t, int, int);
extern __off64_t _IO_seekpos (_IO_FILE *, __off64_t, int);

extern void _IO_free_backup_area (_IO_FILE *) __attribute__ ((__nothrow__));
# 76 "/usr/include/stdio.h" 2 3 4




typedef __gnuc_va_list va_list;
# 91 "/usr/include/stdio.h" 3 4
typedef __off_t off_t;
# 103 "/usr/include/stdio.h" 3 4
typedef __ssize_t ssize_t;







typedef _G_fpos_t fpos_t;




# 161 "/usr/include/stdio.h" 3 4
# 1 "/usr/include/bits/stdio_lim.h" 1 3 4
# 162 "/usr/include/stdio.h" 2 3 4



extern struct _IO_FILE *stdin;
extern struct _IO_FILE *stdout;
extern struct _IO_FILE *stderr;







extern int remove (__const char *__filename) __attribute__ ((__nothrow__));

extern int rename (__const char *__old, __const char *__new) __attribute__ ((__nothrow__));




extern int renameat (int __oldfd, __const char *__old, int __newfd,
       __const char *__new) __attribute__ ((__nothrow__));








extern FILE *tmpfile (void) ;
# 206 "/usr/include/stdio.h" 3 4
extern char *tmpnam (char *__s) __attribute__ ((__nothrow__)) ;





extern char *tmpnam_r (char *__s) __attribute__ ((__nothrow__)) ;
# 224 "/usr/include/stdio.h" 3 4
extern char *tempnam (__const char *__dir, __const char *__pfx)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) ;








extern int fclose (FILE *__stream);




extern int fflush (FILE *__stream);

# 249 "/usr/include/stdio.h" 3 4
extern int fflush_unlocked (FILE *__stream);
# 263 "/usr/include/stdio.h" 3 4






extern FILE *fopen (__const char *__restrict __filename,
      __const char *__restrict __modes) ;




extern FILE *freopen (__const char *__restrict __filename,
        __const char *__restrict __modes,
        FILE *__restrict __stream) ;
# 292 "/usr/include/stdio.h" 3 4

# 303 "/usr/include/stdio.h" 3 4
extern FILE *fdopen (int __fd, __const char *__modes) __attribute__ ((__nothrow__)) ;
# 316 "/usr/include/stdio.h" 3 4
extern FILE *fmemopen (void *__s, size_t __len, __const char *__modes)
  __attribute__ ((__nothrow__)) ;




extern FILE *open_memstream (char **__bufloc, size_t *__sizeloc) __attribute__ ((__nothrow__)) ;






extern void setbuf (FILE *__restrict __stream, char *__restrict __buf) __attribute__ ((__nothrow__));



extern int setvbuf (FILE *__restrict __stream, char *__restrict __buf,
      int __modes, size_t __n) __attribute__ ((__nothrow__));





extern void setbuffer (FILE *__restrict __stream, char *__restrict __buf,
         size_t __size) __attribute__ ((__nothrow__));


extern void setlinebuf (FILE *__stream) __attribute__ ((__nothrow__));








extern int fprintf (FILE *__restrict __stream,
      __const char *__restrict __format, ...);




extern int printf (__const char *__restrict __format, ...);

extern int sprintf (char *__restrict __s,
      __const char *__restrict __format, ...) __attribute__ ((__nothrow__));





extern int vfprintf (FILE *__restrict __s, __const char *__restrict __format,
       __gnuc_va_list __arg);




extern int vprintf (__const char *__restrict __format, __gnuc_va_list __arg);

extern int vsprintf (char *__restrict __s, __const char *__restrict __format,
       __gnuc_va_list __arg) __attribute__ ((__nothrow__));





extern int snprintf (char *__restrict __s, size_t __maxlen,
       __const char *__restrict __format, ...)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 4)));

extern int vsnprintf (char *__restrict __s, size_t __maxlen,
        __const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__printf__, 3, 0)));

# 414 "/usr/include/stdio.h" 3 4
extern int vdprintf (int __fd, __const char *__restrict __fmt,
       __gnuc_va_list __arg)
     __attribute__ ((__format__ (__printf__, 2, 0)));
extern int dprintf (int __fd, __const char *__restrict __fmt, ...)
     __attribute__ ((__format__ (__printf__, 2, 3)));








extern int fscanf (FILE *__restrict __stream,
     __const char *__restrict __format, ...) ;




extern int scanf (__const char *__restrict __format, ...) ;

extern int sscanf (__const char *__restrict __s,
     __const char *__restrict __format, ...) __attribute__ ((__nothrow__));
# 445 "/usr/include/stdio.h" 3 4
extern int fscanf (FILE *__restrict __stream, __const char *__restrict __format, ...) __asm__ ("" "__isoc99_fscanf")

                               ;
extern int scanf (__const char *__restrict __format, ...) __asm__ ("" "__isoc99_scanf")
                              ;
extern int sscanf (__const char *__restrict __s, __const char *__restrict __format, ...) __asm__ ("" "__isoc99_sscanf") __attribute__ ((__nothrow__))

                      ;
# 465 "/usr/include/stdio.h" 3 4








extern int vfscanf (FILE *__restrict __s, __const char *__restrict __format,
      __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 2, 0))) ;





extern int vscanf (__const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__format__ (__scanf__, 1, 0))) ;


extern int vsscanf (__const char *__restrict __s,
      __const char *__restrict __format, __gnuc_va_list __arg)
     __attribute__ ((__nothrow__)) __attribute__ ((__format__ (__scanf__, 2, 0)));
# 496 "/usr/include/stdio.h" 3 4
extern int vfscanf (FILE *__restrict __s, __const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vfscanf")



     __attribute__ ((__format__ (__scanf__, 2, 0))) ;
extern int vscanf (__const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vscanf")

     __attribute__ ((__format__ (__scanf__, 1, 0))) ;
extern int vsscanf (__const char *__restrict __s, __const char *__restrict __format, __gnuc_va_list __arg) __asm__ ("" "__isoc99_vsscanf") __attribute__ ((__nothrow__))



     __attribute__ ((__format__ (__scanf__, 2, 0)));
# 524 "/usr/include/stdio.h" 3 4









extern int fgetc (FILE *__stream);
extern int getc (FILE *__stream);





extern int getchar (void);

# 552 "/usr/include/stdio.h" 3 4
extern int getc_unlocked (FILE *__stream);
extern int getchar_unlocked (void);
# 563 "/usr/include/stdio.h" 3 4
extern int fgetc_unlocked (FILE *__stream);











extern int fputc (int __c, FILE *__stream);
extern int putc (int __c, FILE *__stream);





extern int putchar (int __c);

# 596 "/usr/include/stdio.h" 3 4
extern int fputc_unlocked (int __c, FILE *__stream);







extern int putc_unlocked (int __c, FILE *__stream);
extern int putchar_unlocked (int __c);






extern int getw (FILE *__stream);


extern int putw (int __w, FILE *__stream);








extern char *fgets (char *__restrict __s, int __n, FILE *__restrict __stream)
     ;






extern char *gets (char *__s) ;

# 658 "/usr/include/stdio.h" 3 4
extern __ssize_t __getdelim (char **__restrict __lineptr,
          size_t *__restrict __n, int __delimiter,
          FILE *__restrict __stream) ;
extern __ssize_t getdelim (char **__restrict __lineptr,
        size_t *__restrict __n, int __delimiter,
        FILE *__restrict __stream) ;







extern __ssize_t getline (char **__restrict __lineptr,
       size_t *__restrict __n,
       FILE *__restrict __stream) ;








extern int fputs (__const char *__restrict __s, FILE *__restrict __stream);





extern int puts (__const char *__s);






extern int ungetc (int __c, FILE *__stream);






extern size_t fread (void *__restrict __ptr, size_t __size,
       size_t __n, FILE *__restrict __stream) ;




extern size_t fwrite (__const void *__restrict __ptr, size_t __size,
        size_t __n, FILE *__restrict __s);

# 730 "/usr/include/stdio.h" 3 4
extern size_t fread_unlocked (void *__restrict __ptr, size_t __size,
         size_t __n, FILE *__restrict __stream) ;
extern size_t fwrite_unlocked (__const void *__restrict __ptr, size_t __size,
          size_t __n, FILE *__restrict __stream);








extern int fseek (FILE *__stream, long int __off, int __whence);




extern long int ftell (FILE *__stream) ;




extern void rewind (FILE *__stream);

# 766 "/usr/include/stdio.h" 3 4
extern int fseeko (FILE *__stream, __off_t __off, int __whence);




extern __off_t ftello (FILE *__stream) ;
# 785 "/usr/include/stdio.h" 3 4






extern int fgetpos (FILE *__restrict __stream, fpos_t *__restrict __pos);




extern int fsetpos (FILE *__stream, __const fpos_t *__pos);
# 808 "/usr/include/stdio.h" 3 4

# 817 "/usr/include/stdio.h" 3 4


extern void clearerr (FILE *__stream) __attribute__ ((__nothrow__));

extern int feof (FILE *__stream) __attribute__ ((__nothrow__)) ;

extern int ferror (FILE *__stream) __attribute__ ((__nothrow__)) ;




extern void clearerr_unlocked (FILE *__stream) __attribute__ ((__nothrow__));
extern int feof_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;
extern int ferror_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;








extern void perror (__const char *__s);






# 1 "/usr/include/bits/sys_errlist.h" 1 3 4
# 27 "/usr/include/bits/sys_errlist.h" 3 4
extern int sys_nerr;
extern __const char *__const sys_errlist[];
# 847 "/usr/include/stdio.h" 2 3 4




extern int fileno (FILE *__stream) __attribute__ ((__nothrow__)) ;




extern int fileno_unlocked (FILE *__stream) __attribute__ ((__nothrow__)) ;
# 866 "/usr/include/stdio.h" 3 4
extern FILE *popen (__const char *__command, __const char *__modes) ;





extern int pclose (FILE *__stream);





extern char *ctermid (char *__s) __attribute__ ((__nothrow__));
# 906 "/usr/include/stdio.h" 3 4
extern void flockfile (FILE *__stream) __attribute__ ((__nothrow__));



extern int ftrylockfile (FILE *__stream) __attribute__ ((__nothrow__)) ;


extern void funlockfile (FILE *__stream) __attribute__ ((__nothrow__));
# 936 "/usr/include/stdio.h" 3 4

# 23 "papi_fwrappers.c" 2
# 1 "/usr/include/assert.h" 1 3 4
# 68 "/usr/include/assert.h" 3 4



extern void __assert_fail (__const char *__assertion, __const char *__file,
      unsigned int __line, __const char *__function)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));


extern void __assert_perror_fail (int __errnum, __const char *__file,
      unsigned int __line,
      __const char *__function)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));




extern void __assert (const char *__assertion, const char *__file, int __line)
     __attribute__ ((__nothrow__)) __attribute__ ((__noreturn__));



# 24 "papi_fwrappers.c" 2
# 1 "/usr/include/string.h" 1 3 4
# 29 "/usr/include/string.h" 3 4





# 1 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include/stddef.h" 1 3 4
# 35 "/usr/include/string.h" 2 3 4









extern void *memcpy (void *__restrict __dest,
       __const void *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern void *memmove (void *__dest, __const void *__src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));






extern void *memccpy (void *__restrict __dest, __const void *__restrict __src,
        int __c, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));





extern void *memset (void *__s, int __c, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int memcmp (__const void *__s1, __const void *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 95 "/usr/include/string.h" 3 4
extern void *memchr (__const void *__s, int __c, size_t __n)
      __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


# 126 "/usr/include/string.h" 3 4


extern char *strcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncpy (char *__restrict __dest,
        __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern char *strcat (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));

extern char *strncat (char *__restrict __dest, __const char *__restrict __src,
        size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern int strncmp (__const char *__s1, __const char *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strcoll (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));

extern size_t strxfrm (char *__restrict __dest,
         __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));






# 1 "/usr/include/xlocale.h" 1 3 4
# 28 "/usr/include/xlocale.h" 3 4
typedef struct __locale_struct
{

  struct __locale_data *__locales[13];


  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;


  const char *__names[13];
} *__locale_t;


typedef __locale_t locale_t;
# 163 "/usr/include/string.h" 2 3 4


extern int strcoll_l (__const char *__s1, __const char *__s2, __locale_t __l)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2, 3)));

extern size_t strxfrm_l (char *__dest, __const char *__src, size_t __n,
    __locale_t __l) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 4)));





extern char *strdup (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));






extern char *strndup (__const char *__string, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__malloc__)) __attribute__ ((__nonnull__ (1)));
# 210 "/usr/include/string.h" 3 4

# 235 "/usr/include/string.h" 3 4
extern char *strchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 262 "/usr/include/string.h" 3 4
extern char *strrchr (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));


# 281 "/usr/include/string.h" 3 4



extern size_t strcspn (__const char *__s, __const char *__reject)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern size_t strspn (__const char *__s, __const char *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 314 "/usr/include/string.h" 3 4
extern char *strpbrk (__const char *__s, __const char *__accept)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 342 "/usr/include/string.h" 3 4
extern char *strstr (__const char *__haystack, __const char *__needle)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));




extern char *strtok (char *__restrict __s, __const char *__restrict __delim)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2)));




extern char *__strtok_r (char *__restrict __s,
    __const char *__restrict __delim,
    char **__restrict __save_ptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));

extern char *strtok_r (char *__restrict __s, __const char *__restrict __delim,
         char **__restrict __save_ptr)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (2, 3)));
# 397 "/usr/include/string.h" 3 4


extern size_t strlen (__const char *__s)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern size_t strnlen (__const char *__string, size_t __maxlen)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));





extern char *strerror (int __errnum) __attribute__ ((__nothrow__));

# 427 "/usr/include/string.h" 3 4
extern int strerror_r (int __errnum, char *__buf, size_t __buflen) __asm__ ("" "__xpg_strerror_r") __attribute__ ((__nothrow__))

                        __attribute__ ((__nonnull__ (2)));
# 445 "/usr/include/string.h" 3 4
extern char *strerror_l (int __errnum, __locale_t __l) __attribute__ ((__nothrow__));





extern void __bzero (void *__s, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));



extern void bcopy (__const void *__src, void *__dest, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));


extern void bzero (void *__s, size_t __n) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int bcmp (__const void *__s1, __const void *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 489 "/usr/include/string.h" 3 4
extern char *index (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));
# 517 "/usr/include/string.h" 3 4
extern char *rindex (__const char *__s, int __c)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1)));




extern int ffs (int __i) __attribute__ ((__nothrow__)) __attribute__ ((__const__));
# 536 "/usr/include/string.h" 3 4
extern int strcasecmp (__const char *__s1, __const char *__s2)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));


extern int strncasecmp (__const char *__s1, __const char *__s2, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__pure__)) __attribute__ ((__nonnull__ (1, 2)));
# 559 "/usr/include/string.h" 3 4
extern char *strsep (char **__restrict __stringp,
       __const char *__restrict __delim)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));




extern char *strsignal (int __sig) __attribute__ ((__nothrow__));


extern char *__stpcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpcpy (char *__restrict __dest, __const char *__restrict __src)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));



extern char *__stpncpy (char *__restrict __dest,
   __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
extern char *stpncpy (char *__restrict __dest,
        __const char *__restrict __src, size_t __n)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1, 2)));
# 646 "/usr/include/string.h" 3 4

# 25 "papi_fwrappers.c" 2
# 1 "papi.h" 1
# 235 "papi.h"
# 1 "/usr/include/sys/types.h" 1 3 4
# 28 "/usr/include/sys/types.h" 3 4






typedef __u_char u_char;
typedef __u_short u_short;
typedef __u_int u_int;
typedef __u_long u_long;
typedef __quad_t quad_t;
typedef __u_quad_t u_quad_t;
typedef __fsid_t fsid_t;




typedef __loff_t loff_t;



typedef __ino_t ino_t;
# 61 "/usr/include/sys/types.h" 3 4
typedef __dev_t dev_t;




typedef __gid_t gid_t;




typedef __mode_t mode_t;




typedef __nlink_t nlink_t;




typedef __uid_t uid_t;
# 99 "/usr/include/sys/types.h" 3 4
typedef __pid_t pid_t;





typedef __id_t id_t;
# 116 "/usr/include/sys/types.h" 3 4
typedef __daddr_t daddr_t;
typedef __caddr_t caddr_t;





typedef __key_t key_t;
# 133 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 58 "/usr/include/time.h" 3 4


typedef __clock_t clock_t;



# 74 "/usr/include/time.h" 3 4


typedef __time_t time_t;



# 92 "/usr/include/time.h" 3 4
typedef __clockid_t clockid_t;
# 104 "/usr/include/time.h" 3 4
typedef __timer_t timer_t;
# 134 "/usr/include/sys/types.h" 2 3 4
# 147 "/usr/include/sys/types.h" 3 4
# 1 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include/stddef.h" 1 3 4
# 148 "/usr/include/sys/types.h" 2 3 4



typedef unsigned long int ulong;
typedef unsigned short int ushort;
typedef unsigned int uint;
# 195 "/usr/include/sys/types.h" 3 4
typedef int int8_t __attribute__ ((__mode__ (__QI__)));
typedef int int16_t __attribute__ ((__mode__ (__HI__)));
typedef int int32_t __attribute__ ((__mode__ (__SI__)));
typedef int int64_t __attribute__ ((__mode__ (__DI__)));


typedef unsigned int u_int8_t __attribute__ ((__mode__ (__QI__)));
typedef unsigned int u_int16_t __attribute__ ((__mode__ (__HI__)));
typedef unsigned int u_int32_t __attribute__ ((__mode__ (__SI__)));
typedef unsigned int u_int64_t __attribute__ ((__mode__ (__DI__)));

typedef int register_t __attribute__ ((__mode__ (__word__)));
# 217 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/endian.h" 1 3 4
# 37 "/usr/include/endian.h" 3 4
# 1 "/usr/include/bits/endian.h" 1 3 4
# 38 "/usr/include/endian.h" 2 3 4
# 61 "/usr/include/endian.h" 3 4
# 1 "/usr/include/bits/byteswap.h" 1 3 4
# 28 "/usr/include/bits/byteswap.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 29 "/usr/include/bits/byteswap.h" 2 3 4
# 62 "/usr/include/endian.h" 2 3 4
# 218 "/usr/include/sys/types.h" 2 3 4


# 1 "/usr/include/sys/select.h" 1 3 4
# 31 "/usr/include/sys/select.h" 3 4
# 1 "/usr/include/bits/select.h" 1 3 4
# 23 "/usr/include/bits/select.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 24 "/usr/include/bits/select.h" 2 3 4
# 32 "/usr/include/sys/select.h" 2 3 4


# 1 "/usr/include/bits/sigset.h" 1 3 4
# 24 "/usr/include/bits/sigset.h" 3 4
typedef int __sig_atomic_t;




typedef struct
  {
    unsigned long int __val[(1024 / (8 * sizeof (unsigned long int)))];
  } __sigset_t;
# 35 "/usr/include/sys/select.h" 2 3 4



typedef __sigset_t sigset_t;





# 1 "/usr/include/time.h" 1 3 4
# 120 "/usr/include/time.h" 3 4
struct timespec
  {
    __time_t tv_sec;
    long int tv_nsec;
  };
# 45 "/usr/include/sys/select.h" 2 3 4

# 1 "/usr/include/bits/time.h" 1 3 4
# 75 "/usr/include/bits/time.h" 3 4
struct timeval
  {
    __time_t tv_sec;
    __suseconds_t tv_usec;
  };
# 47 "/usr/include/sys/select.h" 2 3 4


typedef __suseconds_t suseconds_t;





typedef long int __fd_mask;
# 67 "/usr/include/sys/select.h" 3 4
typedef struct
  {






    __fd_mask __fds_bits[1024 / (8 * (int) sizeof (__fd_mask))];


  } fd_set;






typedef __fd_mask fd_mask;
# 99 "/usr/include/sys/select.h" 3 4

# 109 "/usr/include/sys/select.h" 3 4
extern int select (int __nfds, fd_set *__restrict __readfds,
     fd_set *__restrict __writefds,
     fd_set *__restrict __exceptfds,
     struct timeval *__restrict __timeout);
# 121 "/usr/include/sys/select.h" 3 4
extern int pselect (int __nfds, fd_set *__restrict __readfds,
      fd_set *__restrict __writefds,
      fd_set *__restrict __exceptfds,
      const struct timespec *__restrict __timeout,
      const __sigset_t *__restrict __sigmask);



# 221 "/usr/include/sys/types.h" 2 3 4


# 1 "/usr/include/sys/sysmacros.h" 1 3 4
# 30 "/usr/include/sys/sysmacros.h" 3 4
__extension__
extern unsigned int gnu_dev_major (unsigned long long int __dev)
     __attribute__ ((__nothrow__));
__extension__
extern unsigned int gnu_dev_minor (unsigned long long int __dev)
     __attribute__ ((__nothrow__));
__extension__
extern unsigned long long int gnu_dev_makedev (unsigned int __major,
            unsigned int __minor)
     __attribute__ ((__nothrow__));
# 224 "/usr/include/sys/types.h" 2 3 4





typedef __blksize_t blksize_t;






typedef __blkcnt_t blkcnt_t;



typedef __fsblkcnt_t fsblkcnt_t;



typedef __fsfilcnt_t fsfilcnt_t;
# 271 "/usr/include/sys/types.h" 3 4
# 1 "/usr/include/bits/pthreadtypes.h" 1 3 4
# 23 "/usr/include/bits/pthreadtypes.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 24 "/usr/include/bits/pthreadtypes.h" 2 3 4
# 50 "/usr/include/bits/pthreadtypes.h" 3 4
typedef unsigned long int pthread_t;


typedef union
{
  char __size[56];
  long int __align;
} pthread_attr_t;



typedef struct __pthread_internal_list
{
  struct __pthread_internal_list *__prev;
  struct __pthread_internal_list *__next;
} __pthread_list_t;
# 76 "/usr/include/bits/pthreadtypes.h" 3 4
typedef union
{
  struct __pthread_mutex_s
  {
    int __lock;
    unsigned int __count;
    int __owner;

    unsigned int __nusers;



    int __kind;

    int __spins;
    __pthread_list_t __list;
# 101 "/usr/include/bits/pthreadtypes.h" 3 4
  } __data;
  char __size[40];
  long int __align;
} pthread_mutex_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_mutexattr_t;




typedef union
{
  struct
  {
    int __lock;
    unsigned int __futex;
    __extension__ unsigned long long int __total_seq;
    __extension__ unsigned long long int __wakeup_seq;
    __extension__ unsigned long long int __woken_seq;
    void *__mutex;
    unsigned int __nwaiters;
    unsigned int __broadcast_seq;
  } __data;
  char __size[48];
  __extension__ long long int __align;
} pthread_cond_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_condattr_t;



typedef unsigned int pthread_key_t;



typedef int pthread_once_t;





typedef union
{

  struct
  {
    int __lock;
    unsigned int __nr_readers;
    unsigned int __readers_wakeup;
    unsigned int __writer_wakeup;
    unsigned int __nr_readers_queued;
    unsigned int __nr_writers_queued;
    int __writer;
    int __shared;
    unsigned long int __pad1;
    unsigned long int __pad2;


    unsigned int __flags;
  } __data;
# 187 "/usr/include/bits/pthreadtypes.h" 3 4
  char __size[56];
  long int __align;
} pthread_rwlock_t;

typedef union
{
  char __size[8];
  long int __align;
} pthread_rwlockattr_t;





typedef volatile int pthread_spinlock_t;




typedef union
{
  char __size[32];
  long int __align;
} pthread_barrier_t;

typedef union
{
  char __size[4];
  int __align;
} pthread_barrierattr_t;
# 272 "/usr/include/sys/types.h" 2 3 4



# 236 "papi.h" 2
# 1 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include-fixed/limits.h" 1 3 4
# 34 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include-fixed/limits.h" 3 4
# 1 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include-fixed/syslimits.h" 1 3 4






# 1 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include-fixed/limits.h" 1 3 4
# 169 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include-fixed/limits.h" 3 4
# 1 "/usr/include/limits.h" 1 3 4
# 145 "/usr/include/limits.h" 3 4
# 1 "/usr/include/bits/posix1_lim.h" 1 3 4
# 157 "/usr/include/bits/posix1_lim.h" 3 4
# 1 "/usr/include/bits/local_lim.h" 1 3 4
# 39 "/usr/include/bits/local_lim.h" 3 4
# 1 "/usr/include/linux/limits.h" 1 3 4
# 40 "/usr/include/bits/local_lim.h" 2 3 4
# 158 "/usr/include/bits/posix1_lim.h" 2 3 4
# 146 "/usr/include/limits.h" 2 3 4



# 1 "/usr/include/bits/posix2_lim.h" 1 3 4
# 150 "/usr/include/limits.h" 2 3 4
# 170 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include-fixed/limits.h" 2 3 4
# 8 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include-fixed/syslimits.h" 2 3 4
# 35 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include-fixed/limits.h" 2 3 4
# 237 "papi.h" 2
# 1 "papiStdEventDefs.h" 1
# 53 "papiStdEventDefs.h"
enum
{
 PAPI_L1_DCM_idx = 0,
 PAPI_L1_ICM_idx,
 PAPI_L2_DCM_idx,
 PAPI_L2_ICM_idx,
 PAPI_L3_DCM_idx,
 PAPI_L3_ICM_idx,
 PAPI_L1_TCM_idx,
 PAPI_L2_TCM_idx,
 PAPI_L3_TCM_idx,
 PAPI_CA_SNP_idx,
 PAPI_CA_SHR_idx,
 PAPI_CA_CLN_idx,
 PAPI_CA_INV_idx,
 PAPI_CA_ITV_idx,
 PAPI_L3_LDM_idx,
 PAPI_L3_STM_idx,

 PAPI_BRU_IDL_idx,
 PAPI_FXU_IDL_idx,
 PAPI_FPU_IDL_idx,
 PAPI_LSU_IDL_idx,
 PAPI_TLB_DM_idx,
 PAPI_TLB_IM_idx,
 PAPI_TLB_TL_idx,
 PAPI_L1_LDM_idx,
 PAPI_L1_STM_idx,
 PAPI_L2_LDM_idx,
 PAPI_L2_STM_idx,
 PAPI_BTAC_M_idx,
 PAPI_PRF_DM_idx,
 PAPI_L3_DCH_idx,
 PAPI_TLB_SD_idx,
 PAPI_CSR_FAL_idx,

 PAPI_CSR_SUC_idx,
 PAPI_CSR_TOT_idx,
 PAPI_MEM_SCY_idx,
 PAPI_MEM_RCY_idx,
 PAPI_MEM_WCY_idx,
 PAPI_STL_ICY_idx,
 PAPI_FUL_ICY_idx,
 PAPI_STL_CCY_idx,
 PAPI_FUL_CCY_idx,
 PAPI_HW_INT_idx,
 PAPI_BR_UCN_idx,
 PAPI_BR_CN_idx,
 PAPI_BR_TKN_idx,
 PAPI_BR_NTK_idx,
 PAPI_BR_MSP_idx,
 PAPI_BR_PRC_idx,

 PAPI_FMA_INS_idx,
 PAPI_TOT_IIS_idx,
 PAPI_TOT_INS_idx,
 PAPI_INT_INS_idx,
 PAPI_FP_INS_idx,
 PAPI_LD_INS_idx,
 PAPI_SR_INS_idx,
 PAPI_BR_INS_idx,
 PAPI_VEC_INS_idx,
 PAPI_RES_STL_idx,
 PAPI_FP_STAL_idx,
 PAPI_TOT_CYC_idx,
 PAPI_LST_INS_idx,
 PAPI_SYC_INS_idx,
 PAPI_L1_DCH_idx,
 PAPI_L2_DCH_idx,

 PAPI_L1_DCA_idx,
 PAPI_L2_DCA_idx,
 PAPI_L3_DCA_idx,
 PAPI_L1_DCR_idx,
 PAPI_L2_DCR_idx,
 PAPI_L3_DCR_idx,
 PAPI_L1_DCW_idx,
 PAPI_L2_DCW_idx,
 PAPI_L3_DCW_idx,
 PAPI_L1_ICH_idx,
 PAPI_L2_ICH_idx,
 PAPI_L3_ICH_idx,
 PAPI_L1_ICA_idx,
 PAPI_L2_ICA_idx,
 PAPI_L3_ICA_idx,
 PAPI_L1_ICR_idx,

 PAPI_L2_ICR_idx,
 PAPI_L3_ICR_idx,
 PAPI_L1_ICW_idx,
 PAPI_L2_ICW_idx,
 PAPI_L3_ICW_idx,
 PAPI_L1_TCH_idx,
 PAPI_L2_TCH_idx,
 PAPI_L3_TCH_idx,
 PAPI_L1_TCA_idx,
 PAPI_L2_TCA_idx,
 PAPI_L3_TCA_idx,
 PAPI_L1_TCR_idx,
 PAPI_L2_TCR_idx,
 PAPI_L3_TCR_idx,
 PAPI_L1_TCW_idx,
 PAPI_L2_TCW_idx,

 PAPI_L3_TCW_idx,
 PAPI_FML_INS_idx,
 PAPI_FAD_INS_idx,
 PAPI_FDV_INS_idx,
 PAPI_FSQ_INS_idx,
 PAPI_FNV_INS_idx,
 PAPI_FP_OPS_idx,
 PAPI_SP_OPS_idx,
 PAPI_DP_OPS_idx,
 PAPI_VEC_SP_idx,
 PAPI_VEC_DP_idx,
# 181 "papiStdEventDefs.h"
 PAPI_END_idx
};
# 238 "papi.h" 2
# 485 "papi.h"
enum {
   PAPI_ENUM_EVENTS = 0,
   PAPI_ENUM_FIRST,
   PAPI_PRESET_ENUM_AVAIL,


   PAPI_PRESET_ENUM_MSC,
   PAPI_PRESET_ENUM_INS,
   PAPI_PRESET_ENUM_IDL,
   PAPI_PRESET_ENUM_BR,
   PAPI_PRESET_ENUM_CND,
   PAPI_PRESET_ENUM_MEM,
   PAPI_PRESET_ENUM_CACH,
   PAPI_PRESET_ENUM_L1,
   PAPI_PRESET_ENUM_L2,
   PAPI_PRESET_ENUM_L3,
   PAPI_PRESET_ENUM_TLB,
   PAPI_PRESET_ENUM_FP,


   PAPI_NTV_ENUM_UMASKS,
   PAPI_NTV_ENUM_UMASK_COMBOS,
   PAPI_NTV_ENUM_IARR,
   PAPI_NTV_ENUM_DARR,
   PAPI_NTV_ENUM_OPCM,
   PAPI_NTV_ENUM_IEAR,
   PAPI_NTV_ENUM_DEAR,
   PAPI_NTV_ENUM_GROUPS
};
# 555 "papi.h"
# 1 "/usr/include/signal.h" 1 3 4
# 31 "/usr/include/signal.h" 3 4


# 1 "/usr/include/bits/sigset.h" 1 3 4
# 104 "/usr/include/bits/sigset.h" 3 4
extern int __sigismember (__const __sigset_t *, int);
extern int __sigaddset (__sigset_t *, int);
extern int __sigdelset (__sigset_t *, int);
# 34 "/usr/include/signal.h" 2 3 4







typedef __sig_atomic_t sig_atomic_t;

# 58 "/usr/include/signal.h" 3 4
# 1 "/usr/include/bits/signum.h" 1 3 4
# 59 "/usr/include/signal.h" 2 3 4
# 76 "/usr/include/signal.h" 3 4
# 1 "/usr/include/time.h" 1 3 4
# 77 "/usr/include/signal.h" 2 3 4


# 1 "/usr/include/bits/siginfo.h" 1 3 4
# 25 "/usr/include/bits/siginfo.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 26 "/usr/include/bits/siginfo.h" 2 3 4







typedef union sigval
  {
    int sival_int;
    void *sival_ptr;
  } sigval_t;
# 51 "/usr/include/bits/siginfo.h" 3 4
typedef struct siginfo
  {
    int si_signo;
    int si_errno;

    int si_code;

    union
      {
 int _pad[((128 / sizeof (int)) - 4)];


 struct
   {
     __pid_t si_pid;
     __uid_t si_uid;
   } _kill;


 struct
   {
     int si_tid;
     int si_overrun;
     sigval_t si_sigval;
   } _timer;


 struct
   {
     __pid_t si_pid;
     __uid_t si_uid;
     sigval_t si_sigval;
   } _rt;


 struct
   {
     __pid_t si_pid;
     __uid_t si_uid;
     int si_status;
     __clock_t si_utime;
     __clock_t si_stime;
   } _sigchld;


 struct
   {
     void *si_addr;
   } _sigfault;


 struct
   {
     long int si_band;
     int si_fd;
   } _sigpoll;
      } _sifields;
  } siginfo_t;
# 129 "/usr/include/bits/siginfo.h" 3 4
enum
{
  SI_ASYNCNL = -60,

  SI_TKILL = -6,

  SI_SIGIO,

  SI_ASYNCIO,

  SI_MESGQ,

  SI_TIMER,

  SI_QUEUE,

  SI_USER,

  SI_KERNEL = 0x80

};



enum
{
  ILL_ILLOPC = 1,

  ILL_ILLOPN,

  ILL_ILLADR,

  ILL_ILLTRP,

  ILL_PRVOPC,

  ILL_PRVREG,

  ILL_COPROC,

  ILL_BADSTK

};


enum
{
  FPE_INTDIV = 1,

  FPE_INTOVF,

  FPE_FLTDIV,

  FPE_FLTOVF,

  FPE_FLTUND,

  FPE_FLTRES,

  FPE_FLTINV,

  FPE_FLTSUB

};


enum
{
  SEGV_MAPERR = 1,

  SEGV_ACCERR

};


enum
{
  BUS_ADRALN = 1,

  BUS_ADRERR,

  BUS_OBJERR

};


enum
{
  TRAP_BRKPT = 1,

  TRAP_TRACE

};


enum
{
  CLD_EXITED = 1,

  CLD_KILLED,

  CLD_DUMPED,

  CLD_TRAPPED,

  CLD_STOPPED,

  CLD_CONTINUED

};


enum
{
  POLL_IN = 1,

  POLL_OUT,

  POLL_MSG,

  POLL_ERR,

  POLL_PRI,

  POLL_HUP

};
# 273 "/usr/include/bits/siginfo.h" 3 4
typedef struct sigevent
  {
    sigval_t sigev_value;
    int sigev_signo;
    int sigev_notify;

    union
      {
 int _pad[((64 / sizeof (int)) - 4)];



 __pid_t _tid;

 struct
   {
     void (*_function) (sigval_t);
     void *_attribute;
   } _sigev_thread;
      } _sigev_un;
  } sigevent_t;






enum
{
  SIGEV_SIGNAL = 0,

  SIGEV_NONE,

  SIGEV_THREAD,


  SIGEV_THREAD_ID = 4

};
# 80 "/usr/include/signal.h" 2 3 4




typedef void (*__sighandler_t) (int);




extern __sighandler_t __sysv_signal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__));
# 99 "/usr/include/signal.h" 3 4


extern __sighandler_t signal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__));
# 113 "/usr/include/signal.h" 3 4

# 126 "/usr/include/signal.h" 3 4
extern int kill (__pid_t __pid, int __sig) __attribute__ ((__nothrow__));






extern int killpg (__pid_t __pgrp, int __sig) __attribute__ ((__nothrow__));




extern int raise (int __sig) __attribute__ ((__nothrow__));




extern __sighandler_t ssignal (int __sig, __sighandler_t __handler)
     __attribute__ ((__nothrow__));
extern int gsignal (int __sig) __attribute__ ((__nothrow__));




extern void psignal (int __sig, __const char *__s);




extern void psiginfo (__const siginfo_t *__pinfo, __const char *__s);
# 168 "/usr/include/signal.h" 3 4
extern int __sigpause (int __sig_or_mask, int __is_sig);
# 196 "/usr/include/signal.h" 3 4
extern int sigblock (int __mask) __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));


extern int sigsetmask (int __mask) __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));


extern int siggetmask (void) __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));
# 216 "/usr/include/signal.h" 3 4
typedef __sighandler_t sig_t;





extern int sigemptyset (sigset_t *__set) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int sigfillset (sigset_t *__set) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int sigaddset (sigset_t *__set, int __signo) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int sigdelset (sigset_t *__set, int __signo) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));


extern int sigismember (__const sigset_t *__set, int __signo)
     __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));
# 252 "/usr/include/signal.h" 3 4
# 1 "/usr/include/bits/sigaction.h" 1 3 4
# 25 "/usr/include/bits/sigaction.h" 3 4
struct sigaction
  {


    union
      {

 __sighandler_t sa_handler;

 void (*sa_sigaction) (int, siginfo_t *, void *);
      }
    __sigaction_handler;







    __sigset_t sa_mask;


    int sa_flags;


    void (*sa_restorer) (void);
  };
# 253 "/usr/include/signal.h" 2 3 4


extern int sigprocmask (int __how, __const sigset_t *__restrict __set,
   sigset_t *__restrict __oset) __attribute__ ((__nothrow__));






extern int sigsuspend (__const sigset_t *__set) __attribute__ ((__nonnull__ (1)));


extern int sigaction (int __sig, __const struct sigaction *__restrict __act,
        struct sigaction *__restrict __oact) __attribute__ ((__nothrow__));


extern int sigpending (sigset_t *__set) __attribute__ ((__nothrow__)) __attribute__ ((__nonnull__ (1)));






extern int sigwait (__const sigset_t *__restrict __set, int *__restrict __sig)
     __attribute__ ((__nonnull__ (1, 2)));






extern int sigwaitinfo (__const sigset_t *__restrict __set,
   siginfo_t *__restrict __info) __attribute__ ((__nonnull__ (1)));






extern int sigtimedwait (__const sigset_t *__restrict __set,
    siginfo_t *__restrict __info,
    __const struct timespec *__restrict __timeout)
     __attribute__ ((__nonnull__ (1)));



extern int sigqueue (__pid_t __pid, int __sig, __const union sigval __val)
     __attribute__ ((__nothrow__));
# 310 "/usr/include/signal.h" 3 4
extern __const char *__const _sys_siglist[65];
extern __const char *__const sys_siglist[65];


struct sigvec
  {
    __sighandler_t sv_handler;
    int sv_mask;

    int sv_flags;

  };
# 334 "/usr/include/signal.h" 3 4
extern int sigvec (int __sig, __const struct sigvec *__vec,
     struct sigvec *__ovec) __attribute__ ((__nothrow__));



# 1 "/usr/include/bits/sigcontext.h" 1 3 4
# 26 "/usr/include/bits/sigcontext.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 27 "/usr/include/bits/sigcontext.h" 2 3 4

struct _fpreg
{
  unsigned short significand[4];
  unsigned short exponent;
};

struct _fpxreg
{
  unsigned short significand[4];
  unsigned short exponent;
  unsigned short padding[3];
};

struct _xmmreg
{
  __uint32_t element[4];
};
# 109 "/usr/include/bits/sigcontext.h" 3 4
struct _fpstate
{

  __uint16_t cwd;
  __uint16_t swd;
  __uint16_t ftw;
  __uint16_t fop;
  __uint64_t rip;
  __uint64_t rdp;
  __uint32_t mxcsr;
  __uint32_t mxcr_mask;
  struct _fpxreg _st[8];
  struct _xmmreg _xmm[16];
  __uint32_t padding[24];
};

struct sigcontext
{
  unsigned long r8;
  unsigned long r9;
  unsigned long r10;
  unsigned long r11;
  unsigned long r12;
  unsigned long r13;
  unsigned long r14;
  unsigned long r15;
  unsigned long rdi;
  unsigned long rsi;
  unsigned long rbp;
  unsigned long rbx;
  unsigned long rdx;
  unsigned long rax;
  unsigned long rcx;
  unsigned long rsp;
  unsigned long rip;
  unsigned long eflags;
  unsigned short cs;
  unsigned short gs;
  unsigned short fs;
  unsigned short __pad0;
  unsigned long err;
  unsigned long trapno;
  unsigned long oldmask;
  unsigned long cr2;
  struct _fpstate * fpstate;
  unsigned long __reserved1 [8];
};
# 340 "/usr/include/signal.h" 2 3 4


extern int sigreturn (struct sigcontext *__scp) __attribute__ ((__nothrow__));






# 1 "/usr/lib/x86_64-linux-gnu/gcc/x86_64-linux-gnu/4.5.2/include/stddef.h" 1 3 4
# 350 "/usr/include/signal.h" 2 3 4




extern int siginterrupt (int __sig, int __interrupt) __attribute__ ((__nothrow__));

# 1 "/usr/include/bits/sigstack.h" 1 3 4
# 26 "/usr/include/bits/sigstack.h" 3 4
struct sigstack
  {
    void *ss_sp;
    int ss_onstack;
  };



enum
{
  SS_ONSTACK = 1,

  SS_DISABLE

};
# 50 "/usr/include/bits/sigstack.h" 3 4
typedef struct sigaltstack
  {
    void *ss_sp;
    int ss_flags;
    size_t ss_size;
  } stack_t;
# 357 "/usr/include/signal.h" 2 3 4


# 1 "/usr/include/sys/ucontext.h" 1 3 4
# 23 "/usr/include/sys/ucontext.h" 3 4
# 1 "/usr/include/signal.h" 1 3 4
# 24 "/usr/include/sys/ucontext.h" 2 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 25 "/usr/include/sys/ucontext.h" 2 3 4
# 33 "/usr/include/sys/ucontext.h" 3 4
typedef long int greg_t;





typedef greg_t gregset_t[23];
# 94 "/usr/include/sys/ucontext.h" 3 4
struct _libc_fpxreg
{
  unsigned short int significand[4];
  unsigned short int exponent;
  unsigned short int padding[3];
};

struct _libc_xmmreg
{
  __uint32_t element[4];
};

struct _libc_fpstate
{

  __uint16_t cwd;
  __uint16_t swd;
  __uint16_t ftw;
  __uint16_t fop;
  __uint64_t rip;
  __uint64_t rdp;
  __uint32_t mxcsr;
  __uint32_t mxcr_mask;
  struct _libc_fpxreg _st[8];
  struct _libc_xmmreg _xmm[16];
  __uint32_t padding[24];
};


typedef struct _libc_fpstate *fpregset_t;


typedef struct
  {
    gregset_t gregs;

    fpregset_t fpregs;
    unsigned long __reserved1 [8];
} mcontext_t;


typedef struct ucontext
  {
    unsigned long int uc_flags;
    struct ucontext *uc_link;
    stack_t uc_stack;
    mcontext_t uc_mcontext;
    __sigset_t uc_sigmask;
    struct _libc_fpstate __fpregs_mem;
  } ucontext_t;
# 360 "/usr/include/signal.h" 2 3 4





extern int sigstack (struct sigstack *__ss, struct sigstack *__oss)
     __attribute__ ((__nothrow__)) __attribute__ ((__deprecated__));



extern int sigaltstack (__const struct sigaltstack *__restrict __ss,
   struct sigaltstack *__restrict __oss) __attribute__ ((__nothrow__));
# 395 "/usr/include/signal.h" 3 4
# 1 "/usr/include/bits/sigthread.h" 1 3 4
# 31 "/usr/include/bits/sigthread.h" 3 4
extern int pthread_sigmask (int __how,
       __const __sigset_t *__restrict __newmask,
       __sigset_t *__restrict __oldmask)__attribute__ ((__nothrow__));


extern int pthread_kill (pthread_t __threadid, int __signo) __attribute__ ((__nothrow__));
# 396 "/usr/include/signal.h" 2 3 4






extern int __libc_current_sigrtmin (void) __attribute__ ((__nothrow__));

extern int __libc_current_sigrtmax (void) __attribute__ ((__nothrow__));




# 556 "papi.h" 2
# 569 "papi.h"
 typedef unsigned long PAPI_thread_id_t;


 typedef struct _papi_all_thr_spec {
     int num;
     PAPI_thread_id_t *id;
     void **data;
   } PAPI_all_thr_spec_t;

  typedef void (*PAPI_overflow_handler_t) (int EventSet, void *address,
                                long long overflow_vector, void *context);
# 592 "papi.h"
   typedef struct _papi_sprofil {
      void *pr_base;
      unsigned pr_size;
      caddr_t pr_off;
      unsigned pr_scale;



   } PAPI_sprofil_t;


   typedef struct _papi_itimer_option {
     int itimer_num;
     int itimer_sig;
     int ns;
     int flags;
   } PAPI_itimer_option_t;


   typedef struct _papi_inherit_option {
      int eventset;
      int inherit;
   } PAPI_inherit_option_t;


   typedef struct _papi_domain_option {
      int def_cidx;
      int eventset;
      int domain;
   } PAPI_domain_option_t;


   typedef struct _papi_granularity_option {
      int def_cidx;
      int eventset;
      int granularity;
   } PAPI_granularity_option_t;


   typedef struct _papi_preload_option {
      char lib_preload_env[128];
      char lib_preload_sep;
      char lib_dir_env[128];
      char lib_dir_sep;
   } PAPI_preload_info_t;


   typedef struct _papi_component_option {
     char name[128];
     char version[64];
     char support_version[64];
     char kernel_version[64];
     int CmpIdx;
     int num_cntrs;
     int num_mpx_cntrs;
     int num_preset_events;
     int num_native_events;
     int default_domain;
     int available_domains;
     int default_granularity;
     int available_granularities;
     int itimer_sig;
     int itimer_num;
     int itimer_ns;
     int itimer_res_ns;
     int hardware_intr_sig;
     int clock_ticks;
     int opcode_match_width;
     int os_version;
     int reserved[1];
     unsigned int hardware_intr:1;
     unsigned int precise_intr:1;
     unsigned int posix1b_timers:1;
     unsigned int kernel_profile:1;
     unsigned int kernel_multiplex:1;
     unsigned int data_address_range:1;
     unsigned int instr_address_range:1;
     unsigned int fast_counter_read:1;
     unsigned int fast_real_timer:1;
     unsigned int fast_virtual_timer:1;
     unsigned int attach:1;
     unsigned int attach_must_ptrace:1;
     unsigned int edge_detect:1;
     unsigned int invert:1;
     unsigned int profile_ear:1;
     unsigned int cntr_groups:1;
     unsigned int cntr_umasks:1;
     unsigned int cntr_IEAR_events:1;
     unsigned int cntr_DEAR_events:1;
     unsigned int cntr_OPCM_events:1;

     unsigned int cpu:1;
     unsigned int inherit:1;
     unsigned int reserved_bits:10;
   } PAPI_component_info_t;


   typedef struct _papi_mpx_info {
     int timer_sig;
     int timer_num;
     int timer_us;
   } PAPI_mpx_info_t;

   typedef int (*PAPI_debug_handler_t) (int code);


   typedef struct _papi_debug_option {
      int level;
      PAPI_debug_handler_t handler;
   } PAPI_debug_option_t;



   typedef struct _papi_address_map {
      char name[1024];
      caddr_t text_start;
      caddr_t text_end;
      caddr_t data_start;
      caddr_t data_end;
      caddr_t bss_start;
      caddr_t bss_end;
   } PAPI_address_map_t;



   typedef struct _papi_program_info {
      char fullname[1024];
      PAPI_address_map_t address_info;
   } PAPI_exe_info_t;


   typedef struct _papi_shared_lib_info {
      PAPI_address_map_t *map;
      int count;
   } PAPI_shlib_info_t;


typedef char* PAPI_user_defined_events_file_t;
# 759 "papi.h"
   typedef struct _papi_mh_tlb_info {
      int type;
      int num_entries;
      int page_size;
      int associativity;
   } PAPI_mh_tlb_info_t;


   typedef struct _papi_mh_cache_info {
      int type;
      int size;
      int line_size;
      int num_lines;
      int associativity;
   } PAPI_mh_cache_info_t;


   typedef struct _papi_mh_level_info {
      PAPI_mh_tlb_info_t tlb[6];
      PAPI_mh_cache_info_t cache[6];
   } PAPI_mh_level_t;



   typedef struct _papi_mh_info {
      int levels;
      PAPI_mh_level_t level[4];
   } PAPI_mh_info_t;



   typedef struct _papi_hw_info {
      int ncpu;
      int threads;
      int cores;
      int sockets;
      int nnodes;
      int totalcpus;
      int vendor;
      char vendor_string[128];
      int model;
      char model_string[128];
      float revision;
      int cpuid_family;
      int cpuid_model;
      int cpuid_stepping;
      float mhz;
      int clock_mhz;
      PAPI_mh_info_t mem_hierarchy;
   } PAPI_hw_info_t;


   typedef struct _papi_attach_option {
      int eventset;
      unsigned long tid;
   } PAPI_attach_option_t;


      typedef struct _papi_cpu_option {
         int eventset;
         unsigned int cpu_num;
      } PAPI_cpu_option_t;


   typedef struct _papi_multiplex_option {
      int eventset;
      int ns;
      int flags;
   } PAPI_multiplex_option_t;



   typedef struct _papi_addr_range_option {
      int eventset;
      caddr_t start;
      caddr_t end;
      int start_off;
      int end_off;
   } PAPI_addr_range_option_t;





 typedef union
 {
  PAPI_preload_info_t preload;
  PAPI_debug_option_t debug;
  PAPI_inherit_option_t inherit;
  PAPI_granularity_option_t granularity;
  PAPI_granularity_option_t defgranularity;
  PAPI_domain_option_t domain;
  PAPI_domain_option_t defdomain;
  PAPI_attach_option_t attach;
  PAPI_cpu_option_t cpu;
  PAPI_multiplex_option_t multiplex;
  PAPI_itimer_option_t itimer;
  PAPI_hw_info_t *hw_info;
  PAPI_shlib_info_t *shlib_info;
  PAPI_exe_info_t *exe_info;
  PAPI_component_info_t *cmp_info;
  PAPI_addr_range_option_t addr;
  PAPI_user_defined_events_file_t events_file;
 } PAPI_option_t;



 typedef struct _dmem_t {
   long long peak;
   long long size;
   long long resident;
   long long high_water_mark;
   long long shared;
   long long text;
   long long library;
   long long heap;
   long long locked;
   long long stack;
   long long pagesize;
   long long pte;
 } PAPI_dmem_info_t;
# 921 "papi.h"
   typedef struct event_info {
      unsigned int event_code;
      unsigned int event_type;
      unsigned int count;


      char symbol[1024];



      char short_descr[64];

      char long_descr[1024];


      char derived[64];




      char postfix[64];

      unsigned int code[12];


      char name[12]
               [256];


      char note[1024];


   } PAPI_event_info_t;




   int PAPI_accum(int EventSet, long long * values);
   int PAPI_add_event(int EventSet, int Event);
   int PAPI_add_events(int EventSet, int *Events, int number);
   int PAPI_assign_eventset_component(int EventSet, int cidx);
   int PAPI_attach(int EventSet, unsigned long tid);
   int PAPI_cleanup_eventset(int EventSet);
   int PAPI_create_eventset(int *EventSet);
   int PAPI_detach(int EventSet);
   char *PAPI_descr_error(int);
   int PAPI_destroy_eventset(int *EventSet);
   int PAPI_enum_event(int *EventCode, int modifier);
   int PAPI_event_code_to_name(int EventCode, char *out);
   int PAPI_event_name_to_code(char *in, int *out);
   int PAPI_get_dmem_info(PAPI_dmem_info_t *dest);
   int PAPI_get_event_info(int EventCode, PAPI_event_info_t * info);
   const PAPI_exe_info_t *PAPI_get_executable_info(void);
   const PAPI_hw_info_t *PAPI_get_hardware_info(void);
   const PAPI_component_info_t *PAPI_get_component_info(int cidx);
   int PAPI_get_multiplex(int EventSet);
   int PAPI_get_opt(int option, PAPI_option_t * ptr);
   int PAPI_get_cmp_opt(int option, PAPI_option_t * ptr,int cidx);
   long long PAPI_get_real_cyc(void);
   long long PAPI_get_real_nsec(void);
   long long PAPI_get_real_usec(void);
   const PAPI_shlib_info_t *PAPI_get_shared_lib_info(void);
   int PAPI_get_thr_specific(int tag, void **ptr);
   int PAPI_get_overflow_event_index(int Eventset, long long overflow_vector, int *array, int *number);
   long long PAPI_get_virt_cyc(void);
   long long PAPI_get_virt_nsec(void);
   long long PAPI_get_virt_usec(void);
   int PAPI_is_initialized(void);
   int PAPI_library_init(int version);
   int PAPI_list_events(int EventSet, int *Events, int *number);
   int PAPI_list_threads(unsigned long *tids, int *number);
   int PAPI_lock(int);
   int PAPI_multiplex_init(void);
   int PAPI_num_hwctrs(void);
   int PAPI_num_cmp_hwctrs(int cidx);
   int PAPI_num_hwctrs(void);
   int PAPI_num_events(int EventSet);
   int PAPI_overflow(int EventSet, int EventCode, int threshold,
                     int flags, PAPI_overflow_handler_t handler);
   int PAPI_perror(int code, char *destination, int length);
   int PAPI_profil(void *buf, unsigned bufsiz, caddr_t offset,
      unsigned scale, int EventSet, int EventCode,
      int threshold, int flags);
   int PAPI_query_event(int EventCode);
   int PAPI_read(int EventSet, long long * values);
   int PAPI_read_ts(int EventSet, long long * values, long long *cyc);
   int PAPI_register_thread(void);
   int PAPI_remove_event(int EventSet, int EventCode);
   int PAPI_remove_events(int EventSet, int *Events, int number);
   int PAPI_reset(int EventSet);
   int PAPI_set_debug(int level);
   int PAPI_set_cmp_domain(int domain, int cidx);
   int PAPI_set_domain(int domain);
   int PAPI_set_cmp_granularity(int granularity, int cidx);
   int PAPI_set_granularity(int granularity);
   int PAPI_set_multiplex(int EventSet);
   int PAPI_set_opt(int option, PAPI_option_t * ptr);
   int PAPI_set_thr_specific(int tag, void *ptr);
   void PAPI_shutdown(void);
   int PAPI_sprofil(PAPI_sprofil_t * prof, int profcnt, int EventSet, int EventCode, int threshold, int flags);
   int PAPI_start(int EventSet);
   int PAPI_state(int EventSet, int *status);
   int PAPI_stop(int EventSet, long long * values);
   char *PAPI_strerror(int);
   unsigned long PAPI_thread_id(void);
   int PAPI_thread_init(unsigned long (*id_fn) (void));
   int PAPI_unlock(int);
   int PAPI_unregister_thread(void);
   int PAPI_write(int EventSet, long long * values);
# 1047 "papi.h"
   int PAPI_accum_counters(long long * values, int array_len);
   int PAPI_num_counters(void);
   int PAPI_num_components(void);
   int PAPI_read_counters(long long * values, int array_len);
   int PAPI_start_counters(int *events, int array_len);
   int PAPI_stop_counters(long long * values, int array_len);
   int PAPI_flips(float *rtime, float *ptime, long long * flpins, float *mflips);
   int PAPI_flops(float *rtime, float *ptime, long long * flpops, float *mflops);
   int PAPI_ipc(float *rtime, float *ptime, long long * ins, float *ipc);
# 26 "papi_fwrappers.c" 2
# 58 "papi_fwrappers.c"
void PAPIF_ACCUM ( int *EventSet, long long *values, int *check )

{
 *check = PAPI_accum( *EventSet, values );
}
# 74 "papi_fwrappers.c"
void PAPIF_ADD_EVENT ( int *EventSet, int *Event, int *check )

{
 *check = PAPI_add_event( *EventSet, *Event );
}
# 90 "papi_fwrappers.c"
void PAPIF_ADD_EVENTS ( int *EventSet, int *Events, int *number, int *check )

{
 *check = PAPI_add_events( *EventSet, Events, *number );
}
# 106 "papi_fwrappers.c"
void PAPIF_CLEANUP_EVENTSET ( int *EventSet, int *check )

{
 *check = PAPI_cleanup_eventset( *EventSet );
}
# 122 "papi_fwrappers.c"
void PAPIF_CREATE_EVENTSET ( int *EventSet, int *check )

{
 *check = PAPI_create_eventset( EventSet );
}
# 138 "papi_fwrappers.c"
void PAPIF_ASSIGN_EVENTSET_COMPONENT ( int *EventSet, int *cidx, int *check )

{
 *check = PAPI_assign_eventset_component( *EventSet, *cidx );
}
# 154 "papi_fwrappers.c"
void PAPIF_DESTROY_EVENTSET ( int *EventSet, int *check )

{
 *check = PAPI_destroy_eventset( EventSet );
}
# 171 "papi_fwrappers.c"
void PAPIF_GET_DMEM_INFO ( long long *dest, int *check )

{
 *check = PAPI_get_dmem_info( ( PAPI_dmem_info_t * ) dest );
}
# 191 "papi_fwrappers.c"
void PAPIF_GET_EXE_INFO ( char *fullname, char *name, long long *text_start, long long *text_end, long long *data_start, long long *data_end, long long *bss_start, long long *bss_end, int *check, int fullname_len, int name_len )
# 202 "papi_fwrappers.c"
{
 PAPI_option_t e;





 if ( ( *check = PAPI_get_opt( 17, &e ) ) == 0 ) {

  int i;
  strncpy( fullname, e.exe_info->fullname, ( size_t ) fullname_len );
  for ( i = ( int ) strlen( e.exe_info->fullname ); i < fullname_len;
     fullname[i++] = ' ' );
  strncpy( name, e.exe_info->address_info.name, ( size_t ) name_len );
  for ( i = ( int ) strlen( e.exe_info->address_info.name ); i < name_len;
     name[i++] = ' ' );




  *text_start = ( long ) e.exe_info->address_info.text_start;
  *text_end = ( long ) e.exe_info->address_info.text_end;
  *data_start = ( long ) e.exe_info->address_info.data_start;
  *data_end = ( long ) e.exe_info->address_info.data_end;
  *bss_start = ( long ) e.exe_info->address_info.bss_start;
  *bss_end = ( long ) e.exe_info->address_info.bss_end;
 }
}
# 244 "papi_fwrappers.c"
void PAPIF_GET_HARDWARE_INFO ( int *ncpu, int *nnodes, int *totalcpus, int *vendor, char *vendor_str, int *model, char *model_str, float *revision, float *mhz, int vendor_len, int model_len )
# 270 "papi_fwrappers.c"
{
 const PAPI_hw_info_t *hwinfo;
 int i;
 hwinfo = PAPI_get_hardware_info( );
 if ( hwinfo == ((void *)0) ) {
  *ncpu = 0;
  *nnodes = 0;
  *totalcpus = 0;
  *vendor = 0;
  *model = 0;
  *revision = 0;
  *mhz = 0;
 } else {
  *ncpu = hwinfo->ncpu;
  *nnodes = hwinfo->nnodes;
  *totalcpus = hwinfo->totalcpus;
  *vendor = hwinfo->vendor;
  *model = hwinfo->model;
  *revision = hwinfo->revision;
  *mhz = hwinfo->mhz;

  strncpy( vendor_str, hwinfo->vendor_string, ( size_t ) vendor_len );
  for ( i = ( int ) strlen( hwinfo->vendor_string ); i < vendor_len;
     vendor_str[i++] = ' ' );
  strncpy( model_str, hwinfo->model_string, ( size_t ) model_len );
  for ( i = ( int ) strlen( hwinfo->model_string ); i < model_len;
     model_str[i++] = ' ' );







 }
 return;
}
# 319 "papi_fwrappers.c"
void PAPIF_num_hwctrs ( int *num )
{
 *num = PAPI_num_hwctrs( );
}
# 335 "papi_fwrappers.c"
void PAPIF_num_cmp_hwctrs ( int *cidx, int *num )

{
 *num = PAPI_num_cmp_hwctrs( *cidx );
}
# 351 "papi_fwrappers.c"
void PAPIF_GET_REAL_CYC ( long long *real_cyc )
{
 *real_cyc = PAPI_get_real_cyc( );
}
# 366 "papi_fwrappers.c"
void PAPIF_GET_REAL_USEC ( long long *time )
{
 *time = PAPI_get_real_usec( );
}
# 381 "papi_fwrappers.c"
void PAPIF_GET_REAL_NSEC ( long long *time )
{
 *time = PAPI_get_real_nsec( );
}
# 396 "papi_fwrappers.c"
void PAPIF_GET_VIRT_CYC ( long long *virt_cyc )
{
 *virt_cyc = PAPI_get_virt_cyc( );
}
# 411 "papi_fwrappers.c"
void PAPIF_GET_VIRT_USEC ( long long *time )
{
 *time = PAPI_get_virt_usec( );
}
# 426 "papi_fwrappers.c"
void PAPIF_IS_INITIALIZED ( int *level )
{
 *level = PAPI_is_initialized( );
}
# 441 "papi_fwrappers.c"
void PAPIF_LIBRARY_INIT ( int *check )
{
 *check = PAPI_library_init( *check );
}
# 456 "papi_fwrappers.c"
void PAPIF_THREAD_ID ( unsigned long *id )
{
 *id = PAPI_thread_id( );
}
# 471 "papi_fwrappers.c"
void PAPIF_REGISTER_THREAD ( int *check )
{
 *check = PAPI_register_thread( );
}
# 486 "papi_fwrappers.c"
void PAPIF_UNREGSTER_THREAD ( int *check )
{
 *check = PAPI_unregister_thread( );
}
# 502 "papi_fwrappers.c"
void PAPIF_THREAD_INIT ( unsigned long int ( *handle ) ( void ), int *check )

{
 *check = PAPI_thread_init( handle );
}
# 518 "papi_fwrappers.c"
void PAPIF_LIST_EVENTS ( int *EventSet, int *Events, int *number, int *check )

{
 *check = PAPI_list_events( *EventSet, Events, number );
}
# 534 "papi_fwrappers.c"
void PAPIF_MULTIPLEX_INIT ( int *check )
{
 *check = PAPI_multiplex_init( );
}
# 549 "papi_fwrappers.c"
void PAPIF_GET_MULTIPLEX ( int *EventSet, int *check )

{
 *check = PAPI_get_multiplex( *EventSet );
}
# 565 "papi_fwrappers.c"
void PAPIF_SET_MULTIPLEX ( int *EventSet, int *check )

{
 *check = PAPI_set_multiplex( *EventSet );
}
# 582 "papi_fwrappers.c"
void PAPIF_PERROR ( int *code, char *destination_str, int *check, int destination_len )






{

 int i;
 char tmp[128];

 *check = PAPI_perror( *code, tmp, 128 );

 strncpy( destination_str, tmp, ( size_t ) destination_len );

 for ( i = ( int ) strlen( tmp ); i < destination_len;
    destination_str[i++] = ' ' );






}
# 626 "papi_fwrappers.c"
void PAPIF_QUERY_EVENT ( int *EventCode, int *check )

{
 *check = PAPI_query_event( *EventCode );
}
# 645 "papi_fwrappers.c"
void PAPIF_GET_EVENT_INFO ( int *EventCode, char *symbol, char *long_descr, char *short_descr, int *count, char *event_note, int *flags, int *check, int symbol_len, int long_descr_len, int short_descr_len, int event_note_len )
# 655 "papi_fwrappers.c"
{
 PAPI_event_info_t info;
 ( void ) flags;

 int i;
 if ( ( *check = PAPI_get_event_info( *EventCode, &info ) ) == 0 ) {
  strncpy( symbol, info.symbol, ( size_t ) symbol_len );
  for ( i = ( int ) strlen( info.symbol ); i < symbol_len;
     symbol[i++] = ' ' );
  strncpy( long_descr, info.long_descr, ( size_t ) long_descr_len );
  for ( i = ( int ) strlen( info.long_descr ); i < long_descr_len;
     long_descr[i++] = ' ' );
  strncpy( short_descr, info.short_descr, ( size_t ) short_descr_len );
  for ( i = ( int ) strlen( info.short_descr ); i < short_descr_len;
     short_descr[i++] = ' ' );
  *count = ( int ) info.count;
  strncpy( event_note, info.note, ( size_t ) event_note_len );
  for ( i = ( int ) strlen( info.note ); i < event_note_len;
     event_note[i++] = ' ' );
 }
# 686 "papi_fwrappers.c"
}
# 699 "papi_fwrappers.c"
void PAPIF_EVENT_CODE_TO_NAME ( int *EventCode, char *out_str, int *check, int out_len )





{

 char tmp[128];
 int i;
 *check = PAPI_event_code_to_name( *EventCode, tmp );

 strncpy( out_str, tmp, ( size_t ) out_len );

 for ( i = ( int ) strlen( tmp ); i < out_len; out_str[i++] = ' ' );




}
# 731 "papi_fwrappers.c"
void PAPIF_EVENT_NAME_TO_CODE ( char *in_str, int *out, int *check, int in_len )





{

 int slen, i;
 char tmpin[128];


 slen = in_len < 128 ? in_len : 128;
 strncpy( tmpin, in_str, ( size_t ) slen );


 for ( i = slen - 1; i > -1 && tmpin[i] == ' '; tmpin[i--] = '\0' );


 tmpin[128 - 1] = '\0';
 if ( slen < 128 )
  tmpin[slen] = '\0';

 *check = PAPI_event_name_to_code( tmpin, out );




}
# 771 "papi_fwrappers.c"
void PAPIF_NUM_EVENTS ( int *EventCode, int *count )
{
 *count = PAPI_num_events( *EventCode );
}
# 786 "papi_fwrappers.c"
void PAPIF_ENUM_EVENT ( int *EventCode, int *modifier, int *check )

{
 *check = PAPI_enum_event( EventCode, *modifier );
}
# 802 "papi_fwrappers.c"
void PAPIF_READ ( int *EventSet, long long *values, int *check )

{
 *check = PAPI_read( *EventSet, values );
}
# 818 "papi_fwrappers.c"
void PAPIF_READ_TS ( int *EventSet, long long *values, long long *cycles, int *check )

{
 *check = PAPI_read_ts( *EventSet, values, cycles );
}
# 834 "papi_fwrappers.c"
void PAPIF_REMOVE_EVENT ( int *EventSet, int *Event, int *check )

{
 *check = PAPI_remove_event( *EventSet, *Event );
}
# 850 "papi_fwrappers.c"
void PAPIF_REMOVE_EVENTS ( int *EventSet, int *Events, int *number, int *check )

{
 *check = PAPI_remove_events( *EventSet, Events, *number );
}
# 866 "papi_fwrappers.c"
void PAPIF_RESET ( int *EventSet, int *check )
{
 *check = PAPI_reset( *EventSet );
}
# 881 "papi_fwrappers.c"
void PAPIF_SET_DEBUG ( int *debug, int *check )
{
 *check = PAPI_set_debug( *debug );
}
# 896 "papi_fwrappers.c"
void PAPIF_SET_DOMAIN ( int *domain, int *check )
{
 *check = PAPI_set_domain( *domain );
}
# 911 "papi_fwrappers.c"
void PAPIF_SET_CMP_DOMAIN ( int *domain, int *cidx, int *check )

{
 *check = PAPI_set_cmp_domain( *domain, *cidx );
}
# 927 "papi_fwrappers.c"
void PAPIF_SET_GRANULARITY ( int *granularity, int *check )

{
 *check = PAPI_set_granularity( *granularity );
}
# 943 "papi_fwrappers.c"
void PAPIF_SET_CMP_GRANULARITY ( int *granularity, int *cidx, int *check )

{
 *check = PAPI_set_cmp_granularity( *granularity, *cidx );
}
# 959 "papi_fwrappers.c"
void PAPIF_SHUTDOWN ( void )
{
 PAPI_shutdown( );
}
# 974 "papi_fwrappers.c"
void PAPIF_START ( int *EventSet, int *check )
{
 *check = PAPI_start( *EventSet );
}
# 989 "papi_fwrappers.c"
void PAPIF_STATE ( int *EventSet, int *status, int *check )

{
 *check = PAPI_state( *EventSet, status );
}
# 1005 "papi_fwrappers.c"
void PAPIF_STOP ( int *EventSet, long long *values, int *check )

{
 *check = PAPI_stop( *EventSet, values );
}
# 1021 "papi_fwrappers.c"
void PAPIF_WRITE ( int *EventSet, long long *values, int *check )

{
 *check = PAPI_write( *EventSet, values );
}
# 1037 "papi_fwrappers.c"
void PAPIF_LOCK ( int *lock, int *check )

{
 *check = PAPI_lock( *lock );
}
# 1053 "papi_fwrappers.c"
void PAPIF_unlock ( int *lock, int *check )

{
 *check = PAPI_unlock( *lock );
}
# 1071 "papi_fwrappers.c"
void PAPIF_START_COUNTERS ( int *events, int *array_len, int *check )

{
 *check = PAPI_start_counters( events, *array_len );
}
# 1087 "papi_fwrappers.c"
void PAPIF_READ_COUNTERS ( long long *values, int *array_len, int *check )

{
 *check = PAPI_read_counters( values, *array_len );
}
# 1103 "papi_fwrappers.c"
void PAPIF_STOP_COUNTERS ( long long *values, int *array_len, int *check )

{
 *check = PAPI_stop_counters( values, *array_len );
}
# 1119 "papi_fwrappers.c"
void PAPIF_ACCUM_COUNTERS ( long long *values, int *array_len, int *check )

{
 *check = PAPI_accum_counters( values, *array_len );
}
# 1135 "papi_fwrappers.c"
void PAPIF_NUM_COUNTERS ( int *numevents )
{
 *numevents = PAPI_num_counters( );
}
# 1150 "papi_fwrappers.c"
void PAPIF_IPC ( float *rtime, float *ptime, long long *ins, float *ipc, int *check )


{
 *check = PAPI_ipc( rtime, ptime, ins, ipc );
}
# 1167 "papi_fwrappers.c"
void PAPIF_FLIPS ( float *real_time, float *proc_time, long long *flpins, float *mflips, int *check )


{
 *check = PAPI_flips( real_time, proc_time, flpins, mflips );
}
# 1184 "papi_fwrappers.c"
void PAPIF_FLOPS ( float *real_time, float *proc_time, long long *flpops, float *mflops, int *check )


{
 *check = PAPI_flops( real_time, proc_time, flpops, mflops );
}
# 1206 "papi_fwrappers.c"
void PAPIF_GET_CLOCKRATE ( int *cr )
{
 *cr = PAPI_get_opt( 14, ((void *)0) );
}
# 1224 "papi_fwrappers.c"
void PAPIF_GET_PRELOAD ( char *lib_preload_env, int *check, int lib_preload_env_len )





{
 PAPI_option_t p;

 int i;

 if ( ( *check = PAPI_get_opt( 13, &p ) ) == 0 ) {
  strncpy( lib_preload_env, p.preload.lib_preload_env,
     ( size_t ) lib_preload_env_len );
  for ( i = ( int ) strlen( p.preload.lib_preload_env );
     i < lib_preload_env_len; lib_preload_env[i++] = ' ' );
 }





}
# 1258 "papi_fwrappers.c"
void PAPIF_GET_GRANULARITY ( int *eventset, int *granularity, int *mode, int *check )

{
 PAPI_option_t g;

 if ( *mode == 6 ) {
  *granularity = PAPI_get_opt( *mode, &g );
  *check = 0;
 } else if ( *mode == 7 ) {
  g.granularity.eventset = *eventset;
  if ( ( *check = PAPI_get_opt( *mode, &g ) ) == 0 ) {
   *granularity = g.granularity.granularity;
  }
 } else {
  *check = -1;
 }
}
# 1286 "papi_fwrappers.c"
void PAPIF_GET_DOMAIN ( int *eventset, int *domain, int *mode, int *check )

{
 PAPI_option_t d;

 if ( *mode == 4 ) {
  *domain = PAPI_get_opt( *mode, ((void *)0) );
  *check = 0;
 } else if ( *mode == 5 ) {
  d.domain.eventset = *eventset;
  if ( ( *check = PAPI_get_opt( *mode, &d ) ) == 0 ) {
   *domain = d.domain.domain;
  }
 } else {
  *check = -1;
 }
}
# 1326 "papi_fwrappers.c"
void PAPIF_SET_EVENT_DOMAIN ( int *es, int *domain, int *check )

{
 PAPI_option_t d;

 d.domain.domain = *domain;
 d.domain.eventset = *es;
 *check = PAPI_set_opt( 5, &d );
}
# 1346 "papi_fwrappers.c"
void PAPIF_SET_INHERIT ( int *inherit, int *check )
{
 PAPI_option_t i;

 i.inherit.inherit = *inherit;
 *check = PAPI_set_opt( 28, &i );
}
