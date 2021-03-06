// desarguesian_spread.C
// 
// Anton Betten
// July 21, 2014
//
//
//

#include <orbiter.h>
#include <discreta.h>


// global data:

INT t0; // the system time when the program started

int main(int argc, const char **argv);



int main(int argc, const char **argv)
{
	INT verbose_level = 0;
	INT i;
	INT f_n = FALSE;
	INT n;
	INT f_s = FALSE;
	INT s;
	INT f_q = FALSE;
	INT q;
	INT f_poly_q = FALSE;
	const BYTE *poly_q = NULL;
	INT f_poly_Q = FALSE;
	const BYTE *poly_Q = NULL;

	t0 = os_ticks();
	
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-n") == 0) {
			f_n = TRUE;
			n = atoi(argv[++i]);
			cout << "-n " << n << endl;
			}
		else if (strcmp(argv[i], "-s") == 0) {
			f_s = TRUE;
			s = atoi(argv[++i]);
			cout << "-s " << s << endl;
			}
		else if (strcmp(argv[i], "-q") == 0) {
			f_q = TRUE;
			q = atoi(argv[++i]);
			cout << "-q " << q << endl;
			}
		else if (strcmp(argv[i], "-poly_q") == 0) {
			f_poly_q = TRUE;
			poly_q = argv[++i];
			cout << "-poly_q " << poly_q << endl;
			}
		else if (strcmp(argv[i], "-poly_Q") == 0) {
			f_poly_Q = TRUE;
			poly_Q = argv[++i];
			cout << "-poly_Q " << poly_Q << endl;
			}
		}
	if (!f_n) {
		cout << "please use -n option" << endl;
		exit(1);
		}
	if (!f_s) {
		cout << "please use -s option" << endl;
		exit(1);
		}
	if (!f_q) {
		cout << "please use -q option" << endl;
		exit(1);
		}
	INT f_v = (verbose_level >= 1);

	INT Q;
	INT m;

	m = n / s;
	if (m * s != n) {
		cout << "m * s != n" << endl;
		exit(1);
		}

	Q = i_power_j(q, s);
	
	finite_field *Fq;
	finite_field *FQ;
	subfield_structure *SubS;
	desarguesian_spread *D;

	Fq = new finite_field;
	if (f_v) {
		cout << "before Fq->init" << endl;
		}
	Fq->init_override_polynomial(q, poly_q, 0);

	FQ = new finite_field;
	if (f_v) {
		cout << "before FQ->init" << endl;
		}
	FQ->init_override_polynomial(Q, poly_Q, 0);

	SubS = new subfield_structure;
	if (f_v) {
		cout << "before SubS->init" << endl;
		}
	SubS->init(FQ, Fq, verbose_level);

	if (f_v) {
		cout << "Field-basis: ";
		INT_vec_print(cout, SubS->Basis, s);
		cout << endl;
		}
	
	D = new desarguesian_spread;
	if (f_v) {
		cout << "before D->init" << endl;
		}
	D->init(n, m, s, 
		SubS, 
		verbose_level);
	
	D->print_spread_element_table_tex();

	
	the_end(t0);
}

