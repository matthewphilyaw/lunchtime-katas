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

(defn- straight [c]
  (not (some #(> % 0) 
             (map #(abs (reduce - %))
                  (pair-up-with-seq c)))))
    

(defn- is-straight [h]
  (let [cards (sort-by #(% :rank) (h :cards))]
    (if (straight cards) 
      (into h {:kind :straight 
               :rank (hand-ranks :straight) 
               :high-card (first cards)})
      h)))

(defn rank-hand [h]
  (-> 
    (is-straight h)))


(defn winning-hand [hands]
  (first (sort #(if (= (%1 :rank) (%2 :rank))
                    (compare ((%1 :high-card) :rank)
                             ((%2 :high-card) :rank))
                    (compare (%1 :rank) 
                             (%2 :rank)))
               hands)))
