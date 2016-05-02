/***************************************************************
 * Name:      FreecellApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Daniel Blumenfeld (daniblum@yahoo.com)
 * Created:   2012-12-06
 * Copyright: Daniel Blumenfeld ()
 * License:
 **************************************************************/

#include "FreecellApp.h"
// #include "../tinyxml/tinyxml.h"

//(*AppHeaders
#include "FreecellMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(FreecellApp);

bool FreecellApp::OnInit()
{
    //(*AppInitialize
	try{
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	FreecellFrame* Frame = new FreecellFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;
	}
	catch (...) {
		int i = 0;
	}

}
