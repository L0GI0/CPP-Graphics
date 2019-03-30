#include <wx/wxprec.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/checkbox.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/frame.h>
#include <wx/dcbuffer.h>
#include <array>
#include "FreeImage.h"
#include "CImg.h"

class MyFrame : public wxFrame {
	private:
		void WxButtonLoad_Click    (wxCommandEvent&  e);
		void WxButtonCenzura_Click (wxCommandEvent&  e);
		void WxButtonErode_Click   (wxCommandEvent&  e);
		void WxCheckBox_Click      (wxCommandEvent&  e);
		void Draw();
		void Form_Update           (wxUpdateUIEvent& e);

		enum {
			ID_WXBUTTON_LOAD    = 1001,
			ID_WXBUTTON_CENZURA = 1002,
			ID_WXBUTTON_ERODE   = 1003,
			ID_WXCHECKBOX       = 1004,
			ID_WXTEXTCTRL       = 1005,
		};

		wxButton*   m_buttonLoad;
		wxButton*   m_buttonCenzura;
		wxButton*   m_buttonErode;
		wxCheckBox* m_checkBox;
		wxTextCtrl* m_textCtrl;
		wxPanel*    m_panel;

		wxImage     m_original;
		wxImage*    m_image;
		wxBitmap    m_buffer;

		wxString    m_metadata;

		std::array<wxBitmap, 720> m_animations;
		int                       m_tick;

	public:
		MyFrame (wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(600, 400), long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
		~MyFrame();
};

MyFrame::MyFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style) {
	this -> SetSizeHints(wxSize(-1, -1), wxDefaultSize);

	wxBoxSizer* bSizer1;
	wxBoxSizer* bSizer2;
	wxBoxSizer* bSizer3;
	bSizer1 = new wxBoxSizer(wxHORIZONTAL);
	bSizer2 = new wxBoxSizer(wxVERTICAL);
	bSizer3 = new wxBoxSizer(wxHORIZONTAL);

	bSizer2 -> SetMinSize(wxSize(200, 350));

	m_buttonLoad    = new wxButton(this, ID_WXBUTTON_LOAD, wxT("Wczytaj obrazek"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2        -> Add(m_buttonLoad, 0, wxALIGN_CENTER | wxALL, 5);

	m_buttonCenzura = new wxButton(this, ID_WXBUTTON_CENZURA, wxT("Cenzura"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2        -> Add(m_buttonCenzura, 0, wxALIGN_CENTER | wxALL, 5);

	m_buttonErode   = new wxButton(this, ID_WXBUTTON_ERODE, wxT("Erode"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2        -> Add(m_buttonErode, 0, wxALIGN_CENTER | wxALL, 5);

	m_checkBox      = new wxCheckBox(this, ID_WXCHECKBOX, wxT("Animacja"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2        -> Add(m_checkBox, 0, wxALIGN_CENTER | wxALL, 5);

	m_textCtrl      = new wxTextCtrl(this, ID_WXTEXTCTRL, wxEmptyString, wxDefaultPosition, wxSize(290, -1), wxTE_RICH | wxTE_READONLY | wxTE_MULTILINE | wxTE_WORDWRAP);
	m_textCtrl     -> SetMaxSize(wxSize(300, -1));

	bSizer2        -> Add(m_textCtrl, 1, wxALL | wxEXPAND, 5);

	bSizer1        -> Add(bSizer2, 0, wxALL | wxEXPAND, 0);

	m_panel         = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(833, -1), wxTAB_TRAVERSAL);
	m_panel        -> SetMaxSize(wxSize(-1, -1));
	m_panel        -> SetBackgroundColour(wxColour(0, 0, 0));

	bSizer3 -> Add(m_panel, 1, wxEXPAND, 5);

	bSizer1 -> Add(bSizer3, 1, wxEXPAND, 5);


	this -> SetSizer(bSizer1);
	this -> Layout();
	this -> Centre(wxBOTH);

	Bind(wxEVT_BUTTON,    &MyFrame::WxButtonLoad_Click,    this, ID_WXBUTTON_LOAD);
	Bind(wxEVT_BUTTON,    &MyFrame::WxButtonCenzura_Click, this, ID_WXBUTTON_CENZURA);
	Bind(wxEVT_BUTTON,    &MyFrame::WxButtonErode_Click,   this, ID_WXBUTTON_ERODE);
	Bind(wxEVT_CHECKBOX,  &MyFrame::WxCheckBox_Click,      this, ID_WXCHECKBOX);
	Bind(wxEVT_UPDATE_UI, &MyFrame::Form_Update,           this);

	m_image  = new wxImage();
	m_image -> AddHandler(new wxJPEGHandler);

	m_metadata = "";

	m_tick = 0;
	
}

MyFrame::~MyFrame() {
	delete m_buttonLoad;
	delete m_buttonCenzura;
	delete m_buttonErode;
	delete m_checkBox;
	delete m_textCtrl;
	delete m_panel;
}

wxString GetMetadata(FIBITMAP *dib, FREE_IMAGE_MDMODEL model) {
	FITAG *tag = NULL;
	FIMETADATA *mdhandle = NULL;
	wxString meta = "";
	mdhandle = FreeImage_FindFirstMetadata(model, dib, &tag);

	if (mdhandle) {
		do {
			meta << FreeImage_GetTagKey(tag) << ": " << FreeImage_TagToString(model, tag) << "\r\n";
		} while (FreeImage_FindNextMetadata(mdhandle, &tag));
	}

	FreeImage_FindCloseMetadata(mdhandle);
	return meta;
}


wxString GetAllMetadata(FIBITMAP* dib) {
	wxString meta = "";
	meta << "Rozmiar obrazka: " << FreeImage_GetWidth(dib) << "*" << FreeImage_GetHeight(dib) << "\r\n";
	meta << "EXIF Info:" << "\r\n";
	if (FreeImage_GetMetadataCount(FIMD_EXIF_MAIN, dib)) {
		meta << GetMetadata(dib, FIMD_EXIF_MAIN);
	}
	/*
	if (FreeImage_GetMetadataCount(FIMD_COMMENTS, dib)) {
		meta << GetMetadata(dib, FIMD_COMMENTS);
	}
	if (FreeImage_GetMetadataCount(FIMD_EXIF_EXIF, dib)) {
		meta << GetMetadata(dib, FIMD_EXIF_EXIF);
	}
	if (FreeImage_GetMetadataCount(FIMD_EXIF_GPS, dib)) {
		meta << GetMetadata(dib, FIMD_EXIF_GPS);
	}
	if (FreeImage_GetMetadataCount(FIMD_EXIF_INTEROP, dib)) {
		meta << GetMetadata(dib, FIMD_EXIF_INTEROP);
	}
	if (FreeImage_GetMetadataCount(FIMD_EXIF_MAKERNOTE, dib)) {
		FITAG *tagMake = NULL;
		FreeImage_GetMetadata(FIMD_EXIF_MAIN, dib, "Make", &tagMake);

		std::string buffer((char*)FreeImage_GetTagValue(tagMake));
		buffer += " Makernote";

		meta << GetMetadata(dib, FIMD_EXIF_MAKERNOTE);
	}
	if (FreeImage_GetMetadataCount(FIMD_IPTC, dib)) {
		meta << GetMetadata(dib, FIMD_IPTC);
	}
	if (FreeImage_GetMetadataCount(FIMD_GEOTIFF, dib)) {
		meta << GetMetadata(dib, FIMD_GEOTIFF);
	}
	*/
	return meta;
}

void MyFrame::WxButtonLoad_Click(wxCommandEvent& WXUNUSED(e)) {
	wxFileDialog loadFileDialog(this, "Prosz\u0119 wybra\u0107 zdj\u0119cie", "", "", "JPG files (*.jpg)|*.jpg", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (loadFileDialog.ShowModal() == wxID_CANCEL)
		return;

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib;
	fif = FreeImage_GetFileType(loadFileDialog.GetPath(), 0);
	if (fif == FIF_UNKNOWN) {
		fif = FreeImage_GetFIFFromFilename(loadFileDialog.GetPath());
	}
	if ((fif != FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fif)) {
		dib = FreeImage_Load(fif, loadFileDialog.GetPath());
	}
	else {
		return;
	}
	m_metadata =  GetAllMetadata(dib);
	m_textCtrl -> Clear();
	m_textCtrl -> WriteText(m_metadata);
	m_image ->LoadFile(loadFileDialog.GetPath());
	m_original = m_image -> Copy();
}

void MyFrame::WxButtonCenzura_Click(wxCommandEvent& WXUNUSED(e)) {
	if (m_original.IsOk()) {
		wxSize imageSize = m_original.GetSize();
		wxPoint fragmentPos(460, 20);
		wxSize fragmentSize(120, 80);
		unsigned char* data = m_original.GetData();
		cimg_library::CImg<float> CImg(imageSize.x, imageSize.y, 1, 3);
		float* CImgData = CImg.data();
		for (int y = 0; y < imageSize.y; ++y) {
			for (int x = 0; x < imageSize.x; ++x) {
				CImgData[imageSize.x * imageSize.y * 0 + imageSize.x * y + x] = data[3 * (imageSize.x * y + x) + 0];
				CImgData[imageSize.x * imageSize.y * 1 + imageSize.x * y + x] = data[3 * (imageSize.x * y + x) + 1];
				CImgData[imageSize.x * imageSize.y * 2 + imageSize.x * y + x] = data[3 * (imageSize.x * y + x) + 2];
			}
		}
		CImg.blur(3.5);
		unsigned char* newData = static_cast<unsigned char*>(malloc(imageSize.x * imageSize.y * 3 * sizeof(unsigned char)));
		for (int y = 0; y < imageSize.y; ++y) {
			for (int x = 0; x < imageSize.x; ++x) {
				if (fragmentPos.x <= x && x <= (fragmentPos.x + fragmentSize.x) && fragmentPos.y <= y && y <= (fragmentPos.y + fragmentSize.y)) {
					newData[3 * (imageSize.x * y + x) + 0] = CImgData[imageSize.x * imageSize.y * 0 + imageSize.x * y + x];
					newData[3 * (imageSize.x * y + x) + 1] = CImgData[imageSize.x * imageSize.y * 1 + imageSize.x * y + x];
					newData[3 * (imageSize.x * y + x) + 2] = CImgData[imageSize.x * imageSize.y * 2 + imageSize.x * y + x];
				}
				else {
					newData[3 * (imageSize.x * y + x) + 0] = data[3 * (imageSize.x * y + x) + 0];
					newData[3 * (imageSize.x * y + x) + 1] = data[3 * (imageSize.x * y + x) + 1];
					newData[3 * (imageSize.x * y + x) + 2] = data[3 * (imageSize.x * y + x) + 2];
				}
			}
		}
		m_image -> SetData(newData, imageSize.x, imageSize.y);
	}
}

void MyFrame::WxButtonErode_Click(wxCommandEvent& WXUNUSED(e)) {
	if (m_original.IsOk()) {
		wxSize imageSize = m_original.GetSize();
		unsigned char* data = m_original.GetData();
		cimg_library::CImg<float> CImg(imageSize.x, imageSize.y, 1, 3);
		float* CImgData = CImg.data();
		for (int y = 0; y < imageSize.y; ++y) {
			for (int x = 0; x < imageSize.x; ++x) {
				CImgData[imageSize.x * imageSize.y * 0 + imageSize.x * y + x] = data[3 * (imageSize.x * y + x) + 0];
				CImgData[imageSize.x * imageSize.y * 1 + imageSize.x * y + x] = data[3 * (imageSize.x * y + x) + 1];
				CImgData[imageSize.x * imageSize.y * 2 + imageSize.x * y + x] = data[3 * (imageSize.x * y + x) + 2];
			}
		}
		CImg.erode(6);
		unsigned char* newData = static_cast<unsigned char*>(malloc(imageSize.x * imageSize.y * 3 * sizeof(unsigned char)));
		for (int y = 0; y < imageSize.y; ++y) {
			for (int x = 0; x < imageSize.x; ++x) {
				newData[3 * (imageSize.x * y + x) + 0] = CImgData[imageSize.x * imageSize.y * 0 + imageSize.x * y + x];
				newData[3 * (imageSize.x * y + x) + 1] = CImgData[imageSize.x * imageSize.y * 1 + imageSize.x * y + x];
				newData[3 * (imageSize.x * y + x) + 2] = CImgData[imageSize.x * imageSize.y * 2 + imageSize.x * y + x];
			}
		}
		m_image -> SetData(newData, imageSize.x, imageSize.y);
	}
}

void MyFrame::WxCheckBox_Click(wxCommandEvent& WXUNUSED(e)) {
	if (m_original.IsOk() && m_checkBox -> IsChecked()) {
		wxSize imageSize = m_original.GetSize();
		cimg_library::CImg<float> tempOrig(imageSize.x, imageSize.y, 1, 3);
		unsigned char* data = m_original.GetData();
		float* CImgData = tempOrig.data();
		#pragma omp parallel
		#pragma omp for
		for (int y = 0; y < imageSize.y; ++y) {
			for (int x = 0; x < imageSize.x; ++x) {
				CImgData[imageSize.x * imageSize.y * 0 + imageSize.x * y + x] = data[3 * (imageSize.x * y + x) + 0];
				CImgData[imageSize.x * imageSize.y * 1 + imageSize.x * y + x] = data[3 * (imageSize.x * y + x) + 1];
				CImgData[imageSize.x * imageSize.y * 2 + imageSize.x * y + x] = data[3 * (imageSize.x * y + x) + 2];
			}
		}
		float* spec = new float[3]{ 1.0f, 1.0f, 1.0f };
		#pragma omp parallel
		#pragma omp for schedule(dynamic, 16)
		for (int i = 720; i >= 0; --i) {
			cimg_library::CImg<float> gauss(imageSize.x, imageSize.y, 1, 3);
			cimg_library::CImg<float> gauss2(imageSize.x, imageSize.y, 1, 3);
			cimg_library::CImg<float> temp(tempOrig);
			float* CImgData = temp.data();
			gauss.draw_gaussian  (imageSize.x / 2 + (imageSize.x * 0.4) * cos(  i / 2               * acos(-1) / 180.0),
				                  imageSize.y / 2 + (imageSize.y * 0.4) * sin(  i / 2               * acos(-1) / 180.0), 150.0f, spec, 1.0f);
			gauss2.draw_gaussian (imageSize.x / 2 + (imageSize.x * 0.4) * cos(((i / 2 + 180) % 360) * acos(-1) / 180.0),
				                  imageSize.y / 2 + (imageSize.y * 0.4) * sin(((i / 2 + 180) % 360) * acos(-1) / 180.0), 150.0f, spec, 1.0f);
			gauss += gauss2;
			temp.mul(gauss);
			unsigned char* newData = static_cast<unsigned char*>(malloc(imageSize.x * imageSize.y * 3 * sizeof(unsigned char)));
			wxImage animation;
			for (int y = 0; y < imageSize.y; ++y) {
				for (int x = 0; x < imageSize.x; ++x) {
					newData[3 * (imageSize.x * y + x) + 0] = CImgData[imageSize.x * imageSize.y * 0 + imageSize.x * y + x];
					newData[3 * (imageSize.x * y + x) + 1] = CImgData[imageSize.x * imageSize.y * 1 + imageSize.x * y + x];
					newData[3 * (imageSize.x * y + x) + 2] = CImgData[imageSize.x * imageSize.y * 2 + imageSize.x * y + x];
				}
			}
			animation.SetData(newData, imageSize.x, imageSize.y, true);
			animation.Rescale(m_panel->GetSize().x, m_panel->GetSize().y);
			m_animations[i] = wxBitmap(animation);
			delete newData;
		}
	}
}

void MyFrame::Draw() {
	wxClientDC _MyDC(m_panel);
	wxSize XY = m_panel -> GetSize();
	if (XY.x > 0 && XY.y > 0) {
		m_buffer = wxBitmap(XY.x, XY.y);
		wxBufferedDC MyDC(&_MyDC, m_buffer);
		if (m_image -> IsOk()) {
			if (m_checkBox -> IsChecked() && m_animations.size() > 0) {
				MyDC.DrawBitmap(m_animations[m_tick], 0, 0);
				m_tick = (m_tick + 1) % 720;
				Update();
				RefreshRect(wxRect(1, 1, 1, 1), false);
			}
			else {
				wxImage temp(*m_image);
				temp.Rescale(XY.x, XY.y);
				MyDC.DrawBitmap(wxBitmap(temp), 0, 0);
			}
		}
	}
}

void MyFrame::Form_Update(wxUpdateUIEvent& WXUNUSED(e)) {
	Draw();
}

class MyApp : public wxApp {
	public:
		virtual bool OnInit() {
			MyFrame *frame = new MyFrame(NULL, wxID_ANY, "GFK Lab 08 Wiktor H\u0142adki", wxDefaultPosition, wxSize(1116, 577));
			frame -> Show(true);
			SetTopWindow(frame);
			return true;
		}
};

wxIMPLEMENT_APP(MyApp);
//wxIMPLEMENT_APP_CONSOLE(MyApp);