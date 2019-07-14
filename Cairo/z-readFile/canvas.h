#ifndef A
#define A

#include <gtkmm.h>
#include <gtkmm/drawingarea.h>
#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <string>

struct SPoint
    {
    SPoint(double x, double y, double z, double e) : x(x), y(y), z(z), e(e) {}
    double x,y,z,e;
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

#endif // A
