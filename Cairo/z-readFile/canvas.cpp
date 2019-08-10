#include "canvas.h"

extern double xi, xa, yi, ya, zi, za, e, me, mn, xl, yl, zl, el;


bool CCanvas::on_draw(Cairo::RefPtr<Cairo::Context> const & cr)
    {
    Gtk::Allocation allocation{ get_allocation() };
    auto const width { (double)allocation.get_width() };
    auto const height{ (double)allocation.get_height() };

    cr->set_source_rgb(1.,.5,.0);
    cr->set_line_width(1);

    // line crossing the whole window

    double sx=width /(xa-xi);
    double sy=height/(ya-yi);
    double s = (sx>sy)?sx:sy;
    for (auto const & a:VLayer)
        {
        if (a.e <= 0)
            cr->move_to(s*(a.x-xi), s*(a.y-yi));
        else
            cr->line_to(s*(a.x-xi), s*(a.y-yi));
        }
    // cr->line_to(width, height);
    cr->stroke();

    return true;
    }

