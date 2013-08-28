(ns poker-hands.hands-test
  (:require [clojure.test :refer :all]
            [poker-hands.hands :refer :all]))

(def test-input "white: 8c 9d ts jd qh  black: 9d th js kd qc")

(deftesting
  (testing "straigh flush hand"
    (rank-hand )
