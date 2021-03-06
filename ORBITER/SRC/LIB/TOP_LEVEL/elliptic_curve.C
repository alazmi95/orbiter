// elliptic_curve.C
// 
// Anton Betten
// Oct 27, 2009
//
//
// pulled out of crypto.C: November 19, 2014
//
//

#include "orbiter.h"


elliptic_curve::elliptic_curve()
{
	null();
}

elliptic_curve::~elliptic_curve()
{
	freeself();
}


void elliptic_curve::null()
{
	T = NULL;
	A = NULL;
}

void elliptic_curve::freeself()
{
	if (T) {
		FREE_INT(T);
		}
	if (A) {
		FREE_INT(A);
		}
}


void elliptic_curve::init(finite_field *F, INT b, INT c, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	
	if (f_v) {
		cout << "elliptic_curve::init q=" << F->q << " b=" << b << " c=" << c << endl;
		}
	elliptic_curve::F = F;
	q = F->q;
	p = F->p;
	e = F->e;
	elliptic_curve::b = b;
	elliptic_curve::c = c;


	if (f_v) {
		cout << "elliptic_curve::init before compute_points" << endl;
		}

	compute_points(verbose_level);

	if (f_v) {
		cout << "elliptic_curve::init after compute_points" << endl;
		}

#if 0
	if (E.nb < 20) {
		print_integer_matrix_width(cout, E.A, E.nb, E.nb, E.nb, 3);
		}
	
#endif


	
#if 0
	cout << "point : order" << endl;
	for (i = 0; i < E.nb; i++) {
		j = order_of_point(E, i);
		cout << setw(4) << i << " : " << setw(4) << j << endl;
		}
	
	cout << "the curve has " << E.nb << " points" << endl;
#endif

#if 0
	{
	INT a, b, c;
	a = 1;
	b = 2;
	c = E.A[a * E.nb + b];
	cout << "P_" << a << " + P_" << b << " = P_" << c << endl;
	}

	j = multiple_of_point(E, 0, 37);
	cout << "37 * P_0 = P_" << j << endl;
#endif
	if (f_v) {
		cout << "elliptic_curve::init done" << endl;
		}
}


void elliptic_curve::compute_points(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT x, y, y1, y2;
	INT r;
	INT bound;

	if (f_v) {
		cout << "elliptic_curve::compute_points" << endl;
		}
	bound = q + 1 + 2 * ((INT)(sqrt(q)) + 1);// Hasse Weil bound
	


	T = new INT[bound * 3];
	nb = 0;


	// the point at infinity comes first:
	add_point_to_table(0, 1, 0);


	for (x = 0; x < q; x++) {
		r = evaluate_RHS(x);
		if (r == 0) {
			add_point_to_table(x, 0, 1);
			if (nb == bound) {
				cout << "The number of points exceeds the bound" << endl;
				exit(1);
				}
			//cout << nb++ << " : (" << x << "," << 0 << ",1)" << endl;
			}
		else {
			if (F->square_root(r, y)) {
				y1 = y;
				y2 = F->negate(y);
				if (y2 == y1) {
					add_point_to_table(x, y1, 1);
					if (nb == bound) {
						cout << "The number of points exceeds the bound" << endl;
						exit(1);
						}
					}
				else {
					if (y2 < y1) {
						y1 = y2;
						y2 = y;
						}
					add_point_to_table(x, y1, 1);
					if (nb == bound) {
						cout << "The number of points exceeds the bound" << endl;
						exit(1);
						}
					add_point_to_table(x, y2, 1);
					if (nb == bound) {
						cout << "The number of points exceeds the bound" << endl;
						exit(1);
						}
					}
				}
			else {
				// no point for this x coordinate
				}

#if 0
			if (p != 2) {
				l = Legendre(r, q, 0);
					// GALOIS/number_theory.C

				if (l == 1) {
					y = sqrt_mod_involved(r, q);
						// DISCRETA/global.C

					if (F->mult(y, y) != r) {
						cout << "There is a problem with the square root" << endl;
						exit(1);
						}
					y1 = y;
					y2 = F->negate(y);
					if (y2 < y1) {
						y1 = y2;
						y2 = y;
						}
					add_point_to_table(x, y1, 1);
					if (nb == bound) {
						cout << "The number of points exceeds the bound" << endl;
						exit(1);
						}
					add_point_to_table(x, y2, 1);
					if (nb == bound) {
						cout << "The number of points exceeds the bound" << endl;
						exit(1);
						}
					//cout << nb++ << " : (" << x << "," << y << ",1)" << endl;
					//cout << nb++ << " : (" << x << "," << F.negate(y) << ",1)" << endl;
					}
				}
			else {
				y = F->frobenius_power(r, e - 1);
				add_point_to_table(x, y, 1);
				if (nb == bound) {
					cout << "The number of points exceeds the bound" << endl;
					exit(1);
					}
				//cout << nb++ << " : (" << x << "," << y << ",1)" << endl;
				}
#endif

			}
		}


	if (nb == bound) {
		cout << "The number of points exceeds the bound" << endl;
		exit(1);
		}


	if (f_v) {
		cout << "elliptic_curve::compute_points done, we found " << nb << " points" << endl;
		}
}

void elliptic_curve::add_point_to_table(INT x, INT y, INT z)
{
	T[nb * 3 + 0] = x;
	T[nb * 3 + 1] = y;
	T[nb * 3 + 2] = z;
	nb++;
}

INT elliptic_curve::evaluate_RHS(INT x)
// evaluates x^3 + bx + c
{
	INT x2, x3, t;
	
	x2 = F->mult(x, x);
	x3 = F->mult(x2, x);
	t = F->add(x3, F->mult(b, x));
	t = F->add(t, c);
	return t;
}

void elliptic_curve::print_points()
{
	INT i;
	
	cout << "i : point (x,y,x)" << endl;
	for (i = 0; i < nb; i++) {
		cout << setw(4) << i << " & " << T[i * 3 + 0] << "," << T[i * 3 + 1] << "," << T[i * 3 + 2] << "\\\\" << endl;
		}
}

void elliptic_curve::print_points_affine()
{
	INT i;
	
	cout << "i : point (x,y,x)" << endl;
	for (i = 0; i < nb; i++) {
		cout << setw(4) << i << " & ";
		if (T[i * 3 + 2] == 0) {
			cout << "\\cO";
			}
		else {
			cout << "(" << T[i * 3 + 0] << "," << T[i * 3 + 1] << ")";
			}
		cout << "\\\\" << endl;
		}
}


void elliptic_curve::addition(
	INT x1, INT x2, INT x3, 
	INT y1, INT y2, INT y3, 
	INT &z1, INT &z2, INT &z3, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT a, two, three, top, bottom, m;
	
	if (f_v) {
		cout << "elliptic_curve::addition: ";
		cout << "(" << x1 << "," << x2 << "," << x3 << ")";
		cout << " + ";
		cout << "(" << y1 << "," << y2 << "," << y3 << ")";
		cout << endl;
		}
	if (x3 == 0) {
		z1 = y1;
		z2 = y2;
		z3 = y3;
		return;
		}
	if (y3 == 0) {
		z1 = x1;
		z2 = x2;
		z3 = x3;
		return;
		}
	if (x3 != 1) {
		a = F->inverse(x3);
		x1 = F->mult(x1, a);
		x2 = F->mult(x2, a);
		}
	if (y3 != 1) {
		a = F->inverse(y3);
		y1 = F->mult(y1, a);
		y2 = F->mult(y2, a);
		}
	if (x1 == y1 && x2 != y2) {
		if (F->negate(x2) != y2) {
			cout << "x1 == y1 && x2 != y2 && F.negate(x2) != y2" << endl;
			exit(1);
			}
		z1 = 0;
		z2 = 1;
		z3 = 0;
		return;
		}
	if (x1 == y1 && x2 == 0 && y2 == 0) {
		z1 = 0;
		z2 = 1;
		z3 = 0;
		return;
		}
	if (x1 == y1 && x2 == y2) {
		two = F->add(1, 1);
		three = F->add(two, 1);
		top = F->add(F->mult(three, F->mult(x1, x1)), b);
		bottom = F->mult(two, x2);
		a = F->inverse(bottom);  // this does not work in characteristic two !!!
		m = F->mult(top, a);
		}
	else {
		top = F->add(y2, F->negate(x2));
		bottom = F->add(y1, F->negate(x1));
		a = F->inverse(bottom);
		m = F->mult(top, a);
		}
	z1 = F->add(F->add(F->mult(m, m), F->negate(x1)), F->negate(y1));
	z2 = F->add(F->mult(m, F->add(x1, F->negate(z1))), F->negate(x2));
	z3 = 1;
}

void elliptic_curve::draw_grid(char *fname, INT xmax, INT ymax, INT f_with_points, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT x_min = 0, x_max = 1000000;
	INT y_min = 0, y_max = 1000000;
	INT factor_1000 = 1000;
	BYTE fname_full[1000];
	INT f_embedded = TRUE;
	INT f_sideways = FALSE;
	
	if (f_v) {
		cout << "draw_grid" << endl;
		}
	sprintf(fname_full, "%s.mp", fname);
	{
	mp_graphics G(fname_full, x_min, y_min, x_max, y_max, f_embedded, f_sideways);
	G.out_xmin() = 0;
	G.out_ymin() = 0;
	G.out_xmax() = xmax;
	G.out_ymax() = ymax;
	cout << "xmax/ymax = " << xmax << " / " << ymax << endl;
	
	G.header();
	G.begin_figure(factor_1000);
	
	draw_grid2(G, f_with_points, verbose_level);


	G.end_figure();
	G.footer();
	}
	cout << "written file " << fname_full << " of size " << file_size(fname_full) << endl;
	if (f_v) {
		cout << "draw_grid done" << endl;
		}
	
}


void elliptic_curve::draw_grid2(mp_graphics &G, INT f_with_points, INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	INT a, b;
	INT x1, x2, x3;

	INT rad = 10000;
	INT i, h;
	double x_stretch = 0.0010;
	double y_stretch = 0.0010;
	//double x_stretch = 0.01;
	//double y_stretch = 0.01;

	double *Dx, *Dy;
	INT *Px, *Py;
	INT dx = ONE_MILLION * 50 * x_stretch;
	INT dy = ONE_MILLION * 50 * y_stretch;
	INT N = 1000;


	Px = new INT[N];
	Py = new INT[N];
	Dx = new double[N];
	Dy = new double[N];

	
	if (f_v) {
		cout << "draw_grid2" << endl;
		}




	if (f_v) {
		cout << "drawing grid" << endl;
		}


	G.draw_axes_and_grid(
		0., (double)(q - 1), 0., (double)(q - 1), x_stretch, y_stretch, 
		TRUE /* f_x_axis_at_y_min */, TRUE /* f_y_axis_at_x_min */, 
		1 /* x_mod */, 1 /* y_mod */, 1, 1, 
		-2. /* x_labels_offset */, -2. /* y_labels_offset */, 
		0.5 /* x_tick_half_width */, 0.5 /* y_tick_half_width */, 
		TRUE /* f_v_lines */, 1 /* subdivide_v */, TRUE /* f_h_lines */, 1 /* subdivide_h */);



	if (f_with_points) {

		if (f_v) {
			cout << "drawing points, nb=" << nb << endl;
			}

		if (nb >= 40) {
			rad = 2000;
			}
		for (h = 0; h < nb; h++) {
			x1 = T[3 * h + 0];
			x2 = T[3 * h + 1];
			x3 = T[3 * h + 2];
			//get_ab(q, x1, x2, x3, a, b);
			make_affine_point(x1, x2, x3, a, b, 0);

			cout << "point " << h << " : " << x1 << ", " << x2 << ", " << x3 << " : " << a << ", " << b << endl;
			
			Dx[0] = a;
			Dy[0] = b;
			
			for (i = 0; i < 1; i++) {
				Px[i] = Dx[i] * dx;
				Py[i] = Dy[i] * dy;
				}

			//G.nice_circle(Px[a * Q + b], Py[a * Q + b], rad);
			G.nice_circle(Px[0], Py[0], rad);
			}

#if 0

		if (nb < 30) {
			if (f_v) {
				cout << "drawing point labels" << endl;
				}
			// drawing point labels:
			for (i = 0; i < nb; i++) {
				BYTE str[1000];
				sprintf(str, "%ld", i);
				x1 = T[3 * i + 0];
				x2 = T[3 * i + 1];
				x3 = T[3 * i + 2];
				get_ab(q, x1, x2, x3, a, b);
				G.aligned_text(Px[a * Q + b], Py[a * Q + b], "", str);
				}
			}
#endif

		}
	else {
		cout << "elliptic_curve::draw_grid2 not drawing any points" << endl;
		}



#if 0
	if (start_idx < 0) {
		goto done;
		}
	G.sl_ends(0 /* line_beg_style */, 1 /* line_end_style*/);
	
	INT ord;

	if (f_v) {
		cout << "drawing multiples of point" << endl;
		}
	i = start_idx;
	ord = 1;
	while (TRUE) {
		x1 = E->T[3 * i + 0];
		x2 = E->T[3 * i + 1];
		x3 = E->T[3 * i + 2];
		j = E->A[i * E->nb + start_idx];
		ord++;
		y1 = E->T[3 * j + 0];
		y2 = E->T[3 * j + 1];
		y3 = E->T[3 * j + 2];
		get_ab(q, x1, x2, x3, a, b);
		get_ab(q, y1, y2, y3, c, d);
		G.polygon2(Px, Py, a * Q + b, c * Q + d);
		if (j == E->nb - 1) {
			cout << "point P_" << start_idx << " has order " << ord << endl;
			break;
			}
		i = j;
		}
#endif



#if 0
	G.sl_udsty(100);
	a = Xcoord(-1);
	b = Ycoord(-1);
	c = Xcoord(q + 1);
	d = Ycoord(q + 1);
	cout << a << "," << b << "," << c << "," << d << endl;
	G.polygon2(Px, Py, a * Q + b, c * Q + d);
	a = Xcoord(q + 1);
	b = Ycoord(-1);
	c = Xcoord(-1);
	d = Ycoord(q + 1);
	cout << a << "," << b << "," << c << "," << d << endl;
	G.polygon2(Px, Py, a * Q + b, c * Q + d);


	q2 = q >> 1;
	if (ODD(q))
		r = 1;
	else 
		r = 0;
	
	a = Xcoord(q2) + r;
	b = Ycoord(-1);
	c = Xcoord(q2) + r;
	d = Ycoord(q + 1);
	cout << a << "," << b << "," << c << "," << d << endl;
	G.polygon2(Px, Py, a * Q + b, c * Q + d);

	a = Xcoord(-1);
	b = Ycoord(q2) + r;
	c = Xcoord(q + 1);
	d = Ycoord(q2) + r;
	cout << a << "," << b << "," << c << "," << d << endl;
	G.polygon2(Px, Py, a * Q + b, c * Q + d);
#endif


	delete [] Px;
	delete [] Py;
	delete [] Dx;
	delete [] Dy;



	if (f_v) {
		cout << "draw_grid2 done" << endl;
		}
}

void elliptic_curve::make_affine_point(INT x1, INT x2, INT x3, INT &a, INT &b, INT verbose_level)
{
	if (x3 == 0) {
		a = q >> 1;
		b = q;
		}
	else {
		a = x1;
		b = x2;
		}
}



#if 0
INT multiple_of_point(elliptic_curve &E, INT i, INT n)
{
	INT j, a;

	a = E.nb - 1;
	for (j = 0; j < n; j++) {
		a = E.A[a * E.nb + i];
		}
	return a;
}
#endif

void elliptic_curve::compute_addition_table(INT verbose_level)
{
	INT f_v = (verbose_level >= 1);
	//INT f_v3 = (verbose_level >= 3);
	INT i, j, k;
	INT x1, x2, x3;
	INT y1, y2, y3;
	INT z1, z2, z3;

	if (f_v) {
		cout << "elliptic_curve::compute_addition_table" << endl;
		}
	
	A = new INT[nb * nb];
	for (i = 0; i < nb; i++) {
		x1 = T[3 * i + 0];
		x2 = T[3 * i + 1];
		x3 = T[3 * i + 2];
		for (j = 0; j < nb; j++) {
			y1 = T[3 * j + 0];
			y2 = T[3 * j + 1];
			y3 = T[3 * j + 2];
			if (FALSE) {
				cout << "add " << i << " " << j << endl;
				}
			addition(
				x1, x2, x3, 
				y1, y2, y3,
				z1, z2, z3, 0 /*verbose_level - 1*/);

			
			k = index_of_point(z1, z2, z3);
			A[i * nb + j] = k;
			}
		}
	if (f_v) {
		cout << "elliptic_curve::compute_addition_table done" << endl;
		}
}

void elliptic_curve::print_addition_table()
{
	INT_matrix_print(A, nb, nb);
	INT_matrix_print_tex(cout, A, nb, nb);
}

#if 0
INT elliptic_curve::index_of_point(INT x1, INT x2, INT x3)
{
	INT a, i;
	
	if (x3 == 0) {
		return 0;
		}
	if (x3 != 1) {
		a = F->inverse(x3);
		x1 = F->mult(x1, a);
		x2 = F->mult(x2, a);
		x3 = 1;
		}
	for (i = 1; i < nb; i++) {
		if (T[3 * i + 0] == x1 && T[3 * i + 1] == x2) {
			return i;
			}
		}
	cout << "did not find point " << x1 << "," << x2 << "," << x3 << " in table" << endl;
	exit(1);
}
#endif

INT elliptic_curve::index_of_point(INT x1, INT x2, INT x3)
//INT INT_vec_search(INT *v, INT len, INT a, INT &idx)
// This function finds the last occurence of the element a.
// If a is not found, it returns in idx the position where it should be inserted if 
// the vector is assumed to be in increasing order.

{
	INT l, r, m, res, a;
	INT f_found = FALSE;
	INT f_v = FALSE;
	
	if (nb == 0) {
		cout << "elliptic_curve::index_of_point nb == 0" << endl;
		exit(1);
		}

	if (x3 == 0) {
		return 0;
		}
	if (x3 != 1) {
		a = F->inverse(x3);
		x1 = F->mult(x1, a);
		x2 = F->mult(x2, a);
		x3 = 1;
		}

	l = 1;
	r = nb;
	// invariant:
	// v[i] <= a for i < l;
	// v[i] >  a for i >= r;
	// r - l is the length of the area to search in.
	while (l < r) {
		m = (l + r) >> 1;
		// if the length of the search area is even
		// we examine the element above the middle
		if (T[3 * m + 0] > x1) {
			res = 1;
			}
		else if (T[3 * m + 0] < x1) {
			res = -1;
			}
		else {
			if (T[3 * m + 1] > x2) {
				res = 1;
				}
			else if (T[3 * m + 1] < x2) {
				res = -1;
				}
			else {
				res = 0;
				}
			}
		//res = v[m] - a;
		if (f_v) {
			cout << "l=" << l << " r=" << r<< " m=" << m << " T[3 * m + 0]=" << T[3 * m + 0] << " T[3 * m + 1]=" << T[3 * m + 1] << " res=" << res << endl;
			}
		//cout << "search l=" << l << " m=" << m << " r=" 
		//	<< r << "a=" << a << " v[m]=" << v[m] << " res=" << res << endl;
		// so, res is 
		// positive if v[m] > a,
		// zero if v[m] == a,
		// negative if v[m] < a
		if (res <= 0) {
			l = m + 1;
			if (f_v) {
				cout << "elliptic_curve::index_of_point moving to the right" << endl;
				}
			if (res == 0) {
				f_found = TRUE;
				}
			}
		else {
			if (f_v) {
				cout << "elliptic_curve::index_of_point moving to the left" << endl;
				}
			r = m;
			}
		}
	// now: l == r; 
	// and f_found is set accordingly */
	if (!f_found) {
		cout << "elliptic_curve::index_of_point did not find point" << endl;
		cout << "x1=" << x1 << " x2=" << x2 << " x3=" << x3 << endl;
		exit(1);
		}
#if 1
	if (f_found) {
		l--;
		}
#endif
	return l;
}

INT elliptic_curve::order_of_point(INT i)
{
	INT j;
	INT ord;

	j = i;
	ord = 1;
	while (j != 0) {
		j = A[i * nb + j];
		ord++;
		}
	return ord;
}

void elliptic_curve::print_all_powers(INT i)
{
	INT j;
	INT ord;

	j = i;
	ord = 1;
	while (j != 0) {
		cout << ord << " & " << j << " & (" << T[3 * j + 0] << ", " << T[3 * j + 1] << ", " << T[3 * j + 2] << ")\\\\" << endl;
		j = A[i * nb + j];
		ord++;
		}
}



