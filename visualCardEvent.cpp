#include "visualCardEvent.h"

// Begin of VisualCardMouseEvent.cpp
// #include "VisualCardMouseEvent.h"
 
IMPLEMENT_DYNAMIC_CLASS( VisualCardMouseEvent, wxEvent )
DEFINE_EVENT_TYPE( myEVT_VISUALCARD_DROPPED_EVENT )
 
VisualCardMouseEvent::VisualCardMouseEvent( wxWindow* win, CARD_VECTOR cv ): m_cv(cv)
{
	SetEventType( myEVT_VISUALCARD_DROPPED_EVENT );
	SetEventObject( win );
}
