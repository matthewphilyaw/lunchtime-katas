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

(defn- add-hand-info [h kind high-card-fn]
  (into h {:kind kind 
           :rank (hand-ranks kind) 
           :high-card (high-card-fn h)}))

(defn- when-no-rank [hand rank-fn]
  (let [{:keys [rank], :or {rank nil}} hand]
    (if (nil? rank)
        (or (rank-fn hand) hand)
        hand)))

(defn- pair-up-with-seq [c]
  (map vector 
       (take (count c) (iterate inc ((first c) :rank)))
       (take (count c) (map #(% :rank) c))))

(defn- straight [c]
  (not (some #(> % 0) 
             (map #(abs (reduce - %))
                  (pair-up-with-seq c)))))

(defn- a-flush [h]
  (let [suit ((h :cards) :suit)]
    (every-pred #(= (% :suit) suit) h)))
    
(defn- is-straight-flush [h] 
  (and (straight(h :cards)) 
       (a-flush (h :cards))
       (add-hand-info h 
                      :straight-flush
                      (fn [h] (first (h :cards))))))

(defn- is-straight [h]
  (and (straight (h :cards)) 
       (add-hand-info h 
                      :straight 
                      (fn [h] (first (h :cards))))))

(defn rank-hand [h]
  (-> 
    (assoc-in h [:cards] (sort-by #(% :rank) (h :cards)))
    (when-no-rank is-straight-flush)
    (when-no-rank is-straight)))

(defn winning-hand [hands]
  (first (sort #(if (= (%1 :rank) (%2 :rank))
                    (compare ((%1 :high-card) :rank)
                             ((%2 :high-card) :rank))
                    (compare (%1 :rank) 
                             (%2 :rank)))
               hands)))
