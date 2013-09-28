#ifndef MINT_OPTYPE_H
#define MINT_OPTYPE_H

struct mint_op {
  char *name;   /* op name: a unique identifier */
  int type;     /* see enum in op.h */
  void *op;     /* the op function */
  int nparam;   /* # op parameters */
  float *param; /* parameter values */
};

#endif
