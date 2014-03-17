use std::to_str::ToStr;
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

/*impl ToStr for Value {
    fn to_str(&self) -> ~str {
        let n = *self as int;
        n.to_str()
    }
}*/

pub struct Card {
    value: Value,
    suit: Suit 
}