#pragma once

#include <vector>
#include <wx/wx.h>
#include <wx/string.h>
// #include "../tinyxml/tinyxml.h" // pseudo-serialization
#include <wx/xml/xml.h>

typedef enum Suit {
    SuitUnknown = -1,
    Spades,
    Clubs,
    Hearts,
    Diamonds,
} Suit;

typedef enum Colour {
    red,
    black
} Colour;

class Card
{
public:
    static const int NUM_SUITES = 4;
    static const int NUM_CARDS_IN_SUITE = 13;

private:
    static const char* suitNames[NUM_SUITES];
    int number;
    Suit suit;
    // wxString cardCode;
    void SetSuitAndNumber( Suit s, int nbr ) { suit = s; number = nbr;}
    void SetSuitAndNumber( size_t nbr );
    bool Validate() const;

public:
  Card(void): number(0), suit(SuitUnknown) {}
  Card(int nbr, Suit s): number(nbr), suit(s) {}
  Card(size_t nbr);
  // Card( const Card& other ) { this = Card(size_t(other); )
  Card(const wxXmlNode* elem);
  wxString GetCardCode() const
  {
    wxString SuitChar = getSuitText( false );
    return wxString::Format(_("%c%X"), SuitChar.at(0), getNumber() );
  }
  virtual ~Card(void);
  Colour getColour(void) const { return (suit < Hearts) ? black : red ; }
  int getNumber(void) const { return number; }
  int getSuit(void) const { return suit; }
  bool Card::operator==(const Card &other) const {
    return ( size_t(this) == size_t(other) );
  }
  void getNbrText(wxString &wc) const;
  wxString getSuitText(bool longVersion = true) const;
  int Serialize( wxXmlNode* elem ) const;
  operator size_t() const;
};

typedef std::vector< size_t > CARD_VECTOR;

