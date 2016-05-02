#ifndef VISUALCARD_H
#define VISUALCARD_H

#include <wx/wx.h>
#include "Card.h"
class FreecellFrame;

class VisualCard : public wxBitmapButton
{
    public:
        static const long WIDTH = 40;
        static const long HEIGHT = 60;
        VisualCard(FreecellFrame* parent,
                   wxWindowID id,
                   Card& c,
                   const wxBitmap& bm,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxBU_AUTODRAW,
                   const wxValidator& validator = wxDefaultValidator
        );
        static void createBitmap(wxBitmap& bm, const Card& c);
        virtual ~VisualCard();
        /* */
        void onMouseDown(wxMouseEvent& evt);
        void onMouseUp(wxMouseEvent& evt);
        void onMove(wxMouseEvent& evt);
        void onMouseCaptureLost(wxMouseCaptureLostEvent& evt);
        /* */
        void SetCard( const Card& c ) 
        { 
          card = c;
          wxString s(card.GetCardCode());
          SetLabel(s);
        }
        Card* GetCard(void) { return &card; }
        void SlowlyMoveTo(const wxPoint& pt);
        void OnEraseBackGround(wxEraseEvent& event) {};
        // void OnPaint(wxPaintEvent& event);
        virtual const VisualCard* NextCardTo() const;
        operator size_t() const;

    private:
        bool dragging; // will this solve the problem? There is some kind of hard-to-figure-out bug out there
        int x,y;
        FreecellFrame* parent;

    void OnMouseEnterOrLeave(wxMouseEvent& event);

protected:
    // conceptual card this widget is associated with
    Card card;
    const wxBitmap m_bm;
    wxString cardValue;

public:
    DECLARE_EVENT_TABLE()
};

#endif // VISUALCARD_H
