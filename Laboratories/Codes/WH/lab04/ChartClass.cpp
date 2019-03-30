#include <wx/dc.h>
#include <memory>

#include "ChartClass.h"
#include "vecmat.h" //Plik vecmat.h zosta³ podzielony na vecmat.h i vecmat.cpp, deklaracje i definicje klas powinny byæ rozdzielone,
                    //inaczej dochodzi do potencjalnych b³êdów linkera

void ChartClass::Set_Range() {
	double xmin =  9999.9,
		   xmax = -9999.9,
		   ymin =  9999.9,
		   ymax = -9999.9,
		   x,
		   y,
		   step;

	xmin = cfg -> Get_x_start();
	xmax = cfg -> Get_x_stop();

	step = (cfg -> Get_x_stop() - cfg -> Get_x_start()) / static_cast<double> (x_step);
	x = cfg -> Get_x_start();

	for (int i = 0; i <= x_step; ++i, x += step) {
		y = GetFunctionValue(x);
		if (y > ymax) { ymax = y; }
		if (y < ymin) { ymin = y; }
	}

	y_min = ymin;
	y_max = ymax;
	x_min = xmin;
	x_max = xmax;
}

double ChartClass::GetFunctionValue(double x) {
	if (cfg -> Get_F_type() == 1) return x * x;
	if (cfg -> Get_F_type() == 2) return 0.5 * exp(4 * x - 3 * x * x);
	return x + sin(x * 4.0);
}

void ChartClass::Draw(wxDC* dc, int _w, int _h) {
	w = _w - 20;
	h = _h - 20;

	dc -> SetBackground     (*wxWHITE_BRUSH);
	dc -> Clear             ();
	dc -> SetPen            (*wxRED_PEN);
	dc -> DrawRectangle     (10, 10, w, h);

	dc -> SetPen            (*wxBLUE_PEN);

	GenerateTransform();

	DrawXAxis(dc);
	DrawYAxis(dc);
	DrawChart(dc);
}

void ChartClass::GenerateTransform() {
	Matrix transform; // transformata z uk³adu kartezjañskiego na uk³ad ekranu
	transform.data[0][0] = w / ((cfg -> Get_x1()) - (cfg -> Get_x0()));
	transform.data[1][1] = h / ((cfg -> Get_y1()) - (cfg -> Get_y0())) * -1.0;
	transform.data[0][2] = -(cfg -> Get_x0()) * transform.data[0][0] - 10;
	transform.data[1][2] = -(cfg -> Get_y1()) * transform.data[1][1] + 10;

	Matrix transform2; // transformata przesuniêcia
	transform2.data[0][0] = transform2.data[1][1] = 1;
	transform2.data[0][2] = cfg -> Get_dX();
	transform2.data[1][2] = cfg -> Get_dY();

	Matrix transform3; //transformata obrotu
	int Rx, Ry;
	if (cfg -> RotateScreenCenter() ) {
		Rx = w / 2;
		Ry = h / 2;
	}
	else {
		Vector temp;
		temp.Set(0, 0);
		temp = transform * temp;
		Rx   = temp.GetX();
		Ry   = temp.GetY();
	}
	transform3.data[0][0] =  cos(-(cfg -> Get_Alpha()) * acos(-1) / 180);
	transform3.data[1][0] =  sin(-(cfg -> Get_Alpha()) * acos(-1) / 180);
	transform3.data[0][1] = -transform3.data[1][0];
	transform3.data[1][1] =  transform3.data[0][0];
	transform3.data[0][2] =  Rx * (1 - transform3.data[0][0]) + Ry * transform3.data[1][0];
	transform3.data[1][2] = -Rx * transform3.data[1][0] + Ry * (1 - transform3.data[0][0]);

	t = transform3 * transform2 * transform;
}

wxPoint ChartClass::ConvertPoint(double x, double y) {
	Vector vector;
	vector.Set(x, y);
	vector = t * vector;
	return wxPoint(vector.GetX(), vector.GetY());
}

void ChartClass::DrawXAxis(wxDC* dc) {
	wxPoint temp1, temp2, temp3;

	wxDCClipper* clipper = new wxDCClipper(*dc, 10, 10, w, h);

	temp1 = ConvertPoint (x_min, 0);
	temp2 = ConvertPoint (x_max, 0);
	dc   -> DrawLine     (temp1, temp2);

	temp1 = ConvertPoint (x_min / 2.0, -(y_min - y_max) / 100);
	temp2 = ConvertPoint (x_min / 2.0,  (y_min - y_max) / 100);
	dc   -> DrawLine     (temp1, temp2);

	temp1 = ConvertPoint (x_max / 3.0, -(y_min - y_max) / 100);
	temp2 = ConvertPoint (x_max / 3.0,  (y_min - y_max) / 100);
	dc    -> DrawLine    (temp1, temp2);
	
	temp1 = ConvertPoint (x_max * 2.0 / 3.0, -(y_min - y_max) / 100);
	temp2 = ConvertPoint (x_max * 2.0 / 3.0,  (y_min - y_max) / 100);
	dc    -> DrawLine    (temp1, temp2);

	temp1 = ConvertPoint (x_max, 0);
	temp2 = ConvertPoint (x_max - abs(x_min - y_max) / 75, -(x_min - x_max) / 150);
	temp3 = ConvertPoint (x_max - abs(x_min - y_max) / 75,  (x_min - x_max) / 150);
	dc   -> DrawLine     (temp1, temp2);
	dc   -> DrawLine     (temp1, temp3);

	delete clipper;
	temp1 = ConvertPoint    (x_min / 2.0,       -abs(y_min - y_max) / 60);
	temp2 = ConvertPoint    (x_max / 3.0,       -abs(y_min - y_max) / 60);
	temp3 = ConvertPoint    (x_max * 2.0 / 3.0, -abs(y_min - y_max) / 60);
	dc   -> DrawRotatedText (wxString::Format("%.2lf", x_min / 2.0),       temp1, cfg -> Get_Alpha());
	dc   -> DrawRotatedText (wxString::Format("%.2lf", x_max / 3.0),       temp2, cfg -> Get_Alpha());
	dc   -> DrawRotatedText (wxString::Format("%.2lf", x_max * 2.0 / 3.0), temp3, cfg -> Get_Alpha());

}

void ChartClass::DrawYAxis(wxDC* dc) {
	wxPoint temp1, temp2, temp3;

	wxDCClipper* clipper = new wxDCClipper(*dc, 10, 10, w, h);

	temp1 = ConvertPoint (0, y_min);
	temp2 = ConvertPoint (0, y_max);
	dc   -> DrawLine     (temp1, temp2);

	temp1 = ConvertPoint (-(x_min - x_max) / 100, y_min / 2.0);
	temp2 = ConvertPoint ( (x_min - x_max) / 100, y_min / 2.0);
	dc   -> DrawLine     (temp1, temp2);

	temp1 = ConvertPoint (-(x_min - x_max) / 100, y_max / 3.0);
	temp2 = ConvertPoint ( (x_min - x_max) / 100, y_max / 3.0);
	dc   -> DrawLine     (temp1, temp2);
	
	temp1 = ConvertPoint (-(x_min - x_max) / 100, y_max * 2.0 / 3.0);
	temp2 = ConvertPoint ( (x_min - x_max) / 100, y_max * 2.0 / 3.0);
	dc   -> DrawLine     (temp1, temp2);

	temp1 = ConvertPoint (0, y_max);
	temp2 = ConvertPoint (-(x_min - x_max) / 150, y_max - abs(y_min - y_max) / 75);
	temp3 = ConvertPoint ( (x_min - x_max) / 150, y_max - abs(y_min - y_max) / 75);
	dc   -> DrawLine     (temp1, temp2);
	dc   -> DrawLine     (temp1, temp3); 

	delete clipper;
	temp1 = ConvertPoint    (abs(x_min - x_max) / 60, y_min / 2.0);
	temp2 = ConvertPoint    (abs(x_min - x_max) / 60, y_max / 3.0);
	temp3 = ConvertPoint    (abs(x_min - x_max) / 60, y_max * 2.0 / 3.0);
	dc   -> DrawRotatedText (wxString::Format("%.2lf", y_min / 2.0),       temp1, cfg -> Get_Alpha());
	dc   -> DrawRotatedText (wxString::Format("%.2lf", y_max / 3.0),       temp2, cfg -> Get_Alpha());
	dc   -> DrawRotatedText (wxString::Format("%.2lf", y_max * 2.0 / 3.0), temp3, cfg -> Get_Alpha());
}

void ChartClass::DrawChart(wxDC* dc) {
	wxDCClipper clipper(*dc, 10, 10, w, h);

	double step = (x_max - x_min) / static_cast<double> (x_step),
		   x    = x_min + step,
		   y1   = GetFunctionValue(x),
		   y2;

	dc -> SetPen(*wxGREEN_PEN);

	for (int i = 0; i <= x_step; ++i, x += step) {
		y2  = GetFunctionValue(x);
		dc -> DrawLine(ConvertPoint(x - step, y1), ConvertPoint(x, y2));
		y1 = y2;
	}
}