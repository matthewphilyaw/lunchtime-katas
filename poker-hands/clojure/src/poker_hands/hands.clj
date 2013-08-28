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

(defn- abs [n]
  (if (< n 0) (* n -1) n))

(defn- when-no-rank [hand rank-fun]
  (let [{:keys [rank], :or {rank nil}} hand]
    (if (nil? rank)
        (rank-fun hand)
        hand)))

(defn- pair-up-with-seq [c]
  (map vector 
       (take (count c) (iterate inc ((first c) :rank)))
       (take (count c) (map #(% :rank) c))))

(defn- is-straight [h]
  (let [cards (sort-by #(% :rank) (h :cards))]
    (let [straight (reduce + (map #(abs (reduce - %))
                                  (pair-up-with-seq cards)))]
      (if (= straight 0)
        (into h {:kind :straight :rank (hand-ranks :straight) :high-card (first cards)})
        h))))

(defn rank-hand [h]
  (-> 
    (is-straight h)))

