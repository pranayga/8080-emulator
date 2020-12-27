/**
 * @file debug.h
 * @author Pranay Garg (pranayga@andrew.cmu)
 * @author Ronit Banerjee <ronitb@andrew.cmu.edu>
 * @brief Pandu's Debugging Macros
 * @version 0.1
 * @date 2020-12-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef DEBUG

/**
 * @brief      Logs the Line, File and Fun onto STDERR
 *
 * @param      ...   Variadic inputs for fprintf
 */
#define DEBUG_PRINT( fmt, args... ){ \
  fprintf(stderr, "DEBUG: %-15s:%d:%-15s():", __FILE__, \
                                __LINE__, __func__);\
  fprintf(stderr, fmt, args );\
}

/**
 * @brief      Logs an error message when the condition is not met.
 *
 * @param      X     Condition to test.
 * @param      ...   variadic inputs for fprintf
 */
#define WARN( X, fmt, args... ){ \
  if ( !(X) ){\
      fprintf(stderr, "WARN:  %-15s:%d:%-15s():", __FILE__, \
                                __LINE__, __func__);\
      fprintf(stderr, fmt, args );\
  }\
}

#else

/**
 * @brief      Stub for when flag is not passed.
 *
 * @param      ...   Variadic unused, removed by compiler
 */
//{@
#define DEBUG_PRINT( ... ) do {} while( 0 );
#define WARN( ... ) do {} while( 0 );
//@}

#endif /* DEBUG */

#ifdef DECOMPILE

/**
 * @brief      LOGS decomplied opcode
 *
 * @param      ...   Variadic inputs for fprintf
 */
#define DECOMPILE_PRINT( PC, fmt, args... ){ \
  fprintf(stdout, "%x : ", PC);\
  fprintf(stdout, fmt, args );\
}

#else

/**
 * @brief      Stub for when flag is not passed.
 *
 * @param      ...   Variadic unused, removed by compiler
 */
//{@
#define DECOMPILE_PRINT( ... ) do {} while( 0 );
//@}

#endif /* DECOMPILE */

#endif /* _DEBUG_H_ */