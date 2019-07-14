#include "canvas.h"





bool CCanvas::on_draw(Cairo::RefPtr<Cairo::Context> const & cr)
    {
    Gtk::Allocation allocation{ get_allocation() };
    auto const width { (double)allocation.get_width() };
    auto const height{ (double)allocation.get_height() };

    cr->set_source_rgb(1.,.5,.0);
    cr->set_line_width(1);

    // line crossing the whole window
    for (auto const & a:VLayer)
        {
        if (a.e <= 0)
            cr->move_to(000+2*a.x, 000+2*a.y);
        else
            cr->line_to(000+2*a.x, 000+2*a.y);
        }
    // cr->line_to(width, height);
    cr->stroke();

    return true;
    }

