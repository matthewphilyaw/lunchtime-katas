(ns poker-hands.core
  (:use [poker-hands.parser]
        [poker-hands.hands]))

(def test-input "white: 8c 9d ts jd qh  black: 9d th js kd qc")

(defn -main [& args]
  (let [winner (first (sort #(if (= ((%1 :hand) :rank) ((%2 :hand) :rank))
                         (compare (((%1 :hand) :high-card) :rank)
                                  (((%2 :hand) :high-card) :rank))
                         (compare ((%1 :hand) :rank) 
                                  ((%2 :hand) :rank)))

                    (map #(assoc-in % 
                                    [:hand] 
                                    (rank-hand (% :hand)))
                         (parse-input test-input))))]
  (prn :player (winner :player))
  (prn :hand ((winner :hand) :kind))))
