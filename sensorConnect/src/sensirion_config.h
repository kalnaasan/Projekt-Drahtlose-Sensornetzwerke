#ifndef SENSIRION_CONFIG_H
#define SENSIRION_CONFIG_H

/**
 * Zephyr includes
 * - NULL
 * - (probably) all types of int below ----> STILL
*/

#include <zephyr/kernel.h>

/**
 * If your platform does not provide the library stdlib.h you have to remove the
 * include and define NULL yourself (see below).
 */
//#include <stdlib.h>

/**
 * #ifndef NULL
 * #define NULL ((void *)0)
 * #endif
 */

/**
 * If your platform does not provide the library stdint.h you have to
 * define the integral types yourself (see below).
 */
//#include <stdint.h>

/**
 * Typedef section for types commonly defined in <stdint.h>
 * If your system does not provide stdint headers, please define them
 * accordingly. Please make sure to define int64_t and uint64_t.
 */
/* typedef unsigned long long int uint64_t;
 * typedef long long int int64_t;
 * typedef long int32_t;
 * typedef unsigned long uint32_t;
 * typedef short int16_t;
 * typedef unsigned short uint16_t;
 * typedef char int8_t;
 * typedef unsigned char uint8_t;
 */

#ifndef __cplusplus

#endif /* __cplusplus */

#endif /* SENSIRION_CONFIG_H */
