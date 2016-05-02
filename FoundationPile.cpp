#include "FoundationPile.h"
#include "Card.h"

bool FoundationPile::canAddThisCard( Card candidate ) const
{
    if (pile.empty())
        return ( 1 == candidate.getNumber() );
    else {
        Card lastCard = pile.back();
        return (
            ( candidate.getSuit() == lastCard.getSuit() )
            &&
            ( candidate.getNumber() == 1 + lastCard.getNumber() )
        ) ;
    }
}
