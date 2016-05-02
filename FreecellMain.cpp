/***************************************************************
 * Name:      FreecellMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Daniel Blumenfeld (daniblum@yahoo.com)
 * Created:   2012-12-06
 * Copyright: Daniel Blumenfeld ()
 * License:
 **************************************************************/

#include <vld.h>
#include <wx/msgdlg.h>
#include <wx/gdicmn.h>
#include <wx/fontdlg.h>
//(*InternalHeaders(FreecellFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include "FreecellMain.h"
#include "VisualCard.h"

#include "Table.h"
#include "pileRepresentation.h"
#include "visualCardEvent.h"
#include "ConfigData.h"

// DEFINE_EVENT_TYPE(EVT_VC_LEFT_UP)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(FreecellFrame)
const long FreecellFrame::idMenuNewGame = wxNewId();
const long FreecellFrame::idRestartGame = wxNewId();
const long FreecellFrame::idResume = wxNewId();
const long FreecellFrame::idCheckTable = wxNewId();
const long FreecellFrame::idRefreshTable = wxNewId();
const long FreecellFrame::idMenuUndo = wxNewId();
const long FreecellFrame::idMenuQuit = wxNewId();
const long FreecellFrame::idDelayedCleanup = wxNewId();
const long FreecellFrame::idStrictDelayedCleanup = wxNewId();
const long FreecellFrame::idPickFromFoundation = wxNewId();
const long FreecellFrame::idLazyCounting = wxNewId();
const long FreecellFrame::idSaveState = wxNewId();
const long FreecellFrame::idMenuAbout = wxNewId();
const long FreecellFrame::ID_STATUSBAR1 = wxNewId();
//*)
const long FreecellFrame::ID_BITMAP_BTN = wxNewId();
const long  FreecellFrame::ID_MY_WINDOW = wxNewId();

BEGIN_EVENT_TABLE(FreecellFrame,wxFrame)
    //(*EventTable(FreecellFrame)
    // EVT_LEFT_DOWN(FreecellFrame::onMouseDown    )
    // EVT_LEFT_UP  (FreecellFrame::onMouseUp      )
    // EVT_MOTION   (FreecellFrame::onMove         )
	EVT_VISUALCARD_DROPPED_EVENT( FreecellFrame::OnVisualCardDropped )
    EVT_CARD_MOVED_EVENT( FreecellFrame::OnCardMoved )
    //*)
END_EVENT_TABLE()

void FreecellFrame::OnCardMoved( CardMoveEvent& vcEvent )
{
    CardMovement cm = vcEvent.GetCardMovement();
    for ( size_t count = 0; count < cm.fromCard.size(); ++count ) {
      // get origin and destination positions
      Card origPrev = cm.fromCard[count];
      VisualCard* vcDest = vc[origPrev];
      wxPoint destination;
      {
          const Card* dest = vcDest->GetCard();
          if (dest) {
              destination = GetExpectedPosition( *dest );
          } else {
              wxRect wr = GetRectFor( cm.to );
              destination = wr.GetPosition() + wxPoint(0, TP_VERT_DISTANCE * int(count) );
          }
      }
      vcDest->SlowlyMoveTo(destination);
      vcDest->Raise();
      size_t destPrev = table.GetCardPrevToTop(cm.to);
      if ( destPrev ) {
          // vc[destPrev]->Disable();
      }
    }
    Refresh();
}

void FreecellFrame::OnVisualCardDropped( VisualCardMouseEvent& vcEvent)
{
    // Determine pileRepresentation...
  CARD_VECTOR cvFrom = vcEvent.GetCV();
  int positionInPile;
  Card cardFrom = cvFrom[0];
  CardPile* from = (CardPile*) table.GetWhereCardIs(cardFrom, positionInPile);
  AREA_MAP toVector = ClosestLegitimatePiles(vc[cardFrom]);
  bool couldDoIt = false;
  bool IsReadyForFinal = false;
  bool lazyCounting = Menu3->IsChecked(idLazyCounting);
  pileRepresentation& prFrom = from->GetPileRep();
  for ( AREA_MAP::reverse_iterator it = toVector.rbegin(); it != toVector.rend(); ++it ) {
    CardPile* to = it->second;
    pileRepresentation& prTo = to->GetPileRep();
    couldDoIt = table.MoveCard( cardsToDrag, prTo, false, lazyCounting );
    if ( couldDoIt ) {
      IsReadyForFinal = table.updateClearedForFinal();
      CardMovement cm( cardsToDrag, prFrom, prTo );
      history.push_back( cm );
      AddMvmtToPending( cm );
      break; // don't check the rest of the possible candidates
    } // couldDoIt
  }
  if (!couldDoIt) {
    CardMovement cm(cardsToDrag, prFrom, prFrom);
    AddMvmtToPending( cm );
  }
  /*
  if ( IsReadyForFinal ) { // warn user
    wxMessageBox( "You Win !!!" );
  }
  */
  if ( IsReadyForFinal || !( Menu3->IsChecked( idDelayedCleanup ) ) ) {
    CardMovement* pcm;
    for( pcm = table.CanMoveToFoundation(); pcm; pcm = table.CanMoveToFoundation() ) {
      _ASSERT (table.MoveCard( pcm->fromCard, pcm->to, false, lazyCounting ));
      history.push_back( *pcm );
      AddMvmtToPending( *pcm );
      delete pcm;
    }
  }
  if ( table.isFinished() ) {
      // send event?
      wxCommandEvent evt( wxEVT_COMMAND_MENU_SELECTED, idMenuNewGame );
      this->AddPendingEvent(evt);
  }
  Refresh();
}

void FreecellFrame::UpdateStatusBar(void)
{
  wxString s;
  s.Printf( _("M:%d %d"), AbsMousePos.x, AbsMousePos.y);
  StatusBar1->SetStatusText(s, 0);
  s.Printf( _("F:%d %d"), AbsFreecellPos.x, AbsFreecellPos.y);
  StatusBar1->SetStatusText(s, 1);
  s.Printf( _("C:%d %d"), AbsCardPos.x, AbsCardPos.y);
  StatusBar1->SetStatusText(s, 2);
  s.Printf( _("E:%d %d"), evtPos.x, evtPos.y);
  StatusBar1->SetStatusText(s, 3);

}

void FreecellFrame::AddMvmtToPending( const CardMovement& cm )
{
    CardMoveEvent cme( this, cm, NULL );
    ProcessEvent( cme );
}

// End of MyFrame.cpp
FreecellFrame::FreecellFrame(wxWindow* parent,wxWindowID id): dragging(false)
{
    //(*Initialize(FreecellFrame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxMenuBar* MenuBar1;
    wxMenu* Menu2;
    ConfigData conf("MenuData.cfg");


    Create(parent, id, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxMINIMIZE_BOX|wxSTATIC_BORDER, _T("id"));
    SetClientSize(wxSize(410,650));
    SetBackgroundColour(wxColour(0,128,64));
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu1, idMenuNewGame, _("New Game\tF2"), _("Start a new game"), wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem5 = new wxMenuItem(Menu1, idRestartGame, _("Restart Game\tCtrl-R"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem5);
    MenuItem6 = new wxMenuItem(Menu1, idResume, _("Resume Previous Game"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem6);
    MenuItem7 = new wxMenuItem(Menu1, idCheckTable, _("Check Table\tCtrl-A"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem7);
    MenuItem8 = new wxMenuItem(Menu1, idRefreshTable, _("Refresh Table"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem8);
    MenuItem4 = new wxMenuItem(Menu1, idMenuUndo, _("Undo\tCtrl-Z"), wxEmptyString, wxITEM_NORMAL);
    Menu1->Append(MenuItem4);
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu3 = new wxMenu();
    MenuItem9 = new wxMenuItem(Menu3, idDelayedCleanup, _("Delayed Cleanup"), _("Only send cards to foundation automatically if all tableau piles are sorted"), wxITEM_CHECK);
    Menu3->Append(MenuItem9);
    MenuItem9->Check(conf.getBool(ConfigData::DELAYED_CLEANUP));
    MenuItem12 = new wxMenuItem(Menu3, idStrictDelayedCleanup, _("Strict"), _("Delayed Cleanup is strict (i.e. only if sorting is strict)"), wxITEM_CHECK);
    Menu3->Append(MenuItem12);
    MenuItem12->Check(true);
    MenuItem12->Check(conf.getBool(ConfigData::STRICT_DELAYED_CLEANUP));
    MenuItem10 = new wxMenuItem(Menu3, idPickFromFoundation, _("Pick from Foundation"), wxEmptyString, wxITEM_CHECK);
    Menu3->Append(MenuItem10);
    MenuItem11 = new wxMenuItem(Menu3, idLazyCounting, _("\'Lazy\' empty pile counting"), wxEmptyString, wxITEM_CHECK);
    Menu3->Append(MenuItem11);
    MenuItem13 = new wxMenuItem(Menu3, idSaveState, _("Save State"), wxEmptyString, wxITEM_CHECK);
    Menu3->Append(MenuItem13);
    MenuBar1->Append(Menu3, _("Settings"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[4] = { -75, -75, -75, -75 };
    int __wxStatusBarStyles_1[4] = { wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL };
    StatusBar1->SetFieldsCount(4,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(4,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);

    Connect(idMenuNewGame,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&FreecellFrame::OnNewGame);
    Connect(idRestartGame,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&FreecellFrame::OnRestartGame);
    Connect(idResume,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&FreecellFrame::OnResumeGame);
    Connect(idCheckTable,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&FreecellFrame::OnCheckTable);
    Connect(idMenuUndo,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&FreecellFrame::OnUndo);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&FreecellFrame::OnQuit);
    Connect(idStrictDelayedCleanup,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&FreecellFrame::OnStrictDelayedCleanupClicked);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&FreecellFrame::OnAbout);
    //*)
    InitControls();
}

void FreecellFrame::InitControls(void)
{
    for (int s = int(Spades); s <= int(Diamonds); ++s ) {
        for ( int n = 1; n <= 13; ++n ) {
            size_t pos = s*13 + n;
            wxBitmap bitmap(VisualCard::WIDTH, VisualCard::HEIGHT);
            Card c(n, Suit(s));
            VisualCard::createBitmap(bitmap , c);
            vc[pos] = new VisualCard( this, wxNewId(), c, bitmap, wxPoint(10,10), wxSize(VisualCard::WIDTH, VisualCard::HEIGHT) );
            vc[pos]->SetCard(c);
            vc[pos]->Hide();
        }
    }
    for (int idx = 0; idx < Table::NUM_CELLS; ++idx) {
        // + Table::NUM_FOUNDATIONPILES + Table::NUM_TABLEAUPILES; ++idx ) {
        pileRepresentation pr('C', idx+1);
        wxRect wr = GetRectFor(pr);
        wxBitmap bitmap(wr.GetSize().GetX(), wr.GetSize().GetY());
        wxMemoryDC dc;
        dc.SetLogicalScale(2.0, 2.0);
        dc.SelectObject( bitmap );
        dc.SetBackground(*wxLIGHT_GREY);
        dc.Clear();
        dc.SelectObject(wxNullBitmap);
        coaster[idx] = new wxBitmapButton(this, wxNewId(), bitmap, GetExpectedPosition(pr), wr.GetSize());
        coaster[idx]->Lower();
        // coaster[idx]->Disable();
    }

    for (int idx = 0; idx < Table::NUM_FOUNDATIONPILES; ++idx) {
        // + Table::NUM_FOUNDATIONPILES + Table::NUM_TABLEAUPILES; ++idx ) {
        pileRepresentation pr('F', idx+1);
        wxRect wr = GetRectFor(pr);
        wxBitmap bitmap(wr.GetSize().GetX(), wr.GetSize().GetY());
        wxMemoryDC dc;
        dc.SetLogicalScale(2.0, 2.0);
        dc.SelectObject( bitmap );
        dc.SetBackground(*wxLIGHT_GREY);
        dc.Clear();
        dc.SelectObject(wxNullBitmap);
        coaster[idx+Table::NUM_CELLS] = new wxBitmapButton(this, wxNewId(), bitmap, GetExpectedPosition(pr), wr.GetSize());
        coaster[idx]->Lower();
        // coaster[idx]->Disable();
    }

    for (int idx = 0; idx < Table::NUM_TABLEAUPILES; ++idx) {
        // + Table::NUM_FOUNDATIONPILES + Table::NUM_TABLEAUPILES; ++idx ) {
        pileRepresentation pr('T', idx+1);
        wxRect wr = GetRectFor(pr);
        wxBitmap bitmap(wr.GetSize().GetX(), wr.GetSize().GetY());
        wxMemoryDC dc;
        dc.SetLogicalScale(2.0, 2.0);
        dc.SelectObject( bitmap );
        dc.SetBackground(*wxLIGHT_GREY);
        dc.Clear();
        dc.SelectObject(wxNullBitmap);
        coaster[idx+Table::NUM_CELLS+Table::NUM_FOUNDATIONPILES] = new wxBitmapButton(this, wxNewId(), bitmap, GetExpectedPosition(pr), wr.GetSize());
        coaster[idx]->Lower();
        // coaster[idx]->Disable();
    }

    DoNewGame();
}

FreecellFrame::~FreecellFrame()
{
  if ( Menu3->IsChecked(idSaveState)) {
    table.Serialize();
  }
  ConfigData conf("MenuData.cfg");
  conf.setBool(ConfigData::DELAYED_CLEANUP, Menu3->IsChecked(idDelayedCleanup));
  conf.setBool(ConfigData::STRICT_DELAYED_CLEANUP, Menu3->IsChecked(idStrictDelayedCleanup));
  conf.setBool(ConfigData::SAVE_STATE, Menu3->IsChecked(idSaveState));
}

void FreecellFrame::OnQuit(wxCommandEvent& event)
{
  // this->table.Serialize(); // _("Partial"));
    Close();
}

void FreecellFrame::OnExit(wxCommandEvent& event)
{
  // this->table.Serialize(); // _("Partial"));
    Close();
}

void FreecellFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void FreecellFrame::OnDelayedCleanupClicked(wxCommandEvent& event)
{
  bool isChecked = Menu3->IsChecked( idDelayedCleanup );
  wxMenuItem* strictD = Menu3->FindChildItem( idStrictDelayedCleanup );
  strictD->Enable( isChecked );
  if (!isChecked) {
    strictD->Check( false );
    OnStrictDelayedCleanupChanged();
  }
}

void FreecellFrame::OnStrictDelayedCleanupClicked(wxCommandEvent& event)
{
  OnStrictDelayedCleanupChanged();
}

void FreecellFrame::OnStrictDelayedCleanupChanged()
{
  table.SetstrictClearCount( Menu3->IsChecked( idStrictDelayedCleanup ) );
  table.updateClearedForFinal( true );
}


VisualCard* FreecellFrame::getVCForCard(const Card& card) const
{
    size_t pos = card;
    return vc[pos];
}


void FreecellFrame::RepaintCards(void)
{
    int count;

    table.updateClearedForFinal(true);
    for (count = 0; count < Table::NUM_CELLS; ++count) {
        pileRepresentation start('C', count+1); // = pileRepresentation::createIfInRange(pileS);
        if( start.Validate() ) {
          CardPile* cp = (CardPile*) table.GetCardPile( start );
          // coaster[count]->Disable(); // Raise(); // Show(!c);
          for (Card c = cp->GetFirstCard(); c; c = cp->GetNextCard()) {
            // all we need to do is re-position the card
            VisualCard* curVC = vc[c];
            curVC->SetCard(c);
            wxPoint point = GetExpectedPosition(c);
            // c->SetVisualCard( curVC );
            curVC->Hide();
            curVC->SetPosition( point );
            curVC->Raise();
            curVC->Layout();
            curVC->Show();
            // curVC->Disable();
          }
        } else {
          _ASSERT(false);
        }
    } // cell loop

    for (count = 0; count < Table::NUM_FOUNDATIONPILES; ++count) {
        pileRepresentation start ('F', count+1);
        if( start.Validate() ) {
          CardPile* cp = (CardPile*) table.GetCardPile( start );
          // coaster[count + Table::NUM_CELLS]->Disable(); // Show(!c);
          for (Card c = cp->GetFirstCard(); c; c = cp->GetNextCard()) {
            wxPoint point = GetExpectedPosition(c);
            // all we need to do is re-position the card
            VisualCard* curVC = vc[c];
            curVC->SetCard(c);
            curVC->SetPosition( point );
            curVC->Raise();
            curVC->Show();
            // curVC->Disable();
          }
        } else {
          _ASSERT(false);
        }
    } // foundation pile loop

    for (count = 0; count < Table::NUM_TABLEAUPILES; ++count) {
        pileRepresentation start ('T', count+1);
        if( start.Validate() ) {
          CardPile* cp = (CardPile*) table.GetCardPile( start );
          // coaster[count + Table::NUM_CELLS + Table::NUM_FOUNDATIONPILES ]->Disable(); // Show(!c);
          for (Card c = cp->GetFirstCard(); c; c = cp->GetNextCard()) {
            wxPoint point = GetExpectedPosition(c);
            // all we need to do is re-position the card
            VisualCard* curVC = vc[c];
            curVC->SetCard(c);
            curVC->SetPosition( point );
            curVC->Raise();
            curVC->Show();
            // curVC->Disable();
          }
        } else {
          _ASSERT(false);
        }
    } // tableau pile loop
    // Enable();
    Refresh();
}

void FreecellFrame::DoNewGame(bool shuffle)
{
    history.clear();
    table.StartGame(shuffle);
    // table.Serialize(_("Table"));
    RepaintCards();
}


void FreecellFrame::ResumeGame()
{
    history.clear();
    wxString pickedFile;
    do {
      pickedFile = wxFileSelector ( wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, wxEmptyString, _("??????????????.xml") );
    } while ( pickedFile.IsEmpty() );
    table.LoadFromXML( pickedFile );
    RepaintCards();
}


void FreecellFrame::OnNewGame(wxCommandEvent& event)
{
    Enable();
    const wxString options[] = { L"Exit", L"New Game", L"Restart" };
    wxSingleChoiceDialog wxChoice( this, L"What do you want to do next?", L"Next Step", 3, options );
    wxChoice.SetSelection(1);
    int sel = wxChoice.ShowModal();
    if ( wxID_OK == sel ) {
        sel = wxChoice.GetSelection();
        switch (sel) {
            case 0:
                Close();
                break;

            case 1:
                DoNewGame();
                break;

            case 2:
                DoNewGame(false);
                break;
        }
        return;
    }

}

wxPoint FreecellFrame::GetExpectedPosition( const pileRepresentation& pR )
{
    char group = pR.GetGroup();
    int idx = pR.GetIdx()-1;
    wxPoint point;
    switch (group) {
        case 'C':
            point = wxPoint( C_HORZ + C_HORZ_COL * idx, C_VERT );
            break;

        case 'T':
            point = wxPoint( TP_HORZ + TP_HORZ_COL * idx, TP_VERT );
            break;

        case 'F':
            point = wxPoint( FP_HORZ + FP_HORZ_COL * idx, FP_VERT );
            break;

    }
    return point;
}


wxRect FreecellFrame::GetRectFor( const pileRepresentation& pR )
{
    wxPoint point = GetExpectedPosition(pR);
    wxRect retVal( point, wxSize(VisualCard::WIDTH, VisualCard::HEIGHT) );
    return retVal;
}


AREA_MAP FreecellFrame::CheckPilesForProximity( char pileType, int maxCount, const wxRect& wxRectFrom, Card curCard )
{
    AREA_MAP retVal;
    for (int count = 0; count < maxCount; ++count) {
        pileRepresentation start (pileType, count+1);
        if( start.Validate() ) {
            CardPile* cp = (CardPile*) table.GetCardPile( start );
            Card c = cp->GetCardOnTop();
            if ( c && (c == curCard) ) {
                // wxLogStatus(L"Card is same as original - skipping.");
                continue;
            }
            wxRect wxRectThis;
            if (c) {
                wxRectThis = vc[c]->GetRect();
            } else {
                wxRectThis = GetRectFor(start);
            }
            wxRect rect = wxRectFrom.Intersect(wxRectThis);
            size_t area = rect.GetHeight() * rect.GetWidth();
            if ( area ) {
              retVal.insert(AREA_MAP::value_type( area, cp ) );
              // retVal.push_back(cp);
            }
        } else {
            _ASSERT(false);
        }
    } // table loop
    return retVal;
}

AREA_MAP FreecellFrame::ClosestLegitimatePiles( VisualCard* from )
{
    CardPile* cp = 0;
    AREA_MAP retVal;

    wxRect wxRectFrom = from->GetRect();
    Card* curCard = from->GetCard();

    AREA_MAP tmpRet;
    tmpRet = CheckPilesForProximity('C', Table::NUM_CELLS, wxRectFrom, *curCard);
    retVal.insert(tmpRet.begin(), tmpRet.end());
    tmpRet = CheckPilesForProximity('F', Table::NUM_FOUNDATIONPILES, wxRectFrom, *curCard);
    retVal.insert(tmpRet.begin(), tmpRet.end());
    tmpRet = CheckPilesForProximity('T', Table::NUM_TABLEAUPILES, wxRectFrom, *curCard);
    retVal.insert(tmpRet.begin(), tmpRet.end());
    return retVal;
}

void FreecellFrame::OnUndo(wxCommandEvent& event)
{
    if (!history.empty()) {
        CardMovement* hi = &(history.back());
        // pileRepresentation from = hi->to;
        pileRepresentation to = hi->from;
        if ( table.MoveCard( hi->fromCard, to, true, true ) ) {
            history.pop_back();
            // Update position !!!
            RepaintCards();
        }
    }
}

wxPoint FreecellFrame::GetExpectedPosition( Card c )
{
    size_t pos = c;
    const Table::WhereInTable* whereData = &(table.CardPosition[c]);
    long horiz = 0;
    long vert = 0;
    int pileNbr = whereData->pile.GetIdx()-1;
    switch( whereData->pile.GetGroup() ) {
        case 'C':
            horiz = C_HORZ + pileNbr * C_HORZ_COL;
            vert = C_VERT;
            break;

        case 'T':
            horiz = TP_HORZ + pileNbr * TP_HORZ_COL;
            vert = TP_VERT + whereData->posInPile * TP_VERT_DISTANCE;
            break;

        case 'F':
            horiz = FP_HORZ + pileNbr * FP_HORZ_COL;
            vert = FP_VERT;
            break;
    }
    return wxPoint(horiz, vert);
}

void FreecellFrame::OnRestartGame(wxCommandEvent& event)
{
    DoNewGame(false);
}

void FreecellFrame::OnResumeGame(wxCommandEvent& event)
{
    ResumeGame();
}

void FreecellFrame::OnCheckTable(wxCommandEvent& event)
{
  for ( size_t i = 1; i < sizeof( table.CardPosition ) / sizeof ( table.CardPosition[0] ); ++i )
  {
    Table::WhereInTable* wit = &(table.CardPosition[i]);
    wit = wit;
  }
  /*
  // check maintenance
  bool cardFound[sizeof(table.CardPosition) sizeof(table.CardPosition[0])];
  */

}

void FreecellFrame::OnRefreshTable(wxCommandEvent& event)
{
  RepaintCards();
}

/*
void FreecellFrame::onMouseDown(wxMouseEvent& evt)
{
  wxPoint positionInFrame = evt.GetPosition();
  AbsCardPos = wxPoint(0,0);
  AbsMousePos = wxGetMousePosition();
  wxPoint cliArea = GetClientAreaOrigin ();
  wxMenuBar *mb = GetMenuBar();
  wxPoint mbPos = mb->GetPosition();
  wxPoint screenPos = GetScreenPosition();
  AbsFreecellPos = screenPos;
  positionInFrame = wxPoint( AbsMousePos - AbsFreecellPos  );
  UpdateStatusBar();
  wxString s;
  for ( size_t idx = 1; idx < sizeof( vc ) / sizeof( vc[0] ); ++ idx ) {
    VisualCard* vCard = vc[idx];
    wxRect rect = vCard->GetRect();
    bool isCard = rect.Contains(positionInFrame);
    CARD_VECTOR VectorOfCards;
    if (isCard) {
      Card* cardFrom = vCard->GetCard();
      {
        Card nextCard = table.NextCardTo( *cardFrom );
        if (nextCard) {
          // adjust size
          rect.SetHeight( TP_VERT_DISTANCE );
          isCard = rect.Contains(positionInFrame);
        }

        if (isCard) { // keep adding to the vector and checking if cards on top are in succession
          VectorOfCards.push_back(*cardFrom);
          Card curCard = *cardFrom;
          Card next = nextCard;
          while (isCard && next) {
            Colour cc = curCard.getColour();
            Colour nc = next.getColour();
            isCard = ( cc != nc ) && ( curCard.getNumber() == next.getNumber() + 1 );
            if (isCard) {
              VectorOfCards.push_back(next);
              curCard = next;
              next = table.NextCardTo( curCard );
            } else {
              break;
            }
          }
        }
        // check max nbr of cards to be dragged
        if (VectorOfCards.size() > table.maxNbrCardsToDrag() ) {
          isCard = false;
        }
      }
      if (isCard) {
        // see if it is legitimate to move this card
        Card* cardFrom = vCard->GetCard();
        Card nextCard = table.NextCardTo( *cardFrom );
        isCard = true;
        if (isCard) {
          dragging=true;
          {
            cardsToDrag = VectorOfCards;
          }
          CaptureMouse();
          AbsCardPos = vCard->GetPosition();
          evtPos = evt.GetPosition();
          MouseToCardOffset = evtPos - AbsCardPos;
          UpdateStatusBar();
        }
      }
      s.Append(vCard->GetLabel());
      s.Append(_(","));
    }
  }
}

void FreecellFrame::onMove(wxMouseEvent& evt)
{
  if(!dragging) {
    return;
  }
  AbsMousePos = wxGetMousePosition();
  AbsFreecellPos = GetPosition();
  AbsCardPos = vc[cardsToDrag[0]]->GetPosition();
  evtPos = evt.GetPosition();
  UpdateStatusBar();
  wxPoint PosToMove = evtPos - MouseToCardOffset;
  for ( size_t pos = 0; pos < cardsToDrag.size(); ++pos ) {
    vc[cardsToDrag[pos]]->Move( PosToMove.x , PosToMove.y + TP_VERT_DISTANCE * int(pos) );
    vc[cardsToDrag[pos]]->Refresh();
  }
  // cardBeingDragged->Move( PosToMove ); // wxPoint( toCardXY.x, toCardXY.y ) ) ;
}

void FreecellFrame::onMouseUp(wxMouseEvent& evt)
{
  if(!dragging) {
    return;
  }
  AbsMousePos = wxGetMousePosition();
  AbsFreecellPos = GetPosition();
  AbsCardPos = vc[cardsToDrag[0]]->GetPosition();
  evtPos = evt.GetPosition();
  UpdateStatusBar();
  // *
  // wxPoint absPos = wxGetMousePosition();
  // wxPoint newXYInFreeCellPos = wxPoint(absPos.x-xyMouse.x, absPos.y-xyMouse.y);
  // *
  ReleaseMouse();
  dragging=false;
  VisualCardMouseEvent vcEvent( this, cardsToDrag );
  AddPendingEvent(vcEvent);
}
*/
