(ns poker-hands.hands-test
  (:require [clojure.test :refer :all]
            [poker-hands.hands :refer :all]))

(def straight-flush-hand {:hand
                          {:cards '({:rank 13, :face "2", :suit :clubs}
                                    {:rank 12, :face "3", :suit :clubs}
                                    {:rank 11, :face "4", :suit :clubs}
                                    {:rank 10, :face "5", :suit :clubs}
                                    {:rank 9, :face "6", :suit :clubs})}})

(deftest straight-flush-test
  (testing "test for straight flush"
    (is (= :straight-flush 
           (((is-straight-flush straight-flush-hand) :hand) :kind)))))

