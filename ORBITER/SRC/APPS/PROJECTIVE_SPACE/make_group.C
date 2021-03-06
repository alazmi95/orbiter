// make_group.C
// 
// Anton Betten
// Apr 14, 2011
//
// 
//
//

#include "orbiter.h"
#include "discreta.h"

// global data:

INT t0; // the system time when the program started

void create_group(INT verbose_level);


int main(int argc, char **argv)
{
	INT verbose_level = 0;
	INT i;
	
	t0 = os_ticks();
	
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		}


	
	create_group(verbose_level);

	the_end(t0);
}

void create_group(INT verbose_level)
{
	//INT f_v = (verbose_level >= 1);
	INT n = 1;
	INT q = 8;

	action *A;
	action *A2;
	finite_field *F;
	projective_space *P;
	INT i;
	
	F = new finite_field;
	P = new projective_space;

	F->init(q, 0);
	P->init(n, F, 
		TRUE /* f_init_incidence_structure */, 
		verbose_level - 3);

	cout << "Creating linear group" << endl;
	A = new action;
	A->init_general_linear_group(n + 1, F, TRUE /* f_semilinear */, TRUE /* f_basis */, verbose_level - 2);
	
	cout << "Creating action on lines" << endl;
	projective_space_init_line_action(P, A, A2, verbose_level);



	INT generators[] = {
		0,1,1,1,2,
		1,3,2,0,2
		};
	vector_ge gens;
	INT *Elt1;
	
	Elt1 = NEW_INT(A->elt_size_in_INT);

	//set1 = NEW_INT(P->N_points);
	//set2 = NEW_INT(P->N_points);

	gens.init(A);
	gens.allocate(2);
	for (i = 0; i < 2; i++) {
		A->make_element(Elt1, generators + i * 5, verbose_level);
		A->element_move(Elt1, gens.ith(i), 0);
		cout << "generator " << i << ":" << endl;
		A->element_print(gens.ith(i), cout);
		A->element_print_as_permutation(gens.ith(i), cout);
		}

	sims *S;
	INT nb_times = 100;

	S = new sims;
	S->init(A);
	S->init_trivial_group(0);
	S->init_generators(gens, verbose_level);
	S->compute_base_orbits(verbose_level);
	S->closure_group(nb_times, verbose_level);
	S->print(verbose_level);
	cout << "generators:" << endl;
	S->gens.print(cout);

	cout << "list of group elements:" << endl;
	S->print_all_group_elements();
}



