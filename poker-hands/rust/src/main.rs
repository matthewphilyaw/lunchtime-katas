use std::io::buffered::BufferedReader;
use std::io;

use card::{Suit, Value, Card};
mod card;

fn main() {
    let mut stdin = BufferedReader::new(io::stdin());
    let x = Card { value: card::Ace, suit: card::Clubs };

    println!("Value: {}, Suit: {}", x.value.to_str(), x.suit.to_str())
    /*
    // Loop through each line.
    for l in stdin.lines() {

    }

    println!("{:?}", x);
    */
}