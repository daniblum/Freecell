#include "TableauPile.h"
#include "Card.h"

bool TableauPile::canAddThisCard( Card candidate ) const
{
    if (pile.empty())
        return true;
    else {
        Card lastCard = pile.back();
        return (
            ( candidate.getColour() != lastCard.getColour() )
            &&
            ( candidate.getNumber() == (lastCard.getNumber() - 1) )
        ) ;
    }
}

