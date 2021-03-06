#include "deck.h"
#include "card.h"
#include "settings.h"

#include <algorithm>
#include <cassert>
#include <ctime>
#include <iostream>
#include <iterator>
#include <random>

Deck::Deck(int nDecks)
{
	constexpr auto nSuits{static_cast<deck_type::size_type>(CardSuit::max_suits)};
	constexpr auto nRanks{static_cast<deck_type::size_type>(CardRank::max_ranks)};
	m_deck.reserve(nSuits * nRanks * static_cast<deck_type::size_type>(nDecks));

	for (int amount{0}; amount < nDecks; ++amount)
	{
		for (auto suit{static_cast<deck_type::size_type>(0)}; suit < nSuits; ++suit)
		{
			for (auto rank{static_cast<deck_type::size_type>(0)}; rank < nRanks; ++rank)
				m_deck.emplace_back(static_cast<CardRank>(rank),static_cast<CardSuit>(suit));
        }
        ++amount;
    }
    shuffleDeck();
}

const Card& Deck::operator[] (deck_type::size_type index) const
{
	if (index >= m_deck.size())
		throw "Index out of bounds";
	
	return m_deck[index];
}

std::ostream& operator<< (std::ostream& out, const Deck& deck)
{
	if (deck.m_deck.empty())
		return out;
	
	for (Deck::deck_type::size_type i{0}; i < deck.m_deck.size() - 1; ++i)
		std::cout << deck[i] << ' ';
	
	out << deck[deck.m_deck.size() - 1];
	return out;
}

void Deck::shuffleDeck()
{
	static std::mt19937 mt{static_cast<std::mt19937::result_type>(std::time(nullptr))};
	std::shuffle(std::begin(m_deck), std::end(m_deck), mt);
}

int Deck::getDeckTotalValue() const
{
	const Card ace{CardRank::rank_ace, CardSuit::suit_spades};
	const int aceValue{ace.getCardValue()};
	
	int totalValue{0};
	int nAces{0};
    for (const Card& card : m_deck)
	{
        totalValue += card.getCardValue();
		if (card.isSameRank(ace))
			++nAces;
    }
	
	if (totalValue <= settings::blackjackValue || aceValue == 1)
		return totalValue;
	
	// Calculate how many aces to treat as one.
	int nAcesToTreatAsOne{0};
	nAcesToTreatAsOne = (totalValue - settings::blackjackValue) / (aceValue - 1);
	
	// round up if needed
	if ((totalValue - settings::blackjackValue) % (aceValue - 1) != 0)
		++nAcesToTreatAsOne;
	
	return totalValue - (aceValue - 1) * std::min(nAces, nAcesToTreatAsOne);
}

void Deck::addCard(const Card& card)
{
	m_deck.push_back(card);
}

Card Deck::popLastCard()
{
	if (m_deck.empty())
		throw "Trying to pop from an empty deck";
	
    Card card{ m_deck.back() };
    m_deck.pop_back();
    return card;
}

bool Deck::isBlackjack() const
{
	return (getDeckTotalValue() == settings::blackjackValue && getNumberOfCardsInADeck() == 2);
}