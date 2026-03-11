/* $Id: mdx_util.h,v 1.1 2022/03/03 17:18:03 alex Exp $ */
/*******************************************************************************

    mdx_util.h

    Memory Descriptor (MDX) Utilities.

*******************************************************************************/

#ifndef  MDX_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  MDX_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <stdio.h>			/* Standard I/O definitions. */


/*******************************************************************************
    Memory Descriptors (Client View) and Definitions.
*******************************************************************************/

typedef  enum  MdxStorageClass {
    MdxStatic, MdxDynamic, MdxVolatile
}  MdxStorageClass ;

					/* Memory descriptors. */
typedef  struct  _MemoryDx  *MemoryDx ;


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

					/* Global debug switch (1/0 = yes/no). */
extern  int  mdx_util_debug  OCD ("mdx_util") ;

/*******************************************************************************
    Public functions.
*******************************************************************************/

errno_t  mdxAddB P_((MemoryDx descriptor,
                     char byte))
    OCD ("mdx_util") ;

errno_t  mdxAddS P_((MemoryDx descriptor,
                     bool align,
                     const void *data,
                     size_t length))
    OCD ("mdx_util") ;

size_t  mdxAlignment P_((MemoryDx descriptor,
                         ssize_t newBoundary))
    OCD ("mdx_util") ;

errno_t  mdxCopy P_((MemoryDx source,
                     bool isStatic,
                     MemoryDx destination))
    OCD ("mdx_util") ;

errno_t  mdxCreate P_((void *region,
                       size_t length,
                       MdxStorageClass allocation,
                       MemoryDx *descriptor))
    OCD ("mdx_util") ;

errno_t  mdxDestroy P_((MemoryDx descriptor))
    OCD ("mdx_util") ;

MemoryDx  mdxDuplicate P_((MemoryDx descriptor))
    OCD ("mdx_util") ;

size_t  mdxElementSize P_((MemoryDx descriptor,
                           ssize_t elementSize))
    OCD ("mdx_util") ;

errno_t  mdxErase P_((MemoryDx descriptor))
    OCD ("mdx_util") ;

errno_t  mdxGrow P_((MemoryDx descriptor,
                     size_t toAdd))
    OCD ("mdx_util") ;

size_t  mdxIncrement P_((MemoryDx descriptor,
                         ssize_t blockSize))
    OCD ("mdx_util") ;

size_t  mdxLength P_((MemoryDx descriptor))
    OCD ("mdx_util") ;

errno_t  mdxOwn P_((MemoryDx descriptor))
    OCD ("mdx_util") ;

void  *mdxRegionP P_((MemoryDx descriptor,
                      size_t index))
    OCD ("mdx_util") ;

errno_t  mdxSetLength P_((MemoryDx descriptor,
                          size_t length))
    OCD ("mdx_util") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
