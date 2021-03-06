// nauty.C
// 
// Anton Betten
// Jul 16, 2016

#include "orbiter.h"

void canonical_form(INT *Adj, INT *Adj2, INT n, INT nb_edges, INT *edges2, 
	INT *labeling, action *&A, action *&A2, schreier *&Sch, INT verbose_level);
void make_graph_fname(BYTE *fname_full, BYTE *fname_full_tex, INT n, INT *set, INT sz);
void draw_graph_to_file(const BYTE *fname, INT n, INT *set, INT sz, double scale, INT f_embedded, INT f_sideways);

int main(int argc, char **argv)
{
	INT verbose_level = 0;
	INT i;
	INT f_n = FALSE;
	INT n;
	INT f_edges = FALSE;
	INT edges[1000];
	INT nb_edges = 0;
	INT f_all = FALSE;
	INT j, h;
	INT f_scale = FALSE;
	double scale = 0.04;
	INT f_embedded = FALSE;
	INT f_sideways = FALSE;

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
		else if (strcmp(argv[i], "-all") == 0) {
			f_all = TRUE;
			cout << "-all" << endl;
			}
		else if (strcmp(argv[i], "-scale") == 0) {
			f_scale = TRUE;
			sscanf(argv[++i], "%lf", &scale);
			cout << "-scale " << scale << endl;
			}
		else if (strcmp(argv[i], "-edges") == 0) {
			f_edges = TRUE;
			while (TRUE) {
				edges[nb_edges] = atoi(argv[++i]);
				if (edges[nb_edges] == -1) {
					break;
					}
				nb_edges++;
				}
			cout << "-edges ";
			INT_vec_print(cout, edges, nb_edges);
			cout << endl;
			}
		}

	if (!f_n) {
		cout << "Please use option -n <n>" << endl;
		exit(1);
		}
	INT *Adj;
	INT *Adj2;
	INT *edges2;
	INT *labeling;
	INT e, n2;

	n2 = (n * (n - 1)) >> 1;

	Adj = NEW_INT(n * n);
	Adj2 = NEW_INT(n * n);
	edges2 = NEW_INT(n2);
	labeling = NEW_INT(n);


	if (f_edges) {
		INT_vec_zero(Adj, n * n);
		INT_vec_zero(Adj2, n * n);
		for (h = 0; h < nb_edges; h++) {
			e = edges[h];
			k2ij(e, i, j, n);
			Adj[i * n + j] = 1;
			Adj[j * n + i] = 1;
			}
		action *A;
		action *A2;
		schreier *Sch;

		canonical_form(Adj, Adj2, n, nb_edges, edges2, labeling, A, A2, Sch, verbose_level);

		cout << "canonical form: " << endl;
		INT_vec_print(cout, edges2, nb_edges);
		cout << endl;

		delete Sch;
		delete A2;
		delete A;
		}

	if (f_all) {
		INT N, E;
		INT *set;
		INT sz;
		BYTE fname1_tex[1000];
		BYTE fname2_tex[1000];
		BYTE fname1[1000];
		BYTE fname2[1000];
		const BYTE *fname = "table_of_graphs.tex";

		{
		ofstream fp(fname);

		set = NEW_INT(n2);
		N = i_power_j(2, n2);
		fp << "\\begin{tabular}{|c|l|c|c|l|c|l|}" << endl;
		fp << "\\hline" << endl;
		for (E = 0; E < N; E++) {
			//cout << "graph " << E << " / " << N << " : ";
			fp << E << " & ";
			INT_vec_zero(Adj, n * n);
			INT_vec_zero(Adj2, n * n);
			unrank_subset(set, sz, n2, E);
			INT_set_print_tex(fp, set, sz);

			make_graph_fname(fname1, fname1_tex, n, set, sz);
			draw_graph_to_file(fname1, n, set, sz, scale, f_embedded, f_sideways);
			
			fp << " & \\mbox{ \\input GRAPHICS/G4/" << fname1_tex << " } ";
			for (h = 0; h < sz; h++) {
				e = set[h];
				k2ij(e, i, j, n);
				Adj[i * n + j] = 1;
				Adj[j * n + i] = 1;
				}


			action *A;
			action *A2;
			schreier *Sch;
			INT f, l, a;


			canonical_form(Adj, Adj2, n, sz, edges2, labeling, A, A2, Sch, verbose_level);
			fp << " & ";
			fp << "[";
			for (h = 0; h < n; h++) {
				fp << labeling[h];
				if (h < n - 1) {
					fp << ",";
					}
				}
			fp << "]";
			fp << " & ";
			INT_set_print_tex(fp, edges2, sz);
			make_graph_fname(fname2, fname2_tex, n, edges2, sz);
			draw_graph_to_file(fname2, n, edges2, sz, scale, f_embedded, f_sideways);
			fp << " & \\mbox{ \\input GRAPHICS/G4/" << fname2_tex << " } ";
			fp << " & $";
			for (h = 0; h < Sch->nb_orbits; h++) {
				f = Sch->orbit_first[h];
				l = Sch->orbit_len[h];
				for (j = 0; j < l; j++) {
					a = Sch->orbit[f + j];
					fp << a;
					if (j < l - 1) {
						fp << ", ";
						}
					else if (h < Sch->nb_orbits - 1) {
						fp << " | ";
						}
					}
				}
			fp << "$ ";
			fp << "\\\\" << endl;


			delete Sch;
			delete A2;
			delete A;
			}
		fp << "\\hline" << endl;
		fp << "\\end{tabular}" << endl;

		}
		cout << "written file " << fname << " of size " << file_size(fname) << endl;

		FREE_INT(set);
		}

	FREE_INT(Adj);
	FREE_INT(Adj2);
	FREE_INT(edges2);
	FREE_INT(labeling);
}

void canonical_form(INT *Adj, INT *Adj2, INT n, INT nb_edges, INT *edges2, 
	INT *labeling, action *&A, action *&A2, schreier *&Sch, INT verbose_level)
{
	//action *A;
	INT i, j, ii, jj, e, nb_e;
	

	A = create_automorphism_group_and_canonical_labeling_of_graph(Adj, n, labeling, verbose_level);
	for (i = 0; i < n; i++) {
		ii = labeling[i];
		for (j = 0; j < n; j++) {
			jj = labeling[j];
			Adj2[i * n + j] = Adj[ii * n + jj];
			}
		}
	nb_e = 0;
	for (i = 0; i < n; i++) {
		for (j = i + 1; j < n; j++) {
			if (Adj2[i * n + j]) {
				e = ij2k(i, j, n);
				edges2[nb_e++] = e;
				}
			}
		}
	if (nb_e != nb_edges) {
		cout << "nb_e != nb_edges, something is wrong" << endl;
		exit(1);
		}

	delete A;

	// and now we compute the automorphism group of the canonical graph:
	A = create_automorphism_group_of_graph(Adj2, n, verbose_level);
	
	cout << "generators for the automorphism group:" << endl;
	A->Strong_gens->print_generators();
	//action *A2;

	A2 = new action;
	A2->induced_action_on_k_subsets(*A, 2, verbose_level);

	//schreier *S;

	cout << "computing orbits on edges:" << endl;
	Sch = new schreier;
	A2->compute_all_point_orbits(*Sch, 
		*A->Strong_gens->gens, verbose_level);
	cout << "found " << Sch->nb_orbits << " orbits on edges" << endl;

	//delete A2;
	//delete A;
}


void make_graph_fname(BYTE *fname_full, BYTE *fname_full_tex, INT n, INT *set, INT sz)
{
	INT i;
	
	sprintf(fname_full, "graph_%ld", n);
	for (i = 0; i < sz; i++) {
		sprintf(fname_full + strlen(fname_full), "_%ld", set[i]);
		}
	strcpy(fname_full_tex, fname_full);
	sprintf(fname_full + strlen(fname_full), ".mp");
	sprintf(fname_full_tex + strlen(fname_full_tex), ".tex");
}

void draw_graph_to_file(const BYTE *fname, INT n, INT *set, INT sz, double scale, INT f_embedded, INT f_sideways)
{
	INT x_min = 0, x_max = 1000000;
	INT y_min = 0, y_max = 1000000;
	INT x, y, dx, dy;

	x = (x_max - x_min) >> 1;
	y = (y_max - y_min) >> 1;
	dx = x;
	dy = y;
	{
	mp_graphics G(fname, x_min, y_min, x_max, y_max, f_embedded, f_sideways);
	G.out_xmin() = 0;
	G.out_ymin() = 0;
	G.out_xmax() = 1000000;
	G.out_ymax() = 1000000;
	//cout << "xmax/ymax = " << xmax << " / " << ymax << endl;

	G.set_scale(scale);
	G.header();
	G.begin_figure(1000 /*factor_1000*/);

	G.sl_thickness(10); // 100 is normal
	//G.frame(0.05);


	draw_graph(&G, x, y, dx, dy, n, set, sz);
	
	G.end_figure();
	G.footer();
	}
	cout << "written file " << fname << " of size " << file_size(fname) << endl;
}


