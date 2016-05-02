#pragma once

#include "Cell.h"
#include "FoundationPile.h"
#include "TableauPile.h"
#include "CardMovement.h"
#include "DeckOfCards.h"

class Table
{
  friend class FreecellFrame;
public:

  class WhereInTable {
    public:
      bool isInitialized;
        Card card;
        pileRepresentation pile;
        size_t posInPile;
        WhereInTable():isInitialized(false) {}
        void Set( pileRepresentation pr, int pos ) { pile = pr; posInPile = pos; }
    };

protected:
    WhereInTable CardPosition[Card::NUM_SUITES * Card::NUM_CARDS_IN_SUITE + 1];

public:
    static const size_t NUM_CELLS = 4;
    static const size_t NUM_FOUNDATIONPILES = 4;
    static const size_t NUM_TABLEAUPILES = 8;

protected:
    DeckOfCards deck;
    FoundationPile foundationpile[NUM_FOUNDATIONPILES];
    TableauPile tableaupile[NUM_TABLEAUPILES];
    Cell cell[NUM_CELLS];
    bool ClearedForFinal[NUM_TABLEAUPILES];
    bool strictClearCount;

public:
    Table(void);
    ~Table(void);
    void StartGame(bool shuffle = true);
    void CleanTable(void);
    // friend std::ostream& operator<<(std::ostream& os, const Table& t);
    // bool MoveCard( const pileRepresentation& from, const pileRepresentation& to, bool force = false );
    bool MoveCard( CARD_VECTOR from, const pileRepresentation& to, bool force, bool lazyCount );
    const CardPile* GetCardPile( const pileRepresentation& position ) const;
    // size_t getFirstCardOn( const pileRepresentation& position ) const;
    size_t getLastCardOn( const pileRepresentation& position ) const;
    size_t GetCardPrevToTop( const pileRepresentation& position );

    const CardPile* GetWhereCardIs(Card& card, int& posFound) const;
    void ShowStatus(void);
    bool isFinished() const;
    CardMovement* CanMoveToFoundation() const;
    // const WhereInTable* WhereIsCard(const Card& c) const;
    virtual size_t NextCardTo(const Card& c) const;
    int Serialize(const wxString& fileName);
    int Serialize(void);
    int LoadFromXML( const wxString& fileName );
    size_t maxNbrCardsToDrag( CardPile* cp, bool lazyCounting ) const;
    bool updateClearedForFinal(bool verifyAll = false);
    void SetstrictClearCount( bool cc ) { strictClearCount = cc; }

};
