#include <wx/wx.h>
#include "GUIMyFrame1.h"
#include <vector>
#include <utility>
// UWAGA: TO JEST JEDYNY PLIK, KTORY NALEZY EDYTOWAC **************************
#define R_MIN -2.5f
#define R_MAX 2.5f
#define T_SIZE 500
#define STEP (R_MAX - R_MIN) / T_SIZE

typedef std::pair<wxPoint, wxPoint> Line;
typedef std::vector<Line> Contours;

Contours getContourLines(int x, int y, int code) {
	Contours contours;
	switch (code) {
		case 1:
		case 14:
			contours.push_back(Line(wxPoint(x - 1, y), wxPoint(x, y + 1)));
			break;
		case 2:
		case 13:
			contours.push_back(Line(wxPoint(x + 1, y), wxPoint(x, y + 1)));
			break;
		case 3:
		case 12:
			contours.push_back(Line(wxPoint(x - 1, y), wxPoint(x + 1, y)));
			break;
		case 4:
		case 11:
			contours.push_back(Line(wxPoint(x, y - 1), wxPoint(x + 1, y)));
			break;
		case 5:
			contours.push_back(Line(wxPoint(x + 1, y), wxPoint(x, y + 1)));
			contours.push_back(Line(wxPoint(x - 1, y), wxPoint(x, y - 1)));
			break;
		case 10:
			contours.push_back(Line(wxPoint(x - 1, y), wxPoint(x, y + 1)));
			contours.push_back(Line(wxPoint(x + 1, y), wxPoint(x, y - 1)));
			break;
		case 6:
		case 9:
			contours.push_back(Line(wxPoint(x, y - 1), wxPoint(x, y + 1)));
			break;
		case 7:
		case 8:
			contours.push_back(Line(wxPoint(x - 1, y), wxPoint(x, y - 1)));
			break;
		default:
			break;
	}
	return contours;
}

float shepardWeightFunction(float x, float y, float xk, float yk) {
	return 1 / (pow(x - xk, 2) + pow(y - yk, 2));
}

float shepardInterpolation(float x, float y, int n, float points[100][3]) {
	float sumTop = 0.0f,
		  sumBot = 0.0f,
		  temp;
	
	for (int i = 0; i < n; ++i) {
		if (points[i][0] == x && points[i][1] == y) {
			return points[i][2];
		}
		temp = shepardWeightFunction(x, y, points[i][0], points[i][1]);
		sumTop += temp * points[i][2];
		sumBot += temp;
	}
	return sumTop / sumBot;
}

void GUIMyFrame1::DrawMap(int N, float d[100][3], bool Contour, int MappingType, int NoLevels, bool ShowPoints) {
	wxMemoryDC memDC;
	memDC.SelectObject(MemoryBitmap);
	memDC.SetBackground(*wxWHITE_BRUSH);
	memDC.Clear();
	memDC.SetPen(*wxBLACK_PEN);
	memDC.SetPen(wxPen(*wxBLACK, 0.2));
	static float points[T_SIZE][T_SIZE];
	float minVal, maxVal;
	for (int i = 0; i < T_SIZE; ++i) {
		for (int j = 0; j < T_SIZE; ++j) {
			points[i][j] = shepardInterpolation(R_MIN + i * STEP, R_MAX - j * STEP, N, d);
			if (i == 0 && j == 0) {
				minVal = maxVal = points[0][0];
			}
			else {
				if (points[i][j] > maxVal) {
					maxVal = points[i][j];
				}
				if (points[i][j] < minVal) {
					minVal = points[i][j];
				}
			}
		}
	}

	unsigned char* data = static_cast<unsigned char*>(malloc(T_SIZE * T_SIZE * 3));
	float colorRatio;
	static float levels[T_SIZE][T_SIZE];
	if (MappingType != 0) {
		for (int i = 0; i < T_SIZE; ++i) {
			for (int j = 0; j < T_SIZE; ++j) {
				colorRatio = ((points[i][j] - minVal) / (maxVal - minVal) - 0.5) * 2;
				data[j * T_SIZE * 3 + i * 3 + 0] = MappingType == 1 ? 127 - 127 * colorRatio : MappingType == 2 ? (colorRatio < 0 ? 255 * abs(colorRatio) : 0) : MappingType == 3 ? 127 + 127 * colorRatio : 0;
				data[j * T_SIZE * 3 + i * 3 + 1] = MappingType == 1 ? 0                      : MappingType == 2 ? 255 - 255 * abs(colorRatio)                  : MappingType == 3 ? 127 + 127 * colorRatio : 0;
				data[j * T_SIZE * 3 + i * 3 + 2] = MappingType == 1 ? 127 + 127 * colorRatio : MappingType == 2 ? (colorRatio > 0 ? 255 * abs(colorRatio) : 0) : MappingType == 3 ? 127 + 127 * colorRatio : 0;
			}
		}
		wxImage temp(T_SIZE, T_SIZE, data);
		wxBitmap temp2(temp);
		memDC.DrawBitmap(temp2, 0, 0);
	}

	float step, threshold;
	int pointCode;
	Contours contourList, temp3;
	if (Contour) {
		step = (maxVal - minVal) / (NoLevels + 1);
		for (int i = 1; i <= NoLevels; ++i) {
			threshold = minVal + i * step;
			for (int j = 1; j < T_SIZE - 1; ++j) {
				for (int k = 1; k < T_SIZE - 1; ++k) {
					pointCode = 0;
					pointCode +=  points[j - 1][k + 1] < threshold ? 0 : 1;
					pointCode += (points[j + 1][k + 1] < threshold ? 0 : 1) * 2;
					pointCode += (points[j - 1][k - 1] < threshold ? 0 : 1) * 4;
					pointCode += (points[j + 1][k - 1] < threshold ? 0 : 1) * 8;
					if (pointCode == 5 && points[j][k] < threshold) {
						pointCode = 10;
					}
					else if (pointCode == 10 && points[j][k] < threshold) {
						pointCode = 5;
					}
					temp3 = getContourLines(j, k, pointCode);
					contourList.insert(contourList.end(), temp3.begin(), temp3.end());
				}
			}
		}
		for (Line l : contourList) {
			memDC.DrawLine(l.first, l.second);
		}
	}
	
	int px, py;
	if (ShowPoints) {
		for (int i = 0; i < N; ++i) {
			px = (d[i][0] + 2.5f) * 100;
			py = (d[i][1] - 2.5f) * -100;
			memDC.DrawLine(px - 3, py, px + 4, py);
			memDC.DrawLine(px, py - 3, px, py + 4);
		}
	}

}
