#include "MainApp.h"

wxIMPLEMENT_APP(MainApp);

bool MainApp::OnInit() {
    MainFrame* frame = new MainFrame("City Risk App");
    frame->SetClientSize(1000, 600);
    frame->Center();
    frame->Show();
    return true;
}

bool MainApp::adminLoggedIn = false;

int MainApp::OnExit() {
    adminLoggedIn = false;  // Reset on exit
    return wxApp::OnExit();
}

MainFrame::MainFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title) {
    // Initialize cityList
    cityList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_SUNKEN);

    // Add columns to cityList
    cityList->InsertColumn(0, "City Name", wxLIST_FORMAT_LEFT, 120);
    cityList->InsertColumn(1, "Population", wxLIST_FORMAT_RIGHT, 100);
    cityList->InsertColumn(2, "Avg Enemy Dist", wxLIST_FORMAT_RIGHT, 120);
    cityList->InsertColumn(3, "Avg Friendly Dist", wxLIST_FORMAT_RIGHT, 120);
    cityList->InsertColumn(4, "Enemy Power", wxLIST_FORMAT_RIGHT, 100);
    cityList->InsertColumn(5, "Friendly Power", wxLIST_FORMAT_RIGHT, 100);
    cityList->InsertColumn(6, "Risk Factor", wxLIST_FORMAT_RIGHT, 100);

    // Initialize other controls
    riskiestCityInfo = new wxStaticText(this, wxID_ANY, "Riskiest City: Not calculated yet",
        wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL);
    riskiestCityInfo->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    adminLoginBtn = new wxButton(this, wxID_ANY, "Admin Login");
    adminLoginBtn->Bind(wxEVT_BUTTON, &MainFrame::OnAdminLogin, this);

    // Layout
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* topSizer = new wxBoxSizer(wxHORIZONTAL);

    topSizer->Add(cityList, 1, wxEXPAND | wxALL, 5);

    wxBoxSizer* rightPanel = new wxBoxSizer(wxVERTICAL);
    rightPanel->Add(new wxStaticText(this, wxID_ANY, "Most Risky City:"), 0, wxALL, 5);
    rightPanel->Add(riskiestCityInfo, 0, wxALL | wxEXPAND, 5);
    rightPanel->AddStretchSpacer();
    rightPanel->Add(adminLoginBtn, 0, wxALL | wxALIGN_RIGHT, 5);

    topSizer->Add(rightPanel, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(topSizer, 1, wxEXPAND);
    SetSizer(mainSizer);

    LoadCitiesFromCSV();
    UpdateCitiesDisplay();
    UpdateRiskiestCityDisplay();
}

void MainFrame::OnAdminLogin(wxCommandEvent& event) {
    if (MainApp::adminLoggedIn) {
        // Already logged in - open dashboard directly
        AdminDashboard* dashboard = new AdminDashboard(this, "Admin Dashboard", this);
        dashboard->Show();
    }
    else {
        // Not logged in - show login dialog
        AdminLoginDialog dlg(this, "Admin Login");
        if (dlg.ShowModal() == wxID_OK) {
            AdminDashboard* dashboard = new AdminDashboard(this, "Admin Dashboard", this);
            dashboard->Show();
        }
    }
}

void MainFrame::LoadCitiesFromCSV() {
    wxString exePath = wxStandardPaths::Get().GetExecutablePath();
    wxString projectRoot = wxFileName(exePath).GetPath(); // Goes to Debug/Release folder

    // Go up one level to project root, then into data folder
    wxString csvPath = wxFileName(projectRoot).GetPathWithSep()
        + "data" + wxFILE_SEP_PATH
        + "SENG2ExcelFile - Sheet1.csv";

    // Debug output - check this in your Output window
    wxLogDebug("Looking for CSV at: %s", csvPath);

    if (!wxFileExists(csvPath)) {
        wxMessageBox("CSV not found at:\n" + csvPath, "Error", wxICON_ERROR);
        return;
    }

    std::ifstream file(csvPath.ToStdString());
    if (!file.is_open()) {
        wxMessageBox("Could not open CSV file", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Clear existing cities
    for (auto city : cities) {
        delete city;
    }
    cities.clear();

    std::string line;
    // Skip the header line
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;

        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        if (tokens.size() >= 6) {
            try {
                City* city = new City(
                    tokens[0],
                    std::stoi(tokens[1]),
                    std::stod(tokens[2]),
                    std::stod(tokens[3]),
                    std::stoi(tokens[4]),
                    std::stoi(tokens[5])
                );
                cities.push_back(city);
            }
            catch (...) {
                // Skip malformed lines
            }
        }
    }

    City::defineRiskFactor(cities);
    file.close();
}

void MainFrame::UpdateCitiesDisplay() {
    cityList->DeleteAllItems();

    for (size_t i = 0; i < cities.size(); i++) {
        City* city = cities[i];
        long index = cityList->InsertItem(i, city->getName());

        cityList->SetItem(index, 1, wxString::Format("%d", city->getPopulation()));
        cityList->SetItem(index, 2, wxString::Format("%.2f", city->getAvgDistEnemyBase()));
        cityList->SetItem(index, 3, wxString::Format("%.2f", city->getAvgDistFriendlyBase()));
        cityList->SetItem(index, 4, wxString::Format("%d", city->getEstEnemyInfantryPower()));
        cityList->SetItem(index, 5, wxString::Format("%d", city->getEstFriendlyInfantryPower()));
        cityList->SetItem(index, 6, wxString::Format("%.4f", city->getRiskFactor()));
    }
}

void MainFrame::UpdateRiskiestCityDisplay() {
    if (cities.empty()) {
        riskiestCityInfo->SetLabel("No cities loaded");
        return;
    }

    City* riskiest = cities[0];
    for (size_t i = 1; i < cities.size(); i++) {
        if (cities[i]->getRiskFactor() > riskiest->getRiskFactor()) {
            riskiest = cities[i];
        }
    }

    std::ostringstream oss;
    oss << "Most Risky City: " << riskiest->getName() << "\n\n";
    oss << "Population: " << riskiest->getPopulation() << "\n";
    oss << "Avg Enemy Dist: " << std::fixed << std::setprecision(2) << riskiest->getAvgDistEnemyBase() << " km\n";
    oss << "Avg Friendly Dist: " << riskiest->getAvgDistFriendlyBase() << " km\n";
    oss << "Enemy Power: " << riskiest->getEstEnemyInfantryPower() << "\n";
    oss << "Friendly Power: " << riskiest->getEstFriendlyInfantryPower() << "\n";
    oss << "Risk Factor: " << std::fixed << std::setprecision(4) << riskiest->getRiskFactor();

    riskiestCityInfo->SetLabel(oss.str());
}

AdminLoginDialog::AdminLoginDialog(wxWindow* parent, const wxString& title)
    : wxDialog(parent, wxID_ANY, title, wxDefaultPosition, wxSize(300, 200),
        wxDEFAULT_DIALOG_STYLE | wxCLOSE_BOX) {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Username and password fields (unchanged)
    wxStaticText* userLabel = new wxStaticText(this, wxID_ANY, "Username:");
    usernameField = new wxTextCtrl(this, wxID_ANY);

    wxStaticText* passLabel = new wxStaticText(this, wxID_ANY, "Password:");
    passwordField = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);

    // Button sizer for login and cancel
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* loginBtn = new wxButton(this, wxID_OK, "Login");
    wxButton* cancelBtn = new wxButton(this, wxID_CANCEL, "Cancel");

    loginBtn->Bind(wxEVT_BUTTON, &AdminLoginDialog::OnLogin, this);
    cancelBtn->Bind(wxEVT_BUTTON, &AdminLoginDialog::OnCancel, this);

    buttonSizer->Add(loginBtn, 0, wxALL, 5);
    buttonSizer->Add(cancelBtn, 0, wxALL, 5);

    // Add controls to main sizer
    mainSizer->Add(userLabel, 0, wxALL, 5);
    mainSizer->Add(usernameField, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(passLabel, 0, wxALL, 5);
    mainSizer->Add(passwordField, 0, wxEXPAND | wxALL, 5);
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxALL, 10);

    SetSizer(mainSizer);
    Layout();
}

void AdminLoginDialog::OnLogin(wxCommandEvent& event) {
    if (usernameField->GetValue() == "admin" && passwordField->GetValue() == "1234") {
        MainApp::adminLoggedIn = true;
        EndModal(wxID_OK);
    }
    else {
        wxMessageBox("Invalid username or password", "Login Failed", wxOK | wxICON_ERROR);
    }
}

void AdminLoginDialog::OnCancel(wxCommandEvent& event) {
    EndModal(wxID_CANCEL);
}

AdminDashboard::AdminDashboard(wxWindow* parent, const wxString& title, MainFrame* mainFrame)
    : wxFrame(parent, wxID_ANY, title, wxDefaultPosition, wxSize(900, 600)), mainFrame(mainFrame) {
    cityList = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxLC_REPORT | wxLC_SINGLE_SEL | wxBORDER_SUNKEN);

    cityList->InsertColumn(0, "City Name", wxLIST_FORMAT_LEFT, 120);
    cityList->InsertColumn(1, "Population", wxLIST_FORMAT_RIGHT, 100);
    cityList->InsertColumn(2, "Avg Enemy Dist", wxLIST_FORMAT_RIGHT, 120);
    cityList->InsertColumn(3, "Avg Friendly Dist", wxLIST_FORMAT_RIGHT, 120);
    cityList->InsertColumn(4, "Enemy Power", wxLIST_FORMAT_RIGHT, 100);
    cityList->InsertColumn(5, "Friendly Power", wxLIST_FORMAT_RIGHT, 100);
    cityList->InsertColumn(6, "Risk Factor", wxLIST_FORMAT_RIGHT, 100);
    cityList->InsertColumn(7, "Actions", wxLIST_FORMAT_LEFT, 200);

    wxButton* addCityBtn = new wxButton(this, wxID_ANY, "Add City");
    addCityBtn->Bind(wxEVT_BUTTON, &AdminDashboard::OnAddCity, this);

    wxButton* updateCSVBtn = new wxButton(this, wxID_ANY, "Update CSV");
    updateCSVBtn->Bind(wxEVT_BUTTON, &AdminDashboard::OnUpdateCSV, this);

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    buttonSizer->Add(addCityBtn, 0, wxALL, 5);
    buttonSizer->Add(updateCSVBtn, 0, wxALL, 5);

    mainSizer->Add(cityList, 1, wxEXPAND | wxALL, 5);
    mainSizer->Add(buttonSizer, 0, wxALIGN_RIGHT | wxALL, 5);

    SetSizer(mainSizer);
    UpdateCityList();
}

void AdminDashboard::OnAddCity(wxCommandEvent& event) {
    AddCityDialog dlg(this, "Add New City", mainFrame->GetCities());
    if (dlg.ShowModal() == wxID_OK) {
        City::defineRiskFactor(mainFrame->GetCities());
        UpdateCityList();
        mainFrame->UpdateCitiesDisplay();
        mainFrame->UpdateRiskiestCityDisplay();
    }
}

void AdminDashboard::OnDeleteCity(wxCommandEvent& event) {
    int index = event.GetId() - wxID_HIGHEST;
    if (index >= 0 && index < static_cast<int>(mainFrame->GetCities().size())) {
        delete mainFrame->GetCities()[index];
        mainFrame->GetCities().erase(mainFrame->GetCities().begin() + index);
        City::defineRiskFactor(mainFrame->GetCities());
        UpdateCityList();
        mainFrame->UpdateCitiesDisplay();
        mainFrame->UpdateRiskiestCityDisplay();
    }
}

void AdminDashboard::OnModifyCity(wxCommandEvent& event) {
    int index = event.GetId() - wxID_HIGHEST - 1000;
    if (index >= 0 && index < static_cast<int>(mainFrame->GetCities().size())) {
        ModifyCityDialog dlg(this, "Modify City", mainFrame->GetCities()[index]);
        if (dlg.ShowModal() == wxID_OK) {
            City::defineRiskFactor(mainFrame->GetCities());
            UpdateCityList();
            mainFrame->UpdateCitiesDisplay();
            mainFrame->UpdateRiskiestCityDisplay();
        }
    }
}

void AdminDashboard::OnUpdateCSV(wxCommandEvent& event) {
    wxMessageDialog dialog(this, "Update default CSV file or create new one?", "Update CSV",
        wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxICON_QUESTION);
    dialog.SetYesNoCancelLabels("Default", "New File", "Cancel");

    int result = dialog.ShowModal();

    if (result == wxID_CANCEL) {
        return;
    }

    wxString filename;
    if (result == wxID_YES) {
        filename = "SENG2ExcelFile - Sheet1.csv";
    }
    else {
        wxTextEntryDialog nameDlg(this, "Enter filename (must end with .csv):", "New CSV File");
        if (nameDlg.ShowModal() != wxID_OK) {
            return;
        }
        filename = nameDlg.GetValue();
        if (!filename.Lower().EndsWith(".csv")) {
            filename += ".csv";
        }
    }

    // Create data directory if it doesn't exist
    wxString dataDir = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath()
        + wxFILE_SEP_PATH + "data";

    if (!wxDirExists(dataDir)) {
        if (!wxMkdir(dataDir)) {
            wxMessageBox("Failed to create data directory!", "Error", wxOK | wxICON_ERROR);
            return;
        }
    }

    wxString fullPath = dataDir + wxFILE_SEP_PATH + filename;

    // Try to open the file
    std::ofstream file(fullPath.ToStdString());
    if (!file.is_open()) {
        wxMessageBox("Could not open file for writing at:\n" + fullPath,
            "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Write CSV header
    file << "City Name,Population,Average Distance to Enemy Base,"
        << "Average Distance to Friendly Base,Estimated Enemy Infantry Power,"
        << "Estimated Friendly Infantry Power\n";

    // Write city data
    for (const City* city : mainFrame->GetCities()) {
        file << city->getName() << ","
            << city->getPopulation() << ","
            << city->getAvgDistEnemyBase() << ","
            << city->getAvgDistFriendlyBase() << ","
            << city->getEstEnemyInfantryPower() << ","
            << city->getEstFriendlyInfantryPower() << "\n";
    }

    file.close();
    wxMessageBox("CSV file created successfully at:\n" + fullPath,
        "Success", wxOK | wxICON_INFORMATION);
}

void AdminDashboard::UpdateCityList() {
    cityList->DeleteAllItems();

    for (size_t i = 0; i < mainFrame->GetCities().size(); i++) {
        City* city = mainFrame->GetCities()[i];
        long index = cityList->InsertItem(i, city->getName());

        cityList->SetItem(index, 1, wxString::Format("%d", city->getPopulation()));
        cityList->SetItem(index, 2, wxString::Format("%.2f", city->getAvgDistEnemyBase()));
        cityList->SetItem(index, 3, wxString::Format("%.2f", city->getAvgDistFriendlyBase()));
        cityList->SetItem(index, 4, wxString::Format("%d", city->getEstEnemyInfantryPower()));
        cityList->SetItem(index, 5, wxString::Format("%d", city->getEstFriendlyInfantryPower()));
        cityList->SetItem(index, 6, wxString::Format("%.4f", city->getRiskFactor()));

        wxButton* deleteBtn = new wxButton(cityList, wxID_HIGHEST + i, "Delete");
        wxButton* modifyBtn = new wxButton(cityList, wxID_HIGHEST + 1000 + i, "Modify");

        deleteBtn->Bind(wxEVT_BUTTON, &AdminDashboard::OnDeleteCity, this);
        modifyBtn->Bind(wxEVT_BUTTON, &AdminDashboard::OnModifyCity, this);

        wxRect rect;
        cityList->GetItemRect(index, rect);

        wxPoint pos = wxPoint(rect.GetRight() - 210, rect.GetTop() + 2);
        deleteBtn->SetPosition(pos);
        deleteBtn->SetSize(80, rect.GetHeight() - 4);

        pos.x += 85;
        modifyBtn->SetPosition(pos);
        modifyBtn->SetSize(80, rect.GetHeight() - 4);
    }
}

AddCityDialog::AddCityDialog(wxWindow* parent, const wxString& title, std::vector<City*>& citiesRef)
    : wxDialog(parent, wxID_ANY, title), cities(citiesRef) {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 5, 5);

    gridSizer->Add(new wxStaticText(this, wxID_ANY, "City Name:"));
    nameField = new wxTextCtrl(this, wxID_ANY);
    gridSizer->Add(nameField, 1, wxEXPAND);

    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Population:"));
    populationField = new wxTextCtrl(this, wxID_ANY);
    gridSizer->Add(populationField, 1, wxEXPAND);

    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Avg Enemy Distance:"));
    enemyDistField = new wxTextCtrl(this, wxID_ANY);
    gridSizer->Add(enemyDistField, 1, wxEXPAND);

    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Avg Friendly Distance:"));
    friendlyDistField = new wxTextCtrl(this, wxID_ANY);
    gridSizer->Add(friendlyDistField, 1, wxEXPAND);

    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Enemy Power:"));
    enemyPowerField = new wxTextCtrl(this, wxID_ANY);
    gridSizer->Add(enemyPowerField, 1, wxEXPAND);

    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Friendly Power:"));
    friendlyPowerField = new wxTextCtrl(this, wxID_ANY);
    gridSizer->Add(friendlyPowerField, 1, wxEXPAND);

    gridSizer->AddGrowableCol(1, 1);

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* addBtn = new wxButton(this, wxID_OK, "Add");
    wxButton* cancelBtn = new wxButton(this, wxID_CANCEL, "Cancel");

    addBtn->Bind(wxEVT_BUTTON, &AddCityDialog::OnAdd, this);

    buttonSizer->Add(addBtn, 0, wxALL, 5);
    buttonSizer->Add(cancelBtn, 0, wxALL, 5);

    mainSizer->Add(gridSizer, 1, wxEXPAND | wxALL, 10);
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxBOTTOM, 10);

    SetSizerAndFit(mainSizer);
}

void AddCityDialog::OnAdd(wxCommandEvent& event) {
    try {
        std::string name = nameField->GetValue().ToStdString();
        int population = std::stoi(populationField->GetValue().ToStdString());
        double enemyDist = std::stod(enemyDistField->GetValue().ToStdString());
        double friendlyDist = std::stod(friendlyDistField->GetValue().ToStdString());
        int enemyPower = std::stoi(enemyPowerField->GetValue().ToStdString());
        int friendlyPower = std::stoi(friendlyPowerField->GetValue().ToStdString());

        City* city = new City(name, population, enemyDist, friendlyDist, enemyPower, friendlyPower);
        cities.push_back(city);

        EndModal(wxID_OK);
    }
    catch (...) {
        wxMessageBox("Invalid input values", "Error", wxOK | wxICON_ERROR);
    }
}

ModifyCityDialog::ModifyCityDialog(wxWindow* parent, const wxString& title, City* cityToModify)
    : wxDialog(parent, wxID_ANY, title), city(cityToModify) {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    wxFlexGridSizer* gridSizer = new wxFlexGridSizer(2, 5, 5);

    gridSizer->Add(new wxStaticText(this, wxID_ANY, "City Name:"));
    nameField = new wxTextCtrl(this, wxID_ANY, city->getName());
    gridSizer->Add(nameField, 1, wxEXPAND);

    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Population:"));
    populationField = new wxTextCtrl(this, wxID_ANY, wxString::Format("%d", city->getPopulation()));
    gridSizer->Add(populationField, 1, wxEXPAND);

    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Avg Enemy Distance:"));
    enemyDistField = new wxTextCtrl(this, wxID_ANY, wxString::Format("%.2f", city->getAvgDistEnemyBase()));
    gridSizer->Add(enemyDistField, 1, wxEXPAND);

    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Avg Friendly Distance:"));
    friendlyDistField = new wxTextCtrl(this, wxID_ANY, wxString::Format("%.2f", city->getAvgDistFriendlyBase()));
    gridSizer->Add(friendlyDistField, 1, wxEXPAND);

    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Enemy Power:"));
    enemyPowerField = new wxTextCtrl(this, wxID_ANY, wxString::Format("%d", city->getEstEnemyInfantryPower()));
    gridSizer->Add(enemyPowerField, 1, wxEXPAND);

    gridSizer->Add(new wxStaticText(this, wxID_ANY, "Friendly Power:"));
    friendlyPowerField = new wxTextCtrl(this, wxID_ANY, wxString::Format("%d", city->getEstFriendlyInfantryPower()));
    gridSizer->Add(friendlyPowerField, 1, wxEXPAND);

    gridSizer->AddGrowableCol(1, 1);

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* modifyBtn = new wxButton(this, wxID_OK, "Modify");
    wxButton* cancelBtn = new wxButton(this, wxID_CANCEL, "Cancel");

    modifyBtn->Bind(wxEVT_BUTTON, &ModifyCityDialog::OnModify, this);

    buttonSizer->Add(modifyBtn, 0, wxALL, 5);
    buttonSizer->Add(cancelBtn, 0, wxALL, 5);

    mainSizer->Add(gridSizer, 1, wxEXPAND | wxALL, 10);
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxBOTTOM, 10);

    SetSizerAndFit(mainSizer);
}

void ModifyCityDialog::OnModify(wxCommandEvent& event) {
    try {
        city->setName(nameField->GetValue().ToStdString());
        city->setPopulation(std::stoi(populationField->GetValue().ToStdString()));
        city->setAvgDistEnemyBase(std::stod(enemyDistField->GetValue().ToStdString()));
        city->setAvgDistFriendlyBase(std::stod(friendlyDistField->GetValue().ToStdString()));
        city->setEstEnemyInfantryPower(std::stoi(enemyPowerField->GetValue().ToStdString()));
        city->setEstFriendlyInfantryPower(std::stoi(friendlyPowerField->GetValue().ToStdString()));

        EndModal(wxID_OK);
    }
    catch (...) {
        wxMessageBox("Invalid input values", "Error", wxOK | wxICON_ERROR);
    }
}