#include "App.h"
#include "MainFrame.h"
#include <wx/wx.h>

wxIMPLEMENT_APP(App);

bool App::OnInit() {



	MainFrame* frame = new MainFrame("City Risk App");
	frame->Show();
	frame->SetClientSize(1280, 720);
	frame->Center();
	return true;
}