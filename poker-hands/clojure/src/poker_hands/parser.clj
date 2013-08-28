(ns poker-hands.parser
  (:use [poker-hands.deck]))

(defn split-players [h]
  (clojure.string/split h #"  "))

(defn split-hands-from-player [h]
  (map #(clojure.string/split % #":") h))

(defn build-hand-list [h]
  (map #(list (first %) 
              (clojure.string/split (clojure.string/trim (last %)) #" ")) 
       h))

(defn build-hand-map [h]
  (map #(list (first %) 
              (hash-map :cards
                        (map (fn [c] (hash-map :face (str (first c))
                                               :suit (suit (last c))
                                               :rank (rank (first c))))
                             (last %))))
       h))

(defn build-player-hand-map [h]
  (map #(zipmap [:player :hand] %) h))

(defn parse-input [input]
  (-> 
    (split-players input)
    split-hands-from-player
    build-hand-list
    build-hand-map
    build-player-hand-map))
