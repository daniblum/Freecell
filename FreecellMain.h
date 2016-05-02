/***************************************************************
 * Name:      FreecellMain.h
 * Purpose:   Defines Application Frame
 * Author:    Daniel Blumenfeld (daniblum@yahoo.com)
 * Created:   2012-12-06
 * Copyright: Daniel Blumenfeld ()
 * License:
 **************************************************************/

#ifndef FREECELLMAIN_H
#define FREECELLMAIN_H
#include <map>
//(*Headers(FreecellFrame)
#include <wx/menu.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

#include "visualCardEvent.h"
#include "CardMoveEvent.h"
#include "Card.h"
#include "Table.h"

#include "VisualCard.h"
// class VisualCard;

// typedef std::vector< VisualCard* > VC_VEC;

/*
typedef struct histMvmt {
  pileRepresentation fromPile, toPile;
  Card c;
} histMvmt;
*/

typedef std::multimap< size_t, CardPile* > AREA_MAP;

class FreecellFrame: public wxFrame
{
    friend VisualCard;

    std::vector< CardMovement > history;
    std::vector< CardMovement > pendingMovements;

    public:

        FreecellFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~FreecellFrame();
        wxStatusBar* GetStatusBar() { return StatusBar1; }

    private:

        //(*Handlers(FreecellFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnNewGame(wxCommandEvent& event);
        void OnUndo(wxCommandEvent& event);
        void OnRestartGame(wxCommandEvent& event);
        void OnResumeGame(wxCommandEvent& event);
        void OnCheckTable(wxCommandEvent& event);
        void OnRefreshTable(wxCommandEvent& event);
        void OnDelayedCleanupClicked(wxCommandEvent& event);
        void OnStrictDelayedCleanupClicked(wxCommandEvent& event);
        void OnExit(wxCommandEvent& event);

        // void onMouseDown(wxMouseEvent& evt);
        // void onMouseUp(wxMouseEvent& evt);
        // void onMove(wxMouseEvent& evt);
        //*)

        //(*Identifiers(FreecellFrame)
        static const long idMenuNewGame;
        static const long idRestartGame;
        static const long idResume;
        static const long idCheckTable;
        static const long idRefreshTable;
        static const long idMenuUndo;
        static const long idMenuQuit;
        static const long idDelayedCleanup;
        static const long idStrictDelayedCleanup;
        static const long idPickFromFoundation;
        static const long idLazyCounting;
        static const long idSaveState;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)
        static const long ID_BITMAP_BTN;

        static const long ID_MY_WINDOW;

        static const long TP_HORZ = 10;
        static const long TP_VERT = 80;
        static const long TP_HORZ_COL = VisualCard::WIDTH + 10;
        static const long TP_VERT_DISTANCE = 20;

        static const long C_HORZ = 10;
        static const long C_VERT = 10;
        static const long C_HORZ_COL = VisualCard::WIDTH + 5;

        static const long FP_VERT = 10;
        static const long FP_HORZ_COL = VisualCard::WIDTH + 5;
        static const long FP_HORZ = 225;
        bool dragging; // will this solve the problem? There is some kind of hard-to-figure-out bug out there
        // int x,y;
        wxPoint MouseToCardOffset;
        wxPoint AbsMousePos;
        wxPoint AbsFreecellPos;
        wxPoint AbsCardPos;
        wxPoint evtPos;

        //(*Declarations(FreecellFrame)
        wxMenuItem* MenuItem8;
        wxMenuItem* MenuItem7;
        wxMenuItem* MenuItem5;
        wxMenu* Menu3;
        wxMenuItem* MenuItem4;
        wxMenuItem* MenuItem11;
        wxMenuItem* MenuItem13;
        wxMenuItem* MenuItem10;
        wxMenuItem* MenuItem12;
        wxMenuItem* MenuItem3;
        wxStatusBar* StatusBar1;
        wxMenuItem* MenuItem6;
        wxMenuItem* MenuItem9;
        //*)

    protected:
        Table table;
        // VisualCard* cardBeingDragged;
        // underlying table (game data)
        VisualCard *vc[Card::NUM_CARDS_IN_SUITE * Card::NUM_SUITES + 1];
        wxBitmapButton *coaster[Table::NUM_CELLS + Table::NUM_FOUNDATIONPILES + Table::NUM_TABLEAUPILES];
        void InitControls(void);
        void RepaintCards(void);
        VisualCard* getVCForCard(const Card& card) const;

        void DoNewGame(bool shuffle = true);
        void ResumeGame();

        void OnVisualCardDropped( VisualCardMouseEvent& WXUNUSED( event ));
        void OnStrictDelayedCleanupChanged();
        void OnCardMoved( CardMoveEvent& WXUNUSED( event ));
        AREA_MAP ClosestLegitimatePiles( VisualCard* from );
        AREA_MAP CheckPilesForProximity( char pileType, int maxCount, const wxRect& wxRectFrom, Card curCard );
        wxRect GetRectFor( const pileRepresentation& pR );

        wxPoint GetExpectedPosition( Card c );
        wxPoint GetExpectedPosition( const pileRepresentation& pR );
        void AddMvmtToPending( const CardMovement& cm );
        void UpdateStatusBar(void);
        // long const getIdLazyCounting() const { return idLazyCounting; };

        CARD_VECTOR cardsToDrag;

        DECLARE_EVENT_TABLE()
};

#endif // FREECELLMAIN_H
