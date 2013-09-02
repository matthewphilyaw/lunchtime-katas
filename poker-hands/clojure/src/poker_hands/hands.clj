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

(defn- hand-info [kind best-hand remainder]
  {:kind kind 
   :rank (hand-ranks kind) 
   :best-hand best-hand 
   :remainder remainder})

(defn- when-no-rank [hand rank-fn]
  (let [{:keys [rank], :or {rank nil}} hand]
    (or (and (nil? rank)
             (rank-fn hand))
        hand)))

(defn- a-straight [c]
  (let [ranks (map :rank c)]
    (every? true? (map #(= (- %1 %2) 1) (rest ranks) ranks))))

(defn- a-flush [c]
  (let [suits (map :suit c)]
   (every? true? (map #(= (first suits) %) suits)))) 
    
(defn- is-straight-flush [h] 
  (and (a-straight (h :cards)) 
       (a-flush (h :cards))
       (into h (hand-info :straight-flush
                          (h :cards)
                          nil))))

(defn- is-straight [h]
  (and (a-straight (h :cards)) 
       (into h (hand-info :straight
                          (h :cards)
                          nil))))

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
