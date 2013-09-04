(ns poker-hands.core
  (:use [poker-hands.parser]
        [poker-hands.hands]))

(def test-input "white: 3c 4d 5s 6c 7d  black: ad jd kd qd td")

(defn -main [& args] (prn (map :kind (map rank-hand (parse-input test-input)))))
; (let [hand (winning-hand (map rank-hand (parse-input test-input)))]
;   (prn :player (hand :player-name))
;   (prn :hand (hand :kind))))
