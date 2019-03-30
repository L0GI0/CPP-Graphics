#include "GUIMyFrame1.h"
#include <algorithm>

GUIMyFrame1::GUIMyFrame1( wxWindow* parent )
:
MyFrame1( parent )
{
 m_staticText1->SetLabel(_(L"Jasno\u015B\u0107"));
 m_b_threshold->SetLabel(_(L"Pr\u00F3g 128"));
 this->SetBackgroundColour(wxColor(192, 192, 192));
 m_scrolledWindow->SetScrollbars(25, 25, 52, 40);
 m_scrolledWindow->SetBackgroundColour(wxColor(192, 192, 192));
}

void GUIMyFrame1::m_scrolledWindow_update( wxUpdateUIEvent& event )
{
 Repaint();
}

void GUIMyFrame1::m_b_grayscale_click( wxCommandEvent& event )
{
	Img_Cpy = Img_Org.ConvertToGreyscale();
}

void GUIMyFrame1::m_b_blur_click( wxCommandEvent& event )
{
	Img_Cpy = Img_Org.Blur(5);
}

void GUIMyFrame1::m_b_mirror_click( wxCommandEvent& event )
{
	Img_Cpy = Img_Org.Mirror();
}

void GUIMyFrame1::m_b_replace_click( wxCommandEvent& event )
{
	Img_Cpy = Img_Org.Copy();
	Img_Cpy.Replace(254, 0, 0, 0, 0, 255);
}

void GUIMyFrame1::m_b_rescale_click( wxCommandEvent& event )
{
	Img_Cpy = Img_Org.Copy();
	Img_Cpy.Rescale(320, 240);
	wxClientDC dc(m_scrolledWindow);
	m_scrolledWindow->DoPrepareDC(dc);
	dc.Clear();
}

void GUIMyFrame1::m_b_rotate_click( wxCommandEvent& event )
{
	Img_Cpy = Img_Org.Rotate(30 * acos(-1) / 180, wxPoint(Img_Org.GetSize().GetWidth() / 2, Img_Org.GetSize().GetHeight() / 2));
}

void GUIMyFrame1::m_b_rotate_hue_click( wxCommandEvent& event )
{
	Img_Cpy = Img_Org.Copy();
	Img_Cpy.RotateHue(0.5);
}

void GUIMyFrame1::m_b_mask_click( wxCommandEvent& event )
{
	Img_Cpy = Img_Org.Copy();
	Img_Cpy.SetMaskFromImage(Img_Mask, 0, 0, 0);
	wxClientDC dc(m_scrolledWindow);
	m_scrolledWindow->DoPrepareDC(dc);
	dc.Clear();
}

void GUIMyFrame1::m_s_brightness_scroll( wxScrollEvent& event )
{
// Tutaj, w reakcji na zmiane polozenia suwaka, wywolywana jest funkcja
// Brightness(...), ktora zmienia jasnosc. W tym miejscu nic nie
// zmieniamy. Do uzupelnienia pozostaje funkcja Brightness(...)
Brightness(m_s_brightness->GetValue() - 100);
Repaint();
}

void GUIMyFrame1::m_s_contrast_scroll( wxScrollEvent& event )
{
// Tutaj, w reakcji na zmiane polozenia suwaka, wywolywana jest funkcja
// Contrast(...), ktora zmienia kontrast. W tym miejscu nic nie
// zmieniamy. Do uzupelnienia pozostaje funkcja Contrast(...)
Contrast(m_s_contrast->GetValue() - 100);
Repaint();
}

void GUIMyFrame1::m_b_prewitt_click( wxCommandEvent& event )
{
	int dataSize = Img_Org.GetSize().GetWidth() * Img_Org.GetSize().GetHeight() * 3;
	unsigned char* data    = Img_Org.GetData();
	unsigned char* newData = static_cast <unsigned char*> (malloc(dataSize));
	int w = Img_Org.GetSize().GetWidth(),
		j1, j2, j3, j4, j5, j6,
		value1, value2;
	for (int i = 0; i < dataSize; ++i) {
		j1    = (i % (w * 3)) - 3 < 0 ? i - w * 3 : i - w * 3 - 3;
		j1    = j1 < 0 ? j1 + w * 3 : j1;
		j2    = (i % (w * 3)) - 3 < 0 ? i : i - 3;
		j3    = (i % (w * 3)) - 3 < 0 ? i + w * 3 : i + w * 3 - 3;
		j3    = j3 > dataSize ? j3 - w * 3 : j3;
		j4    = (i % (w * 3)) + 3 > w ? i - w * 3 : i - w * 3 + 3;
		j4    = j4 < 0 ? j4 + w * 3 : j4;
		j5    = (i % (w * 3)) + 3 < 0 ? i : i + 3;
		j6    = (i % (w * 3)) + 3 > w ? i + w * 3 : i + w * 3 + 3;
		j6    = j6 > dataSize ? j6 - w * 3 : j6;
		value1 = (data[j4] + data[j5] + data[j6] - data[j1] - data[j2] - data[j3]);
		//value1 < 0 ? 0 : value1 > 255 ? 255 : value1;
		value2 = (data[j1] + data[j2] + data[j3] - data[j4] - data[j5] - data[j6]);
		//value2 < 0 ? 0 : value2 > 255 ? 255 : value2;
		newData[i] = sqrt(value1 * value1 + value2 * value2) / 3 < 0 ? 0 : sqrt(value1 * value1 + value2 * value2) / 3 > 255 ? 255 : sqrt(value1 * value1 + value2 * value2) / 3;
	}
	Img_Cpy.SetData(newData, Img_Org.GetSize().GetWidth(), Img_Org.GetSize().GetHeight());
	wxClientDC dc(m_scrolledWindow);
	m_scrolledWindow->DoPrepareDC(dc);
	dc.Clear();
	Img_Cpy.SetMask(false);
}

void GUIMyFrame1::m_b_threshold_click( wxCommandEvent& event )
{
	int dataSize = Img_Org.GetSize().GetWidth() * Img_Org.GetSize().GetHeight() * 3;
	unsigned char* data    = Img_Org.GetData();
	unsigned char* newData = static_cast <unsigned char*> (malloc(dataSize));
	for (int i = 0; i < dataSize; ++i) {
		newData[i] = data[i] > 128 ? 255 : 0;
	}
	Img_Cpy.SetData(newData, Img_Org.GetSize().GetWidth(), Img_Org.GetSize().GetHeight());
	wxClientDC dc(m_scrolledWindow);
	m_scrolledWindow->DoPrepareDC(dc);
	dc.Clear();
	Img_Cpy.SetMask(false);
}


void GUIMyFrame1::Brightness(int value)
{
	int dataSize = Img_Org.GetSize().GetWidth() * Img_Org.GetSize().GetHeight() * 3;
	unsigned char* data    = Img_Org.GetData();
	unsigned char* newData = static_cast <unsigned char*> (malloc(dataSize));
	for (int i = 0; i < dataSize; ++i) {
		newData[i] = data[i] + value < 0 ? 0 : data[i] + value > 255 ? 255 : data[i] + value;
	}
	Img_Cpy.SetData(newData);
}

void GUIMyFrame1::Repaint()
{
 wxBitmap bitmap(Img_Cpy);          // Tworzymy tymczasowa bitmape na podstawie Img_Cpy
 wxClientDC dc(m_scrolledWindow);   // Pobieramy kontekst okna
 m_scrolledWindow->DoPrepareDC(dc); // Musimy wywolac w przypadku wxScrolledWindow, zeby suwaki prawidlowo dzialaly
 dc.DrawBitmap(bitmap, 0, 0, true); // Rysujemy bitmape na kontekscie urzadzenia
}

void GUIMyFrame1::Contrast(int value)
{
	int dataSize = Img_Org.GetSize().GetWidth() * Img_Org.GetSize().GetHeight() * 3;
	unsigned char* data    = Img_Org.GetData();
	unsigned char* newData = static_cast <unsigned char*> (malloc(dataSize));
	double mod = 1.0 + value / 100.0;
	for (int i = 0; i < dataSize; ++i) {
		newData[i] = mod * (data[i] - 127) + 127 < 0 ? 0 : mod * (data[i] - 127) + 127 > 255 ? 255 : mod * (data[i] - 127) + 127;
	}
	Img_Cpy.SetData(newData);
}