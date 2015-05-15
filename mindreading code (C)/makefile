###################################################################
#
# makefile for 'match' game.
# Author: Robert Schapire (schapire@research.att.com)
#
# To use, the following macros must be set:
#    EXPERT_LIST:   a list of the *procedure names* of the experts to
#	be used, separated by commas
#    EXPERT_FILES_o: a list of the *files* that contain the code needed
#	to run the experts.  Each file should end with a .o suffix.
#	Also, if any files are needed which contain auxiliary
#	routines, these should also be included on this list.
#    ARCHIVE_DIR:  the name of the directory in which to write archive
#	files of every game played (by default, the user's login name)
#
# For example, these macros might be set by the following lines:
#	EXPERT_LIST = expert_proc1, expert_proc2, expert_proc3
#	EXPERT_FILES_o = expert_file1.o expert_file2.o expert_file3.o
#	ARCHIVE_DIR = $(PWD)/archive
####################################################################

EXPERT_LIST = var_tree_exp_sym_py, var_tree_exp_sym_y
EXPERT_FILES_o = var_tree_exp_sym_py.o var_tree_exp_sym_y.o dutch_trees.o

#EXPERT_LIST = last_y_exp
#EXPERT_FILES_o = last_y_exp.o

#EXPERT_LIST = hagelbarger_exp
#EXPERT_FILES_o = hagelbarger.o

ARCHIVE_DIR = $(PWD)/archive

CC = gcc
CFLAGS = -O
COMPILE.c = $(CC) $(CFLAGS) -c

race:	$(EXPERT_FILES_o) expert-race.o
	$(CC) $(CFLAGS) -o race $(EXPERT_FILES_o) expert-race.o -lm
	chmod a+rx race

match: $(EXPERT_FILES_o) expert3.o
	$(CC) $(CFLAGS) -o match $(EXPERT_FILES_o) expert3.o -lm
	chmod a+rx match

expert-race.o: expert-race.c expert.h makefile
	$(COMPILE.c) expert-race.c -DEXPERT_LIST="$(EXPERT_LIST)" \
			   -DARCHIVE_DIR="\"$(ARCHIVE_DIR)\""
	-mkdir $(ARCHIVE_DIR)
	chmod a+rwx $(ARCHIVE_DIR)

expert3.o: expert3.c expert.h makefile
	$(COMPILE.c) expert3.c -DEXPERT_LIST="$(EXPERT_LIST)" \
			   -DARCHIVE_DIR="\"$(ARCHIVE_DIR)\""
	-mkdir $(ARCHIVE_DIR)
	chmod a+rwx $(ARCHIVE_DIR)

$(EXPERT_FILES_o) hagelbarger.o: expert.h dutch_trees.h makefile

tar:
	tar -cf /tmp/exp.tar const_exp.c dutch_trees.c dutch_trees.h \
			expert-race.c expert.h expert3.c hagelbarger.c \
			last_y_exp.c last_yp_exp.c makefile \
			var_tree_exp_sym_py.c var_tree_exp_sym_y.c \
			var_tree_exp_y.c var_tree_exp_yp.c
	compress -c /tmp/exp.tar | uuencode exp.tar.Z > exp.uu
