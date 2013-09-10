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

(defn- add-hand-info [hand kind]
  (into hand
        {:rank (kind hand-ranks)
         :kind kind}))

(defn- group-by-rank [hand]
  (seq (group-by :rank (:cards hand))))

(defn- build-group-map [grouped] 
  (map #(hash-map :rank (first %)
                  :size (count (last %))
                  :cards (last %)) grouped))

(defn- order-by-size-then-rank [groups]
  (sort #(or (and (= (:size %1) (:size %2))
                  (compare (:rank %1) (:rank %2)))
             (compare (:size %2) (:size %1)))
        groups))

(defn- order-by-hand [hand]
  (assoc-in hand [:cards] 
            (-> (group-by-rank hand)
                build-group-map
                order-by-size-then-rank)))

(defn- filter-by-size [cards size]
  (filter #(= (:size %) size) cards))

(defn- straight [ranks]
    (every? true? (map #(= (- %1 %2) 1) (rest ranks) ranks)))

(defn- flush [suits]
   (every? true? (map #(= (first suits) %) suits)))

(defn- are-single-groups [hand & {:keys [a-straight a-flush]}]
  (and (= 5 (count (filter-by-size (:cards hand) 1)))
       (= (straight (map :rank 
                         (:cards hand))) 
          a-straight)
       (= (flush (map #(:suit (first (:cards %))) 
                      (:cards hand)))
          a-flush)))

(defn- is-straight-flush [hand] 
  (and (are-single-groups hand :a-straight true :a-flush true)
       (add-hand-info hand :straight-flush)))

(defn- is-four-of-a-kind [hand]
  (and (= 1 (count (filter-by-size (:cards hand) 4)))
       (add-hand-info hand :four-of-a-kink)))

(defn- is-full-house [hand]
  (and (= 1 (count (filter-by-size (:cards hand) 3)))
       (= 1 (count (filter-by-size (:cards hand) 2)))
       (add-hand-info hand :full-house)))
    
(defn- is-flush [hand]
  (and (are-single-groups hand :a-straight false :a-flush true)
       (add-hand-info hand :flush)))

(defn- is-straight [hand]
  (and (are-single-groups hand :a-straight true :a-flush false)
       (add-hand-info hand :straight)))

(defn- is-three-of-a-kind [hand]
  (and (= 1 (count (filter-by-size (:cards hand) 3))) 
       (= 2 (count (filter-by-size (:cards hand) 1)))
       (add-hand-info hand :three-of-a-kind)))

(defn- is-two-pair [hand]
  (and (= 2 (count (filter-by-size (:cards hand) 2)))
       (add-hand-info hand :two-pair)))

(defn- is-one-pair [hand]
  (and (= 1 (count (filter-by-size (:cards hand) 2))) 
       (= 3 (count (filter-by-size (:cards hand) 1)))
       (add-hand-info hand :one-pair)))

(defn- is-high-card [hand] 
  (and (are-single-groups hand :a-sraight false :a-flush false)
       (add-hand-info hand :high-card)))

(defn- when-no-rank [hand rank-fn]
  (let [{:keys [rank], :or {rank nil}} hand]
    (or (and (nil? rank)
             (rank-fn hand))
        hand)))

(defn- rank [h]
  (-> (order-by-hand h)
      (when-no-rank is-straight-flush)
      (when-no-rank is-four-of-a-kind)
      (when-no-rank is-full-house)
      (when-no-rank is-flush)
      (when-no-rank is-straight)
      (when-no-rank is-three-of-a-kind)
      (when-no-rank is-two-pair)
      (when-no-rank is-one-pair)
      (when-no-rank is-high-card)))


(defn rank-hand [h]
  (let [ranked (rank h)]
    (assoc-in ranked [:cards] 
              (apply concat 
                     (map :cards 
                          (:cards ranked))))))


(defn winning-hand [hands]
  (first (sort #(if (= (%1 :rank) (%2 :rank))
                    (compare ((%1 :high-card) :rank)
                             ((%2 :high-card) :rank))
                    (compare (%1 :rank) 
                             (%2 :rank)))
               hands)))
