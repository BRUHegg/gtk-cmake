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

constexpr double CDU_LETTER_WIDTH = 21;
constexpr double CDU_LETTER_HEIGHT = 39;

int get_cdu_letter_idx(char c)
{
    if(c >= '0' && c <= '9')
        return 1+c-'0';
    else if(c >= 'A' && c <= 'Z')
        return 11+c-'A';
    else if(c == '%')
        return 37;
    else if(c == '(')
        return 38;
    else if(c == ')')
        return 39;
    else if(c == '-')
        return 40;
    else if(c == '_')
        return 41;
    else if(c == '+')
        return 42;
    else if(c == '=')
        return 43;
    else if(c == '|')
        return 44;
    else if(c == ':')
        return 45;
    else if(c == '<')
        return 46;
    else if(c == '.')
        return 47;
    else if(c == '>')
        return 48;
    else if(c == ',')
        return 49;
    else if(c == '/')
        return 50;
    else if(c == '*')
        return 51;
    else if(c == '@')
        return 52;
    return 0;
}

void draw_cdu_letter(cairo_t *cr, char c, geom::vect2_t pos, geom::vect2_t scale)
{
    if(scale.x == 0 || scale.y == 0)
        return;

    int idx = get_cdu_letter_idx(c);
    geom::vect2_t offs = {-CDU_LETTER_WIDTH * scale.x * double(idx), 0};
    geom::vect2_t img_pos = pos + offs;
    cairo_save(cr);
    cairo_scale(cr, scale.x, scale.y);
    cairo_set_source_surface(cr, tex_mngr.data["cdu_big_white"], 
        img_pos.x/scale.x, img_pos.y/scale.y);
    cairo_rectangle(cr, pos.x/scale.y, pos.y/scale.y, CDU_LETTER_WIDTH, CDU_LETTER_HEIGHT);
    cairo_clip(cr);
    cairo_paint(cr);
    cairo_restore(cr);
}

void draw_cdu_line(cairo_t *cr, std::string& s, geom::vect2_t pos, geom::vect2_t scale)
{
    for(size_t i = 0; i < s.size(); i++)
    {
        draw_cdu_letter(cr, s[i], pos, scale);
        pos.x += CDU_LETTER_WIDTH;
    }
}


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
    std::string tgt = "HELLO WORLD0-_+=";
    draw_cdu_line(cr, tgt, {100, 100}, {0.7, 0.7});
    draw_cdu_line(cr, tgt, {100, 139}, {1, 1});
}

static gboolean clicked(GtkWidget *widget, GdkEventButton *event,
    gpointer user_data)
{
    UNUSED(widget);
    UNUSED(user_data);
    if(double(event->x) <= double(bm->get_width()) && 
        double(event->y) <= double(bm->get_height()))
    {
        std::cout << int(bm->get_at(size_t(event->x), size_t(event->y))) << "\n";
    }

    return TRUE;
}

int main(int argc, char *argv[])
{
    if(!tex_mngr.load({"cdu", "cdu_big_white"}, ""))
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