(ns poker-hands.hands)

(def hand-ranks {:straight-flush  1
                 :four-of-a-kind  2
                 :full-house      3
                 :flush           4
                 :straight        5
                 :three-of-a-kind 6
                 :two-pair        7
                 :one-pair        8
                 :high-card       9})

(defn when-no-rank [hand rank-fun]
  (let [{:keys [rank], :or {rank nil}} hand]
    (if (nil? rank)
        (rank-fun hand)
        hand)))

(defn is-straight-flush [h]
  (let [cards ((h :hand) :cards)]
   cards))
