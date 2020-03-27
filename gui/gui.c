#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <gtk/gtk.h>

#define WIDTH 3
#define HEIGHT 3
#define BASE 10
#define BUF_SIZE 19

GtkWidget *window;
GtkWidget *grid;

GtkWidget *display;
GtkTextBuffer *buffer;

GtkWidget *buttons[WIDTH * HEIGHT];
GtkWidget *zero_button;
GtkWidget *plus_button;
GtkWidget *minus_button;
GtkWidget *mult_button;
GtkWidget *div_button;
GtkWidget *equal_button;
GtkWidget *open_button;
GtkWidget *close_button;
GtkWidget *ac_button;
GtkWidget *c_button;

char display_text[BUF_SIZE];
int display_len = 0;

// calculates an expression involving only numbers, +, -, * and /
long simple_calc(char *s) {
	long result = 0;
	long denominator = 0;
	
	while (*s) {
		switch (*s) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			result = strtol(s, &s, BASE);
			break;
		case '+': 
			switch (*(s + 1)) {
			case '-': // evaluate +- to - (for example, 1+-2 should be 1-2)
				++s;
				result -= strtol(++s, &s, BASE);
				break;
			default:
				result += strtol(++s, &s, BASE);
			}
			break;
		case '-':
			switch (*(s + 1)) {
			case '-': // evaluate -- to + (for example, 1--2 should be 1+2)
				++s;
				result += strtol(++s, &s, BASE);
				break;
			default:
				result -= strtol(++s, &s, BASE);
			}
			break;
		case '*':
			result *= strtol(++s, &s, BASE);
			break;
		case '/':
			denominator = strtol(++s, &s, BASE);
			if (denominator) {
				result /= denominator;
			} else {
				return 0; // division by 0
			}
			break;
		default:
			return 0; // invalid expression
		}
	}
	
	return result;
}

void calc(GtkWidget *widget, gpointer user_data) {
	int open_pos = -1, close_pos = -1;

	for (int i = 0; i < display_len; i++) {
		// evaluate innnermost brackets first
		switch (display_text[i]) {
		case '(':
			open_pos = i;
			break;
		case ')':
			// extract innermost part
			close_pos = i;
			if (open_pos < 0 || open_pos >= close_pos) { // invalid expression
				i = display_len;
				break;
			}
			char inner[close_pos - open_pos];
			memcpy(inner, display_text + open_pos + 1, close_pos - open_pos - 1);
			inner[close_pos - open_pos - 1] = '\0';

			// evaluate
			long result = simple_calc(inner);

			// convert to string
			int result_len = snprintf(NULL, 0, "%ld", result);
			char result_str[result_len + 1];
			snprintf(result_str, result_len + 1, "%ld", result);

			// substitute into string
			display_text[open_pos] = '\0';
			strcat(display_text, result_str);

			// append the remaining part
			char tail[display_len - close_pos];
			memcpy(tail, display_text + close_pos + 1, display_len - close_pos);
			strcat(display_text, tail);

			// update display_len and go back to beginning
			display_len -= close_pos - open_pos + 1 - result_len;
			i = -1;
		}
	}

	long ans = simple_calc(display_text);
	display_len = snprintf(NULL, 0, "%ld", ans);
	snprintf(display_text, display_len + 1, "%ld", ans);
	gtk_text_buffer_set_text(buffer, display_text, -1);
}

void click(GtkWidget *widget, gpointer user_data) {
	if (display_len >= BUF_SIZE - 1) {
		return;
	}
	strcat(display_text, gtk_button_get_label(GTK_BUTTON(widget)));
	display_len++;
	gtk_text_buffer_set_text(buffer, display_text, -1);
}

void ac(GtkWidget *widget, gpointer user_data) {
	display_text[0] = '\0';
	display_len = 0;
	gtk_text_buffer_set_text(buffer, display_text, -1);
}

void c(GtkWidget *widget, gpointer user_data) {
	display_text[display_len - 1] = '\0';
	display_len--;
	gtk_text_buffer_set_text(buffer, display_text, -1);
}

void attach_buttons(void) {
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			int i = y * WIDTH + x;

			gchar *label = g_strdup_printf("%d", i + 1);
			buttons[i] = gtk_button_new_with_label(label);
			g_signal_connect(buttons[i], "clicked", G_CALLBACK(click), NULL);
			gtk_grid_attach(GTK_GRID(grid), buttons[i], x, y, 1, 1);

			g_free(label);
		}
	}

	zero_button = gtk_button_new_with_label("0");
	g_signal_connect(zero_button, "clicked", G_CALLBACK(click), NULL);
	gtk_grid_attach(GTK_GRID(grid), zero_button, 0, HEIGHT, 1, 1);

	open_button = gtk_button_new_with_label("(");
	g_signal_connect(open_button, "clicked", G_CALLBACK(click), NULL);
	gtk_grid_attach(GTK_GRID(grid), open_button, 1, HEIGHT, 1, 1);

	close_button = gtk_button_new_with_label(")");
	g_signal_connect(close_button, "clicked", G_CALLBACK(click), NULL);
	gtk_grid_attach(GTK_GRID(grid), close_button, 2, HEIGHT, 1, 1);

	plus_button = gtk_button_new_with_label("+");
	g_signal_connect(plus_button, "clicked", G_CALLBACK(click), NULL);
	gtk_grid_attach(GTK_GRID(grid), plus_button, WIDTH, 0, 1, 1);

	minus_button = gtk_button_new_with_label("-");
	g_signal_connect(minus_button, "clicked", G_CALLBACK(click), NULL);
	gtk_grid_attach(GTK_GRID(grid), minus_button, WIDTH, 1, 1, 1);

	mult_button = gtk_button_new_with_label("*");
	g_signal_connect(mult_button, "clicked", G_CALLBACK(click), NULL);
	gtk_grid_attach(GTK_GRID(grid), mult_button, WIDTH, 2, 1, 1);

	div_button = gtk_button_new_with_label("/");
	g_signal_connect(div_button, "clicked", G_CALLBACK(click), NULL);
	gtk_grid_attach(GTK_GRID(grid), div_button, WIDTH, HEIGHT, 1, 1);

	equal_button = gtk_button_new_with_label("=");
	g_signal_connect(equal_button, "clicked", G_CALLBACK(calc), NULL);
	gtk_grid_attach(GTK_GRID(grid), equal_button, WIDTH, HEIGHT + 1, 1, 1);

	c_button = gtk_button_new_with_label("C");
	g_signal_connect(c_button, "clicked", G_CALLBACK(c), NULL);
	gtk_grid_attach(GTK_GRID(grid), c_button, 2, HEIGHT + 1, 1, 1);

	ac_button = gtk_button_new_with_label("AC");
	g_signal_connect(ac_button, "clicked", G_CALLBACK(ac), NULL);
	gtk_grid_attach(GTK_GRID(grid), ac_button, 0, HEIGHT + 1, (WIDTH + 1)/2, 1);
}

void attach_display(void) {
	display = gtk_text_view_new();
	gtk_text_view_set_left_margin(GTK_TEXT_VIEW(display), 10);
	gtk_text_view_set_right_margin(GTK_TEXT_VIEW(display), 10);
	gtk_text_view_set_top_margin(GTK_TEXT_VIEW(display), 10);
	gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(display), 10);

	// display should not be editable
	gtk_text_view_set_editable(GTK_TEXT_VIEW(display), FALSE);

	// hide cursor
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(display), FALSE);

	// set display text
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(display));
	gtk_text_buffer_set_text(buffer, display_text, -1);

	// attach display to top
	gtk_grid_attach(GTK_GRID(grid), display, -1, -1, WIDTH + 2, 1);
}

void activate(GtkApplication *app, gpointer user_data) {
	/* create a new window */
	window = gtk_application_window_new(app);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

	/* Here we construct the container that is going pack our buttons */
	grid = gtk_grid_new();
	attach_buttons();
	attach_display();

	/* Pack the container in the window */
	gtk_container_add(GTK_CONTAINER(window), grid);

	// set window title
	gtk_window_set_title(GTK_WINDOW(window), "Calculator");

	// add CSS button properties
	GtkCssProvider *provider = gtk_css_provider_new();
	GdkDisplay *gdk_display = gdk_display_get_default();
	GdkScreen *screen = gdk_display_get_default_screen(gdk_display);
	gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
	gtk_css_provider_load_from_data(GTK_CSS_PROVIDER(provider), "button { border-radius:0px; }", -1, NULL);

	/* Now that we are done packing our widgets, we show them all
	 * in one go, by calling gtk_widget_show_all() on the window.
	 * This call recursively calls gtk_widget_show() on all widgets
	 * that are contained in the window, directly or indirectly.
	 */
	gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
	// activate GTK application
	GtkApplication *app = gtk_application_new(NULL, G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	int status = g_application_run(G_APPLICATION(app), argc, argv);

	// clean up
	g_object_unref(app);

	return status;
}
