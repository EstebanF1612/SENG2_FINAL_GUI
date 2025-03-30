#ifndef MAINAPP_H
#define MAINAPP_H

#include <wx/wx.h>
#include <wx/listctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include "City.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

class MainApp : public wxApp {
public:
    virtual bool OnInit();
    static bool adminLoggedIn;
    virtual int OnExit();
};

class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title);
    std::vector<City*>& GetCities() { return cities; }
    void UpdateCitiesDisplay();
    void UpdateRiskiestCityDisplay();

private:
    void OnAdminLogin(wxCommandEvent& event);
    void LoadCitiesFromCSV();

    std::vector<City*> cities;
    wxListCtrl* cityList;
    wxStaticText* riskiestCityInfo;
    wxButton* adminLoginBtn;
};

class AdminLoginDialog : public wxDialog {
public:
    AdminLoginDialog(wxWindow* parent, const wxString& title);

private:
    void OnLogin(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);  // New handler

    wxTextCtrl* usernameField;
    wxTextCtrl* passwordField;
};

class AdminDashboard : public wxFrame {
public:
    AdminDashboard(wxWindow* parent, const wxString& title, MainFrame* mainFrame);

private:
    void OnAddCity(wxCommandEvent& event);
    void OnDeleteCity(wxCommandEvent& event);
    void OnModifyCity(wxCommandEvent& event);
    void OnUpdateCSV(wxCommandEvent& event);
    void UpdateCityList();

    MainFrame* mainFrame;
    wxListCtrl* cityList;
};

class AddCityDialog : public wxDialog {
public:
    AddCityDialog(wxWindow* parent, const wxString& title, std::vector<City*>& cities);

private:
    void OnAdd(wxCommandEvent& event);

    std::vector<City*>& cities;
    wxTextCtrl* nameField;
    wxTextCtrl* populationField;
    wxTextCtrl* enemyDistField;
    wxTextCtrl* friendlyDistField;
    wxTextCtrl* enemyPowerField;
    wxTextCtrl* friendlyPowerField;
};

class ModifyCityDialog : public wxDialog {
public:
    ModifyCityDialog(wxWindow* parent, const wxString& title, City* city);

private:
    void OnModify(wxCommandEvent& event);

    City* city;
    wxTextCtrl* nameField;
    wxTextCtrl* populationField;
    wxTextCtrl* enemyDistField;
    wxTextCtrl* friendlyDistField;
    wxTextCtrl* enemyPowerField;
    wxTextCtrl* friendlyPowerField;
};

#endif