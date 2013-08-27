(ns poker-hands.parser-test
  (:require [clojure.test :refer :all]
            [poker-hands.parser :refer :all]))

(def test-input "white: 2d 4d 5h ks 6c  black: 5d 7h 8h 9h 9c")

(deftest split-players-test
  (testing "split players"
    (is (= ["white: 2d 4d 5h ks 6c" "black: 5d 7h 8h 9h 9c"] 
           (split-players test-input)))))

