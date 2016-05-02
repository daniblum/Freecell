#include "CardPile.h"

#include <climits>
#include <algorithm>

#include "Card.h"
#include "pileRepresentation.h"

CardPile::CardPile(void): pileRep('A', -1), cursor(0)
{
}

pileRepresentation CardPile::GetPileRep(void) const
{
    return pileRep;
}

void CardPile::CleanPile(void)
{
    pile.clear();
}

CardPile::~CardPile(void)
{
}

Card CardPile::GetCardOnTop() const
{
    if (pile.empty()) {
        return Card();
    } else {
        return pile.back();
    }
}

Card CardPile::GetCardPrevToTop() const
{
    if (pile.size() <= 1) {
        return Card();
    } else {
        return pile[pile.size()-2];
    }
}

Card CardPile::GetFirstCard()
{
  cursor = 0;
  if (pile.empty()) {
    return Card();
  } else {
    return pile.at(cursor++);
  }
}

void CardPile::AddCard(Card c)
{
  assert(c);
    pile.push_back(c);
}

int CardPile::PosWhereCardIs( const Card& card ) const
{
    CARD_VECTOR::const_iterator it = std::find( pile.begin(), pile.end(), card );
    if ( it == pile.end() )
        return INT_MAX;
    else {
        return it - pile.begin();
    }
}

Card CardPile::GetNextCard()
{
    if (cursor >= pile.size()) {
        return Card();
    } else {
        return pile[cursor++];
    }
}

Card CardPile::NextCardTo(const Card& c) const
{
  size_t posCard = PosWhereCardIs(c);
  if (posCard >= pile.size()-1) {
      return Card();
  } else {
      return pile.at(posCard+1);
  }
}

int CardPile::Serialize( wxXmlNode* elem ) const
{
  const pileRepresentation pr = this->GetPileRep();
  std::string prep;
  pr.ToString( prep );
  elem->AddAttribute("pr", prep.c_str());

  for (CARD_VECTOR::const_iterator it = pile.begin(); it != pile.end(); ++it) {
    Card(*it).Serialize( elem );
  }
  return 0;
}


/*
std::ostream& operator<<(std::ostream& os, const CardPile& tp)
{
    if (tp.firstCard) {
        Card* currentCard = tp.firstCard;
        Card* prevCard = 0;
        while (currentCard) {
            os << *currentCard;
            prevCard = currentCard;
            currentCard = currentCard->onTop;
        }
        os << std::endl;
    } else {
        os << " Empty." << std::endl;
    }
    return os;
}
*/