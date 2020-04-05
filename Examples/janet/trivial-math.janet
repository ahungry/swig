(import build/trivialmath :as tm)

(pp "hi")

(pp (tm/sum-nums 1 2))
(pp (tm/diff-nums 1 2))

(def point (tm/make-point 5 10))

(pp point)

(pp (tm/sum-point point))
