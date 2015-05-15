/*************************************************************************

                              expert.c

Authors: Yoav Freund     (yoav@research.att.com)
         Robert Schapire (schapire@research.att.com)

Please do not distribute this file without the permission of the
authors.  Please contact the authors about bugs, suggestions for
improvement, etc.


This file implements the expert prediction strategy outlined in the paper
"How to use expert advice" by Cesa-Bianchi, Freund, Haussler, Helmbold,
Schapire and Warmuth.

Each expert is a procedure that can be viewed as an object.  No experts
are included in this file, but the list of all experts to be used is
given by the macro EXPERT_LIST (defined at compilation time in the
makefile.  Each of these experts is in fact a family of experts that
may be parametrized by an integer between 0 and some maximal value
(that can be obtained by querying the expert, as described below).

Each expert takes as input a command c and a parameter p (both integers)
and returns a double-precision value.  The possible values of c are:

NUMBER: this returns the number of experts that are parameterized by
this expert.  The parameter p is ignored.

INITIALIZATION: this initializes the expert (i.e., its entire family of
experts).  The parameter p is ignored.

PREDICTION: this returns the next prediction of the expert parameterized
by the given value of p.

UPDATE: this updates the state of the expert.  In this case, the 2 least
significant bits of p encode the last bit output by the program,
and the user's last input bit.  Specifically, p is set to the value
2*m+y where y is the user's last bit and m is the last prediction made
by this program.

SEND_ARCHIVE_ID: this instructs the expert to send an identification
string to the archive file using the procedure "archive_expert_id".

(See const_exp.c for a very simple example.)

*************************************************************************/

#include "expert.h"
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/time.h> 
#include <sys/timeb.h>
/* #include <time.h> */
#include <pwd.h>
#include <string.h>
#include <stdlib.h>

#define  POW2(N)     (1 << (N))           /* 2^N */
#define  BIT(X, I)   (((X) >> (I)) & 1)   /* Ith l.s. bit of X */
#define  ALLOC(T, N)  ((T *) (malloc((N) * sizeof(T))))
                         /* allocates N blocks of type T */
#define  NUM_TRIALS_TO_WIN   (99)

#define MY_SYMB     'x'
#define YOUR_SYMB   '-'
#define BOTH_SYMB   '*'
#define EMPTY_SYMB  ' '
#define BORDER_SYMB '|'

char *archive_file_name;
FILE *archive_file, *tty;

typedef double EXPERT();         /* define the type of an expert */
typedef double (*EXPERT_PTR)();  /* the type of a pointer to an expert */
extern EXPERT EXPERT_LIST;

EXPERT_PTR expert[] = { EXPERT_LIST };  /* array of expert families */

#define NUM_EXPERT_TYPES  (sizeof(expert)/sizeof(EXPERT_PTR))

int trial, num_trials, num_wins, num_loss;
int num_exp[NUM_EXPERT_TYPES];
double loss;
double *weight[NUM_EXPERT_TYPES];
double *gweight[NUM_EXPERT_TYPES];
double *pred[NUM_EXPERT_TYPES];
double *exp_loss[NUM_EXPERT_TYPES];
double beta, gamm;
int num_experts;

int random_seed;
int even_play_mode;

#define ENCRYPT_SIZE 200
int encryption_ON=0;
int e_tbl[ENCRYPT_SIZE];

int fixed_len_mode;

/***************
**
**  probability of >= r heads in n flips of a fair coin
**
***************/
double
binomial_tail(n,r)
int n,r;
{
  double s,t;
  double i;
  double nf = n;
  double rf = r;

  t = -nf*log(2.);
  s = exp(t);
  for (i=1.;i <= rf; i++) {
    t += log((nf-i+1.)/i);
    s += exp(t);
  }
  return s;
}

/**********
**
**  random bit that is 1 with probability p
**
**********/
int
random_bit(p)
double p;
{
  return (random() < ((unsigned int) (1 << 31)) * p);
}

/************
**
** functions F, U are taken directly from "experts" paper.
** function ginv(x) is equal to g(1/x) given in "experts" paper.
** compute_beta computes the values of beta, gamm and cbeta to be used.
************/
static double cbeta;

double
ginv(x)
double x;
{
  return 1. + 2.*x - 2.*sqrt(x*x + x);
}

void
compute_beta()
{
  double old_beta;

  if (fixed_len_mode) {
    beta = ginv(2. * log((double) num_experts)/num_trials);
    gamm = 1.0;
  } else if (num_experts == 1) {
    beta = gamm = 1.0;
  } else {
    old_beta = (trial == 1 ? 0. : beta);
    beta = ginv(log((double) num_experts)/trial);
    gamm = log(2./(1.+beta))/log(2./(1.+old_beta));
  }
  cbeta = (beta == 1.0 ? 0.5 : (1.+beta)*log(2./(1.+beta)) / (2.*(1.-beta)));
}    

double
F(r)
double r;
{
  return (r <= 0.5 - cbeta ? 0. :
	 (r >= 0.5 + cbeta ? 1. :
	  0.5 * (1. - (1. - 2.*r)/(2.*cbeta))));
}

double
U(q)
double q;
{
  return 1-(1-beta)*q;
}

/***********
**
**  initializes all of the experts, and sets up weight and pred arrays
**
***********/
void
initialize_experts()
{
  int i,j;

  num_experts = 0;
  for (i = 0; i < NUM_EXPERT_TYPES; i++)
    {
      num_experts += num_exp[i] = (*expert[i])(NUMBER, 0);
      (*expert[i])(INITIALIZE, 0);
      weight[i] = ALLOC(double, num_exp[i]);
      gweight[i] = ALLOC(double, num_exp[i]);
      pred[i] = ALLOC(double, num_exp[i]);
      exp_loss[i] = ALLOC(double, num_exp[i]);
      for(j = 0; j < num_exp[i]; j++) {
	exp_loss[i][j] = 0.0;
	weight[i][j] = 1.0;
      }
    }
  if (fixed_len_mode)
    compute_beta();
}

/*********
**
** gets next prediction of each expert and combines the predictions
** as described in "experts" paper
**
*********/
double
get_experts_pred()
{
  double sw = 0.0;
  double pw = 0.0;
  int i,j;
  double r;

  for (i = 0; i < NUM_EXPERT_TYPES; i++)
    for (j = 0; j < num_exp[i]; j++) {
      sw += gweight[i][j] = pow(weight[i][j], gamm);
      pred[i][j] = (*expert[i])(PREDICTION, j);
      pw += gweight[i][j] * pred[i][j];
    }
  r = pw/sw;
  return F(r);
}

/*********
**
** updates weights on experts as described in "experts" paper, and
** passes each expert family an update flag.
**
*********/
void
update_experts(p,y)
int p,y;
{
  int i, j;
  double q;

  for (i = 0; i < NUM_EXPERT_TYPES; i++) {
    (*expert[i])(UPDATE, 2*p+y);
    for (j = 0; j < num_exp[i]; j++) {
      exp_loss[i][j] += q = fabs(pred[i][j] - y);
      weight[i][j] = gweight[i][j] * U(q);
    }
  }
}

/*********
**
** converts string s to integer *i.  Returns 0 iff s does not
** represent a valid integer.
**
*********/
strtoint(s, i)
char *s;
int *i;
{
  char *p;

  *i = strtol(s, &p, 10);
  return (*p == '\0');
}

/*********
**
** generate a random encryption table and print it
**
*********/
void init_encrypt()
{
  int i,j;
  for(i=0; i < ENCRYPT_SIZE; i++) e_tbl[i] = random_bit(0.5);
  
  printf("The encryption table\n");
  printf("                1/6        2/7        3/8        4/9       \n");
  printf("     0123456789 0123456789 0123456789 0123456789 0123456789\n");
  for(i=0; i< ENCRYPT_SIZE; i += 50) {
    printf("%3d:",i);
    for(j=0; j<50; j++) printf((j % 10 == 0 ? " %1d" : "%1d"),e_tbl[i+j]);
    printf("\n");
  }
  printf(" The computer's guess will appear as a number in braces,\n");
  printf(" which refers to an entry in the table above. \n\n");
}

/*********
**
** find a random encryption for the bit b
**
*********/
int encrypt(int b)
{
  int i;
  while(b != e_tbl[ i = random() % ENCRYPT_SIZE]);
  return(i);
}

/*********
**
** returns pointer to string containing user's id.
**
*********/
char *
getulogin()
{
  char *getlogin();
  char *ln;
  struct passwd *p;
  static char *loginname = NULL;

  if (!loginname) {
    if (ln = getlogin())
      loginname = strdup(ln);
    else if (p = getpwuid(getuid()))
      loginname = strdup(p->pw_name);
    else {
      loginname = "UNKNOWN";
      fprintf(stderr, "Warning: unable to find login name.\r\n");
    }
  }
  return loginname;
}


/*********
**
** initializes archive file.
**
*********/
void
initialize_archive()
{
  int i;
  time_t t = time(NULL);
  char line[128];

/*
  if (!(archive_file = fopen(archive_file_name, "a"))) {
    fprintf(stderr, "Warning: failed to open archive file.\r\n");
    return;
  }
*/
  archive_file = NULL;  /* turn off archiving */
  return;

  sprintf(line, "chmod a+r %s", archive_file_name);
  system(line);
  fprintf(archive_file, "\n\nNEWGAME:\n");    /* new game marker */
  fprintf(archive_file, "User: %s\n", getulogin());
  if (gethostname(line, sizeof(line)))
    strcpy(line, "(unknown)");
  fprintf(archive_file, "Host: %s\n", line);
  fprintf(archive_file, "Date: %s", ctime(&t));   /* date */
  fprintf(archive_file, "Encryption: %d\n", encryption_ON);

  if (fixed_len_mode)
    fprintf(archive_file, "NumTrials: %d\n", num_trials);
  else
    fprintf(archive_file, "NumTrials: (unlimited)\n");
  fprintf(archive_file, "RandomSeed: %d\n", random_seed);
  fprintf(archive_file, "NumExpertTypes: %d\n", NUM_EXPERT_TYPES);
  for (i = 0; i < NUM_EXPERT_TYPES; i++) {
    (*expert[i])(SEND_ARCHIVE_ID, 0);
    fprintf(archive_file, "  NumExperts: %d\n", num_exp[i]);
  }
  fprintf(archive_file, "History: \n");
}

/*********
** archives the id of an expert
*********/
void
archive_expert_id(id)
char *id;
{
  if (archive_file)
    fprintf(archive_file, "  ExpertName: %s\n", id);
}

/*********
** sends single trial to archive file
*********/
void
archive_trial(m,y)
int m,y;
{
  if (archive_file)
    fprintf(archive_file, "%d %d\n", m, y);
}

#define MIN_FINAL_WEIGHT  (0.01)

/*********
** records normalized final weights of experts in archive file for
** all experts with normalized weight >= MIN_FINAL_WEIGHT, and also
** for the expert with the highest weight in each family.
*********/
void
archive_weights()
{
  int i,j;
  double s, t;
  int max[NUM_EXPERT_TYPES];

  if (!archive_file)
    return;

  fprintf(archive_file, "AvgLoss: %lf\n",
	  ((double) (num_trials-num_wins))/num_trials);
  fprintf(archive_file, "AvgDeterLoss: %lf\n", loss/num_trials);
  fprintf(archive_file, "Weights:\n");
  s = 0.;
  for (i = 0; i < NUM_EXPERT_TYPES; i++) {
    max[i] = 0;
    for (j = 0; j < num_exp[i]; j++) {
      s += weight[i][j];
      if (weight[i][j] > weight[i][max[i]])
	max[i] = j;
    }
  }
  for (i = 0; i < NUM_EXPERT_TYPES; i++) {
    (*expert[i])(SEND_ARCHIVE_ID, 0);
    for (j = 0; j < num_exp[i]; j++)
      if ((t=weight[i][j]/s) >= MIN_FINAL_WEIGHT || j == max[i])
	fprintf(archive_file, "%d %lf %lf\n", j, t, exp_loss[i][j]/num_trials);
  }
}

/*********
**
** sets all of the parameters by reading in options from command line
**
*********/
set_parameters(argc, argv)
int argc;
char *argv[];
{
  int i, error;
  struct timeval tp;
  struct timezone tz;

  num_trials = 2*NUM_TRIALS_TO_WIN-1;             /* set default values */
  gettimeofday(&tp,&tz);
  random_seed = (int) tp.tv_sec;
  encryption_ON = 0;
  archive_file_name = ALLOC(char, 128);
  sprintf(archive_file_name, "%s/%s", ARCHIVE_DIR, getulogin());

  for (i = 1, error = 0; i < argc && !error; i++) {
    if (strcmp(argv[i], "-r") == 0)  /* set random seed */
      error = (++i >= argc || !strtoint(argv[i], &random_seed));
    else if (strcmp(argv[i], "-e") == 0) {
      encryption_ON=1;
    }
    else if (strcmp(argv[i], "-a") == 0) {  /* set archive file name */
      if (++i >= argc)
	error = 1;
      else
	archive_file_name = argv[i];
    }
    else if (strcmp(argv[i], "-h") == 0) {
      fprintf(tty, "Options are:\n");
      fprintf(tty, "  -a  <file> : send archive information to <file>\n");
      fprintf(tty, "  -e         : print an encryption of the computer's guess\n");
      fprintf(tty, "  -r  <n>    : set random seed to <n>\n");
      exit(-1);
    } 
    else
      error = 1;
  }
  if (error) {
    fprintf(tty, "Illegal option chosen.  Use '-h' option for help.\n");
    exit(-1);
  }
  srandom(random_seed);
  fixed_len_mode = (num_trials > 0);
}

/**********
**
**  print introductory comments.
**
**********/
print_intro_comments(argc, argv)
int argc;
char *argv[];
{
  char line[128];

  fprintf(tty, "\nWelcome to the MIND-READING GAME!\n\nHere's how to play:\n");
  fprintf(tty, "In each round, we'll each choose a bit (0 or 1).\n");
  fprintf(tty, "If the bits match, the EVEN player wins.\n");
  fprintf(tty, "If the bits don't match, the ODD player wins.\n");
  fprintf(tty, "After each round, I'll show how many rounds we've each won.\n");
  fprintf(tty, "My score is represented by '%c' and yours by '%c'.\n", MY_SYMB, YOUR_SYMB);
  fprintf(tty, "We'll play until one of us wins %d rounds (two laps).\n",NUM_TRIALS_TO_WIN);
  even_play_mode = -1;
  while (even_play_mode == -1) {
    fprintf(tty, "\nDo you want to play E(VEN) or O(DD)? ");
    scanf("%s", line);
    if (*line == 'e' || *line == 'E')
      even_play_mode = 1;
    else if (*line == 'o' || *line == 'O')
      even_play_mode = 0;
  }
  fprintf(tty, "\nOkay, let's play!\n\n");
}


/*********
**
**  prints final comments for the user.
**
*********/
print_final_comments()
{
  fprintf(tty, "\n\n");
  if (num_wins > num_loss)
    fprintf(tty, "                            I WON!\n\n");
  else
    fprintf(tty, "                          YOU WON!!!\n\n");
}


#define SCOREWIDTH 25
#define SCOREDIGITS 3

/*********
**
** outputs one "dot" or square of the race track
**
*********/
void
outputdot(xb, yb)
int xb, yb;
{
  fprintf(tty, "%c", (xb && yb ? BOTH_SYMB :
		      xb ?       MY_SYMB :
		      yb ?       YOUR_SYMB :
		      EMPTY_SYMB));
}

#define TEST1(X,I)    ((I) >= (X) && (I) <= (X) + 1)
#define TESTDOT(X,I)  (TEST1((X),(I)) || TEST1((X)-(NUM_TRIALS_TO_WIN+1)/2,(I)))

/*********
**
** outputs the current state of the race track
**
*********/
void
outputrace()
{
  int i;

  fprintf(tty, "  |");
  for (i = 0; i < (NUM_TRIALS_TO_WIN+1)/2; i++)
    outputdot(TESTDOT(num_wins, i), TESTDOT(num_loss,i));
  fprintf(tty, "%c   %3d  %3d", (num_wins == NUM_TRIALS_TO_WIN ? MY_SYMB :
				 num_loss == NUM_TRIALS_TO_WIN ? YOUR_SYMB :
				 BORDER_SYMB),
	  num_loss, num_wins);
}


/*********
**
** implements interface for game.  Gets one bit at a time from user
** and from team of experts.
**
*********/
main(argc, argv)
int argc;
char *argv[];
{
  int your_guess, my_guess;
  int input_from_tty;
  int r, j, t, i;
  double p;

  tty = stdout;
  set_parameters(argc, argv);

  print_intro_comments(argc, argv);

  input_from_tty = !(system("tty -s") >> 8);
  if (input_from_tty) system("stty raw -echo");

  initialize_experts();
  num_wins = 0;
  num_loss = 0;
  fprintf(tty, "          ");
  outputrace();
  loss = 0.0;
  initialize_archive();
  for(trial=1; num_wins < NUM_TRIALS_TO_WIN && num_loss < NUM_TRIALS_TO_WIN; trial++) {
    if (!fixed_len_mode)
      compute_beta();
    p = get_experts_pred();
    my_guess = random_bit(p);
    if(encryption_ON) 
      printf("\r\n%3d {%3d}?",trial,encrypt(my_guess));
    else
      fprintf(tty, "\r\n%3d? ",trial);
    do {
      your_guess = getchar();
      if (your_guess == 3 || your_guess == 4 || your_guess == EOF) {
	if (input_from_tty) system("stty -raw echo");
	if (fixed_len_mode)
	  fprintf(tty, "\nGame ended prematurely.\n");
	num_trials = trial - 1;
	if (archive_file)
	  fprintf(archive_file, "InterruptedAt: %d\n", num_trials);
	archive_weights();
	exit(-1);
      }
      your_guess = your_guess - '0';
    } while (your_guess != 0 && your_guess != 1);
    fprintf(tty, "%d : %d",your_guess,my_guess);
    if (even_play_mode)
      your_guess = 1 - your_guess;
    archive_trial(my_guess, your_guess);
    if (my_guess == your_guess)
      num_wins++;
    else
      num_loss++;
    loss += fabs(p - your_guess);
    update_experts(my_guess,your_guess);
    t = trial - 2*num_wins;
    outputrace();
  }
  if (input_from_tty) system("stty -raw echo");
  print_final_comments();
  archive_weights();
}
