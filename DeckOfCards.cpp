#include "DeckOfCards.h"
#include <time.h>

#include "Card.h"

// bool DeckOfCards::CardsInitialized = false;

DeckOfCards::DeckOfCards(void): isShuffled(false)
{
    for (int n = Card::NUM_CARDS_IN_SUITE; n > 0; --n) {
        for (int s = 0; s < Card::NUM_SUITES; ++s) {
            origPile.push_back( Card( n, Suit(s) ) ); // Card::GetPositionInCardArray(n, Suit(s)) );
        }
    }
    srand((unsigned int)time(NULL));
}

int DeckOfCards::LoadFromXML(const wxXmlNode* td)
{
  int errVal = 1;
  origPile.clear();
  const wxXmlNode* cardElem;
  for (cardElem = td->GetChildren(); cardElem; cardElem = cardElem->GetNext() ) {
    if (errVal)
      errVal = 0;
    Card c(cardElem);
    origPile.push_back(c);
  }
  Reset();
  return errVal;
}

Card DeckOfCards::Peek(void) const
{
  return tempPile.back();
}

void DeckOfCards::PutBackOnFront(void)
{
  Card c = tempPile.back();
  tempPile.pop_back();
  tempPile.insert(tempPile.begin(), c);
}


void DeckOfCards::Shuffle(void)
{
    CARD_VECTOR tmp;
    while (!origPile.empty()) {
        int pos = rand() % origPile.size();
        CARD_VECTOR::iterator itC;
        itC = origPile.begin() + pos;
        Card nextCardPicked = *itC;
        tmp.push_back(nextCardPicked);
        origPile.erase(itC);
    }
    origPile = tmp;
    Reset();
    isShuffled = true;
}

void DeckOfCards::RecalcMaxNum()
{
  maxNumOnDeck = 0;
  maxNumCount = 0;
  for ( CARD_VECTOR::const_iterator it = tempPile.begin(); it != tempPile.end(); ++it) {
    Card c = *it;
    int n = c.getNumber();
    if (n == maxNumOnDeck) {
      ++maxNumCount;
    } else if (n > maxNumOnDeck) {
      maxNumCount = 1;
      maxNumOnDeck = n;
    }
  }
}

Card DeckOfCards::DealOneCard(void)
{
  Card c = tempPile.back();
  tempPile.pop_back();
  RecalcMaxNum();
  if ( c.getNumber() >= maxNumOnDeck ) {
    if ( !(--maxNumCount) ) {
      RecalcMaxNum();
    }
  }
  return c;
}

DeckOfCards::~DeckOfCards(void)
{
}
