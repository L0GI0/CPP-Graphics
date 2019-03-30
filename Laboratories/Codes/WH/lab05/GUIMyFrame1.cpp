#include "GUIMyFrame1.h"
#include <vector>
#include <fstream>
#include "vecmat.h"

struct Point {
 float x, y, z;
 Point(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
};

struct Color {
 int R, G, B;
 Color(int _R, int _G, int _B) : R(_R), G(_G), B(_B) {}
};

struct Segment {
 Point begin, end;
 Color color;
 Segment(Point _begin, Point _end, Color _color) : begin(_begin), end(_end), color(_color) {}
};

std::vector<Segment> data;

GUIMyFrame1::GUIMyFrame1( wxWindow* parent )
:
MyFrame1( parent )
{
 m_button_load_geometry->SetLabel(_("Wczytaj Geometri\u0119"));
 m_staticText25->SetLabel(_("Obr\u00F3t X:"));
 m_staticText27->SetLabel(_("Obr\u00F3t Y:"));
 m_staticText29->SetLabel(_("Obr\u00F3t Z:"));

 WxSB_TranslationX->SetRange(0, 200); WxSB_TranslationX->SetValue(100);
 WxSB_TranslationY->SetRange(0, 200); WxSB_TranslationY->SetValue(100);
 WxSB_TranslationZ->SetRange(0, 200); WxSB_TranslationZ->SetValue(100);

 WxSB_RotateX->SetRange(0, 360); WxSB_RotateX->SetValue(0);
 WxSB_RotateY->SetRange(0, 360); WxSB_RotateY->SetValue(0);
 WxSB_RotateZ->SetRange(0, 360); WxSB_RotateZ->SetValue(0);

 WxSB_ScaleX->SetRange(1, 200); WxSB_ScaleX->SetValue(100);
 WxSB_ScaleY->SetRange(1, 200); WxSB_ScaleY->SetValue(100);
 WxSB_ScaleZ->SetRange(1, 200); WxSB_ScaleZ->SetValue(100);
}

void GUIMyFrame1::WxPanel_Repaint( wxUpdateUIEvent& event )
{
 Repaint();
}

void GUIMyFrame1::m_button_load_geometry_click( wxCommandEvent& event )
{
 wxFileDialog WxOpenFileDialog(this, wxT("Choose a file"), wxT(""), wxT(""), wxT("Geometry file (*.geo)|*.geo"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

 if (WxOpenFileDialog.ShowModal() == wxID_OK)
 {
  double x1, y1, z1, x2, y2, z2;
  int r, g, b;

  std::ifstream in(WxOpenFileDialog.GetPath().ToAscii());
  if (in.is_open())
  {
   data.clear();
   while (!in.eof())
   {
    in >> x1 >> y1 >> z1 >> x2 >> y2 >> z2 >> r >> g >> b;
    data.push_back(Segment(Point(x1, y1, z1), Point(x2, y2, z2), Color(r, g, b)));
   }
   in.close();
  }
 }
}

void GUIMyFrame1::Scrolls_Updated( wxScrollEvent& event )
{
WxST_TranslationX->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationX->GetValue() - 100) / 50.0));
WxST_TranslationY->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationY->GetValue() - 100) / 50.0));
WxST_TranslationZ->SetLabel(wxString::Format(wxT("%g"), (WxSB_TranslationZ->GetValue() - 100) / 50.0));

WxST_RotateX->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateX->GetValue()));
WxST_RotateY->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateY->GetValue()));
WxST_RotateZ->SetLabel(wxString::Format(wxT("%d"), WxSB_RotateZ->GetValue()));

WxST_ScaleX->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleX->GetValue() / 100.0));
WxST_ScaleY->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleY->GetValue() / 100.0));
WxST_ScaleZ->SetLabel(wxString::Format(wxT("%g"), WxSB_ScaleZ->GetValue() / 100.0));


Repaint();
}

Vector4 GUIMyFrame1::Normalize(Vector4 vector) {
	Vector4 vector2(vector);
	vector2.data[0] /= vector.data[3];
	vector2.data[1] /= vector.data[3];
	vector2.data[2] /= vector.data[3];
	return vector2;
}

Matrix4 GUIMyFrame1::GenerateTransform1() {
	double angle;

	Matrix4 transform1;
	transform1.data[0][0] =  WxSB_ScaleX       -> GetValue() / 100.0;
	transform1.data[1][1] =  WxSB_ScaleY       -> GetValue() / 100.0;
	transform1.data[2][2] =  WxSB_ScaleZ       -> GetValue() / 100.0 * 1.0;

	Matrix4 transform2;
	angle = WxSB_RotateZ ->  GetValue() * acos(-1) / 180;
	transform2.data[0][0] =  cos(angle);
	transform2.data[1][1] =  transform2.data[0][0];
	transform2.data[0][1] =  sin(angle);
	transform2.data[1][0] = -transform2.data[0][1];
	transform2.data[2][2] =  1;

	Matrix4 transform3;
	angle = WxSB_RotateY ->  GetValue() * acos(-1) / 180;
	transform3.data[0][0] =  cos(angle);
	transform3.data[2][2] =  transform3.data[0][0];
	transform3.data[2][0] =  sin(angle);
	transform3.data[0][2] = -transform3.data[2][0];
	transform3.data[1][1] =  1;

	Matrix4 transform4;
	angle = WxSB_RotateX ->  GetValue() * acos(-1) / 180;
	transform4.data[1][1] =  cos(angle);
	transform4.data[2][2] =  transform4.data[1][1];
	transform4.data[1][2] =  sin(angle);
	transform4.data[2][1] = -transform4.data[1][2];
	transform4.data[0][0] =  1;

	Matrix4 transform5;
	transform5.data[0][3] = (WxSB_TranslationX -> GetValue() - 100.0) / 50.0;
	transform5.data[1][3] = (WxSB_TranslationY -> GetValue() - 100.0) / 50.0;
	transform5.data[2][3] = (WxSB_TranslationZ -> GetValue() - 100.0) / 50.0;
	transform5.data[0][0] = transform5.data[1][1] = transform5.data[2][2] = 1;

	

	return transform5 * transform4 * transform3 * transform2 * transform1;
}

Matrix4 GUIMyFrame1::GenerateTransform2() {
	Matrix4 transform1;
	transform1.data[0][0] = 1;
	transform1.data[1][1] = 1;
	transform1.data[3][2] = 1.0 / -cameraLocation;

	Matrix4 transform2;
	wxSize panelSize = WxPanel ->  GetSize();
	transform2.data[0][0]       =  panelSize.GetWidth () / 2;
	transform2.data[1][1]       = -panelSize.GetHeight() / 2;
	transform2.data[0][3]       =  transform2.data[0][0];
	transform2.data[1][3]       = -transform2.data[1][1];

	return transform2 * transform1;
}

void GUIMyFrame1::Repaint() {	
	Matrix4 transform1 = GenerateTransform1();
	Matrix4 transform2 = GenerateTransform2();

	wxClientDC  _dc(WxPanel);
	wxBufferedDC dc(&_dc);

	dc.SetBackground (*wxWHITE_BRUSH);
	dc.Clear         ();

	for (Segment seg : data) {
		dc.SetPen(wxPen(wxColour(seg.color.R, seg.color.G, seg.color.B)));
		Vector4 pt1, pt2;
		pt1.Set(seg.begin.x, seg.begin.y, seg.begin.z);
		pt2.Set(seg.end.x,   seg.end.y,   seg.end.z  );
		pt1 = Normalize(transform1 * pt1);
		pt2 = Normalize(transform1 * pt2);
		if (pt1.GetZ() > cameraLocation && pt2.GetZ() <= cameraLocation || pt2.GetZ() > cameraLocation && pt1.GetZ() <= cameraLocation) {
			Vector4 temp1 = pt2.GetZ() <= cameraLocation ? pt2 : pt1;
			Vector4 temp2 = pt2.GetZ() <= cameraLocation ? pt1 : pt2;
			double ratio  = abs((cameraLocation - temp1.data[2]) / (temp2.data[2] - temp1.data[2]));
			temp1.data[0] = (temp2.data[0] - temp1.data[0]) * ratio + temp1.data[0];
			temp1.data[1] = (temp2.data[1] - temp1.data[1]) * ratio + temp1.data[1];
			temp1.data[2] = cameraLocation;
			pt1 = Normalize(transform2 * temp1);
			pt2 = Normalize(transform2 * temp2);
		}
		else if (pt1.GetZ() <= cameraLocation && pt2.GetZ() <= cameraLocation) {
			continue;
		}
		else {
			pt1 = Normalize(transform2 * pt1);
			pt2 = Normalize(transform2 * pt2);
		}
		dc.DrawLine(pt1.GetX(), pt1.GetY(), pt2.GetX(), pt2.GetY());
	}
}