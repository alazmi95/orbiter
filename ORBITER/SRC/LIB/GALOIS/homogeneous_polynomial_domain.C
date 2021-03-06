// homogeneous_polynomial_domain.C
//
// Anton Betten
//
// September 9, 2016



#include "galois.h"

homogeneous_polynomial_domain::homogeneous_polynomial_domain()
{
	null();
}

homogeneous_polynomial_domain::~homogeneous_polynomial_domain()
{
	freeself();
}

void homogeneous_polynomial_domain::freeself()
{
	INT i;
	
	if (v) {
		FREE_INT(v);
		}
	if (Monomials) {
		FREE_INT(Monomials);
		}
	if (symbols) {
		for (i = 0; i < n; i++) {
			FREE_BYTE(symbols[i]);
			}
		FREE_PBYTE(symbols);
		}
	if (symbols_latex) {
		for (i = 0; i < n; i++) {
			FREE_BYTE(symbols_latex[i]);
			}
		FREE_PBYTE(symbols_latex);
		}
	if (Variables) {
		FREE_INT(Variables);
		}
	if (Affine) {
		FREE_INT(Affine);
		}
	if (Affine_to_monomial) {
		FREE_INT(Affine_to_monomial);
		}
	if (coeff2) {
		FREE_INT(coeff2);
		}
	if (coeff3) {
		FREE_INT(coeff3);
		}
	if (coeff4) {
		FREE_INT(coeff4);
		}
	if (factors) {
		FREE_INT(factors);
		}
	if (my_affine) {
		FREE_INT(my_affine);
		}
	if (P) {
		delete P;
		}
	if (base_cols) {
		FREE_INT(base_cols);
		}
	if (type1) {
		FREE_INT(type1);
		}
	if (type2) {
		FREE_INT(type2);
		}
	null();
}

void homogeneous_polynomial_domain::null()
{
	v = NULL;
	Monomials = NULL;
	symbols = NULL;
	symbols_latex = NULL;
	Variables = NULL;
	Affine = NULL;
	Affine_to_monomial = NULL;
	coeff2 = NULL;
	coeff3 = NULL;
	coeff4 = NULL;
	factors = NULL;
	my_affine = NULL;
	P = NULL;
	base_cols = NULL;
	type1 = NULL;
	type2 = NULL;
}

void homogeneous_polynomial_domain::init(finite_field *F, INT n, INT degree, INT f_init_incidence_structure, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "homogeneous_polynomial_domain::init" << endl;
		}
	homogeneous_polynomial_domain::F = F;
	q = F->q;
	homogeneous_polynomial_domain::n = n;
	homogeneous_polynomial_domain::degree = degree;
	
	v = NEW_INT(n);
	type1 = NEW_INT(degree + 1);
	type2 = NEW_INT(degree + 1);

	make_monomials(verbose_level);
	
	coeff2 = NEW_INT(nb_monomials);
	coeff3 = NEW_INT(nb_monomials);
	coeff4 = NEW_INT(nb_monomials);
	factors = NEW_INT(degree);

	my_affine = NEW_INT(degree);



	P = new projective_space;
	if (f_v) {
		cout << "homogeneous_polynomial_domain::init before P->init" << endl;
		}
	P->init(n - 1, F, 
		f_init_incidence_structure, 
		verbose_level);
	if (f_v) {
		cout << "homogeneous_polynomial_domain::init after P->init" << endl;
		}
	base_cols = NEW_INT(nb_monomials);
	
	if (f_v) {
		cout << "homogeneous_polynomial_domain::init done" << endl;
		}
	
}

void homogeneous_polynomial_domain::make_monomials(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i, j, a, h, idx, t;

	if (f_v) {
		cout << "homogeneous_polynomial_domain::make_monomials" << endl;
		}
	
	nb_monomials = INT_n_choose_k(n + degree - 1, n - 1);

	diophant *D;

	D = new diophant;

	D->open(1, n);
	D->fill_coefficient_matrix_with(1);
	D->RHSi(0) = degree;
	D->sum = degree;

	D->solve_all_betten(0 /* verbose_level */);

	if (f_v) {
		cout << "homogeneous_polynomial_domain::make_monomials We found " << D->_resultanz << " monomials" << endl;
		}

	INT nb_sol;

	D->get_solutions_full_length(Monomials, nb_sol, 0 /* verbose_level */);
	if (f_v) {
		cout << "There are " << nb_sol << " monomials." << endl;

		if (nb_sol < 100) {
			INT_matrix_print(Monomials, nb_sol, n);
			}
		else {
			cout << "too many to print" << endl;
			}
		}

	if (nb_sol != nb_monomials) {
		cout << "homogeneous_polynomial_domain::make_monomials nb_sol != nb_monomials" << endl;
		exit(1);
		}

	delete D;
	


	rearrange_monomials_by_partition_type(verbose_level);
	
	if (f_v) {
		cout << "After rearranging by type:" << endl;
		if (nb_monomials < 100) {
			INT_matrix_print(Monomials, nb_monomials, n);
			}
		else {
			cout << "too many to print" << endl;
			}
		}

	BYTE label[1000];
	symbols = NEW_PBYTE(n);
	for (i = 0; i < n; i++) {

		INT l;
		
		if (n == 3) {
			label[0] = 'X' + i;
			}
		else {
			label[0] = 'A' + i;
			}
		label[1] = 0;
		l = strlen(label);
		symbols[i] = NEW_BYTE(l + 1);
		strcpy(symbols[i], label);
		}
	symbols_latex = NEW_PBYTE(n);
	for (i = 0; i < n; i++) {

		INT l;
		
		if (n == 3) {
			label[0] = 'X' + i;
			}
		else {
			label[0] = 'A' + i;
			}
		label[1] = 0;
		l = strlen(label);
		symbols_latex[i] = NEW_BYTE(l + 1);
		strcpy(symbols_latex[i], label);
		}


	Variables = NEW_INT(nb_monomials * degree);
	for (i = 0; i < nb_monomials; i++) {
		h = 0;
		for (j = 0; j < n; j++) {
			a = Monomials[i * n + j];
			for (t = 0; t < a; t++) {
				Variables[i * degree + h] = j;
				h++;
				}
			}
		if (h != degree) {
			cout << "homogeneous_polynomial_domain::make_monomials h != degree" << endl;
			exit(1);
			}
		}
	if (f_v) {
		cout << "homogeneous_polynomial_domain::make_monomials the variable lists are:" << endl;
		if (nb_monomials < 100) {
			for (i = 0; i < nb_monomials; i++) {
				cout << i << " : ";
				INT_vec_print(cout, Variables + i * degree, degree);
				cout << endl;
				}
			}
		else {
			cout << "too many to print" << endl;
			}
		}

	nb_affine = i_power_j(n, degree);

	if (nb_affine < ONE_MILLION) {
		Affine = NEW_INT(nb_affine * degree);
		if (f_v) {
			cout << "homogeneous_polynomial_domain::make_monomials  Affine, nb_affine=" << nb_affine << endl;
			}
		for (h = 0; h < nb_affine; h++) {
			AG_element_unrank(n /* q */, Affine + h * degree, 1, degree, h);
			}
		if (FALSE) {
			cout << "homogeneous_polynomial_domain::make_monomials  Affine" << endl;
			INT_matrix_print(Affine, nb_affine, degree);
			}
		Affine_to_monomial = NEW_INT(nb_affine);
		for (i = 0; i < nb_affine; i++) {
			if (i > 0 && (i & ((1 << 20) - 1)) == 0) {
				cout << "homogeneous_polynomial_domain::make_monomials i = " << i << " / " << nb_affine << endl;
				}
			INT_vec_zero(v, n);
			for (j = 0; j < degree; j++) {
				a = Affine[i * degree + j];
				v[a]++;
				}
			idx = index_of_monomial(v);
			Affine_to_monomial[i] = idx;
			}
		}
	else {
		cout << "homogeneous_polynomial_domain::make_monomials nb_affine is too big, skipping Affine_to_monomial" << endl;
		Affine = NULL;
		Affine_to_monomial = NULL;
		}

	if (FALSE) {
		cout << "homogeneous_polynomial_domain::make_monomials Affine : idx:" << endl;
		for (i = 0; i < nb_affine; i++) {
			cout << i << " : ";
			INT_vec_print(cout, Affine + i * degree, degree);
			cout << " : " << Affine_to_monomial[i] << endl;
			}
		}
	

	if (f_v) {
		cout << "homogeneous_polynomial_domain::make_monomials done" << endl;
		}
}

void homogeneous_polynomial_domain::rearrange_monomials_by_partition_type(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "homogeneous_polynomial_domain::rearrange_monomials_by_partition_type" << endl;
		}

	Heapsort_general(Monomials, nb_monomials, 
		homogeneous_polynomial_domain_compare_monomial, 
		homogeneous_polynomial_domain_swap_monomial, 
		this);


	if (f_v) {
		cout << "homogeneous_polynomial_domain::rearrange_monomials_by_partition_type done" << endl;
		}
}

INT homogeneous_polynomial_domain::index_of_monomial(INT *v)
{

#if 0
	INT i, j;
	
	for (i = 0; i < nb_monomials; i++) {
		for (j = 0; j < n; j++) {
			if (v[j] != Monomials[i * n + j]) {
				break;
				}
			}
		if (j == n) {
			return i;
			}
		}
#endif
	INT idx;
	
	if (!search_general(Monomials, nb_monomials, v, idx, 
		homogeneous_polynomial_domain_compare_monomial_with, 
		this /* extra_data */, 0 /* verbose_level */)) {
		cout << "homogeneous_polynomial_domain::index_of_monomial Did not find the monomial" << endl;
		exit(1);
		}
	return idx;
}

void homogeneous_polynomial_domain::print_monomial(ostream &ost, INT i)
{
	INT j, a;
	
	for (j = 0; j < n; j++) {
		a = Monomials[i * n + j];
		if (a == 0) {
			continue;
			}
		ost << symbols_latex[j];
		if (a > 1) {
			ost << "^" << a;
			}
		}
}

void homogeneous_polynomial_domain::print_monomial(ostream &ost, INT *mon)
{
	INT j, a;
	
	for (j = 0; j < n; j++) {
		a = mon[j];
		if (a == 0) {
			continue;
			}
		ost << symbols_latex[j];
		if (a > 1) {
			ost << "^" << a;
			}
		}
}

void homogeneous_polynomial_domain::print_monomial(BYTE *str, INT i)
{
	INT j, a;
	
	str[0] = 0;
	for (j = 0; j < n; j++) {
		a = Monomials[i * n + j];
		if (a == 0) {
			continue;
			}
		strcat(str + strlen(str), symbols_latex[j]);
		if (a >= 10) {
			sprintf(str + strlen(str), "^{%ld}", a);
			}
		else if (a > 1) {
			sprintf(str + strlen(str), "^%ld", a);
			}
		}
}

void homogeneous_polynomial_domain::print_equation(ostream &ost, INT *coeffs)
{
	INT i, c;
	INT f_first = TRUE;


	for (i = 0; i < nb_monomials; i++) {
		c = coeffs[i];
		if (c == 0) {
			continue;
			}
		if (f_first) {
			f_first = FALSE;
			}
		else {
			ost << " + ";
			}
		if (c > 1) {
			ost << c;
			}
		print_monomial(ost, i);
		}
}

void homogeneous_polynomial_domain::print_equation_with_line_breaks_tex(ostream &ost, INT *coeffs, INT nb_terms_per_line, const BYTE *new_line_text)
{
	INT i, c, cnt = 0;
	INT f_first = TRUE;


	for (i = 0; i < nb_monomials; i++) {
		c = coeffs[i];
		if (c == 0) {
			continue;
			}

		if ((cnt % nb_terms_per_line) == 0 && cnt) {
			ost << new_line_text;
			}


		if (f_first) {
			f_first = FALSE;
			}
		else {
			ost << " + ";
			}
		if (c > 1) {
			ost << c;
			}
		print_monomial(ost, i);
		cnt++;
		}
}

void homogeneous_polynomial_domain::enumerate_points(INT *coeff, INT *Pts, INT &nb_pts, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT rk, a;
	//INT *v;

	if (f_v) {
		cout << "homogeneous_polynomial_domain::enumerate_points P->N_points=" << P->N_points << endl;
		}
	//v = NEW_INT(n);
	nb_pts = 0;
	for (rk = 0; rk < P->N_points; rk++) {
		unrank_point(v, rk);
		a = evaluate_at_a_point(coeff, v);
		if (a == 0) {
			Pts[nb_pts++] = rk;
			}
		}

	//FREE_INT(v);
	if (f_v) {
		cout << "homogeneous_polynomial_domain::enumerate_points done" << endl;
		}
}

INT homogeneous_polynomial_domain::evaluate_at_a_point_by_rank(INT *coeff, INT pt)
{
	INT a;
	
	unrank_point(v, pt);
	a = evaluate_at_a_point(coeff, v);
	return a;
}

INT homogeneous_polynomial_domain::evaluate_at_a_point(INT *coeff, INT *pt_vec)
{
	INT i, a, b, c;
	
	a = 0;
	for (i = 0; i < nb_monomials; i++) {
		if (coeff[i] == 0) {
			continue;
			}
		b = F->evaluate_monomial(Monomials + i * n, pt_vec, n);
		c = F->mult(coeff[i], b);
		a = F->add(a, c);
		}
	return a;
}

void homogeneous_polynomial_domain::substitute_linear(INT *coeff_in, INT *coeff_out, 
	INT *Mtx_inv, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);

	if (f_v) {
		cout << "homogeneous_polynomial_domain::substitute_linear" << endl;
		}

	substitute_semilinear(coeff_in, coeff_out, 
		FALSE /* f_semilinear */, 0 /* frob_power */, Mtx_inv, verbose_level);
	if (f_v) {
		cout << "homogeneous_polynomial_domain::substitute_linear done" << endl;
		}
}

void homogeneous_polynomial_domain::substitute_semilinear(INT *coeff_in, INT *coeff_out, 
	INT f_semilinear, INT frob_power, INT *Mtx_inv, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT a, b, c, i, j, idx;
	INT *A;
	INT *V;

	if (f_v) {
		cout << "homogeneous_polynomial_domain::substitute_semilinear" << endl;
		}


	if (f_semilinear) {
		for (i = 0; i < nb_monomials; i++) {
			c = coeff_in[i];
			coeff4[i] = F->frobenius_power(c, frob_power);
			}
		}
	else {
		INT_vec_copy(coeff_in, coeff4, nb_monomials);
		}


	INT_vec_zero(coeff3, nb_monomials);
	for (i = 0; i < nb_monomials; i++) {
		c = coeff4[i];
		if (c == 0) {
			continue;
			}

#if 0
		cout << "homogeneous_polynomial_domain::substitute_semilinear monomial " << c << " * ";
		print_monomial(cout, i);
		cout << endl;
#endif
		
		V = Variables + i * degree;
			// a list of the indices of the variables which appear in the monomial
			// (possibly with repeats)
			// Example: the monomial x_0^3 becomes 0,0,0

#if 0
		cout << "variables: ";
		INT_vec_print(cout, V, degree);
		cout << endl;

		cout << "Mtx:" << endl;
		INT_matrix_print(Mtx_inv, n, n);
#endif

		INT_vec_zero(coeff2, nb_monomials);
		for (a = 0; a < nb_affine; a++) {
			if (Affine) {
				A = Affine + a * degree;
				}
			else {
				A = my_affine;
				AG_element_unrank(n /* q */, my_affine, 1, degree, a);
					// sequence of length degree over the alphabet  0,...,n-1.
				}
			for (j = 0; j < degree; j++) {
				//factors[j] = Mtx_inv[V[j] * n + A[j]];
				factors[j] = Mtx_inv[A[j] * n + V[j]];
				}

			b = F->product_n(factors, degree);
			if (Affine_to_monomial) {
				idx = Affine_to_monomial[a];
				}
			else {
				INT_vec_zero(v, n);
				for (j = 0; j < degree; j++) {
					a = Affine[i * degree + j];
					v[a]++;
					}
				idx = index_of_monomial(v);
				}

#if 0
			cout << "affine " << a << " / " << nb_affine << " : ";
			INT_vec_print(cout, A, 3);
			cout << " factors ";
			INT_vec_print(cout, factors, 3);
			cout << " b=" << b << " idx=" << idx << endl;
#endif
			coeff2[idx] = F->add(coeff2[idx], b);
			}
		for (j = 0; j < nb_monomials; j++) {
			coeff2[j] = F->mult(coeff2[j], c);
			}

#if 0
		cout << "homogeneous_polynomial_domain::substitute_semilinear monomial " << c << " * ";
		print_monomial(cout, i);
		cout << " yields:" << endl;
		INT_vec_print(cout, coeff2, nb_monomials);
		cout << endl;
#endif
		
		for (j = 0; j < nb_monomials; j++) {
			coeff3[j] = F->add(coeff2[j], coeff3[j]);
			}
		}
#if 0
	cout << "homogeneous_polynomial_domain::substitute_semilinear input:" << endl;
	INT_vec_print(cout, coeff_in, nb_monomials);
	cout << endl;
	cout << "homogeneous_polynomial_domain::substitute_semilinear output:" << endl;
	INT_vec_print(cout, coeff3, nb_monomials);
	cout << endl;
#endif





	INT_vec_copy(coeff3, coeff_out, nb_monomials);
	if (f_v) {
		cout << "homogeneous_polynomial_domain::substitute_semilinear done" << endl;
		}
}

INT homogeneous_polynomial_domain::is_zero(INT *coeff)
{
	INT i;
	
	for (i = 0; i < nb_monomials; i++) {
		if (coeff[i]) {
			return FALSE;
			}
		}
	return TRUE;
}

void homogeneous_polynomial_domain::unrank_point(INT *v, INT rk)
{
	P->unrank_point(v, rk);
}

INT homogeneous_polynomial_domain::rank_point(INT *v)
{
	INT rk;

	rk = P->rank_point(v);
	return rk;
}

void homogeneous_polynomial_domain::unrank_coeff_vector(INT *v, INT rk)
{
	PG_element_unrank_modified(*F, v, 1, nb_monomials, rk);
}

INT homogeneous_polynomial_domain::rank_coeff_vector(INT *v)
{
	INT rk;

	PG_element_rank_modified(*F, v, 1, nb_monomials, rk);
	return rk;
}

INT homogeneous_polynomial_domain::test_weierstrass_form(INT rk, 
	INT &a1, INT &a2, INT &a3, INT &a4, INT &a6, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT m_one;

	unrank_coeff_vector(coeff2, rk);
	if (f_v) {
		cout << "homogeneous_polynomial_domain::test_weierstrass_form" << endl;
		}
	if (n != 3) {
		cout << "homogeneous_polynomial_domain::test_weierstrass_form n != 3" << endl;
		exit(1);
		}
	if (degree != 3) {
		cout << "homogeneous_polynomial_domain::test_weierstrass_form degree != 3" << endl;
		exit(1);
		}
	if (coeff2[1] || coeff2[3] || coeff2[6]) {
		return FALSE;
		}
	PG_element_normalize_from_front(*F, coeff2, 1, nb_monomials);
	if (coeff2[0] != 1) {
		return FALSE;
		}
	m_one = F->negate(1);
	if (coeff2[7] != m_one) {
		return FALSE;
		}
	a1 = F->negate(coeff2[4]);
	a2 = coeff2[2];
	a3 = F->negate(coeff2[8]);
	a4 = coeff2[5];
	a6 = coeff2[9];
	return TRUE;
}

void homogeneous_polynomial_domain::vanishing_ideal(INT *Pts, INT nb_pts, INT &r, INT *Kernel, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT i, j;
	INT *System;

	if (f_v) {
		cout << "homogeneous_polynomial_domain::vanishing_ideal" << endl;
		}
	System = NEW_INT(MAX(nb_pts, nb_monomials) * nb_monomials);
	for (i = 0; i < nb_pts; i++) {
		unrank_point(v, Pts[i]);
		for (j = 0; j < nb_monomials; j++) {
			System[i * nb_monomials + j] = F->evaluate_monomial(Monomials + j * n, v, n);
			}
		}
	if (f_v && FALSE) {
		cout << "homogeneous_polynomial_domain::vanishing_ideal The system:" << endl;
		INT_matrix_print(System, nb_pts, nb_monomials);
		}

	if (f_v) {
		cout << "homogeneous_polynomial_domain::vanishing_ideal before RREF_and_kernel" << endl;
		}
	r = F->RREF_and_kernel(nb_monomials, nb_pts, System, 0 /* verbose_level */);
	if (f_v) {
		cout << "homogeneous_polynomial_domain::vanishing_ideal The system has rank " << r << endl;
		}
	if (TRUE) {
		cout << "homogeneous_polynomial_domain::vanishing_ideal The system in RREF:" << endl;
		INT_matrix_print(System, r, nb_monomials);
		cout << "homogeneous_polynomial_domain::vanishing_ideal The kernel:" << endl;
		INT_matrix_print(System + r * nb_monomials, nb_monomials - r, nb_monomials);
		}
	INT_vec_copy(System + r * nb_monomials, Kernel, (nb_monomials - r) * nb_monomials);
	FREE_INT(System);
}

INT homogeneous_polynomial_domain::compare_monomials(INT *M1, INT *M2)
{
	INT h, a;
	INT ret = 0;
	
	INT_vec_zero(type1, degree + 1);
	INT_vec_zero(type2, degree + 1);

	for (h = 0; h < n; h++) {
		a = M1[h];
		type1[a]++;
		}
	for (h = 0; h < n; h++) {
		a = M2[h];
		type2[a]++;
		}
	for (h = degree; h >= 0; h--) {
		if (type2[h] > type1[h]) {
			//cout << "type2[h] > type1[h] h=" << h << ", needs swap" << endl;
			ret = 1;
			goto the_end;
			}
		if (type2[h] < type1[h]) {
			ret = -1;
			goto the_end;
			}
		}
	
	for (a = degree; a >= 1; a--) {
		for (h = 0; h < n; h++) {
			if ((M1[h] != a) && (M2[h] != a)) {
				continue;
				}
			if (M1[h] > M2[h]) {
				ret = -1;
				goto the_end;
				}
			if (M1[h] < M2[h]) {
				//cout << "M1[h] < M2[h] h=" << h << ", needs swap" << endl;
				ret = 1;
				goto the_end;
				}
			}
		}

the_end:
	return ret;

}


void homogeneous_polynomial_domain::print_monomial_ordering(ostream &ost)
{
	INT h;
	
	//ost << "The ordering of monomials is:\\\\" << endl;
	ost << "$$" << endl;
	ost << "\\begin{array}{|r|r|r|}" << endl;
	ost << "\\hline" << endl;
	ost << "h &  \\mbox{monomial} & \\mbox{vector} \\\\" << endl;
	ost << "\\hline" << endl;
	ost << "\\hline" << endl;
	for (h = 0; h < nb_monomials; h++) {
		ost << h << " & ";
		print_monomial(ost, h);
		ost << " & ";
		INT_vec_print(ost, Monomials + h * n, n);
		ost << "\\\\" << endl; 
		}
	ost << "\\hline" << endl;
	ost << "\\end{array}" << endl;
	ost << "$$" << endl;
}




INT homogeneous_polynomial_domain_compare_monomial_with(void *data, INT i, INT *data2, void *extra_data)
{
	homogeneous_polynomial_domain *HPD = (homogeneous_polynomial_domain *) extra_data;
	INT *Data;
	INT ret, n;

	Data = (INT *) data;
	n = HPD->n;
	ret = HPD->compare_monomials(Data + i * n, data2);
	return ret;
}

INT homogeneous_polynomial_domain_compare_monomial(void *data, INT i, INT j, void *extra_data)
{
	homogeneous_polynomial_domain *HPD = (homogeneous_polynomial_domain *) extra_data;
	INT *Data;
	INT ret, n;

	Data = (INT *) data;
	n = HPD->n;
	ret = HPD->compare_monomials(Data + i * n, Data + j * n);
	return ret;
}

void homogeneous_polynomial_domain_swap_monomial(void *data, INT i, INT j, void *extra_data)
{
	homogeneous_polynomial_domain *HPD = (homogeneous_polynomial_domain *) extra_data;
	INT *Data;
	INT h, a, n;

	Data = (INT *) data;
	n = HPD->n;

	for (h = 0; h < n; h++) {
		a = Data[i * n + h];
		Data[i * n + h] = Data[j * n + h];
		Data[j * n + h] = a;
		}
	
}


