#include <sstream>
#include "Card.h"

const char* Card::suitNames[] = {"Spd", "Clb", "Hrt", "Dmd"};

Card::~Card(void)
{
}

bool Card::Validate() const
{
    return ( 
        ( number > 0 ) && ( number <= NUM_CARDS_IN_SUITE )
        &&
        ( suit > SuitUnknown ) && ( suit <= Diamonds )
        ) ;
}

void Card::SetSuitAndNumber( size_t nbr )
{ 
  if (nbr) {
    --nbr; 
    suit = Suit(nbr/NUM_CARDS_IN_SUITE); 
    number = nbr%NUM_CARDS_IN_SUITE + 1; 
  } else {
    suit = SuitUnknown;
    number = 0;
  }
};

Card::Card(size_t nbr)
{
    Card();
    SetSuitAndNumber(nbr);
}

void Card::getNbrText(wxString &wc) const
{
    switch(number) {
        case 1:
            wc = L"A";
            break;

        case 11:
            wc = L"J";
            break;

        case 12:
            wc = L"Q";
            break;

        case 13:
            wc = L"K";
            break;

        default:
            wc = wxString::Format( L"%2d", number );
            break;
    }
}

wxString Card::getSuitText(bool longVersion) const
{
  wxString wc = wxString::FromAscii(suitNames[suit]);
  if (!longVersion) {
    wc.erase(1);
  }
  return wc;
}

int Card::Serialize( wxXmlNode* elem ) const
{
  wxXmlNode* cardElem = new wxXmlNode( elem, wxXML_ELEMENT_NODE, "Card" );
  cardElem->AddAttribute("Name", GetCardCode());

  /*
  wxString nbrTxt;
  sp
  this->getNbrText( nbrTxt );
  wxString suitTxt;
  this->getSuitText( suitTxt );
  wxString name = nbrTxt;
  name.append( _(" of ") );
  name.append( suitTxt );
  */
  // char buffer[20];
  // const wxString SuitChar = getSuitText( false );
  // sprintf(buffer, "%c%X", SuitChar.at(0), this->getNumber());
  // cardElem->SetAttribute( "Name", GetCardCode().ToUTF8().data() );
  // elem->LinkEndChild( cardElem );
  return 0;
}

Card::Card(const wxXmlNode* cardElem)
{
  // assume element is "Card"
  const char* cardName = cardElem->GetAttribute("Name");
  // Translate it
  switch (cardName[0]) {
    case 'C':
      suit = Clubs;
      break;
    case 'D':
      suit = Diamonds;
      break;
    case 'S':
      suit = Spades;
      break;
    case 'H':
      suit = Hearts;
      break;
    default:
      assert(false);
      break;
  }
  const char nbr[] = "0123456789ABCD";
  number = strchr(nbr, cardName[1]) - nbr;

}

/*
Card::Card(int nbr, Suit s)
{
  if ( SuitUnknown == s ) 
}; // : number(nbr), suit(s) { InitializeIfNecessary(); }
*/
/*
void Card::SetCardCode()
{
  // char buffer[20];
  wxString SuitChar = getSuitText( false );
  cardCode = cardCode.Format(_("%c%X"), SuitChar.at(0), this->getNumber() );
  // sprintf(buffer, "%c%X", SuitChar.at(0), this->getNumber());
}
*/
Card::operator size_t() const
{ 
  if ( SuitUnknown == suit )
    return 0;
  return size_t( suit ) * NUM_CARDS_IN_SUITE + number; 
}
