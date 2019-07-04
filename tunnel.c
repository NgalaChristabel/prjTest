/* !!! TO EDIT !!! */
 /************************************************************************
  *PROJECT TITLE: EXECUTION TUNNELLING
                                                      *
  * PURPOSE OF THE SOFTWARE: Tunnelling down an N-layer reference model to assert properties or apply operations vetted for the chosen layer(s).
  *- The program uses N (>=3) service procedures to transition between layers. The service [transition] procedures svcXY_i (i=1..N) pass arguments, X and Y for to use at the targeted layers, a set of layer-dependent context information and corresponding predicates that are used to make assertions/operations and/or to make assertions for different layers.
  * - layers are insulated from each other, and so context information (beyond some standard defaults) are explicitly passed and vetted. Context information is passed down (or up) through each service layer, possibly modified, until the desired service layer is reached, where service operations are then executed.
  * - the transition between layers for a given problem is guided by a [mathematical] lattice, called the "vertical lattice guide" (VLG) or "Service Lattice".  (It is possible, but discouraged, for transitions to be guided between two VLG nodes within a given layer.)  More technically, the lattice must have a top element (if tunnelling up) and a bottom element if tunnelling down is accepted. That is, the lattice must be a complete semi-lattice in the direction of acceptable tunnelling. It is a complete lattice if transitions are both up and down. This program currently focuses on providing structures for the VLG.
  * - the transition between nodes within a layer---possibly including, or excluding, nodes of the vertical lattice guide---is guided by a separate [mathematical] lattice, called the "horizontal lattice guide" (HLG) or "Constraint lattice". HLG is like VLG, except that all nodes are strictly within a given layer. Technically, HLG transitions across silos (i.e., partitions) within a layer.
  * - Conceptually, layers provide services and Service lattices enable service provision across layers. Silos contain [the effects] of operations, and Constraint lattices help restrict access. (For more conceptual/theoretical details, see the file: ../u2010/acad/ideas/ideas.tunnelling-latticeProcessStructure-26082015.txt)
  * - In its simplest form, our lattice is *currently* a linear list, with a bottom element (layer 0). We assume N = 3; and the top element, if eventually needed, would be N+1 (= 4).
  *************************************************************************/

#include <stdio.h>	/* for standard I/O functions */
#include <stdlib.h>	/* for "system" system call */
#include <time.h>	/* for current time/date (in random no. generation) */

#include "defs.h"	/* general defines, eg. msg_w, BMAX */

/* number of services in each layer; M >=2 */
#define M 4

/* number of service layers; for now, assume N=7 */
#define N 7

/* minimum and maximum contexts and services */
#define MIN_CTX 1
#define MAX_CTX (N)
#define MIN_SVC 1
#define MAX_SVC (M)

/* minimum and maximum predicates: For now, correspond to services. */
#define MIN_PDC (MIN_SVC)
#define MAX_PDC (MAX_SVC)

/*
  For context and services embedded into a lattice structure, define their top and bottom elements.
 */
#define BOT_CTX ((MIN_CTX)+ 1)
#define TOP_CTX ((MAX_CTX)+ 1)
#define BOT_SVC ((MIN_SVC)+ 1)
#define TOP_SVC ((MAX_SVC)+ 1)



typedef enum latticedir {
	ERR_LAT = 0,	/* Error: signals an error has occurred */
	NEU_LAT = 1,	/* Neutral: no effect; stay at current node */
	UP_LAT = 2,	/* Up: next node up lattice, towards TOP */
	DOWN_LAT = 3,	/* Down: next node down lattice, towards BOT */
	IND_LAT = 4,	/* Independent: adjacent but not UP or DOWN */
	LUB_LAT = 5,	/* LUB: next node is lub of nodes considered */
	GLB_LAT = 6,	/* GLB: next node is glb of nodes considered */
	SET_LAT = 7	/* Set: consider a set of next nodes */
} Latticedir;



/* */
typedef enum layers {Layer0, Layer1, Layer2, Layer3, Layer4, Layer5, Layer6, Layer7}
    Layers;
/*
typedef enum layers Layers;
 */

/* context information */
typedef int Context;	/* structure holds context information; int for now */
typedef int Context_idx;	/* index to Context in Contextset */

typedef struct contextset {
  int cur;		/* index, i, to current context for layer i = 1.. N) */
  Context ctx[N+1];	/* array of contexts, for layers 1..N, plus layer 0 */
} Contextset;




/* typedefs of Predicates (pointers to functions) to assert properties at each level of interest and transmit possible modified context.
 */
typedef Latticedir (*PF0)(int, int);
typedef Latticedir (*PF1)(int, int, Contextset *c);
typedef Latticedir (*PF2)(int, int, Contextset *c, PF1);
typedef Latticedir (*PF3)(int, int, Contextset *c, PF1, PF2);

/* To use later, if aggregating functions of varying parameters (c.f., stdarg library), instead of defining pXY as above:
 */
/*
typedef int (*Predicate)();
*/	/* function that returns true or false.
				   NOTE: C may not let you use a typedef here!
				*/

/*
   function to assert constraints/properties at the different layers.
   values returned should be -1, 0, 1, -2 for lattice transition down, at or up, or error.
 */


typedef struct predicatestruct {
  Layers cur;			/* tag for current layer */
  PF0 pdc0;			/* at layer 0 (bottom) */
  PF1 pdc1;			/* at layer 1 */
  PF2 pdc2;			/* at layer 2 */
  PF3 pdc3;			/* at layer 3 */
  PF3 pdc4;			/* at layer 4 (top) */
} Predicatestruct;

/*
typedef struct predicateset {
    int cur;
*/		/* index to (i.e., layer for) current predicate */
/*    Predicate pdc[N];
 */	/* array [1 to N] of Predicates; one per layer */
/*  } Predicateset;
*/


/* &&&&&&&&&&&&&&&&&&&&[ START: function prototypes ]&&&&&&&&&&&&&&&& */

/* function prototypes to allow for mutual recursion, etc. */
int svcXY_1(int x, int y, Contextset c, Predicatestruct pdc);
int svcXY_2(int x, int y, Contextset c, Predicatestruct pdc);
int svcXY_3(int x, int y, Contextset c, Predicatestruct pdc);

/* &&&&&&&&&&&&&&&&&&&&[ END: function prototypes ]&&&&&&&&&&&&&&&& */



/* &&&&&&&&&&&&&[ END: static variables (local to file) ]&&&&&&&&&&& */

	/*
	  BMAX is from  declarations in defs.h.
	  Reserve twice as much space, since we use sprintf on buf but do not yet know how to limit the space used by sprintf. The assumption is that an error (e.g., runaway recursion), not wilful programming, would consume more than twice the allocated space.
	*/

static  char buf[2*BMAX+1];	/* !!! CHANGE NAME AND MAKE GLOBAL !!! */

/* &&&&&&&&&&&&&[ END: static variables (local to file) ]&&&&&&&&&&& */


/* &&&&&&&&&&&&&&&&&&&&[ START: Accessory functions ]&&&&&&&&&&&&&&&& */

static int randnum(int a, int b)
     /* generate random number in range [a,b] inclusive; a < b */
     /* rand() generates an integer in range 0 to RAND_MAX. we generate a random number in the (closed) interval [a,b] by getting a random value in the range corresponding to abs(a-b), then adding an offset, a, so the value is in the interval.
      */
{
  int r = 0;

  if (a >= b) {				/* check if value is valid */	
    printf("\n***WARNING: invalid range []:");
    printf(" %1d >= %1d. Returning invalid random number= %1d.", a, b, (a - (a-b)));
    return (a - (a-b));			/* return out-of-range value */
  }

  /* for changing results, seed random number generator with
     current time! Otherwise, use a constant value, such as 1. */

  /*
  srand((unsigned int)time('\0'));
  */

  r = rand();			/* generate a random number */

  /* modulo arithmetic; +1 for no. of ints in range */
  r = r - (r/(b-a+1))*(b-a+1);
  r += a;			/* add offset */

  /*
    r = (rand()*1.0) / RAND_MAX;
  */
	/* fractional value */

  return r;
}

/* &&&&&&&&&&&&&&&&&&&&[ END: Accessory functions ]&&&&&&&&&&&&&&&& */



/* &&&&&&&&&&&&[ START: string names of enumerated data values]&&&&&&&&&& */

/* return a character string, the string name of lattice direction type. */
char *sname_layers(Layers l)
{
  char *s = '\0';
  /*  
  char buf[7] = "\0\0\0\0\0\0\0";
  */
		/* use a non-local variable! */
  
  /* FOR NOW: just check set of valid contexts; manually change layers change*/
  /*
  if ((Layer0 <= l) && (l <= Layer3)) {
    sprintf(buf, "Layer%1d", (int)l);
    return (buf);
  } else {
    printf("\n*** WARNING: Unknown service layer value (%1d); null string returned", l);
    s = '\0';
  }
  */

  /* give string names: should normally test for */
  switch (l) {
  case Layer0: s = "Layer0";
    break;
  case Layer1: s = "Layer1";
    break;
  case Layer2: s = "Layer2";
    break;
  case Layer3: s = "Layer3";
    break;
  default:
    printf("\n*** WARNING: Unknown service layer value (%1d); null string returned", l);
    s = '\0';
    break;
  }

  return s;
}


/* return a character string, the string name of lattice direction type. */
char *sname_latticedir(Latticedir ld) {
  char *s = '\0';

    switch (ld) {
  case ERR_LAT: s = "ERR_LAT";	/* Error: signals an error has occurred */
      break;
  case NEU_LAT: s = "NEU_LAT";	/* Neutral: no effect; stay at current node */
    break;
  case UP_LAT: s = "UP_LAT";	/* Up: next node up lattice, towards TOP */
    break;
  case DOWN_LAT: s = "DOWN_LAT"; /* Down: next node down lattice, towards BOT */
    break;
  case IND_LAT: s = "IND_LAT";	/* Independent: adjacent but not UP or DOWN */
    break;
  case LUB_LAT: s = "LUB_LAT";	/* LUB: next node is lub of nodes considered */
    break;
  case GLB_LAT: s = "GLB_LAT";	/* GLB: next node is glb of nodes considered */
    break;
  case SET_LAT: s = "SET_LAT";	/* Set: consider a set of next nodes */
    break;
  default:
    printf("\n*** WARNING: Unknown lattice direction value (%1d); null string returned", ld);
    s = '\0';
    break;
  }

  return s;
}

/* &&&&&&&&&&&&&[ END: string names of enumerated data values]&&&&&&&&&&&& */


/* &&&&&&&&&&&&&&&[ START: Lattice interface operations ]&&&&&&&&&&&&&& */

/* 
   Generate random direction on lattice, biased to given (preferred) direction.
   NOTE: *manually* modify code to accepted range of values.
 */
Latticedir lat_randdirection(Latticedir ld)
{
  Latticedir res = ERR_LAT;
 
  /* 
     FOR NOW: consider only NEU_LAT, UP_LAT, DOWN_LAT, ERR_LAT
     Remember to manually modify checks in all sections of code.
  */

  if ((randnum(0,2) == 0) && 	/* bias to preference  1/3 of the time */
      ((ld == NEU_LAT) || (ld == UP_LAT) ||	/* ensure ld is valid */
       (ld == DOWN_LAT) || (ld == IND_LAT) || (ld == ERR_LAT))){
    res = ld;
    return res;
  }

  switch (randnum(0,3)) {
  case 1:
    res = NEU_LAT;
    break;
  case 2:
    res = UP_LAT;
    break;
  case 3:
    res = DOWN_LAT;
    break;
  case 4:
    res = IND_LAT;
    /* FOR NOW: IND_LAT is an error.
    break;
    */
  case 0:
    res = ERR_LAT;
    /* ERR_LAT is same as default action.
    break;
    */
  default:
    /* reduce likelihood of invalid values */
    res = (randnum(0,3) ? lat_randdirection(ld) : ERR_LAT);
    break;
  }

  return res;
}

/* &&&&&&&&&&&&&&&[ END: Lattice interface operations ]&&&&&&&&&&&&&& */



/* &&&&&&&&&&&&&&&&&[ START: Predicates asserted at layers ]&&&&&&&&&&&&&& */

Latticedir pred_0(int x, int y)
{
  /* FOR NOW: do operations at current lattice node */
  Latticedir res = NEU_LAT;

  printf("\n\t\tpred_0: x = %d; y = %d", x, y);
  printf("\n\t\tCurrent direction of Lattice: %s.", sname_latticedir(res));

  return res;
}

Latticedir pred_1(int x, int y, Contextset *c)
{
  /* FOR NOW: do operations at current lattice node */
  /*  Latticedir res = NEU_LAT; */
  /* FOR NOW: get random direction on lattice, with 75% bias towards NEU_LAT */
  /*  Latticedir res = lat_randdirection(randnum(0,3) ? NEU_LAT : UP_LAT); */
  /* FOR NOW: get random direction on lattice */
  Latticedir res = lat_randdirection(NEU_LAT);

  if (c == '\0') {
     printf("\n*** WARNING: pred_1: invalid context parameter (nil); ignored.");
  } else {
     printf("\npred_1: x = %d; y = %d; Context index = %d; context value: %d", x, y, c->cur, c->ctx[c->cur]);
     printf("\n\tCurrent direction of Lattice: %s.", sname_latticedir(res));
  }

  return res;
}

Latticedir pred_2(int x, int y, Contextset *c, PF1 p1)
{
  /* FOR NOW: get random direction on lattice */
  Latticedir res = lat_randdirection(randnum(0,1) ? UP_LAT : DOWN_LAT);

  if (c == '\0') {
     printf("\n*** WARNING: pred_2: invalid context parameter (nil); ignored.");
  } else {
     printf("\npred_2: x = %d; y = %d; Context index = %d; context value: %d", x, y, c->cur, c->ctx[c->cur]);
     printf("\n\tCurrent direction of Lattice: %s.", sname_latticedir(res));
  }

  return res;
}

Latticedir pred_3(int x, int y, Contextset *c, PF1 p1, PF2 p2)
{
  /* FOR NOW: get random direction on lattice */
  Latticedir res = lat_randdirection(DOWN_LAT);

  if (c == '\0') {
     printf("\n*** WARNING: pred_3: invalid context parameter (nil); ignored.");
  } else {
     printf("\npred_3:\t x = %d; y = %d;\n\tContext index = %d; context value: %d", x, y, c->cur, c->ctx[c->cur]);
     printf("\n\tCurrent direction of Lattice: %s.", sname_latticedir(res));
  }

  return res;
}


/* &&&&&&&&&&&&&&[ START: predicate creating functions ]&&&&&&&&&&&&&& */

PF0 predmake0(Layers l)   /* create new predicate function for level 0 */
{
    if (l != Layer0) {
      printf("\n*** WARNING: Incorrect layer: %s ... Creating default layer %s predicate instead.", sname_layers(l), sname_layers(Layer0));
    }
    return (pred_0);        /* FOR NOW: return pointer to pred_0, for layer 0 predicates */
}


PF1 predmake1(Layers l)   /* create new predicate function for level 1 */
{
    if (l != Layer1) {
      printf("\n*** WARNING: Incorrect layer: %1d ... Creating default layer %1d predicate instead.", sname_layers(l), sname_layers(Layer1));
    }
    
    return (pred_1);        /* FOR NOW: return pointer to pred_1, for layer 1 predicates */
}


PF2 predmake2(Layers l)   /* create new predicate function for level 2 */
{
    if (l != Layer2) {
      printf("\n*** WARNING: Incorrect layer: %s ... Creating default layer %s predicate instead.", sname_layers(l), sname_layers(Layer2));
    }
    return (pred_2);        /* FOR NOW: return pointer to pred_2, for layer 2 predicates */
}


PF3 predmake3(Layers l)   /* create new predicate function for level 3 */
{
    if (l != Layer3) {
      printf("\n*** WARNING: Incorrect layer: %s ... Creating default layer %s predicate instead.", sname_layers(l), sname_layers(Layer3));
    }
    return (pred_3);        /* FOR NOW: return pointer to pred_3, for layer 3 predicates */
}

/* &&&&&&&&&&&&&&[ END: predicate creating functions ]&&&&&&&&&&&&&& */


/* &&&&&&&&&&&&&&[ START: predicate context functions ]&&&&&&&&&&&&&& */


/*
  set predicate to level opt.
  If current predicate level s > than target level t (obtained from opt) then,
  create possibly default predicates in each of the levels s+1 to t inclusive.
  NOTE: manually edit algorithm if no. of predicate values change.
 */
Predicatestruct setpred(Predicatestruct *pdc , int opt)
{
  Predicatestruct p;		/* predicate set to construct */
  int i = 0, lo = 0, hi = 0;

  /* initialise p to default values; (FOR NOW, default predicate is nil) */
  /* guaranteed safe system defaults */
  p.cur = Layer0;		/* default layer == BOT */
  p.pdc0 = predmake0(Layer0);	/* set internal, system default layer */

  /* reset predicates to defaults, one per level: FOR NOW, all are nil. */
  for (i = MIN_PDC; i <= MAX_PDC; ++i) {
    switch (i) {
    case 0:
      p.pdc0 = predmake0(Layer0);	/* set internal, system default layer */
      break;
    case 1:
      p.pdc1 = '\0';
      break;
    case 2:
      p.pdc2 = '\0';
      break;
    case 3:
      p.pdc3 = '\0';
      break;
    case 4:
      p.pdc4 = '\0';
      break;
    default:
      msg_w("setpred","cannot _reset_ predicates beyond level 3");
      break;
    }
  }

  /* consider only valid opt values */

  /* *textually* set maximum value for  valid index values */
  hi = (3 <= MAX_PDC) ? 3 : MAX_PDC;	/* FOR NOW: manually restrict opt
					   to values in range 0-3. 
					*/

  if (MIN_PDC <= opt && opt <= hi) {

    /* copy available preds below or at pdc->cur, but not beyond opt */
    lo = pdc->cur <= opt ? pdc->cur : opt;	/* low point to copy to */
    for (i = MIN_PDC; i <= lo; ++i) {
      switch (i) {
      case 0:
	p.pdc0 = pdc->pdc0;
	break;
      case 1:
	p.pdc1 = pdc->pdc1;
	break;
      case 2:
	p.pdc2 = pdc->pdc2;
	break;
      case 3:
	p.pdc3 = pdc->pdc3;
	break;
      case 4:	/* not used now: initialised but falls through to warning. */
	p.pdc4 = pdc->pdc4;
	/*
	break;
	*/
      default:
	/*
	msg_w("setpred","cannot _copy_ predicates outside levels %1d to %1d");
	*/
	if (BMAX > sprintf(buf, "cannot _copy_ predicates outside levels %1d to %1d", MIN_PDC, lo)) {
	msg_w("setpred", buf);
	} else {
	  buf[BMAX+1] = '\0';		/* truncate buffered string */
	  msg_w("setpred", buf);	/* output truncated message */
	  msg_e("setpred", "Output buffer overflow");	/* exit pgm */
	}
	break;
      }
    }

    /* 
       Create and insert predicates from lo+1 to opt, if need be.
       NOTE: should predmakeN() be more sophisticated, we may 
             intialise p.cur to LayerN current or updated values!
	     Uncomment such initialisations, as need be, in code below.
    */
    for (i = lo+1; i <= opt; ++i) {
      switch (i) {
      case 0:
	/*
	p.cur = Layer0;
	*/
	p.pdc0 = predmake0(Layer0);
	break;
      case 1:
	/*
	p.cur = Layer1;
	*/
	p.pdc1 = predmake1(Layer1);
	break;
      case 2:
	/*
	p.cur = Layer2;
	*/
	p.pdc2 = predmake2(Layer2);
	break;
      case 3:
	/*
	p.cur = Layer3;
	*/
	p.pdc3 = predmake3(Layer3);
	break;
      case 4:	/* not used now: initialised but falls through to warning. */
	/*
	p.pdc4 = predmake4(Layer4);
	*/
	/*
	break;
	*/
      default:
	msg_w("setpred","cannot create predicates beyond level 3");
	break;
      }
    }

      /* make current layer correspond to opt */
    switch (opt) {
    case 0:
      p.cur = Layer0;
      break;
    case 1:
      p.cur = Layer1;
      break;
    case 2:
      p.cur = Layer2;
      break;
    case 3:
      p.cur = Layer3;
      break;
    case 4:	/* not used now: initialised but falls through to warning. */
      /*
      p.cur = Layer4;
      */
      /*
      break;
      */
    default:
      msg_w("setpred","cannot have a current layer predicates beyond level 3");
      break;
    }
  } else {
    printf("\n*** WARNING: setpred: Value %1d to set is invalid! Ignored.", opt);
  }
  
return (p);  /* new copy of possibly modified predicate set */
}

/* relative change of predicates */
Predicatestruct relpred(Predicatestruct *pdc , int opt)
{
  int t =  pdc->cur + opt;	/* add displacement */
  if (MIN_PDC <= t && t <= MAX_PDC) {
    if (pdc-> cur != t) {	/* get new copy only if level changed */
      return (setpred(pdc, t));
    }
  } else {
    printf("\n*** WARNING: relpred: Relative predicate %1d is invalid! Ignored.", opt);
  }

return (*pdc);  /* return unchanged predicate context */
}


/* next predicate down */
Predicatestruct preddown(Predicatestruct *pdc)
{
  return(relpred(pdc, -1));
}

/* next predicate up */
Predicatestruct predup(Predicatestruct *pdc)
{
  return(relpred(pdc, +1));
}




/* &&&&&&&&&&&&&&[ END: predicate context functions ]&&&&&&&&&&&&&& */


/* &&&&&&&&&&&&&&&&&&&&&&[ START: context functions ]&&&&&&&&&&&&&&&&&& */

/* Function ctx is actually a suite of context changing functions, and so should be something like
ctx(c,opt) in the code fragment above. opt corresponds to one of the following options (at least):
ctx(c,opt) =
case opt of
next_down: ctx_next(-1):  get the context for next layer down.  i.e., set cur to cur -1 (if possible).
next_up: ctx_next(1):  get the context for next layer up.  i.e., set cur to cur +1 (if possible).
reset: ctx_reset(k): reset context index to level k.
*/

/* context operations based on certain options */
/*
Contextset contexts;
*/

Contextset setctx(Contextset *c, int opt)
{
  /*  Contextset t;
   */
		/* reset context index to level opt */
  if (MIN_CTX <= opt && opt <= MAX_CTX) {
    c->cur = opt;
  } else {
    printf("\n*** WARNING: setctx: Value %1d to set is invalid! Ignored.", opt);
  }

return (*c);  /* possibly modified context set */
}

/* relative change of context */
Contextset relctx(Contextset *c, int opt)
{
  int t =  c->cur + opt;	/* add displacement */
  if (MIN_CTX <= t && t <= MAX_CTX) {
    c->cur = t;
  } else {
    printf("\n*** WARNING: relctx: Relative context %1d is invalid! Ignored.", opt);
  }

return (*c);  /* possibly modified context */
}


/* next context down */
Contextset ctxdown(Contextset *c)
{
  return(relctx(c, -1));
}

/* next context up */
Contextset ctxup(Contextset *c)
{
  return(relctx(c, +1));
}


/* make a new context for layer l */
Contextset ctxmake(Layers l)
{
  Contextset c;
  static int cnt = 1;       /* TO REMOVE! counter is just to distinguish dummy contexts. */
  cnt = cnt % 1000;         /* modulo 1000, just to avoid integer overflow */

    /* FOR NOW: just initialise context */
  if ((l < Layer0) || (Layer3 < l)) {    /* FOR NOW: highest level is 3 */
    if (BMAX > sprintf(buf, "\t\tLevel %s is invalid; Level %s assumed.", sname_layers(l), sname_layers(Layer0))) {
      buf[BMAX+1] = '\0';		/* truncate buffered string */
      msg_w("ctxmake", buf);		/* output truncated message */
      msg_e("ctxmake", "Output buffer overflow");	/* exit pgm */
    } else {
      msg_w("ctxmake", buf);
    }
    l = Layer0;
  }

  c.cur = l;
  c.ctx[0]= -1 + cnt;			/* dummy value; level 0 not used yet! */
  c.ctx[1]= 10 + cnt;		/* dummy value! */
  c.ctx[2]= 20 + cnt;		/* dummy value! */
  c.ctx[3]= 30 + cnt;		/* dummy value! */

  return(c);
}

/* &&&&&&&&&&&&&&&&&&&&&&[ END: context functions ]&&&&&&&&&&&&&&&&&& */

/* &&&&&&&&&&[ START: service call template for calls WITHIN LAYER ]&&&&&& */

/*
  Services to implement (after vetting given layer 1), with arguments X, Y, and context c, independent of constraints vetted at the various layers.
*/


    /* services to execute at layer 0 */
int opsvcXY_0(int x, int y, Contextset *c)
{
  {
      /* service to execute at this layer */
    printf("\n\t\tLayer %1d services: Default service %s executing ...", 0, "opsvcXY_0");
    printf("\n\t\t\tx = %d; y = %d;\n\t\t\tCurrent context (%1d) and value: %d", x, y, c->cur, c->ctx[c->cur]);
    printf("\n\t\t... Layer %1d services: Default service %s completed.\n", 0, "opsvcXY_0");
  }

  return 0; /* adjust to return something more sensible */
}

    /* services to execute at layer 1 */
int opsvcXY_1(int x, int y, Contextset *c)
{
  {
      /* service to execute at this layer */
    printf("\n\tLayer %1d services: service %s executing ...", 1, "opsvcXY_1");
    printf("\n\t\tx = %d; y = %d;\n\t\tCurrent context (%1d) and value: %d", x, y, c->cur, c->ctx[c->cur]);
    printf("\n\t... Layer %1d services: service %s completed\n", 1, "opsvcXY_1");
  }

  return 0; /* adjust to return something more sensible */
}

    /* services to execute at layer 2 */
int opsvcXY_2(int x, int y, Contextset *c)
{
  {
      /* service to execute at this layer */
    printf("\n\tLayer %1d services: service %s executing ...", 2, "opsvcXY_2");
    printf("\n\t\tx = %d; y = %d;\n\t\tCurrent context (%1d) and value: %d", x, y, c->cur, c->ctx[c->cur]);
    printf("\n\t... Layer %1d services: service %s completed\n", 2, "opsvcXY_2");
  }

  return 0; /* adjust to return something more sensible */
}

    /* services to execute at layer 3 */
int opsvcXY_3(int x, int y, Contextset *c)
{
  {
    printf("\n\tLayer %1d services: service %s executing ...", 3, "opsvcXY_3");
    printf("\n\t\tx = %d; y = %d;\n\t\tCurrent context (%1d) and value: %d", x, y, c->cur, c->ctx[c->cur]);
    printf("\n\t... Layer %1d services: service %s completed\n", 3, "opsvcXY_3");
  }

  return 0; /* adjust to return something more sensible */
}


/* &&&&&&&&&&[ END: service call template for calls WITHIN LAYER ]&&&&&&&& */


/* &&&&&&&&&&&[ START: service templates for calls ACROSS layers ]&&&&&&&&& */



/* Default (bottom level) services */
int svcXY_0(int x, int y)
{
  Latticedir res = ERR_LAT;
  Contextset c = ctxmake(Layer0);

  printf("\n\t\tLevel %1d: Default service calls (%s) START ... \n", 0, "svcXY_0");

  res = pred_0(x, y);
  if (res != ERR_LAT) {
    opsvcXY_0(x, y, &c);
  } else {
    msg_w("svcXY_0", "Error carrying out default operation. Default operation Ignored.");
  }


  printf("\n\t\t... Layer %1d: Default service calls (%s) COMPLETED\n", 0, "svcXY_0");

  return ((res != ERR_LAT)? 0: 1); /* adjust to rtn something more sensible */
}

/*
  Layer 1: p1 asserts layer 1 arguments and then executes svcXY in context c.
 */
/*
int svcXY_1(int x, int y, Contextset c, PF1 p1)
{
  int res = p1(x, y, &c);
*/

int svcXY_1(int x, int y, Contextset c, Predicatestruct pdc)
{
  PF1 p1 = pdc.pdc1;
  Latticedir res = ERR_LAT;

  /* before use, ensure that current predicate is for layer 1 */
  if ((pdc.cur == Layer1) && ( p1 != '\0')) {
    res = p1(x, y, &c);
  } else {
    if (p1 != '\0') {
      printf("\n***WARNING: %s: predicate function not that for Level %s.\n", "svcXY_1", sname_layers(Layer1));
    } else {
      printf("\n***WARNING: %s: (nil) function for Level %s.\n", "svcXY_1", sname_layers(Layer1));
    }

    res = ERR_LAT;
  }

  switch (res) {
  case DOWN_LAT:
    msg_w("svcXY_1","\n\tUnknown level BELOW level 1: Carrying out default action.");
    svcXY_0(x, y);
    break;
  case NEU_LAT:
    opsvcXY_1(x, y, &c);	/* service to execute at this layer */
    break;
  case UP_LAT: {
/*    PF2 p2 = predmake2(Layer2); */	/* create new pred. function for level 2 */
/*    c = ctxmake(Layer2); */	/* create new context for level 2 */
/*    pdc.cur = Layer2; */		/* set current layer to Layer2 */
/*    pdc.pdc2 = p2;*/		/* going to higher level: ensure new
				   predicate is used for level 2
				*/
    /* TODO !!! : check out use of ctxup, given ctxmake above
    svcXY_2(x, y, ctxup(&c), pdc);
    */
	/* service to execute next layer up*/
 
/*   svcXY_2(x, y, c, pdc); */	/* service to execute next layer up*/
   svcXY_2(x, y, ctxup(&c), predup(&pdc));
  }
    break;
  case ERR_LAT:
    /*
    break;
    */
  default:
    printf("\n***WARNING: Level 1: assertion failed. carrying out default action\n");
    break;
  }

  return ((res != ERR_LAT)? 0: 1); /* adjust to rtn something more sensible */
}


/*
  Layer 2: p2 asserts layer 2 arguments; transmit possibly modified context, ctxdown (c)
 */
/* 
int svcXY_2(int x, int y, Contextset c,  PF1 p1, PF2 p2)
{
  int res = p2(x, y, &c, p1);
*/

int svcXY_2(int x, int y, Contextset c, Predicatestruct pdc)
{
  PF2 p2 = pdc.pdc2;
  Latticedir res = ERR_LAT;

  /* before use, ensure that current predicate is for layer 3 */
  if ((pdc.cur == Layer2) && ( p2 != '\0')) {
    res = p2(x, y, &c, pdc.pdc1);
  } else {
    if (p2 != '\0') {
      printf("\n***WARNING: %s: predicate function not that for Level %s.\n", "svcXY_2", sname_layers(Layer2));
    } else {
      printf("\n***WARNING: %s: (nil) function for Level %s.\n", "svcXY_2", sname_layers(Layer2));
    }

    res = ERR_LAT;
  }

  switch (res) {
  case DOWN_LAT:
    /*    pdc.cur = Layer1; */		/* set current layer to Layer1 */
    svcXY_1(x, y, ctxdown(&c), preddown(&pdc));
    break;
  case NEU_LAT:
    opsvcXY_2(x, y, &c);
    break;
  case UP_LAT: {
    /*    PF3 p3 = predmake3(Layer3); */ /* create new pred. function for level 3 */
    /*    c = ctxmake(Layer3); */        /* create new context for level 3 */
    /*    pdc.cur = Layer3; */		/* set current layer to Layer3 */
    /*    pdc.pdc3 = p3; */		/* going to higher level: ensure new
				   predicate is used for level 3 */
    /* TODO !!! : check out use of ctxup, given ctxmake above
    svcXY_3(x, y, ctxup(&c), pdc);
    */
    svcXY_3(x, y, ctxup(&c), predup(&pdc));
  }
    break;
  case ERR_LAT:
    /*
    break;
    */
  default:
    printf("\n***WARNING: Level 2: assertion failed. carrying out default action\n");
    break;
  }

  return ((res != ERR_LAT)? 0: 1); /* adjust to rtn something more sensible */
}


/*
  Layer 3: p3 asserts layer 3 args; transmit possibly modified context, ctx (c)
 */

int svcXY_3(int x, int y, Contextset c, Predicatestruct pdc)
{
  PF3 p3 = pdc.pdc3;
  Latticedir res = ERR_LAT;

  /* before use, ensure that current predicate is for layer 3 */
  if ((pdc.cur == Layer3) && ( p3 != '\0')) {
    res = p3(x, y, &c, pdc.pdc1, pdc.pdc2);
  } else {
    if (p3 != '\0') {
      printf("\n***WARNING: %s: predicate function not that for Level %s.\n", "svcXY_3", sname_layers(Layer3));
    } else {
      printf("\n***WARNING: %s: (nil) function for Level %s.\n", "svcXY_3", sname_layers(Layer3));
    }

    res = ERR_LAT;
  }

  switch (res) {
  case DOWN_LAT:
    /*    pdc.cur = Layer2;
     *//* set current layer to Layer2  */
    svcXY_2(x, y, ctxdown(&c), preddown(&pdc));	/* !!! TODO !!! check use of ctxdown & pdc.cur */
    break;
  case NEU_LAT:
    opsvcXY_3(x, y, &c);
    break;
  case UP_LAT:
    msg_w("svcXY_3","\n\tUnknown level above level 3: Carrying out default action.");
    opsvcXY_0(x, y, &c);	/* !!!CHECK!!! Check call skipping layers! */
    break;
  case ERR_LAT:
    /*
    break;
    */
  default:
    printf("\n***WARNING: Level 3 assertion failed: Carrying out default action\n");
    break;
  }

  return ((res != ERR_LAT)? 0: 1); /* adjust to rtn something more sensible */
 }


/* &&&&&&&&&&&&[ END: service templates for calls ACROSS layers ]&&&&&&&&&& */



/* &&&&&&&&&&&&&&&&&&&&[ START: initialisation routines ]&&&&&&&&&&&&&&&& */


void initialise(int *x, int *y, Contextset *c, Predicatestruct *pdc)
{

  /* seed random number generator with todays date */
  time_t t = time('\0');	/* get current time; var t is for debugging */
  srand((unsigned) t);		/* seed random no with current time */

  /* initialise arguments, x, y */
  *x = 2;				/* dummy value! */
  *y = 3;				/* dummy value! */

  /* initialise context */
  c->cur = 3;			/* FOR NOW: highest level is 3 */
  c->ctx[0]= -1;			/* dummy value; level 0 not used yet! */
  c->ctx[1]= 10;			/* dummy value! */
  c->ctx[2]= 20;			/* dummy value! */
  c->ctx[3]= 30;			/* dummy value! */

  /* initialise predicates  for levels 1-3*/
  pdc->cur = Layer3;			/* initialise to top level */
  pdc->pdc0 = pred_0;
  pdc->pdc1 = pred_1;
  pdc->pdc2 = pred_2;
  pdc->pdc3 = pred_3;


  return;
}

/* &&&&&&&&&&&&&&&&&&&&[ END: initialisation routines ]&&&&&&&&&&&&&&&& */

/* &&&&&&&&&&&&&&&&&&&&[ START: main program ]&&&&&&&&&&&&&&&& */

int main( void )
{
  int res = 0;
  int x = 0, y= 0;		/* arguments passed to service levels */
  Contextset c;
  Predicatestruct pdc;
  /*
  pdc.pdc1 = '\0';   pdc.pdc2 = '\0';  pdc.pdc3 = '\0';
  */

  initialise(&x, &y, &c, &pdc);

  printf("\nCalling from main program. ...");
  res = svcXY_3(x, y, c,  pdc);
  printf("\n ... finished service call.\n\n");

  system("pause");		/* pause output, before final exit */
  return res;
}

/* &&&&&&&&&&&&&&&&&&&&[ END: main program ]&&&&&&&&&&&&&&&& */
