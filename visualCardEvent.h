#pragma once
#include "wx\event.h"
#include <wx/wx.h>
#include "Card.h"

// Begin of VisualCardMouseEvent.h
 
// class VisualCard;
 
class VisualCardMouseEvent : public wxMouseEvent
{
  CARD_VECTOR m_cv;
   //  VisualCard* m_vc;

public:
	VisualCardMouseEvent( wxWindow* win = (wxWindow*) NULL, CARD_VECTOR cvTmp = CARD_VECTOR() );
	wxEvent* Clone() const {return new VisualCardMouseEvent(*this);}
  CARD_VECTOR GetCV() { return m_cv; }
 
	DECLARE_DYNAMIC_CLASS( VisualCardMouseEvent )
};

typedef void (wxEvtHandler::*VisualCardEventFunction)(VisualCardMouseEvent&);

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE( myEVT_VISUALCARD_DROPPED_EVENT, 1 )
END_DECLARE_EVENT_TYPES()

#define EVT_VISUALCARD_DROPPED_EVENT(func)                              \
	DECLARE_EVENT_TABLE_ENTRY( myEVT_VISUALCARD_DROPPED_EVENT,      \
		-1,                                    \
		-1,                                    \
		(wxObjectEventFunction)                \
		(VisualCardEventFunction) & func,              \
		(wxObject *) NULL ),
// End of VisualCardMouseEvent.h 

