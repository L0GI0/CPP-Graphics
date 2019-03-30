#ifndef __ChartClass__
#define __ChartClass__

#include <memory>
#include "ConfigClass.h"
#include "vecmat.h" //Plik vecmat.h zosta³ podzielony na vecmat.h i vecmat.cpp, deklaracje i definicje klas powinny byæ rozdzielone,
                    //inaczej dochodzi do potencjalnych b³êdów linkera

class ChartClass {
    private:
        std::shared_ptr<ConfigClass> cfg;
        int    x_step,                            // liczba odcinkow z jakich bedzie sie skladal wykres
		       w, h;                              // rozmiar obszaru rysowania
        double x_min, x_max,                      // zakres zmiennej x
               y_min, y_max;                      // zakres wartosci przyjmowanych przez funkcje
        double GetFunctionValue(double x);        // zwraca wartosci rysowanej funkcji
		void   GenerateTransform();               // tworzy bie¿¹c¹ macierz przekszta³ceñ
		void   DrawXAxis(wxDC *dc);               // rysuje oœ X
		void   DrawYAxis(wxDC *dc);               // rysuje oœ Y
		void   DrawChart(wxDC *dc);               // rysuje krzyw¹ wykresu
		wxPoint ConvertPoint(double x, double y); // funkcja transformujaca odcinki
		Matrix t;                                 // bie¿aca macierz przekszta³ceñ
       
    public:
        ChartClass(std::shared_ptr<ConfigClass> c) { cfg = std::move(c); x_step = 200; Set_Range();}
        void   Set_Range();                               // ustala wartosci zmiennych x_min, y_min, x_max, y_max
		double Get_Y_min() { Set_Range(); return y_min; } // zwraca y_min
		double Get_Y_max() { Set_Range(); return y_max; } // zwraca y_max
        void   Draw(wxDC *dc, int w, int h);              // rysuje wykres
		
};

#endif
