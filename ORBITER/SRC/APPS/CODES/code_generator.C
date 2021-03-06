// code_generator.C
//
// Anton Betten
//
// moved here from codes.C: May 18, 2009
//
// December 30, 2003

#include "codes.h"

// #############################################################################
// start of class code_generator
// #############################################################################


void code_generator::read_arguments(int argc, const char **argv)
{
	INT i;
	INT f_n = FALSE;
	INT f_k = FALSE;
	INT f_q = FALSE;
	INT f_d = FALSE;
	INT f_N = FALSE;
	
	
	gen->read_arguments(argc, argv, 0);

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-debug") == 0) {
			f_debug = TRUE;
			cout << "-debug " << endl;
			}
		else if (strcmp(argv[i], "-schreier_depth") == 0) {
			schreier_depth = atoi(argv[++i]);
			cout << "-schreier_depth " << schreier_depth << endl;
			}
		else if (strcmp(argv[i], "-n") == 0) {
			f_n = TRUE;
			n = atoi(argv[++i]);
			cout << "-n " << n << endl;
			}
		else if (strcmp(argv[i], "-k") == 0) {
			f_k = TRUE;
			f_linear = TRUE;
			k = atoi(argv[++i]);
			cout << "-k " << k << endl;
			}
		else if (strcmp(argv[i], "-nmk") == 0) {
			f_nmk = TRUE;
			f_linear = TRUE;
			nmk = atoi(argv[++i]);
			cout << "-nmk " << nmk << endl;
			}
		else if (strcmp(argv[i], "-N") == 0) {
			f_N = TRUE;
			f_nonlinear = TRUE;
			N = atoi(argv[++i]);
			cout << "-N " << N << endl;
			}
		else if (strcmp(argv[i], "-q") == 0) {
			f_q = TRUE;
			q = atoi(argv[++i]);
			cout << "-q " << q << endl;
			}
		else if (strcmp(argv[i], "-d") == 0) {
			f_d = TRUE;
			d = atoi(argv[++i]);
			cout << "-d " << d << endl;
			}
		else if (strcmp(argv[i], "-draw_poset") == 0) {
			f_draw_poset = TRUE;
			cout << "-draw_poset " << endl;
			}
		else if (strcmp(argv[i], "-print_data_structure") == 0) {
			f_print_data_structure = TRUE;
			cout << "-print_data_structure " << endl;
			}
		else if (strcmp(argv[i], "-list") == 0) {
			f_list = TRUE;
			cout << "-list" << endl;
			}
		else if (strcmp(argv[i], "-draw_schreier_trees") == 0) {
			gen->f_draw_schreier_trees = TRUE;
			strcpy(gen->schreier_tree_prefix, argv[++i]);
			gen->schreier_tree_xmax = atoi(argv[++i]);
			gen->schreier_tree_ymax = atoi(argv[++i]);
			gen->schreier_tree_f_circletext = atoi(argv[++i]);
			gen->schreier_tree_rad = atoi(argv[++i]);
			gen->schreier_tree_f_embedded = atoi(argv[++i]);
			gen->schreier_tree_f_sideways = atoi(argv[++i]);
			gen->schreier_tree_scale = atoi(argv[++i]) * 0.01;
			gen->schreier_tree_line_width = atoi(argv[++i]) * 0.01;
			cout << "-draw_schreier_trees " << gen->schreier_tree_prefix 
				<< " " << gen->schreier_tree_xmax 
				<< " " << gen->schreier_tree_ymax 
				<< " " << gen->schreier_tree_f_circletext 
				<< " " << gen->schreier_tree_f_embedded 
				<< " " << gen->schreier_tree_f_sideways 
				<< " " << gen->schreier_tree_scale 
				<< " " << gen->schreier_tree_line_width 
				<< endl;
			}
		else if (strcmp(argv[i], "-table_of_nodes") == 0) {
			f_table_of_nodes = TRUE;
			cout << "-table_of_nodes" << endl;
			}
		}
	
	if (f_linear && f_nonlinear) {
		cout << "We cannot be both linear and nonlinear" << endl;
		exit(1);
		}

	if (!f_n && !f_nmk) {
		cout << "Please use option -n <n> or -nmk <nmk> " << endl;
		exit(1);
		}
	if (!f_q) {
		cout << "Please use option -q <q> to specify q" << endl;
		exit(1);
		}
	if (!f_d) {
		cout << "Please use option -d <d> to specify d" << endl;
		exit(1);
		}
	if (!f_k && !f_nmk && !f_N) {
		cout << "Please use option -k <k> or -nmk <nmk> for linear codes or -N <N> for nonlinear codes" << endl;
		exit(1);
		}
	if (f_nmk) {
		if (f_n) {
			k = n - nmk;
			}
		else if (f_k) {
			n = k + nmk;
			}
		else {
			n = 200;
			k = n - nmk;
			}
		}
	cout << "n=" << n << endl;
	if (f_linear) {
		cout << "k=" << k << endl;
		}
	if (f_nonlinear) {
		cout << "N=" << N << endl;
		}
	cout << "q=" << q << endl;
	cout << "d=" << d << endl;
		
	f_irreducibility_test = TRUE;
	
	INT p, h;
	
	is_prime_power(q, p, h);
	if (h > 1) {
		f_semilinear = TRUE;
		}
	else {
		f_semilinear = FALSE;
		}

}

code_generator::code_generator()
{
	null();
}

code_generator::~code_generator()
{
	freeself();
}

void code_generator::null()
{
	verbose_level = 0;
	f_nmk = FALSE;
	f_linear = FALSE;
	f_nonlinear = FALSE;
	gen = NULL;
	F = NULL;
	v1 = NULL;
	v2 = NULL;
	A = NULL;
	description = NULL;
	L = NULL;
	schreier_depth = 1000;
	f_list = FALSE;
	f_table_of_nodes = FALSE;
	f_use_invariant_subset_if_available = TRUE;
	f_debug = FALSE;
	f_draw_poset = FALSE;
	f_print_data_structure = FALSE;
	f_draw_schreier_trees = FALSE;
}

void code_generator::freeself()
{
	if (A) {
		delete A;
		}
	if (F) {
		delete F;
		}
	if (v1) {
		FREE_INT(v1);
		}
	if (v2) {
		FREE_INT(v2);
		}
	if (gen) {
		delete gen;
		}
	if (description) {
		delete description;
		}
	if (L) {
		delete L;
		}
	null();
}

void code_generator::init(int argc, const char **argv)
{
	F = new finite_field;
	A = new action;
	gen = new generator;
	INT f_basis = TRUE;
	

	read_arguments(argc, argv);
	
	//INT verbose_level = gen->verbose_level;
	
	INT f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "code_generator::init" << endl;
		}


	if (f_v) {
		cout << "code_generator::init initializing finite field of order " << q << endl;
		}
	F->init(q, 0);
	if (f_v) {
		cout << "code_generator::init initializing finite field of order " << q << " done" << endl;
		}



	if (f_linear) {
		if (f_nmk) {
			sprintf(directory_path, "CODES_NMK_%ld_%ld_%ld/", nmk, q, d);
			sprintf(prefix, "codes_nmk_%ld_%ld_%ld", nmk, q, d);
			}
		else {
			sprintf(directory_path, "CODES_%ld_%ld_%ld_%ld/", n, k, q, d);
			sprintf(prefix, "codes_%ld_%ld_%ld_%ld", n, k, q, d);
			}
		}
	else if (f_nonlinear) {
		sprintf(directory_path, "NONLINEAR_CODES_%ld_%ld_%ld/", n, q, d);
		sprintf(prefix, "nonlinear_codes_%ld_%ld_%ld", n, q, d);
		}
	if (strlen(directory_path)) {
		BYTE cmd[1000];

		sprintf(cmd, "mkdir %s", directory_path);
		system(cmd);
		}
	sprintf(gen->fname_base, "%s%s", directory_path, prefix);
	
	
	if (f_linear) {
		nmk = n - k;

		if (f_v) {
			cout << "code_generator::init calling init_projective_group, dimension = " << nmk << endl;
			}

		v1 = NEW_INT(nmk);
		v2 = NEW_INT(nmk);

		A->init_projective_group(nmk, F, 
			f_semilinear, 
			f_basis, 
			verbose_level - 2);
	
		if (f_v) {
			cout << "code_generator::init finished with init_projective_group" << endl;
			}

		gen->depth = n;
		rc.init(F, nmk, n, d);
		Strong_gens = A->Strong_gens;
		}
	else if (f_nonlinear) {


		description = new linear_group_description;
		L = new linear_group;
		description->null();
		description->f_affine = TRUE;
		description->n = n;
		description->input_q = q;
		description->F = F;
		description->f_monomial_group = TRUE;
		description->f_semilinear = FALSE;
		description->f_special = FALSE;
		
		L->init(description, verbose_level);
		
		longinteger_object go;

		L->Strong_gens->group_order(go);
		cout << "created strong generators for a group of order " << go << endl;
		L->Strong_gens->print_generators();

		A = L->A2;
		Strong_gens = L->Strong_gens;
		gen->depth = N;
		}

	if (f_v) {
		cout << "code_generator::init degree = " << A->degree << endl;
		cout << "code_generator::init depth = " << gen->depth << endl;
		}
	

	
	
	if (f_v) {
		cout << "code_generator::init group set up, calling gen->init" << endl;
		cout << "A->f_has_strong_generators=" << A->f_has_strong_generators << endl;
		}
	
	gen->init(A, A, Strong_gens, gen->depth /* sz */, verbose_level);


#if 0
	if (f_v) {
		cout << "code_generator::init group set up, calling gen->init_check_func" << endl;
		}

	gen->init_check_func(check_mindist, this /* candidate_check_data */);

	if (f_v) {
		cout << "code_generator::init group set up, calling gen->init_incremental_check_func" << endl;
		}

	gen->init_incremental_check_func(check_mindist_incremental, this /* candidate_check_data */);
#endif
	if (f_v) {
		cout << "code_generator::init group set up, calling gen->init_early_test_func" << endl;
		}
	gen->init_early_test_func(
		check_mindist_early_test_func, 
		this,  
		verbose_level);
	//gen->f_its_OK_to_not_have_an_early_test_func = TRUE;

	

#if 0
	if (FALSE && gen->A->degree < 1000) {
		cout << "the elements of PG(" << n - k - 1 << "," << F->q << ") are:" << endl;
		display_all_PG_elements(n - k - 1, *F);
		}
#endif

	gen->f_print_function = TRUE;
	gen->print_function = print_code;
	gen->print_function_data = this;
	

	INT nb_oracle_nodes = ONE_MILLION;
	
	if (f_v) {
		cout << "code_generator::init group set up, calling gen->init_oracle" << endl;
		}

	gen->init_oracle(nb_oracle_nodes, verbose_level - 1);

	if (f_v) {
		cout << "code_generator::init group set up, calling gen->root[0].init_root_node" << endl;
		}

	gen->root[0].init_root_node(gen, gen->verbose_level - 2);
}


void code_generator::print(INT len, INT *S)
{
	INT N, j;
	INT *codewords;
	
	if (len == 0) {
		return;
		}

	if (f_linear) {
		cout << "generator matrix:" << endl;
		for (j = 0; j < len; j++) {
			PG_element_unrank_modified(*F, 
				rc.M1 + j, len /* stride */, nmk /* len */, 
				S[j]);
			}
		print_integer_matrix(cout, rc.M1, nmk, len);

		INT_matrix_print_tex(cout, rc.M1, nmk, len);

		N = i_power_j(F->q, nmk);
		codewords = NEW_INT(N);
		F->codewords_affine(len, nmk, 
			rc.M1, 
			codewords,
			verbose_level);

		cout << "The " << N << " codewords are: ";
		INT_vec_print(cout, codewords, N);
		cout << endl;

		FREE_INT(codewords);


		F->compute_and_print_projective_weights(rc.M1, len, nmk);

		if (len > k) {
			INT *A, *B;
			INT k1;

			cout << "computing the dual code:" << endl;

			A = NEW_INT(n * n);
			INT_vec_copy(rc.M1, A, nmk * len);
			F->perp_standard(len, nmk, A, 0 /* verbose_level*/);
			B = A + nmk * len;
			print_integer_matrix(cout, B, len - nmk, len);

			INT_matrix_print_tex(cout, B, len - nmk, len);


			k1 = len - nmk;
		
			N = i_power_j(F->q, k1);
			codewords = NEW_INT(N);
			F->codewords_affine(len, k1, 
				B, 
				codewords,
				verbose_level);

			cout << "The " << N << " codewords are: ";
			INT_vec_print(cout, codewords, N);
			cout << endl;

			FREE_INT(codewords);
		
			cout << "before F->compute_and_print_projective_weights" << endl;
			F->compute_and_print_projective_weights(B, len, len - nmk);
			}
		}
	else if (f_nonlinear) {
		cout << "print nonlinear not yet implemented" << endl;
		}	
}

void code_generator::main(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT depth;
	INT f_embedded = TRUE;
	INT f_sideways = FALSE;

	if (f_v) {
		cout << "code_generator::main" << endl;
		}
	depth = gen->main(t0, 
		schreier_depth, 
		f_use_invariant_subset_if_available, 
		f_debug, 
		verbose_level);

	if (f_table_of_nodes) {
		INT *Table;
		INT nb_rows, nb_cols;

		gen->get_table_of_nodes(Table, 
			nb_rows, nb_cols, 0 /*verbose_level*/);
	
		INT_matrix_write_csv("data.csv", Table, nb_rows, nb_cols);


		FREE_INT(Table);
		}

	if (f_list) {

		{
		spreadsheet *Sp;
		gen->make_spreadsheet_of_orbit_reps(Sp, depth);
		BYTE fname_csv[1000];
		sprintf(fname_csv, "orbits_%ld.csv", depth);
		Sp->save(fname_csv, verbose_level);
		delete Sp;
		}

#if 1
		INT f_show_orbit_decomposition = TRUE;
		INT f_show_stab = TRUE;
		INT f_save_stab = TRUE;
		INT f_show_whole_orbit = FALSE;
		
		gen->list_all_orbits_at_level(depth, 
			TRUE, 
			print_code, 
			this, 
			f_show_orbit_decomposition, 
			f_show_stab, 
			f_save_stab, 
			f_show_whole_orbit);

#if 0
		INT d;
		for (d = 0; d < 3; d++) {
			gen->print_schreier_vectors_at_depth(d, verbose_level);
			}
#endif
#endif
		}

	if (f_draw_poset) {
		gen->draw_poset(gen->fname_base, depth, 
			0 /* data1 */, f_embedded, f_sideways, 
			verbose_level);
		}
	if (f_print_data_structure) {
		gen->print_data_structure_tex(depth, verbose_level);
		}
	if (f_v) {
		cout << "code_generator::main done" << endl;
		}
}

void code_generator::early_test_func_by_using_group(
	INT *S, INT len, 
	INT *candidates, INT nb_candidates, 
	INT *good_candidates, INT &nb_good_candidates, 
	INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT f_vv = (verbose_level >= 2);

	if (f_v) {
		cout << "code_generator::early_test_func_by_using_group" << endl;
		}

	if (f_vv) {
		cout << "S=";
		INT_vec_print(cout, S, len);
		cout << " testing " << nb_candidates << " candidates" << endl;
		//INT_vec_print(cout, candidates, nb_candidates);
		//cout << endl;
		}

	INT i, j, node, f, l, pt, nb_good_orbits;
	oracle *O;
	INT f_orbit_is_good;
	INT s, a;

	node = gen->find_oracle_node_for_set(len, S, 
		FALSE /* f_tolerant */, 0);
	O = gen->root + node;

	if (f_v) {
		cout << "code_generator::early_test_func_by_using_group for ";
		O->print_set(gen);
		cout << endl;
		}

	schreier Schreier;

	Schreier.init(gen->A2);

	Schreier.init_generators_by_hdl(
		O->nb_strong_generators, O->hdl_strong_generators, 0);

	Schreier.orbits_on_invariant_subset_fast(
		nb_candidates, candidates, 
		0/*verbose_level*/);

	if (f_v) {
		cout << "code_generator::early_test_func_by_using_group after Schreier.compute_all_orbits_on_invariant_subset, we found " 
		<< Schreier.nb_orbits << " orbits" << endl;
		}
	nb_good_candidates = 0;
	nb_good_orbits = 0;
	for (i = 0; i < Schreier.nb_orbits; i++) {
		f = Schreier.orbit_first[i];
		l = Schreier.orbit_len[i];
		pt = Schreier.orbit[f];
		S[len] = pt;
		if (f_linear) {
			if (rc.check_rank_last_two_are_fixed(len + 1, 
				S, verbose_level - 1)) {
				f_orbit_is_good = TRUE;
				}
			else {
				f_orbit_is_good = FALSE;
				}
			}
		else {
			f_orbit_is_good = TRUE;
			for (s = 0; s < len; s++) {
				a = S[s];
				if (Hamming_distance(a, pt) < d) {
					f_orbit_is_good = FALSE;
					break;
					}
				}
			}
		if (f_orbit_is_good) {
			for (j = 0; j < l; j++) {
				pt = Schreier.orbit[f + j];
				good_candidates[nb_good_candidates++] = pt;
				}	
			nb_good_orbits++;
			}
		}

	INT_vec_heapsort(good_candidates, nb_good_candidates);
	if (f_v) {
		cout << "code_generator::early_test_func_by_using_group after Schreier.compute_all_orbits_on_invariant_subset, we found " 
			<< nb_good_candidates << " good candidates in " 
			<< nb_good_orbits << " good orbits" << endl;
		}
}

INT code_generator::Hamming_distance(INT a, INT b)
{
	INT f_v = TRUE;
	INT d = 0;
	INT i;

	if (f_v) {
		cout << "code_generator::Hamming_distance a=" << a << " b=" << b << endl;
		}
	if (f_nonlinear) {
		if (q == 2) {
			d = Hamming_distance_binary(a, b, n);
			}
		else {
			cout << "code_generator::Hamming_distance f_nonlinear and q != 2" << endl;
			exit(1);
			}
		}
	if (f_linear) {
		PG_element_unrank_modified(*F, 
			v1, 1 /* stride */, nmk /* len */, 
			a);
		PG_element_unrank_modified(*F, 
			v2, 1 /* stride */, nmk /* len */, 
			b);
		for (i = 0; i < nmk; i++) {
			if (v1[i] != v2[i]) {
				d++;
				}
			}
		}
	if (f_v) {
		cout << "code_generator::Hamming_distance a=" << a << " b=" << b << " d=" << d << endl;
		}
	return d;
}


// #############################################################################
// callback functions
// #############################################################################

void check_mindist_early_test_func(INT *S, INT len, 
	INT *candidates, INT nb_candidates, 
	INT *good_candidates, INT &nb_good_candidates, 
	void *data, INT verbose_level)
{
	code_generator *cg = (code_generator *) data;
	INT f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "check_mindist_early_test_func S=";
		INT_vec_print(cout, S, len);
		cout << " testing " << nb_candidates << " candidates" << endl;
		//INT_vec_print(cout, candidates, nb_candidates);
		//cout << endl;
		}

	cg->early_test_func_by_using_group(S, len, 
		candidates, nb_candidates, 
		good_candidates, nb_good_candidates, 
		verbose_level);

#if 0
	nb_good_candidates = 0;
	for (i = 0; i < nb_candidates; i++) {
		S[len] = candidates[i];
		if (cg->rc.check_rank_last_two_are_fixed(len + 1, 
			S, verbose_level - 1)) {		
			good_candidates[nb_good_candidates++] = candidates[i];
			}
		if ((i % 1000) == 0 && i) {
			INT t1, dt;
	
			t1 = os_ticks();
			dt = t1 - cg->gen->t0;
			cout << "Time ";
			time_check_delta(cout, dt);
			cout << " : " << i << endl;
			}
		}
#endif
	

	if (f_v) {
		cout << "check_mindist_early_test_func nb_good_candidates=" 
			<< nb_good_candidates << endl;
		}
}

INT check_mindist(INT len, INT *S, void *data, INT verbose_level)
{
	code_generator *cg = (code_generator *) data;
	INT f_OK = TRUE;
	INT f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "checking set ";
		print_set(cout, len, S);
		}

	if (cg->f_linear) {
		if (!cg->rc.check_rank(len, S, verbose_level - 1)) {
			return FALSE;
			}
		}
	else {
		cout << "check_mindist not yet implemented for nonlinear codes" << endl;
		}

	
	if (f_OK) {
		if (f_v) {
			cout << "OK" << endl;
			}
		return TRUE;
		}
	else {
		return FALSE;
		}
}

INT check_mindist_incremental(INT len, INT *S, void *data, INT verbose_level)
{
	code_generator *cg = (code_generator *) data;
	INT f_OK = TRUE;
	INT f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "checking set ";
		print_set(cout, len, S);
		cout << " (incrementally)";
		}
	if (cg->f_linear) {
		if (!cg->rc.check_rank_last_two_are_fixed(len, S, 
			verbose_level - 1)) {
			return FALSE;
			}
		}
	else {
		cout << "check_mindist_incremental not yet implemented for nonlinear codes" << endl;
		}

	
	if (f_OK) {
		if (f_v) {
			cout << "OK" << endl;
			}
		return TRUE;
		}
	else {
		return FALSE;
		}
}

void print_code(INT len, INT *S, void *data)
{
	code_generator *cg = (code_generator *) data;
	
	cg->print(len, S);
}



