#ifndef __GUIMyFrame1__
#define __GUIMyFrame1__
#include "GUI.h"
#include "vecmat.h"

#include <wx/filedlg.h>
#include <wx/dcmemory.h>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <wx/colourdata.h>
#include <wx/colordlg.h>

class GUIMyFrame1 : public MyFrame1
{
	protected:
		void WxPanel_Repaint( wxUpdateUIEvent& event );
		void m_button_load_geometry_click( wxCommandEvent& event );
		void Scrolls_Updated( wxScrollEvent& event );
		double cameraLocation = -2.0;
	public:
		GUIMyFrame1( wxWindow* parent );
		void Repaint();
		Vector4 Normalize(Vector4 vector);
		Matrix4 GenerateTransform1();
		Matrix4 GenerateTransform2();
};

#endif // __GUIMyFrame1__
