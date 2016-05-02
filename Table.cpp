#include "Table.h"
/*
#include <iostream>
#include <fstream>
*/
#include <cstdio>
#include <wx/log.h>

Table::Table(void)
{
    int count;
    for (count = 0; count < NUM_TABLEAUPILES; ++count) {
        pileRepresentation pR('T', count+1);
        tableaupile[count].SetPileRep(pR);
        tableaupile[count].CleanPile();
        ClearedForFinal[count] = false;
    }
    for (count = 0; count < NUM_FOUNDATIONPILES; ++count) {
        pileRepresentation pR('F', count+1);
        foundationpile[count].SetPileRep(pR);
        foundationpile[count].CleanPile();
    }
    for (count = 0; count < NUM_CELLS; ++count) {
        pileRepresentation pR('C', count+1);
        cell[count].SetPileRep(pR);
        cell[count].CleanPile();
    }
    /* */
    for (count = 1; count <= Card::NUM_SUITES * Card::NUM_CARDS_IN_SUITE; ++count) {
        CardPosition[count].card = count;
        size_t ct = CardPosition[count].card;
        ct = ct;
        if (ct == count) {
        }
    }
    /* */
}

void Table::CleanTable(void)
{
    size_t count;
    for (count = 0; count < NUM_TABLEAUPILES; ++count) {
        tableaupile[count].CleanPile();
    }
    for (count = 0; count < NUM_FOUNDATIONPILES; ++count) {
        foundationpile[count].CleanPile();
    }
    for (count = 0; count < NUM_CELLS; ++count) {
        cell[count].CleanPile();
    }
}

Table::~Table(void)
{
    CleanTable();
}

const CardPile* Table::GetCardPile( const pileRepresentation& position ) const
{
    _ASSERT( position.Validate() );
    const CardPile* base;
    switch (position.GetGroup()) {
        case 'C': // cell is empty?
            base = &(cell[0]);
            break;

        case 'T':
            base = &(tableaupile[0]);
            break;

        case 'F':
            base = &(foundationpile[0]);
            break;

        default:
            _ASSERT(false);
    }
    return base + position.GetIdx()-1 ;
}

size_t Table::getLastCardOn( const pileRepresentation& position ) const
{
    _ASSERT( position.Validate() );
    const CardPile* pile = GetCardPile( position );
    return pile->GetCardOnTop();
}

bool Table::MoveCard( CARD_VECTOR from
                     , const pileRepresentation& toPR
                     , bool force
                     , bool lazyCount)
{
    bool OK = false;
    _ASSERT( toPR.Validate() );
    Card fromCard =  from[0];
    if (fromCard) {
        CardPile* toPile = (CardPile*) GetCardPile(toPR);
        OK = (force || toPile->canAddThisCard( fromCard ));
        if (OK) {
          OK = (force || (from.size() <= maxNbrCardsToDrag( toPile, lazyCount ) ) );
        }
        if (OK) {
          // Move all cards
          WhereInTable startMiniPile = CardPosition[fromCard];
          CardPile* cpFrom = (CardPile*) GetCardPile( startMiniPile.pile );
          for ( CARD_VECTOR::iterator it = from.begin(); it != from.end(); ++it ) {
            Card CardToMove = *it;
            CardPosition[CardToMove].Set( toPR, toPile->size());
            toPile->AddCard(CardToMove);
          }
          cpFrom->pile.resize(startMiniPile.posInPile);
        }
    } // if (fromCard)
    return OK;
}

const CardPile* Table::GetWhereCardIs(Card& card, int& posFound) const
{
    size_t posInCardArray = card;
    const WhereInTable& cardPos = CardPosition[posInCardArray];
    const CardPile* retVal = GetCardPile( cardPos.pile );
    posFound = cardPos.posInPile;
    _ASSERT(retVal);
    return retVal;
}

void Table::StartGame(bool shuffle)
{
    CleanTable();
    size_t tableauCount = 0;
    if (shuffle)
        deck.Shuffle();
    deck.Reset();
    while (!deck.empty()) {
      TableauPile* cp = tableaupile + tableauCount;
      /*
      bool couldFind = false;
      size_t iterations = deck.size();
      int minNum, maxNum;;
      do {
        const int maxDiff = 3;
        Card c = deck.Peek();
        Card cardOnTop = cp->GetCardOnTop();
        maxNum = cardOnTop ? cardOnTop.getNumber() : Card::NUM_CARDS_IN_SUITE;
        minNum = maxNum - maxDiff ;
        int num = c.getNumber();
        if ( ( num >= minNum ) && ( num <= maxNum ) ) {
          couldFind = true;
        } else {
          deck.PutBackOnFront();
          --iterations;
        }
      } while ( !couldFind && iterations );
      */
        Card c = deck.DealOneCard();
        if ( cp->GetCardOnTop() && ( c.getNumber() > cp->GetCardOnTop().getNumber() ) ) {
          c = c;
        }
        size_t numcard = c;
        CardPosition[numcard].Set(cp->GetPileRep(), cp->size());
        cp->AddCard( c );
        if ( ++tableauCount >= (NUM_TABLEAUPILES) )
            tableauCount = 0;
    }
    _ASSERT(!cell[0].GetFirstCard());
}

bool Table::isFinished() const
{
    // just check size of Foundation Piles?
    bool OK = true;
    for (size_t count = 0; count < NUM_FOUNDATIONPILES; ++count) {
        if ( foundationpile[count].size() != Card::NUM_CARDS_IN_SUITE) {
            OK = false;
            break;
        }
    }
    return OK;
}

CardMovement* Table::CanMoveToFoundation() const
{
    Card from;
    int count;

    int minNbrUsable = 15;
    
    for ( size_t foundationCount = 0; foundationCount < Table::NUM_FOUNDATIONPILES; ++foundationCount ) {
        Card c = foundationpile[foundationCount].GetCardOnTop();
        if (c) {
            minNbrUsable = std::min( minNbrUsable, c.getNumber() );
        } else {
            minNbrUsable = 0;
            break;
        }
    }
    minNbrUsable+=2;

    for( count = 0; count < Table::NUM_CELLS; ++count ) {
        Card from = cell[count].GetCardOnTop();
        if (from && Card(from).getNumber() <= minNbrUsable ) {
            for ( int foundationCount = 0; foundationCount < Table::NUM_FOUNDATIONPILES; ++foundationCount ) {
                if (foundationpile[foundationCount].canAddThisCard(from)) {
                  CARD_VECTOR cv;
                  cv.push_back( from );
                    return new CardMovement( cv, pileRepresentation('C', count+1), pileRepresentation('F', foundationCount+1));
                }
            }
        }
    }
    for( count = 0; count < Table::NUM_TABLEAUPILES; ++count ) {
        Card from = tableaupile[count].GetCardOnTop();
        if (from && Card(from).getNumber() <= minNbrUsable ) {
            for ( int foundationCount = 0; foundationCount < Table::NUM_FOUNDATIONPILES; ++foundationCount ) {
                if (foundationpile[foundationCount].canAddThisCard(from)) {
                  CARD_VECTOR cv;
                  cv.push_back( from );
                    return new CardMovement( cv, pileRepresentation('T', count+1), pileRepresentation('F', foundationCount+1));
                }
            }
        }
    }
    return 0;
}

/*
const Table::WhereInTable* Table::WhereIsCard(const Card& c) const
{
    return &(CardPosition[c]);
}
*/

size_t Table::GetCardPrevToTop( const pileRepresentation& position )
{
    CardPile* pile = (CardPile*) GetCardPile( position );
    return pile->GetCardPrevToTop();
}

int Table::Serialize(void)
{
  wxDateTime now = wxDateTime::Now();
  wxString fn = now.FormatISOCombined('-');
  fn.Replace(_(":"), _(""));
  fn.Replace(_("-"), _(""));
  return Serialize(fn);
}

int Table::Serialize(const wxString& fileName)
{
  wxXmlDocument doc;
  // wxXmlNode* msg;
  // TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
  // doc.LinkEndChild( decl );
  // delete decl;
  
  wxXmlNode * root = new wxXmlNode( wxXML_ELEMENT_NODE, "Freecell_Table" );
  doc.SetDocumentNode(root);
  doc.SetVersion("1.0");

  // doc.LinkEndChild( root );
  // Deck
  wxXmlNode *deckElement = new wxXmlNode( root, wxXML_ELEMENT_NODE, "Deck" );
  // root->LinkEndChild( deckElement );
  deck.Reset();
  while (!deck.empty()) {
    Card c = deck.DealOneCard();
    c.Serialize(deckElement);
  }

  wxXmlNode *tableDisplay = new wxXmlNode( root, wxXML_ATTRIBUTE_NODE, "Table_Display" );
  // root->LinkEndChild( tableDisplay );

  wxXmlNode *cellsElement = new wxXmlNode( tableDisplay, wxXML_ATTRIBUTE_NODE, "Cells" );
  // tableDisplay->LinkEndChild( cellsElement );
  for (size_t cpCnt = 0; cpCnt < NUM_CELLS; ++cpCnt) {
    const CardPile* cp = cell + cpCnt;
    wxXmlNode *cpElement = new wxXmlNode( cellsElement, wxXML_ATTRIBUTE_NODE, "Cell" );
    // cellsElement->LinkEndChild( cpElement );
    cp->Serialize( cpElement );
  }

  wxXmlNode *FoundationsElement = new wxXmlNode(tableDisplay, wxXML_ATTRIBUTE_NODE, "Foundations" );
  // tableDisplay->LinkEndChild( FoundationsElement );
  for (size_t cpCnt = 0; cpCnt < NUM_FOUNDATIONPILES; ++cpCnt) {
    const CardPile* cp = foundationpile + cpCnt;
    wxXmlNode *cpElement = new wxXmlNode( FoundationsElement, wxXML_ATTRIBUTE_NODE, "Foundation" );
    // FoundationsElement->LinkEndChild( cpElement );
    cp->Serialize( cpElement );
  }

  wxXmlNode *TableausElement = new wxXmlNode( tableDisplay, wxXML_ATTRIBUTE_NODE, "Tableaus" );
  // tableDisplay->LinkEndChild( TableausElement );
  for (size_t cpCnt = 0; cpCnt < NUM_TABLEAUPILES; ++cpCnt) {
    const CardPile* cp = tableaupile + cpCnt;
    wxXmlNode *cpElement = new wxXmlNode( TableausElement, wxXML_ATTRIBUTE_NODE, "Tableau" );
    // TableausElement->LinkEndChild( cpElement );
    cp->Serialize( cpElement );
  }
 
  /*
  wxXmlNode *child = new wxXmlNode( "First Child" );

  root->LinkEndChild( child );

  wxXmlNode *grandchild = new wxXmlNode( "First Grandchild" );

  child->LinkEndChild( grandchild );
   */
  wxString Name(fileName);
  root->AddAttribute( "Name", Name.mb_str( wxConvUTF8 ) );
   
  Name = Name + _(".xml");
   
  if(doc.Save(Name))
      wxMessageBox(_("File Successfully saved\n\n")+Name);

  // doc.Clear();

  return 0;
}

int Table::LoadFromXML( const wxString& fileName )
{
  CleanTable();
    wxLog::SetLogLevel(wxLOG_Max);
    wxLog::SetVerbose();
/*
    // use stdout always
    wxLog *logger_cout = new wxLogStream(&std::cout);
    wxLog::SetActiveTarget(logger_cout);
*/
    FILE *f = fopen("mylog.txt", "w");
    if(f)
    {
        // and a file log
        wxLog *logger_file = new wxLogStderr(f);
        wxLogChain *logChain = new wxLogChain(logger_file);
    }
    wxLogStatus(wxT("log started"));
  wxXmlDocument doc( fileName );
  // doc.LoadFile();
  wxXmlNode* root = doc.GetDocumentNode();
  if (!root)
    wxLogFatalError(_("Error - '%s' has no root."), fileName );
  // Recover deck of cards?
  wxXmlNode* deckElem = root->GetChildren(); // "Deck");
  if (!deckElem)
    wxLogFatalError(_("Error - no Deck element") );
  if ( deck.LoadFromXML(deckElem) ) {
    wxLogFatalError(_("Error - no Deck elements") );
  }
  // Then:
  wxXmlNode* td = deckElem->GetNext(); // root->FirstChildElement("Table_Display");
  wxXmlNode* groupElem;
  for ( groupElem = td->GetChildren() /* FirstChildElement() */; groupElem; groupElem = groupElem->GetNext() ) {
    wxXmlNode *pileElem;
    for (pileElem = groupElem->GetChildren(); pileElem; pileElem = pileElem->GetNext() )
    {
      // get cardPile elem
      CardPile* cp;
	  const char *prs = pileElem->GetAttribute("pr");
      const pileRepresentation pr( prs );
      cp = (CardPile*) this->GetCardPile(pr);
      wxXmlNode* cardElem;
      for (cardElem = pileElem->GetChildren(); cardElem; cardElem = cardElem->GetNext() ) {
        Card c( cardElem );
        CardPosition[c].Set(cp->GetPileRep(), cp->size());
        cp->AddCard( c );
      }
    }
  }
  // doc.Clear();
  fclose(f);

  return 0;
}

size_t Table::NextCardTo(const Card& c) const
{
  const WhereInTable* wh = &(CardPosition[c]);
  if (wh){
    const CardPile* cp = GetCardPile(wh->pile);
    if (cp) {
      return cp->NextCardTo(c);
    }
  }
  return 0;
}

size_t Table::maxNbrCardsToDrag( CardPile* cp, bool lazyCounting ) const
{
  // first, calculate number of free cells
  int freeCells = 1;
  for ( size_t ct = 0; ct < NUM_CELLS; ++ct ) {
    if ( !cell[ct].size() )
      ++freeCells;
  }
  int freeTableauPiles = 0;
  for ( size_t ct = 0; ct < NUM_TABLEAUPILES; ++ct ) {
    if ( !tableaupile[ct].size() && ( cp != tableaupile+ct ) )
      ++freeTableauPiles;
  }
  return lazyCounting ? freeTableauPiles + freeCells : pow((double)2.0, freeTableauPiles) * freeCells;
}

bool Table::updateClearedForFinal(bool verifyAll)
{
  bool finalResult = true;
  for (size_t cnt = 0; cnt < NUM_TABLEAUPILES; ++cnt) {
    if (verifyAll || !ClearedForFinal[cnt]) {
      // Recalculate - see if all cards can be cleared
      TableauPile* tp = &(tableaupile[cnt]);
      Card c = tp->GetFirstCard();
      bool result = true;
      int number;
      Colour clr;
      if (c) {
        number = c.getNumber();
        clr = c.getColour();
        for (c = tp->GetNextCard(); c; c = tp->GetNextCard()) {
          Colour newClr = c.getColour();
          int newNbr = c.getNumber();
          bool cont;
          if (strictClearCount) {
            cont = ( (newNbr == number - 1) && (newClr != clr) );
          } else {
            cont = ( newNbr <= number );
          }
          if ( cont ) {
            clr = newClr;
            number = newNbr;
          } else {
            result = false;
            break;
          }
        }
      }
      if (result) {
        ClearedForFinal[cnt] = true;
      } else {
        finalResult = false;
        if (verifyAll)
          ClearedForFinal[cnt] = false;

      }
    } // clearedforfinal
  }
  return finalResult;
}
