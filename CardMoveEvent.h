#pragma once
#include "wx\event.h"

// Begin of CardMoveEvent.h
 
#include <wx/wx.h>
#include "CardMovement.h"
// #include "pileRepresentation.h"
class VisualCard;
 
class CardMoveEvent : public wxCommandEvent
{
    CardMovement m_cm;
    VisualCard* m_vc;

public:
	CardMoveEvent( wxWindow* win = (wxWindow*) NULL, CardMovement cm = CardMovement(), VisualCard* vc = NULL );
	wxEvent* Clone() const {return new CardMoveEvent(*this);}
  CardMovement GetCardMovement() { return m_cm; }
  VisualCard* GetVisualCard() { return m_vc; }
 
	DECLARE_DYNAMIC_CLASS( CardMoveEvent )
};

typedef void (wxEvtHandler::*CardMoveEventFunction)(CardMoveEvent&);

BEGIN_DECLARE_EVENT_TYPES()
	DECLARE_EVENT_TYPE( myEVT_CARD_MOVED_EVENT, 1 )
END_DECLARE_EVENT_TYPES()

#define EVT_CARD_MOVED_EVENT(func)                              \
	DECLARE_EVENT_TABLE_ENTRY( myEVT_CARD_MOVED_EVENT,      \
		-1,                                    \
		-1,                                    \
		(wxObjectEventFunction)                \
		(CardMoveEventFunction) & func,              \
		(wxObject *) NULL ),
// End of CardMoveEvent.h 

