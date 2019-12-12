// inclusion protection should not be a problem since #22594
#ifndef DEFINES_HEADER_H__
#define DEFINES_HEADER_H__

// empty macro followed by another one shouldn't be a problem either since #22594
#define SOME_MACRO_WITHOUT_VALUE1
#define SOME_MACRO_WITHOUT_VALUE2
#define SOME_MACRO_WITHOUT_VALUE3
#define FIRST_CONSTANT 5
#define OTHER_CONSTANT 6
#define SOME_MACRO_WITHOUT_VALUE4
#define SOME_MACRO_WITHOUT_VALUE5
#define LAST_CONSTANT 7

// This was copied from RDB header
/** @addtogroup ARRAY_SIZES
 *  ------ array sizes ------
 *  @{
 */
#define RDB_SIZE_OBJECT_NAME       32       /**< m */
#define RDB_SIZE_SCP_NAME          64       /**< maximum length of an SCP sender / receiver         @version 0x010C */
#define RDB_SIZE_FILENAME        1024       /**< number of bytes in a filename (may include path)   @version 0x0100 */
#define RDB_SIZE_TRLIGHT_PHASES     8       /**< maximum number of phases for a traffic light       @version 0x0100 */
/** @} */

#endif /* DEFINES_HEADER_H__ */
