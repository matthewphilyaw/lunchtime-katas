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

(defn when-no-rank [hand rank-fun]
  (let [{:keys [rank], :or {rank nil}} hand]
    (if (nil? rank)
        (rank-fun hand)
        hand)))

;; todo - create a series of ranking functions for poker hands. 
;; chain them with the function above such that it looks like
;; 
;; start with highest rank function, then keep applying till 
;; the hand is ranked.
;;
;; (->
;;      (rank-fun hand)
;;      (when-no-rank rank-fun)
;;      (when-no-rank rank-fun)
;;      (when-no-rank rank-fun)
;;      ...
;;      rank-fun - high card function, as it will put in the high card if rank
;;                 is found. note that I'm thinking it would be worth while to 
;;                 include a cards-left entry {:cards-left []} that the 
;;                 high-card function will use if rank is found.
;;
;; Another approach might be to create another key on the hand-ranks that 
;; contains the function to call for that rank.

