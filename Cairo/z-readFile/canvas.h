#ifndef CANVAS_H
#define CANVAS_H

#include <gtkmm.h>
#include <gtkmm/drawingarea.h>
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <string>

struct SPoint
    {
    double x,y,z,e;

    SPoint(double const & x, double const & y, double const & z, double const & e)
         : x(x), y(y), z(z), e(e)
        {}
    };

using CLayer = std::vector<SPoint>;
extern CLayer VLayer;

class CCanvas : public Gtk::DrawingArea
    {
    public:
        std::vector<SPoint> points;

        CCanvas()
            {

            }

        virtual ~CCanvas() { };

    protected:
        // Override default signal handler:
        bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;


    }; // CCanvas

#endif // CANVAS_H
