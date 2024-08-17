#include <cairo.h>
#include <gtk/gtk.h>
#include "cairo_utils.hpp"
#include <iostream>
#include "bytemap.hpp"

#define UNUSED(x) (void)(x)

constexpr gint CANVAS_WIDTH = 900;
constexpr gint CANVAS_HEIGHT = CANVAS_WIDTH;
constexpr double SCALE_PX = 40;
constexpr double LN_THICK_PX = 3;

byteutils::Bytemap *bm;
cairo_utils::texture_manager_t tex_mngr;


static void do_drawing(cairo_t *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr,
                              gpointer user_data)
{
    UNUSED(widget);
    UNUSED(user_data);
    do_drawing(cr);

    return FALSE;
}

static void do_drawing(cairo_t *cr)
{
    cairo_utils::draw_image(cr, tex_mngr.data["cdu"], {0, 0}, {1, 1}, false);
}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event,
    gpointer user_data)
{
    UNUSED(widget);
    UNUSED(user_data);
    std::cout << event->x << " " << event->y << "\n";
    std::cout << int(bm->get_at(size_t(event->x), size_t(event->y))) << "\n";

    return TRUE;
}

int main(int argc, char *argv[])
{
    if(!tex_mngr.load({"cdu"}, ""))
    {   
        std::cout << "Failed to load .png\n";
        exit(0);
    }
    else
    {
        geom::vect2_t sz = cairo_utils::get_surf_sz(tex_mngr.data["cdu"]);
        bm = new byteutils::Bytemap(size_t(sz.x), size_t(sz.y));
        bool ret = bm->load("cdu_key_map.byt");
        if(!ret)
        {
            std::cout << "Failed to load bytemap\n";
            exit(0);
        }
    }
    

    GtkWidget *window;
    GtkWidget *darea;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    darea = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), darea);

    g_signal_connect(G_OBJECT(darea), "draw",
                     G_CALLBACK(on_draw_event), NULL);
    g_signal_connect(window, "destroy",
                     G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect(window, "button-press-event", 
      G_CALLBACK(clicked), NULL);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_default_size(GTK_WINDOW(window), CANVAS_WIDTH, CANVAS_HEIGHT);
    gtk_window_set_title(GTK_WINDOW(window), "GTK window");

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}