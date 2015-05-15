typedef struct node_rec {
  struct node_rec *son[2];
  double wt[2], end_wt, sum_wt;
} *NODE, NODE_REC;

typedef struct bit_rec {
  int bit;
  struct bit_rec *next;
} *BITSTRING, BIT_REC;

extern void update_wt();
extern double get_sum_wt();
extern double get_pred_wt();
extern NODE init_node();
extern BITSTRING add_bit();
