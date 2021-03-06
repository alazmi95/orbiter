// layered_graph_main.C
// 
// Anton Betten
// January 8, 2014
//
//
// 
//
//

#include "orbiter.h"
#include "discreta.h"


// global data:

INT t0; // the system time when the program started

void draw_vertex_callback(layered_graph *LG, mp_graphics *G, INT layer, INT node, INT x, INT y, INT dx, INT dy);
void draw_vertex_callback_standard(layered_graph *LG, mp_graphics *G, INT layer, INT node, INT x, INT y, INT dx, INT dy);
void draw_vertex_callback_placeholders(layered_graph *LG, mp_graphics *G, INT layer, INT node, INT x, INT y, INT dx, INT dy);
void draw_vertex_callback_graph(layered_graph *LG, mp_graphics *G, INT layer, INT node, INT x, INT y, INT dx, INT dy);
void draw_vertex_callback_tournament(layered_graph *LG, mp_graphics *G, INT layer, INT node, INT x, INT y, INT dx, INT dy);
INT get_depth(layered_graph *LG, INT layer, INT node);
INT get_data(layered_graph *LG, INT layer, INT node, INT *Data, INT cur_depth);
void draw_begining_callback(layered_graph *LG, mp_graphics *G, INT x_max, INT y_max, INT f_rotated, INT dx, INT dy);
void draw_ending_callback(layered_graph *LG, mp_graphics *G, INT x_max, INT y_max, INT f_rotated, INT dx, INT dy);

#define MAX_FILES 1000

	INT f_my_type = FALSE;
	const BYTE *the_type = NULL;
	INT f_boxed = FALSE;
	INT boxed_group_size = 1;
	INT f_text_underneath = FALSE;
	INT x_max = 10000;
	INT y_max = 10000;
	INT f_data1 = FALSE;
	INT f_placeholder_labels = FALSE;
	INT f_select_layer = FALSE;
	INT nb_select_layer = 0;
	INT select_layer[1000];
	INT f_nodes_empty = FALSE;
	INT f_scriptsize = FALSE;
	INT f_numbering_on = FALSE;
	double numbering_on_scale = 1.0;

int main(int argc, const char **argv)
{
	INT i;
	INT verbose_level = 0;
	INT f_file = FALSE;
	const BYTE *fname;
	INT f_draw = FALSE;
	INT f_spanning_tree = FALSE;
	INT xmax = 1000000;
	INT ymax = 1000000;
	INT f_circle = TRUE;
	INT f_corners = FALSE;
	INT rad = 50;
	INT f_embedded = FALSE;
	INT f_sideways = FALSE;
	INT f_show_level_info = FALSE;
	INT f_label_edges = FALSE;
	INT f_y_stretch = FALSE;
	double y_stretch = 1.;
	INT f_x_stretch = FALSE;
	double x_stretch = 1.;
	INT f_scale = FALSE;
	double scale = .45;
	INT f_line_width = FALSE;
	double line_width = 1.5;
	INT f_rotated = FALSE;

	t0 = os_ticks();
	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-v") == 0) {
			verbose_level = atoi(argv[++i]);
			cout << "-v " << verbose_level << endl;
			}
		else if (strcmp(argv[i], "-file") == 0) {
			f_file = TRUE;
			fname = argv[++i];
			cout << "-file " << fname << endl;
			}
		else if (strcmp(argv[i], "-draw") == 0) {
			f_draw = TRUE;
			//draw_fname = argv[++i];
			cout << "-draw " << endl;
			}
		else if (strcmp(argv[i], "-spanning_tree") == 0) {
			f_spanning_tree = TRUE;
			cout << "-spanning_tree " << endl;
			}
		else if (strcmp(argv[i], "-rad") == 0) {
			rad = atoi(argv[++i]);
			cout << "-rad " << rad << endl;
			}
		else if (strcmp(argv[i], "-xin") == 0) {
			x_max = atoi(argv[++i]);
			cout << "-xin " << x_max << endl;
			}
		else if (strcmp(argv[i], "-yin") == 0) {
			y_max = atoi(argv[++i]);
			cout << "-yin " << y_max << endl;
			}
		else if (strcmp(argv[i], "-xout") == 0) {
			xmax = atoi(argv[++i]);
			cout << "-xout " << xmax << endl;
			}
		else if (strcmp(argv[i], "-yout") == 0) {
			ymax = atoi(argv[++i]);
			cout << "-yout " << ymax << endl;
			}
		else if (strcmp(argv[i], "-corners") == 0) {
			f_corners = TRUE;
			cout << "-corners " << endl;
			}
		else if (strcmp(argv[i], "-as_graph") == 0) {
			f_my_type = TRUE;
			the_type = "as_graph";
			cout << "-as_graph " << endl;
			}
		else if (strcmp(argv[i], "-as_tournament") == 0) {
			f_my_type = TRUE;
			the_type = "as_tournament";
			cout << "-as_tournament " << endl;
			}
		else if (strcmp(argv[i], "-select_layer") == 0) {
			f_select_layer = TRUE;
			select_layer[nb_select_layer] = atoi(argv[++i]);
			cout << "-select_layer " << select_layer[nb_select_layer] << endl;
			nb_select_layer++;
			}
		else if (strcmp(argv[i], "-nodes_empty") == 0) {
			f_nodes_empty = TRUE;
			cout << "-nodes_empty " << endl;
			}
		else if (strcmp(argv[i], "-embedded") == 0) {
			f_embedded = TRUE;
			cout << "-embedded " << endl;
			}
		else if (strcmp(argv[i], "-sideways") == 0) {
			f_sideways = TRUE;
			cout << "-sideways " << endl;
			}
		else if (strcmp(argv[i], "-show_level_info") == 0) {
			f_show_level_info = TRUE;
			cout << "-show_level_info " << endl;
			}
		else if (strcmp(argv[i], "-label_edges") == 0) {
			f_label_edges = TRUE;
			cout << "-label_edges " << endl;
			}
		else if (strcmp(argv[i], "-y_stretch") == 0) {
			f_y_stretch = TRUE;
			sscanf(argv[++i], "%lf", &y_stretch);
			cout << "-y_stretch " << endl;
			}
		else if (strcmp(argv[i], "-x_stretch") == 0) {
			f_x_stretch = TRUE;
			sscanf(argv[++i], "%lf", &x_stretch);
			cout << "-x_stretch " << endl;
			}
		else if (strcmp(argv[i], "-boxed") == 0) {
			f_boxed = TRUE;
			boxed_group_size = atoi(argv[++i]);
			cout << "-boxed " << boxed_group_size << endl;
			}
		else if (strcmp(argv[i], "-data1") == 0) {
			f_data1 = TRUE;
			cout << "-data1 " << endl;
			}
		else if (strcmp(argv[i], "-text_underneath") == 0) {
			f_text_underneath = TRUE;
			cout << "-text_underneath " << endl;
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
		else if (strcmp(argv[i], "-rotated") == 0) {
			f_rotated = TRUE;
			cout << "-rotated " << endl;
			}
		else if (strcmp(argv[i], "-scriptsize") == 0) {
			f_scriptsize = TRUE;
			cout << "-scriptsize " << endl;
			}
		else if (strcmp(argv[i], "-numbering_on") == 0) {
			f_numbering_on = TRUE;
			sscanf(argv[++i], "%lf", &numbering_on_scale);
			cout << "-numbering_on " << endl;
			}
		}
	if (!f_file) {
		cout << "Please use option -file <fname>" << endl;
		exit(1);
		}

	layered_graph *LG;

	LG = new layered_graph;
	if (file_size(fname) <= 0) {
		cout << "file " << fname << " does not exist" << endl;
		exit(1);
		}
	LG->read_file(fname, verbose_level - 1);

	cout << "Layered graph read from file" << endl;

	INT data1;

	
	data1 = LG->data1;

	cout << "data1=" << data1 << endl;
	
	if (f_y_stretch) {
		LG->place_with_y_stretch(y_stretch, verbose_level - 1);
		}
	if (f_spanning_tree) {
		// create new x coordinates
		LG->create_spanning_tree(TRUE /* f_place_x */, verbose_level);
		}
	if (f_numbering_on) {
		// create depth first ranks at each node:
		LG->create_spanning_tree(FALSE /* f_place_x */, verbose_level);
		}
	
	layered_graph_draw_options O;

	O.init(xmax, ymax, x_max, y_max, rad, f_circle, f_corners, f_nodes_empty, 
		f_select_layer, nb_select_layer, select_layer, 
		TRUE, draw_begining_callback, 
		TRUE, draw_ending_callback, 
		TRUE, draw_vertex_callback, 
		f_show_level_info, f_embedded, f_sideways, f_label_edges, 
		f_rotated, 
		scale, line_width);
	
	BYTE fname_out[1000];

	strcpy(fname_out, fname);
	chop_off_extension_if_present(fname_out, ".layered_graph");
	strcat(fname_out, "_draw");
	//replace_extension_with(fname_out, const char *new_ext);

	LG->draw_with_options(fname_out, &O, 0 /* verbose_level */);
	



	delete LG;


}

void draw_vertex_callback(layered_graph *LG, mp_graphics *G, INT layer, INT node, INT x, INT y, INT dx, INT dy)
{
	cout << "draw_vertex_callback node " << node << endl;
	if (f_my_type) {
		if (strcmp(the_type, "as_graph") == 0) {
			draw_vertex_callback_graph(LG, G, layer, node, x, y, dx, dy);
			}
		if (strcmp(the_type, "as_tournament") == 0) {
			draw_vertex_callback_tournament(LG, G, layer, node, x, y, dx, dy);
			}
		}
	else if (f_placeholder_labels) {
		draw_vertex_callback_placeholders(LG, G, layer, node, x, y, dx, dy);
		}
	else {
		draw_vertex_callback_standard(LG, G, layer, node, x, y, dx, dy);
		}

	if (f_numbering_on) {
		BYTE str[1000];
		
		sprintf(str, "%ld", LG->L[layer].Nodes[node].depth_first_node_rank);
		G->aligned_text(x + (INT)(dx * numbering_on_scale), 
			y - (INT)(dy * numbering_on_scale), "", 
			str);
		}
}

void draw_vertex_callback_standard(layered_graph *LG, mp_graphics *G, INT layer, INT node, INT x, INT y, INT dx, INT dy)
{
	//INT d1;
	//BYTE str[1000];

	cout << "draw_vertex_callback_standard x=" << x << " y=" << y << " dx = " << dx << " dy=" << dy << endl;
	//d1 = LG->L[layer].Nodes[node].data1;
	//nb_V = LG->data1;
	//sprintf(str, "%ld", d1);
	BYTE str[1000000];
	BYTE str2[1000000];

	str[0] = 0;
	str2[0] = 0;

	if (f_data1) {
		if (LG->L[layer].Nodes[node].f_has_data1) {
			cout << "draw_vertex_callback_standard node " << node << " drawing data1" << endl;

			sprintf(str, "%ld", LG->L[layer].Nodes[node].data1);
			}
		}
	else {
		if (LG->L[layer].Nodes[node].f_has_vec_data) {
			cout << "has vector data" << endl;
			INT *D;
			INT len;

			D = LG->L[layer].Nodes[node].vec_data;
			len = LG->L[layer].Nodes[node].vec_data_len;
			if (len) {
				sprintf(str, "%ld", D[len - 1]);
				}
			}
		else {
			cout << "does not have vector data" << endl;
			strcpy(str, LG->L[layer].Nodes[node].label);
			}
		}
	if (f_scriptsize) {
		sprintf(str2, "{\\scriptsize %s}", str);
		}
	else {
		strcpy(str2, str);
		}
	if (LG->L[layer].Nodes[node].radius_factor < 0.95) {
		str2[0] = 0;
		}
	G->aligned_text(x, y, "", str2);
}

void draw_vertex_callback_placeholders(layered_graph *LG, mp_graphics *G, INT layer, INT node, INT x, INT y, INT dx, INT dy)
{
	//INT d1;
	//BYTE str[1000];

	cout << "draw_vertex_callback_placeholders x=" << x << " y=" << y << " dx = " << dx << " dy=" << dy << endl;
	//d1 = LG->L[layer].Nodes[node].data1;
	//nb_V = LG->data1;
	//sprintf(str, "%ld", d1);
	BYTE str[1000000];
	INT i, r, l, d, rk;
	INT *digits;

	str[0] = 0;

	rk = layer;
	
	sprintf(str, "\\mylabelX");

	r = rk;
	l = 1;
	while (TRUE) {
		d = r % 10;
		r = r / 10;
		if (r == 0) {
			break;
			}
		l++;
		}
	digits = NEW_INT(l);
	r = rk;
	l = 1;
	while (TRUE) {
		d = r % 10;
		digits[l - 1] = d;
		r = r / 10;
		if (r == 0) {
			break;
			}
		l++;
		}
	for (i = 0; i < l; i++) {
		sprintf(str + strlen(str), "%c", (char) ('a' + digits[l - 1 - i]));
		}
	FREE_INT(digits);


	rk = node;
	
	sprintf(str + strlen(str), "X");

	r = rk;
	l = 1;
	while (TRUE) {
		d = r % 10;
		r = r / 10;
		if (r == 0) {
			break;
			}
		l++;
		}
	digits = NEW_INT(l);
	r = rk;
	l = 1;
	while (TRUE) {
		d = r % 10;
		digits[l - 1] = d;
		r = r / 10;
		if (r == 0) {
			break;
			}
		l++;
		}
	for (i = 0; i < l; i++) {
		sprintf(str + strlen(str), "%c", (char) ('a' + digits[l - 1 - i]));
		}
	FREE_INT(digits);

	cout << "placeholder " << str << endl;
	G->aligned_text(x, y, "", str);
}

void draw_vertex_callback_graph(layered_graph *LG, mp_graphics *G, INT layer, INT node, INT x, INT y, INT dx, INT dy)
{
	INT d1, nb_V, depth;

	cout << "draw_vertex_callback x=" << x << " y=" << y << " dx = " << dx << " dy=" << dy << endl;
	d1 = LG->L[layer].Nodes[node].data1;
	nb_V = LG->data1;


	if (LG->L[layer].Nodes[node].f_has_vec_data) {

		BYTE str[1000000];
		INT i;
		INT *D;
		INT len;

		D = LG->L[layer].Nodes[node].vec_data;
		len = LG->L[layer].Nodes[node].vec_data_len;
		
		sprintf(str, "graph_begin %ld %ld %ld %ld %ld %ld %ld %ld ", layer, node, x, y, dx, dy, nb_V, len);
		for (i = 0; i < len; i++) {
			sprintf(str + strlen(str), " %ld", D[i]);
			}
		G->comment(str);

		if (LG->L[layer].Nodes[node].f_has_distinguished_element) {

			INT distinguished_edge;


			distinguished_edge = LG->L[layer].Nodes[node].distinguished_element_index;

			cout << "dinstingished edge = " << distinguished_edge << endl;

			draw_graph_with_distinguished_edge(G, x, y, dx, dy, nb_V, D, len, distinguished_edge, 0 /*verbose_level*/);
				// in GALOIS/draw.C
			}
		else {
			draw_graph(G, x, y, dx, dy, nb_V, D, len);
				// in GALOIS/draw.C
			}
		G->comment("graph_end");


		}

	else if (d1 >= 0) {
		INT *D;
		depth = get_depth(LG, layer, node);

		D = NEW_INT(depth + 1);
		get_data(LG, layer, node, D, depth);
		cout << "draw_vertex_callback layer=" << layer << " node=" << node << " data = ";
		INT_vec_print(cout, D, depth);
		cout << endl;

		BYTE str[1000000];
		INT i;

		sprintf(str, "graph_begin %ld %ld %ld %ld %ld %ld %ld %ld ", layer, node, x, y, dx, dy, nb_V, depth);
		for (i = 0; i < depth; i++) {
			sprintf(str + strlen(str), " %ld", D[i]);
			}
		G->comment(str);
		draw_graph(G, x, y, dx, dy, nb_V, D, depth);
			// in GALOIS/draw.C
		G->comment("graph_end");
		
		FREE_INT(D);
		}
}

void draw_vertex_callback_tournament(layered_graph *LG, mp_graphics *G, INT layer, INT node, INT x, INT y, INT dx, INT dy)
{
	INT verbose_level = 0;
	INT d1, nb_V, depth;

	cout << "draw_vertex_callback x=" << x << " y=" << y << " dx = " << dx << " dy=" << dy << endl;
	d1 = LG->L[layer].Nodes[node].data1;
	nb_V = LG->data1;


	if (LG->L[layer].Nodes[node].f_has_vec_data) {

		BYTE str[1000000];
		INT i;
		INT *D;
		INT len;

		D = LG->L[layer].Nodes[node].vec_data;
		len = LG->L[layer].Nodes[node].vec_data_len;
		
		sprintf(str, "tournament_begin %ld %ld %ld %ld %ld %ld %ld %ld ", layer, node, x, y, dx, dy, nb_V, len);
		for (i = 0; i < len; i++) {
			sprintf(str + strlen(str), " %ld", D[i]);
			}
		G->comment(str);
		draw_tournament(G, x, y, dx, dy, nb_V, D, len, verbose_level);
			// in GALOIS/draw.C
		G->comment("tournament_end");


		}

	else if (d1 >= 0) {
		INT *D;
		depth = get_depth(LG, layer, node);

		D = NEW_INT(depth + 1);
		get_data(LG, layer, node, D, depth);
		cout << "draw_vertex_callback layer=" << layer << " node=" << node << " data = ";
		INT_vec_print(cout, D, depth);
		cout << endl;

		BYTE str[1000000];
		INT i;

		sprintf(str, "tournament_begin %ld %ld %ld %ld %ld %ld %ld %ld ", layer, node, x, y, dx, dy, nb_V, depth);
		for (i = 0; i < depth; i++) {
			sprintf(str + strlen(str), " %ld", D[i]);
			}
		G->comment(str);
		draw_tournament(G, x, y, dx, dy, nb_V, D, depth, verbose_level);
			// in GALOIS/draw.C
		G->comment("tournament_end");
		
		FREE_INT(D);
		}
}

INT get_depth(layered_graph *LG, INT layer, INT node)
{
	INT d1, d2, d3;
	
	d1 = LG->L[layer].Nodes[node].data1;
	d2 = LG->L[layer].Nodes[node].data2;
	d3 = LG->L[layer].Nodes[node].data3;
	if (d2 >= 0) {
		return get_depth(LG, d2, d3) + 1;
		}
	else {
		return 0;
		}
}

INT get_data(layered_graph *LG, INT layer, INT node, INT *Data, INT cur_depth)
{
	INT d1, d2, d3;
	
	d1 = LG->L[layer].Nodes[node].data1;
	d2 = LG->L[layer].Nodes[node].data2;
	d3 = LG->L[layer].Nodes[node].data3;
	if (cur_depth) {
		Data[cur_depth - 1] = d1;
		}
	if (d2 >= 0) {
		return get_data(LG, d2, d3, Data, cur_depth - 1) + 1;
		}
	else {
		return 0;
		}
}

void draw_begining_callback(layered_graph *LG, mp_graphics *G, INT x_max, INT y_max, INT f_rotated, INT dx, INT dy)
{
	INT l, x, y, l1, l2, ll, j, x0, x1, y0, y1;
	INT Px[100];
	INT Py[100];
	
	cout << "draw_begining_callback" << endl;
	if (!f_boxed) {
		return;
		}

	G->sl_thickness(100); // 100 is normal

	for (l = 0; l < LG->nb_layers + boxed_group_size - 1; l += boxed_group_size) {
		l1 = l;
		l2 = MINIMUM(l + boxed_group_size, LG->nb_layers - 1);
		if (l2 == l1) {
			break;
			}
		cout << "l1=" << l << " l2=" << l2 << endl;
		LG->coordinates(LG->L[l1].Nodes[0].id, x_max, y_max, FALSE, x, y);
		y0 = y;
		Px[0] = x;
		Py[0] = y;
		LG->coordinates(LG->L[l2].Nodes[0].id, x_max, y_max, FALSE, x, y);
		Px[1] = x;
		Py[1] = y;
		y1 = y;

		x0 = INT_MAX;
		x1 = INT_MIN;
		for (ll = l1; ll <= l2; ll++) {
			for (j = 0; j < LG->L[ll].nb_nodes; j++) {
				LG->coordinates(LG->L[ll].Nodes[j].id, x_max, y_max, FALSE, x, y);
				x0 = MINIMUM(x0, x);
				x1 = MAXIMUM(x1, x);
				}
			}
		x0 -= 2 * dx;
		x1 += 2 * dx;
		y0 -= 3 * dy / 2 /* >> 1 */;
		y1 += 3 * dy / 2 /* >> 1 */;

		if (f_rotated) {
			double dx0, dx1, dy0, dy1;
			double dxx0, dxx1, dyy0, dyy1;

			dx0 = ((double) x0) / x_max;
			dx1 = ((double) x1) / x_max;
			dy0 = ((double) y0) / y_max;
			dy1 = ((double) y1) / y_max;

			dxx0 = 1. - dy0;
			dxx1 = 1. - dy1;
			dyy0 = dx0;
			dyy1 = dx1;
			x0 = dxx0 * x_max;
			x1 = dxx1 * x_max;
			y0 = dyy0 * y_max;
			y1 = dyy1 * y_max;
			}
		Px[0] = x0;
		Py[0] = y0;
		Px[1] = x0;
		Py[1] = y1;
		Px[2] = x1;
		Py[2] = y1;
		Px[3] = x1;
		Py[3] = y0;

		G->polygon5(Px, Py, 0, 1, 2, 3, 0);
		}

	G->sl_thickness(30); // 100 is normal
	
}

void draw_ending_callback(layered_graph *LG, mp_graphics *G, INT x_max, INT y_max, INT f_rotated, INT dx, INT dy)
{
	cout << "draw_ending_callback" << endl;

	if (f_text_underneath) {
		INT i, j;
		INT x, y;
		BYTE str[1000];
		
		G->st_overwrite(TRUE);
		for (i = 0; i < LG->nb_layers; i++) {


			if (f_select_layer) {
				INT idx;
			
				if (!INT_vec_search_linear(select_layer, nb_select_layer, i, idx)) {
					continue;
					}
			
				}



			for (j = 0; j < LG->L[i].nb_nodes; j++) {
				if (LG->L[i].Nodes[j].label) {
					sprintf(str, "%s", LG->L[i].Nodes[j].label);
					LG->coordinates(LG->L[i].Nodes[j].id, x_max, y_max, f_rotated, x, y);
					cout << "Node " << i << " / " << j << " label: " << str << " x=" << x << " y=" << y << endl;
					y -= dy * 1.6;
					G->aligned_text(x, y, "", str);
					}
				} // next j
			} //next i
		}

}


