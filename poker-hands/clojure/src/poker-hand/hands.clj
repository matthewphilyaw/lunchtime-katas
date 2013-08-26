(ns poker-hand.hands)

(def hand-ranks {{:kind :straight-flush,  :rank 1}
                 {:kind :four-of-a-kind,  :rank 2}
                 {:kind :full-house,      :rank 3}
                 {:kind :flush,           :rank 4}
                 {:kind :straight,        :rank 5}
                 {:kind :three-of-a-kind, :rank 6}
                 {:kind :two-pair,        :rank 7}
                 {:kind :one-pair,        :rank 8}
                 {:kind :high-card        :rank 9}})