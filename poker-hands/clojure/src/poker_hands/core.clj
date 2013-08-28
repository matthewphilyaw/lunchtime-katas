(ns poker-hands.core
  (:use [poker-hands.parser]
        [poker-hands.hands]))

(def test-input "white: td kd jd qd ad  black: 9d th js kd qc")

(defn -main [& args]
 (let [hand (winning-hand (map rank-hand (parse-input test-input)))]
   (prn :player (hand :player-name))
   (prn :hand (hand :kind))))
