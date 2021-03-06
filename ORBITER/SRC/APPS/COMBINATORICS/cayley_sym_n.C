// cayley_sym_n.C
//
// Anton Betten
// February 16, 2015
//

#include "orbiter.h"


INT t0;

void do_it(INT n, INT f_special, INT f_bubble, INT f_pancake, INT f_burnt_pancake, INT verbose_level);
void get_submatrices(INT n, INT *Adj, INT N, INT N0, INT **&P, INT verbose_level);
void mult_matrix(INT *P, INT *Q, INT *R, INT N);



int main(int argc, char **argv)
{
	INT i;
	INT verbose_level = 0;
	INT f_n = FALSE;
	INT n = 0;
	INT f_star = FALSE;
	INT f_bubble = FALSE;
	INT f_pancake = FALSE;
	INT f_burnt_pancake = FALSE;
	
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
		else if (strcmp(argv[i], "-star") == 0) {
			f_star = TRUE;
			cout << "-star" << endl;
			}
		else if (strcmp(argv[i], "-bubble") == 0) {
			f_bubble = TRUE;
			cout << "-bubble" << endl;
			}
		else if (strcmp(argv[i], "-pancake") == 0) {
			f_pancake = TRUE;
			cout << "-pancake" << endl;
			}
		else if (strcmp(argv[i], "-burnt_pancake") == 0) {
			f_burnt_pancake = TRUE;
			cout << "-burnt_pancake" << endl;
			}
		}
	


	if (!f_n) {
		cout << "please specify -n <n>" << endl;
		exit(1);
		}

	do_it(n, f_star, f_bubble, f_pancake, f_burnt_pancake, verbose_level);
	
}

void do_it(INT n, INT f_star, INT f_bubble, INT f_pancake, INT f_burnt_pancake, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "do_it" << endl;
		}

	action *A;
	longinteger_object go;
	INT goi;
	INT *v;
	INT i, j;
	INT nb_gens = 0;
	INT deg = 0;
	vector_ge *gens;
	BYTE fname_base[1000];


	if (f_star) {
		sprintf(fname_base, "Cayley_Sym_%ld_star", n);
		}
	else if (f_bubble) {
		sprintf(fname_base, "Cayley_Sym_%ld_coxeter", n);
		}
	else if (f_pancake) {
		sprintf(fname_base, "Cayley_pancake_%ld", n);
		}
	else if (f_burnt_pancake) {
		sprintf(fname_base, "Cayley_burnt_pancake_%ld", n);
		}
	else {
		cout << "please specify the type of Cayley graph" << endl;
		exit(1);
		}

	cout << "fname=" << fname_base << endl;


	if (f_star) {
		nb_gens = n - 1;
		deg = n;
		}
	else if (f_bubble) {
		nb_gens = n - 1;
		deg = n;
		}
	else if (f_pancake) {
		nb_gens = n - 1;
		deg = n;
		}
	else if (f_burnt_pancake) {
		nb_gens = n - 1;
		deg = 2 * n;
		}


	A = new action;
	A->init_symmetric_group(deg, 0 /*verbose_level*/);
	A->group_order(go);

	goi = go.as_INT();
	cout << "Created group Sym(" << deg << ") of size " << goi << endl;

	strong_generators *SG;


	if (f_burnt_pancake) {
		INT *factors;
		INT nb_factors;
		INT deg1;
		INT nb_perms;
		INT *perms;
		vector_ge *G_gens;
		longinteger_object target_go;
		longinteger_domain D;

		
		G_gens = new vector_ge;
		
		order_Bn_group_factorized(n, factors, nb_factors);

		D.multiply_up(target_go, factors, nb_factors);
		cout << "target group order = " << target_go << endl;

		generators_Bn_group(n, deg1, nb_perms, perms, verbose_level);
		
		G_gens->init(A);
		G_gens->allocate(nb_perms);

		for (i = 0; i < nb_perms; i++) {
			A->make_element(G_gens->ith(i), perms + i * deg1, 0 /* verbose_level */);
			}

		generators_to_strong_generators(A, 
			TRUE /* f_target_go */, target_go, 
			G_gens, SG, verbose_level - 3);
		
		delete G_gens;
		FREE_INT(perms);
		FREE_INT(factors);
		}
	else {
		SG = A->Strong_gens;
		}

	sims *G;
	longinteger_object G_go;

	cout << "creating group G:" << endl;
	G = SG->create_sims(0 /* verbose_level */);
	G->group_order(G_go);
	cout << "created group G of order " << G_go << endl;
	goi = G_go.as_INT();
	

	gens = new vector_ge;

	gens->init(A);
	gens->allocate(nb_gens);

	v = NEW_INT(deg);


	if (f_star) {
		for (i = 0; i < nb_gens; i++) {
			perm_identity(v, deg);
			v[0] = i + 1;
			v[i + 1] = 0;
			A->make_element(gens->ith(i), v, 0 /* verbose_level */);
			}
		}
	else if (f_bubble) {
		for (i = 0; i < nb_gens; i++) {
			perm_identity(v, deg);
			v[i] = i + 1;
			v[i + 1] = i;
			A->make_element(gens->ith(i), v, 0 /* verbose_level */);
			}
		}
	else if (f_pancake) {
		for (i = 0; i < nb_gens; i++) {
			perm_identity(v, deg);
			for (j = 0; j <= 1 + i; j++) {
				v[j] = 1 + i - j;
				}
			A->make_element(gens->ith(i), v, 0 /* verbose_level */);
			}
		}
	else if (f_burnt_pancake) {
		for (i = 0; i < nb_gens; i++) {
			perm_identity(v, deg);
			for (j = 0; j <= 1 + i; j++) {
				v[2 * j + 0] = 2 * (1 + i - j) + 1;
				v[2 * j + 1] = 2 * (1 + i - j) + 0;
				}
			A->make_element(gens->ith(i), v, 0 /* verbose_level */);
			}
		}

	cout << "generators:" << endl;
	for (i = 0; i < nb_gens; i++) {
		cout << "generator " << i << ":" << endl;
		A->element_print(gens->ith(i), cout);
		}

	
#if 0
	sims *Sims;

	Sims = A->Sims;	
#endif

	
	INT *Adj;
	INT goi1;

	G->create_Cayley_graph(gens, Adj, goi1, verbose_level);



	cout << "The adjacency matrix of a graph with " << goi << " vertices has been computed" << endl;
	//INT_matrix_print(Adj, goi, goi);


	save_as_colored_graph_easy(fname_base, goi, Adj, 0 /* verbose_level */);


	return;


	INT N0;
	INT **P;
	//INT N2;
	INT h;

	N0 = goi / n;
	get_submatrices(n, Adj, goi, N0, P, verbose_level);

	for (h = 0; h < n - 1; h++) {
		cout << "P_" << h << "=" << endl;
		INT_matrix_print(P[h], N0, N0);
		}


	for (h = 0; h < n - 1; h++) {
		FREE_INT(P[h]);
		}
	FREE_PINT(P);

	//delete CG;
	FREE_INT(Adj);
	delete gens;
	FREE_INT(v);
}


void get_submatrices(INT n, INT *Adj, INT N, INT N0, INT **&P, INT verbose_level)
{
	
	//INT *Q;
	INT h, i, j;

	P = NEW_PINT(n - 1);
	for (h = 0; h < n - 1; h++) {
		P[h] = NEW_INT(N0 * N0);
		for (i = 0; i < N0; i++) {
			for (j = 0; j < N0; j++) {
				P[h][i * N0 + j] = Adj[(1 + h) * N * N0 + i * N + j];
				}
			}
		}
}

#if 0
	Q = NEW_INT(N0 * N0);
	
	N2 = N - N0;
	Adj2 = NEW_INT(N2 * N2);
	for (u = 0; u < n - 1; u++) {
		for (v = 0; v < n - 1; v++) {
			mult_matrix(P[u], P[v], Q, N0);
			cout << "P_" << u << " * P_" << v << " = " << endl;
			INT_matrix_print(Q, N0, N0);
			for (i = 0; i < N0; i++) {
				for (j = 0; j < N0; j++) {
					Adj2[u * N0 * N2 + i * N2 + v * N0 + j] = Q[i * N0 + j]; 
					}
				}
			}
		}

	cout << "Adj2=" << endl;
	INT_matrix_print(Adj2, N2, N2);

	FREE_INT(Q);
}
#endif

void mult_matrix(INT *P, INT *Q, INT *R, INT N)
{
	INT i, j, h, a;

	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			a = 0;
			for (h = 0; h < N; h++) {
				a += P[i * N + h] * Q[h * N + j];
				}
			R[i * N + j] = a;
			}
		}
}

