#include "MainFrame.h"
#include <wx/wx.h>

enum IDs {
    ADMINLOG_BUTTON_ID = 2

};

MainFrame::MainFrame(const wxString& title) : wxFrame(nullptr, wxID_ANY, title) {
    // Panels
    
    wxPanel* ListPanel = new wxPanel(this, wxID_ANY, wxPoint(0, 0), wxSize(640, 720));
    ListPanel->SetBackgroundColour(wxColour(89, 194, 218));

    wxPanel* RiskiestCityPanel = new wxPanel(this, wxID_ANY, wxPoint(640, 0), wxSize(640, 360));
    RiskiestCityPanel->SetBackgroundColour(wxColour(89, 170, 190));

    wxPanel* ButtonPanel = new wxPanel(this, wxID_ANY, wxPoint(640, 360), wxSize(640, 360));
    
    //buttons
    wxButton* button = new wxButton(ButtonPanel, ADMINLOG_BUTTON_ID, "Administrator Login", wxPoint(260, 250), wxSize(150, 50));
    button->SetBackgroundColour(wxColour(89, 194, 218));

    // Text
    wxStaticText* RiskiestCityText = new wxStaticText(RiskiestCityPanel, wxID_ANY, "Riskiest City", wxPoint(240, 25), wxDefaultSize, wxALIGN_CENTER_HORIZONTAL | wxALIGN_TOP);
    RiskiestCityText->SetFont(wxFont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    RiskiestCityText->SetForegroundColour(wxColour(0, 0, 0));

    // Binders

}