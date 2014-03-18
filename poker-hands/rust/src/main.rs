use std::io::buffered::BufferedReader;
use std::io;

use card::{Suit, Value, Card};
mod card;

fn main() {
    let mut stdin = BufferedReader::new(io::stdin());
    let x = Card { value: 2 as card::value, suit: card::Clubs };

    println!("{}", x);
    /*
    // Loop through each line.
    for l in stdin.lines() {

    }

    println!("{:?}", x);
    */
}