#ifdef COMMENT
    file ff.h
#endif

#ifdef COMMENT
	Copyright abandoned, 1983, The Rand Corporation
#endif

#include <stdio.h>
/* #define UNIXV7       / * if this is Version 7   */
/* #define UNIXV6       / * if this is Version 6   */

#ifndef _NFILE
#define _NFILE 20
#endif /* _NFILE */

extern int errno;

#ifdef OUT
#ifdef  BIGADDR
#define FF_BSIZE   (BUFSIZ * 4) /* Size of a FF block */
#else
#define FF_BSIZE    BUFSIZ      /* Size of a FF block */
#endif  /* BIGADDR */
#endif /* OUT */
#define FF_BSIZE    BUFSIZ      /* Size of a FF block */


#define NOFILE _NFILE           /* Number of system opens allowed */
#define NOFFFDS (NOFILE+NOFILE) /* # of active FF files */

/* file description structure.  one for each distinct file open */
typedef
struct ff_file {
    struct ff_buf *fb_qf;       /* Queue of blks associated with this file */
    char	fn_fd,		/* File Descriptor	*/
		fn_mode,	/* Open mode for file	*/
		fn_refs;	/* # of references	*/
#ifdef UNIXV7
    dev_t       fn_dev;         /* Device file is on    */
    ino_t       fn_ino;         /* Inode of file        */
#endif
#ifdef UNIXV6
    char        fn_minor,       /* Device file is on    */
		fn_major;
    short       fn_ino;         /* Inode of file        */
#endif
    long        fn_realblk;     /* Image of sys fil pos */
    long        fn_size;        /* Current file size    */
#ifdef  EUNICE
    char       *fn_memaddr;     /* address of in-core file */
#endif  /* EUNICE */
} Ff_file;
extern Ff_file ff_files[];

/* stream structure */
typedef
struct ff_stream {
    char        f_mode,         /* Open mode for handle */
		f_count;	/* Reference count	*/
    Ff_file    *f_file; 	/* Fnode pointer	*/
    long	f_offset;	/* Current file position*/
} Ff_stream;			/*  or buffered amount	*/
/* f_flag bits */
#define F_READ	01		/* File opened for read */
#define F_WRITE 02		/* File opened for write*/
extern Ff_stream ff_streams[];

/* Buffer structure.  one for each buffer in cache */
typedef struct  ff_buf {
    struct ff_buf *fb_qf,       /* Q of blks associated */
	       *fb_qb,          /*  with this file      */
	       *fb_forw,        /* forw ptr */
	       *fb_back;        /* back ptr */
    Ff_file    *fb_file;	/* Fnode blk is q'd on	*/
    long        fb_bnum;        /* Block # of this blk  */
    short       fb_count,       /* Byte count of block  */
		fb_wflg;        /* Block modified flag  */
    char       *fb_buf;         /* Actual data buffer [FF_BSIZE]  */
} Ff_buf;

/* list of all buffers. there is only one of these */
typedef struct ff_rbuf {
    struct ff_buf *fb_qf,       /* not used */
		  *fb_qb,       /* not used */
		  *fb_forw,     /* first buf in chain */
		  *fb_back;     /* last buf in chain */
    short          fr_count;    /* total number of buffers */
} Ff_rbuf;
extern Ff_rbuf ff_flist;

typedef struct ff_st {
    int 	fs_seek,	/* Total seek sys calls */
		fs_read,	/*	 read  "    "	*/
		fs_write;	/*	 write "    "	*/
    int         fs_ffseek,      /* Total seek calls */
		fs_ffread,      /*       read   "   */
		fs_ffwrite;     /*       write  "   */
} Ff_stats;
extern Ff_stats ff_stats;

extern void       ff_sort (Ff_file *);
extern int        ff_close (Ff_stream *);
extern short      ff_alloc (int, int);
extern short      ff_free (int, int);
extern short      ff_use (char *, int);
extern int        ff_flush (Ff_stream *);
extern int        ff_getc  (Ff_stream *);
extern int        ff_putc  (int, Ff_stream *);
extern int        ff_sync  (int);
extern int        ff_read (Ff_stream *, char *, int, int, char *);
extern int        ff_write (Ff_stream *, char *, int);
extern int        ff_point (Ff_stream *, long, char **, int);
extern long       ff_size (Ff_stream *);
extern long       ff_grow (Ff_stream *);
extern long       ff_pos (Ff_stream *);
extern long       ff_seek (Ff_stream *, long, int);
extern Ff_buf    *ff_getblk (Ff_file *, long);
extern Ff_buf    *ff_gblk (Ff_file *, long, int);
extern Ff_buf    *ff_haveblk (Ff_file *, long);
extern Ff_buf    *ff_putblk (Ff_buf *, int);
extern Ff_stream *ff_open (char *, int, int);
extern Ff_stream *ff_fdopen (int, int, int);
extern int        lenbrk (char *, int, int, char *);
extern char       ff_fd (Ff_stream *);
