/*
//	@(#) TODDY: General data Structures V0.0.1  (Buea/Essex)	2002/07/26
//      defs.h - general #defines


// The TODDY Project Copyright (C) 1996-2004 William S. Shu

// This software is distributed without any warranty; without even the
// implied warranty of merchantability or fitness for a particular purpose.
// See file LICENSE for details and terms of copying.

*/


#include <stdlib.h>			/* EXIT_FAILURE of exit() routine */

#ifndef defs_h
#define defs_h




/* uncomment line if using stacks/queues to hold graph nodes */
/* #define GRAPH_OPERATIONS */

/* uncomment line for built-in error diagnostics */
#define DEBUG



/* public defined here: used in automatic generation of header files */
#define public

/* STILL TO STRUCTURE AND CORRECT THE CONTENT OF THIS FILE !!! */

/* 
// #define public
// #define private static
*/
#define and &&
#define or ||
#define not !

#define FALSE 0
#define TRUE 1


/* maximum buffer size (for i/o of chars) */
#define BMAX 256


/*
 * Definitions for doing memory allocation.
 */

#define alloc(n, type)	((type *) malloc((unsigned) (n) * sizeof(type)))
#define dispose(p)	{ free((void *) (p)); (p) = NULL; }/* reset p to NULL*/



#ifdef DEBUG

	/* General format is msg_xxx(A,B) where:
	 *	A is procedure/block/macro/... name where error occured.
	 *		Argument A is used only when debugging.
	 *	B is any useful error message
	 */
/* use this version when debugging */
#define msg_w(A1,B1)	fprintf(stderr,"\n*** WARNING ***: file \"%s\": in function %s() at line %d:- %s\n",__FILE__,A1,__LINE__,B1)

#define msg_e(A1,B1)	fprintf(stderr,"\n*** ERROR ***: file \"%s\": in function %s() at line %d:- %s\nProgram may fail later!\n",__FILE__,A1,__LINE__,B1)

#define msg_f(A1,B1)	(fprintf(stderr,"\n*** FATAL ERROR ***: file \"%s\": in function %s() at line %d:- %s\nExiting program!\n",__FILE__,A1,__LINE__,B1),exit(EXIT_FAILURE))

#else

#define msg_w(A,B)	fprintf(stderr,"*** Warning: %s\n",B)
#define msg_e(A,B)	fprintf(stderr,"*** Error ***: %s\nProgram may fail later!\n",B)
#define msg_f(A,B)	(fprintf(stderr,"*** FATAL ERROR ***: %s\nExiting program!",B),exit(EXIT_FAILURE))

#endif /* of DEBUG */




#endif /* of defs_h */
