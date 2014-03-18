use std::to_str::ToStr;
use std::fmt;
// structure for hand

pub enum Suit {
    Clubs,
    Diamonds,
    Hearts,
    Spades
}

impl ToStr for Suit {
    fn to_str(&self) -> ~str {
        match *self {
            Clubs => ~"Clubs",
            Diamonds => ~"Diamonds",
            Hearts => ~"Hearts",
            Spades => ~"Spades"
        }
    }
}

impl fmt::Default for Suit {
    fn fmt(suit: &Suit, f: &mut fmt::Formatter)  {
        write!(f.buf, "{}", suit.to_str())
    }
}

pub enum Value {
    Two = 2,
    Thre,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Jack,
    Queen,
    King,
    Ace
}

impl ToStr for Value {
    fn to_str(&self) -> ~str {
        match *self {
            Ace => ~"Ace",
            King => ~"King",
            Queen => ~"Queen",
            Jack => ~"Jack",
            Ten => ~"Ten",
            _ => { 
                let v = *self as int; 
                v.to_str()
            }
        }
    }
}

impl fmt::Default for Value {
    fn fmt(value: &Value, f: &mut fmt::Formatter)  {
        write!(f.buf, "{}", value.to_str())
    }
}

pub struct Card {
    value: Value,
    suit: Suit 
}

impl ToStr for Card {
    fn to_str(&self) -> ~str {
        format!("{} of {}", self.value, self.suit)
    }
}

impl fmt::Default for Card {
    fn fmt(obj: &Card, f: &mut fmt::Formatter)  {
        write!(f.buf, "{}", obj.to_str())
    }
}