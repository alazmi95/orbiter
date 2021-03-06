// treedraw.C
//
// Anton Betten
// January 12, 2004

#include "orbiter.h"


void draw_vertex_callback(tree *T, mp_graphics *G, INT *v, INT layer, tree_node *N, INT x, INT y, INT dx, INT dy);
void draw_vertex_callback_placeholders(tree *T, mp_graphics *G, INT *v, INT layer, tree_node *N, INT x, INT y, INT dx, INT dy);
void draw_vertex_callback_standard(tree *T, mp_graphics *G, INT *v, INT layer, tree_node *N, INT x, INT y, INT dx, INT dy);
void draw_vertex_callback_graph(tree *T, mp_graphics *G, INT *v, INT layer, tree_node *N, INT x, INT y, INT dx, INT dy);


	INT f_my_type = FALSE;
	const BYTE *the_type = NULL;
	INT graph_nb_V = 0;
	INT f_graph_perm = FALSE;
	INT graph_perm[1000];
	INT f_multiple_circles = FALSE;
	INT nb_circles = 1;

int main(int argc, const char **argv)
{
	INT t0 = os_ticks();
	INT verbose_level = 0;
	INT xmax = 1000000;
	INT ymax = 1000000;
	INT xmax_out = 1000000;
	INT ymax_out = 1000000;
	INT f_i = FALSE;
	INT f_e = FALSE;
	INT i;
	BYTE fname_base[1000];
	BYTE fname_out[1000];
	BYTE ext[1000];
	INT f_no_circletext = FALSE;
	INT f_circle = FALSE;
	INT f_file = FALSE;
	const BYTE *fname = NULL;
	INT f_on_circle = FALSE;
	INT f_graph = FALSE;
	INT f_rad = FALSE;
	INT rad = 3000;
	INT f_embedded = FALSE;
	INT f_sideways = FALSE;
	INT f_scale = FALSE;
	double scale = .45;
	INT f_line_width = FALSE;
	double line_width = 1.5;
	INT f_placeholder_labels = FALSE;
	INT f_circletext = FALSE;
	INT f_count_leaves = FALSE;

	cout << argv[0] << endl;
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[i + 1]);
			cout << "-v " << verbose_level <<endl;
			}
		else if (strcmp(argv[i], "-x") == 0) {
			xmax = atoi(argv[i + 1]);
			i++;
			cout << "-x " << xmax << endl;
			}
		else if (strcmp(argv[i], "-y") == 0) {
			ymax = atoi(argv[i + 1]);
			i++;
			cout << "-y " << ymax << endl;
			}
		else if (strcmp(argv[i], "-x_out") == 0) {
			xmax_out = atoi(argv[i + 1]);
			i++;
			cout << "-x_out " << xmax_out << endl;
			}
		else if (strcmp(argv[i], "-y_out") == 0) {
			ymax_out = atoi(argv[i + 1]);
			i++;
			cout << "-y_out " << ymax_out << endl;
			}
		else if (strcmp(argv[i], "-i") == 0) {
			f_i = TRUE;
			cout << "-i " << endl;
			}
		else if (strcmp(argv[i], "-e") == 0) {
			f_e = TRUE;
			cout << "-e " << endl;
			}
		else if (strcmp(argv[i], "-circletext") == 0) {
			f_circletext = TRUE;
			cout << "-circletext " << endl;
			}
		else if (strcmp(argv[i], "-no_circletext") == 0) {
			f_no_circletext = TRUE;
			cout << "-no_circletext " << endl;
			}
		else if (strcmp(argv[i], "-circle") == 0) {
			f_circle = TRUE;
			cout << "-circle " << endl;
			}
		else if (strcmp(argv[i], "-on_circle") == 0) {
			f_on_circle = TRUE;
			cout << "-on_circle " << endl;
			}
		else if (strcmp(argv[i], "-file") == 0) {
			f_file = TRUE;
			fname = argv[++i];
			cout << "-file " << fname << endl;
			}
		else if (strcmp(argv[i], "-graph") == 0) {
			f_graph = TRUE;
			graph_nb_V = atoi(argv[++i]);
			f_my_type = TRUE;
			the_type = "as_graph";
			cout << "-graph " << graph_nb_V << endl;
			}
		else if (strcmp(argv[i], "-graph_perm") == 0) {
			f_graph_perm = TRUE;
			graph_nb_V = atoi(argv[++i]);
			INT j;
			for (j = 0; j < graph_nb_V; j++) {
				graph_perm[j] = atoi(argv[++i]);
				}
			cout << "-graph_perm " << graph_nb_V << " ";
			INT_vec_print(cout, graph_perm, graph_nb_V);
			cout << endl;
			}
		else if (strcmp(argv[i], "-rad") == 0) {
			f_rad = TRUE;
			rad = atoi(argv[++i]);
			cout << "-rad " << rad << endl;
			}
		else if (strcmp(argv[i], "-multiple_circles") == 0) {
			f_multiple_circles = TRUE;
			nb_circles = atoi(argv[++i]);
			cout << "-multiple_circles " << nb_circles << endl;
			}
		else if (strcmp(argv[i], "-embedded") == 0) {
			f_embedded = TRUE;
			cout << "-embedded" << endl;
			}
		else if (strcmp(argv[i], "-sideways") == 0) {
			f_sideways = TRUE;
			cout << "-sideways" << endl;
			}
		else if (strcmp(argv[i], "-scale") == 0) {
			f_scale = TRUE;
			sscanf(argv[++i], "%lf", &scale);
			cout << "-scale " << scale << endl;
			}
		else if (strcmp(argv[i], "-line_width") == 0) {
			f_line_width = TRUE;
			sscanf(argv[++i], "%lf", &line_width);
			cout << "-line_width " << line_width << endl;
			}
		else if (strcmp(argv[i], "-placeholder_labels") == 0) {
			f_placeholder_labels = TRUE;
			cout << "-placeholder_labels " << endl;
			}
		else if (strcmp(argv[i], "-count_leaves") == 0) {
			f_count_leaves = TRUE;
			cout << "-count_leaves " << endl;
			}
		}

	if (!f_file) {
		cout << "please use option -file <fname>" << endl;
		exit(1);
		}
	strcpy(fname_base, fname);
	get_extension_if_present(fname_base, ext);
	chop_off_extension_if_present(fname_base, ext);
	sprintf(fname_out, "%s", fname_base);
		
	tree T;

	cout << "Trying to read file " << fname << " of size " << file_size(fname) << endl;

	if (file_size(fname) <= 0) {
		cout << "treedraw.out the input file " << fname << " does not exist" << endl;
		exit(1);
		}
	T.init(fname, xmax, ymax, verbose_level);
	
	if (/* T.nb_nodes > 200 ||*/ f_no_circletext) {
		f_circletext = FALSE;
		}
	if (f_on_circle) {
		T.root->place_on_circle(xmax, ymax, T.max_depth);
		}

	if (f_count_leaves) {
		T.f_count_leaves = TRUE;
		}

	if (f_graph) {
		cout << "treedraw.out drawing as graph" << endl;
		T.draw(fname_out, xmax, ymax, xmax_out, ymax_out, rad, f_circle, f_circletext, 
			f_i, f_e, TRUE, draw_vertex_callback, f_embedded, f_sideways, f_on_circle, 
			scale, line_width);
		}
	else if (f_placeholder_labels) {
		T.draw(fname_out, xmax, ymax, xmax_out, ymax_out, rad, f_circle, f_circletext, 
			f_i, f_e, TRUE, draw_vertex_callback_placeholders, f_embedded, f_sideways, f_on_circle, 
			scale, line_width);
		}
	else {
		T.draw(fname_out, xmax, ymax, xmax_out, ymax_out, rad, f_circle, f_circletext, 
			f_i, f_e, FALSE, NULL, f_embedded, f_sideways, f_on_circle, 
			scale, line_width);
		}
	
	time_check(cout, t0);
	cout << endl;
}


void draw_vertex_callback(tree *T, mp_graphics *G, INT *v, INT layer, tree_node *N, INT x, INT y, INT dx, INT dy)
{

	cout << "draw_vertex_callback" << endl;
	if (f_my_type) {
		if (strcmp(the_type, "as_graph") == 0) {
			draw_vertex_callback_graph(T, G, v, layer, N, x, y, dx, dy);
			}
		}
	else {
		draw_vertex_callback_standard(T, G, v, layer, N, x, y, dx, dy);
		}
	cout << "draw_vertex_callback done" << endl;
}

void draw_vertex_callback_placeholders(tree *T, mp_graphics *G, INT *v, INT layer, tree_node *N, INT x, INT y, INT dx, INT dy)
{
	INT rk, r, l, d, i, *digits;
	BYTE str[1000];

	cout << "draw_vertex_callback_placeholders" << endl;

	rk = N->DFS_rank + 1;
	
	sprintf(str, "\\mylabel");

	r = rk;
	l = 0;
	while (r) {
		d = r % 10;
		r = r / 10;
		l++;
		}
	digits = NEW_INT(l);
	r = rk;
	l = 0;
	while (r) {
		d = r % 10;
		digits[l] = d;
		r = r / 10;
		l++;
		}
	for (i = 0; i < l; i++) {
		sprintf(str + strlen(str), "%c", (char) ('a' + digits[l - 1 - i]));
		}
	FREE_INT(digits);
	G->aligned_text(x, y, "", str);
	cout << "draw_vertex_callback_placeholders done" << endl;
}

void draw_vertex_callback_standard(tree *T, mp_graphics *G, INT *v, INT layer, tree_node *N, INT x, INT y, INT dx, INT dy)
{
	//INT d1;
	BYTE str[1000];

	cout << "draw_vertex_callback_standard x=" << x << " y=" << y << " dx = " << dx << " dy=" << dy << endl;
	//d1 = LG->L[layer].Nodes[node].data1;
	//nb_V = LG->data1;
	
	//sprintf(str, "%ld", N->value);
	sprintf(str, "%ld", N->int_data);

	G->aligned_text(x, y, "", str);
}

void draw_vertex_callback_graph(tree *T, mp_graphics *G, INT *v, INT layer, tree_node *N, INT x, INT y, INT dx, INT dy)
{
	//INT d1;
	//BYTE str[1000];

	cout << "draw_vertex_callback_graph x=" << x << " y=" << y << " dx = " << dx << " dy=" << dy << endl;
	//d1 = LG->L[layer].Nodes[node].data1;
	//nb_V = LG->data1;
	
	//sprintf(str, "%ld", N->value);

	//G->aligned_text(x, y, "", str);

	if (f_graph_perm) {
		INT i, a, b, x, y, x1, y1;
		for (i = 0; i < layer; i++) {
			a = v[i];
			k2ij(a, x, y, graph_nb_V);
			x1 = graph_perm[x];
			y1 = graph_perm[y];
			b = ij2k(x1, y1, graph_nb_V);
			v[i] = b;
			}
		}


	if (f_multiple_circles) {
		draw_graph_on_multiple_circles(G, x, y, dx, dy, graph_nb_V, v, layer, nb_circles);
			// in GALOIS/draw.C
		}
	else {
		draw_graph(G, x, y, dx, dy, graph_nb_V, v, layer);
			// in GALOIS/draw.C
		}

}



