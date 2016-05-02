#include <math.h>
#include "CardMovement.h"
#include "VisualCard.h"
#include "FreecellMain.h"
#include "visualCardEvent.h"

BEGIN_EVENT_TABLE(VisualCard,wxBitmapButton  )
    EVT_LEFT_DOWN(VisualCard::onMouseDown    )
    EVT_LEFT_UP  (VisualCard::onMouseUp      )
    EVT_MOTION   (VisualCard::onMove         )
    EVT_MOUSE_CAPTURE_LOST(VisualCard::onMouseCaptureLost )
    EVT_ENTER_WINDOW(VisualCard::OnMouseEnterOrLeave)
    EVT_LEAVE_WINDOW(VisualCard::OnMouseEnterOrLeave)
END_EVENT_TABLE()

VisualCard::VisualCard(FreecellFrame* parent,
                   wxWindowID id,
                   Card& c,
                   const wxBitmap& bm,
                   const wxPoint& pos,
                   const wxSize& size,
                   long style,
                   const wxValidator& validator) : 
        wxBitmapButton(parent, id, bm, pos, size, style, validator, wxT("Drag me around"))
            , card(c), m_bm(bm)
        {
    //ctor
    VisualCard::parent = parent;
    dragging = false;
    // c.getNbrText(cardValue);
    // cardValue += L" ";
    // wxString tmp;
    // c.getSuitText(tmp);
    // cardValue += tmp;
    // SetToolTip(cardValue);
    SetCard(c);
    SetLabel( wxString(c.GetCardCode()) );
    cardValue = c.GetCardCode();
    SetToolTip( c.GetCardCode() );

}

void VisualCard::OnMouseEnterOrLeave(wxMouseEvent& event)
{
    // this->GetTa
    // event.Skip();
}

VisualCard::~VisualCard()
{
    //dtor
}

void VisualCard::createBitmap(wxBitmap& bm, const Card& c)
{
    // create the bitmap
    wxMemoryDC dc;
    dc.SetLogicalScale(2.0, 2.0);
    dc.SelectObject( bm );
    dc.SetPen(*wxGREY_PEN);
    dc.SetBackground(*wxLIGHT_GREY);
    dc.Clear();
    dc.DrawRoundedRectangle(0, 0, VisualCard::WIDTH/2.0, VisualCard::HEIGHT/2.0, 3.0);
    wxFont font( 36, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, _T("Lucida Sans Unicode") );
    switch( c.getColour() ) {

        case black:
            dc.SetTextForeground( *wxBLACK );
            break;

        case red:
            dc.SetTextForeground( *wxRED );
            break;

    }
    wchar_t suitList[4] = {0x2660 , 0x2663, 0x2665, 0x2666};
    wxString showText;
    c.getNbrText(showText);
    size_t pos = c.getSuit();
    dc.DrawText( showText, 2, 3 );
    wxChar st = suitList[pos];
    dc.DrawRotatedText( st, 10, 0, 0.0 ); 
    dc.SelectObject( wxNullBitmap );
}
void VisualCard::onMouseDown(wxMouseEvent& evt)
{
  // parent->onMouseDown(evt);
  CARD_VECTOR VectorOfCards;
  VectorOfCards.push_back(card);
  Card nextCard = parent->table.NextCardTo( card );
  Card next = nextCard;
  Card curCard = card;
  bool isCard = true;
  while (isCard && next) {
    Colour cc = curCard.getColour();
    Colour nc = next.getColour();
    isCard = ( cc != nc ) && ( curCard.getNumber() == next.getNumber() + 1 );
    if (isCard) {
      VectorOfCards.push_back(next);
      curCard = next;
      next = parent->table.NextCardTo( curCard );
    } else {
      break;
    }
  }
  // check max nbr of cards to be dragged
  if (VectorOfCards.size() > parent->table.maxNbrCardsToDrag( 0, parent->Menu3->IsChecked(parent->idLazyCounting /*getIdLazyCounting()*/ ) ) ) {
    isCard = false;
  }
  if (isCard) {
    dragging=true;
    parent->cardsToDrag = VectorOfCards;
    for ( size_t pos = 0; pos < parent->cardsToDrag.size(); ++pos ) {
      parent->vc[parent->cardsToDrag[pos]]->Raise();
      // vc[cardsToDrag[pos]]->Refresh();
    }
    CaptureMouse();
    x = evt.GetX();
    y = evt.GetY();
  }
  /*
    dragging=true;
    CaptureMouse();
    x = evt.GetX();
    y = evt.GetY();
    Lower();
    
    const VisualCard* vc = this->NextCardTo();
    vc = vc;
    */
    //evt.Veto();
}

void VisualCard::onMouseUp(wxMouseEvent& evt)
{
  // parent->onMouseUp(evt);
  /* */
    if (!dragging) {
        return;
    }
    wxPoint mouseOnScreen = wxGetMousePosition();
    int newx = mouseOnScreen.x - x;
    int newy = mouseOnScreen.y - y;
    ReleaseMouse();
    dragging=false;
    VisualCardMouseEvent vcEvent( parent, parent->cardsToDrag );
    parent->AddPendingEvent(vcEvent);
    /* */
}

void VisualCard::onMove(wxMouseEvent& evt)
{
  // parent->onMove(evt);
  /* */
    if(dragging)
    {
      wxPoint mouseOnScreen = wxGetMousePosition();
      wxPoint PosToMove( mouseOnScreen.x - x, mouseOnScreen.y - y );
      wxPoint ptm = parent->ScreenToClient( PosToMove );
      for ( size_t pos = 0; pos < parent->cardsToDrag.size(); ++pos ) {
        parent->vc[parent->cardsToDrag[pos]]->Move( ptm.x, ptm.y + parent->TP_VERT_DISTANCE * int(pos) );
        // vc[cardsToDrag[pos]]->Refresh();
      }
    }
    /* */
}

void VisualCard::onMouseCaptureLost(wxMouseCaptureLostEvent& evt)
{
  dragging = false;
}


/*
void VisualCard::OnPaint(wxPaintEvent& event)
{
  wxBitmapButton::OnPaint(event);
}
*/

void VisualCard::SlowlyMoveTo(const wxPoint& pt)
{
    parent->Disable();
    wxPoint MicroMovedPoint = GetPosition();
    double curPosY = MicroMovedPoint.y;
    double curPosX = MicroMovedPoint.x;
    double TotalYDisplacement = pt.y - MicroMovedPoint.y;
    double TotalXDisplacement = pt.x - MicroMovedPoint.x;
    const double MaxMicroDisp = 50;
    double TotalDisplacement = sqrt( 1.0 * TotalYDisplacement * TotalYDisplacement + TotalXDisplacement * TotalXDisplacement );
    double XScale = TotalXDisplacement / TotalDisplacement;
    double YScale = TotalYDisplacement / TotalDisplacement;
    double maxMicroXDisp = MaxMicroDisp * XScale;
    int signX = ( maxMicroXDisp > 0 ) ? 1 : -1 ;
    maxMicroXDisp *= signX;
    if (maxMicroXDisp < 1) maxMicroXDisp = 1;
    double maxMicroYDisp = MaxMicroDisp * YScale;
    int signY = ( maxMicroYDisp > 0 ) ? 1 : -1 ;
    maxMicroYDisp *= signY;
    Raise();
    while ( MicroMovedPoint != pt ) {
        double YDisplacement = abs(pt.y - curPosY);
        double XDisplacement = abs(pt.x - curPosX);
        double newXDisp = signX * std::min<double>( XDisplacement, maxMicroXDisp );
        double newYDisp = signY * std::min<double>( YDisplacement, maxMicroYDisp );
        curPosX += newXDisp;
        curPosY += newYDisp;
        MicroMovedPoint.x = curPosX;
        MicroMovedPoint.y = curPosY;
        Move(MicroMovedPoint);
        wxMicroSleep( 2000 );
        wxYield();
    }
    parent->Enable();
}
/* */

const VisualCard* VisualCard::NextCardTo() const
{
  const Table& t = this->parent->table;
  size_t c = t.NextCardTo(this->card);
  if (c) {
    return parent->vc[c];
  }
  return 0;
}

VisualCard::operator size_t() const
{ 
  return card;
}
