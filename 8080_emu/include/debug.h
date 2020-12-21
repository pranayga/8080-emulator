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
 * @brief      When the condition X is false, a breakpoint is generated.
 *
 * @param      X     Condition to test
 */
#define ASSERT( X ){\
  if ( !(X) ){\
    breakpoint();\
  }\
}

/**
 * @brief      Logs the Line, File and Fun onto STDERR
 *
 * @param      ...   Variadic inputs for fprintf
 */
#define DEBUG_PRINT( ... ){\
  fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__);\
}

/**
 * @brief      Logs an error message when the condition is not met.
 *
 * @param      X     Condition to test.
 * @param      ...   variadic inputs for fprintf
 */
#define WARN( X, ... ){\
  if ( !(X) ){\
      fprintf(stderr, "WARN::%s:%d:%s(): " fmt, __FILE__, \
                                __LINE__, __func__, __VA_ARGS__);\
  }\
}

#else

/**
 * @brief      Stub for when flag is not passed.
 *
 * @param      ...   Variadic unused, removed by compiler
 */
//{@
#define ASSERT( ... ) do {} while( 0 );
#define DEBUG_PRINT( ... ) do {} while( 0 );
#define WARN( ... ) do {} while( 0 );
//@}

#endif /* DEBUG */

#endif /* _DEBUG_H_ */