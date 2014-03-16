use std::io::buffered::BufferedReader;
use std::io;

fn main() {
    let mut stdin = BufferedReader::new(io::stdin());

    // Loop through each line.
    for l in stdin.lines() {

    }

}