/* $Id: adx_util.h,v 1.1 2022/03/14 05:34:29 alex Exp $ */
/*******************************************************************************

    adx_util.h

    Array Descriptor (ADX) Utilities.

*******************************************************************************/

#ifndef  ADX_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  ADX_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    Memory Descriptors (Client View) and Definitions.
*******************************************************************************/

typedef  enum  AdxStorageClass {
    AdxStatic, AdxDynamic, AdxVolatile
}  AdxStorageClass ;

					/* Memory descriptors. */
typedef  struct  _ArrayDx  *ArrayDx ;


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

					/* Global debug switch (1/0 = yes/no). */
extern  int  adx_util_debug  OCD ("adx_util") ;

/*******************************************************************************
    Public functions.
*******************************************************************************/

errno_t  adxAdd P_((ArrayDx descriptor,
                    const void *elements,
                    size_t numElements,
                    ssize_t index))
    OCD ("adx_util") ;

errno_t  adxCopy P_((ArrayDx source,
                     bool isStatic,
                     ArrayDx destination))
    OCD ("adx_util") ;

size_t  adxCount P_((ArrayDx descriptor))
    OCD ("adx_util") ;

errno_t  adxCreate P_((size_t offset,
                       size_t elementSize,
                       void *elements,
                       size_t numElements,
                       AdxStorageClass allocation,
                       ArrayDx *descriptor))
    OCD ("adx_util") ;

errno_t  adxDelete P_((ArrayDx descriptor,
                       size_t numElements,
                       ssize_t index))
    OCD ("adx_util") ;

errno_t  adxDestroy P_((ArrayDx descriptor))
    OCD ("adx_util") ;

ArrayDx  adxDuplicate P_((ArrayDx descriptor))
    OCD ("adx_util") ;

size_t  adxElementSize P_((ArrayDx descriptor))
    OCD ("adx_util") ;

errno_t  adxErase P_((ArrayDx descriptor))
    OCD ("adx_util") ;

void  *adxGet P_((ArrayDx descriptor,
                  ssize_t index))
    OCD ("adx_util") ;

size_t  adxIncrement P_((ArrayDx descriptor,
                         ssize_t numElements))
    OCD ("adx_util") ;

errno_t  adxOwn P_((ArrayDx descriptor))
    OCD ("adx_util") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
