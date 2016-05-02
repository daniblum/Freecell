#include "CardMoveEvent.h"

// Begin of CardMoveEvent.cpp
 
IMPLEMENT_DYNAMIC_CLASS( CardMoveEvent, wxEvent )
DEFINE_EVENT_TYPE( myEVT_CARD_MOVED_EVENT )
 
CardMoveEvent::CardMoveEvent( wxWindow* win, CardMovement cm, VisualCard* vc ): m_cm(cm), m_vc(vc)
{
	SetEventType( myEVT_CARD_MOVED_EVENT );
	SetEventObject( win );
}
