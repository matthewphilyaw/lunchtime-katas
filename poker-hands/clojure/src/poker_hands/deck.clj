(ns poker-hands.deck)

(def rank {
  \2 13
  \3 12
  \4 11
  \5 10
  \6 9
  \7 8
  \8 7
  \9 6
  \t 5
  \j 4
  \q 3
  \k 2
  \a 1 })

(def suit {
  \c :clubs,
  \d :diamonds,
  \h :hearts,
  \s :spades })
