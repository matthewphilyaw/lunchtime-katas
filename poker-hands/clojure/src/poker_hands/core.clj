(ns poker-hands.core
  (:use [poker-hands.hands]))

(def test-input "white: 2d 4d 5h ks 6c  black: 5d 7h 8h 9h 9c")

(def straight-flush-hand {:hand
                          {:cards '({:rank 13, :face "2", :suit :clubs}
                                    {:rank 12, :face "3", :suit :clubs}
                                    {:rank 11, :face "4", :suit :clubs}
                                    {:rank 10, :face "5", :suit :clubs}
                                    {:rank 9, :face "6", :suit :clubs})}})

(defn -main [& args]
  (prn (is-straight-flush straight-flush-hand)))
