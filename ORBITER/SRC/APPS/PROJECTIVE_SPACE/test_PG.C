// test_PG.C
// 
// Anton Betten
// 1/17/2010
//
//
// 
//
//

#include "orbiter.h"


// global data:

INT t0; // the system time when the program started

void test1(INT n, INT q, INT verbose_level);

int main(int argc, char **argv)
{
	INT verbose_level = 0;
	INT i;
	INT f_n = FALSE;
	INT n = 0;
	INT f_q = FALSE;
	INT q;
	
 	t0 = os_ticks();
	
	for (i = 1; i < argc - 1; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-n") == 0) {
			f_n = TRUE;
			n = atoi(argv[++i]);
			cout << "-n " << n << endl;
			}
		else if (strcmp(argv[i], "-q") == 0) {
			f_q = TRUE;
			q = atoi(argv[++i]);
			cout << "-q " << q << endl;
			}
		}
	if (!f_n) {
		cout << "please use -n option" << endl;
		exit(1);
		}
	if (!f_q) {
		cout << "please use -q option" << endl;
		exit(1);
		}
	test1(n, q, verbose_level);
	
	the_end(t0);
}


void test1(INT n, INT q, INT verbose_level)
{
	finite_field *F;
	projective_space *P;

	F = new finite_field;
	P = new projective_space;

	F->init(q, 0);
	P->init(n, F, 
		TRUE /* f_init_incidence_structure */, 
		verbose_level);

	delete P;
	delete F;
}

