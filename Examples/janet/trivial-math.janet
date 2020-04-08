(import build/trivialmath :as tm)

(pp "hi")

(pp (tm/sum-nums 1 2))
(pp (tm/diff-nums 1 2))

(def point (tm/make-point 5 10))

(pp point)

(pp (tm/sum-point point))

(def alt-point (tm/new-point 1))

(tm/point-x-set alt-point 20)
(tm/point-y-set alt-point 200)

(pp (tm/point-x-get alt-point))
(pp (tm/point-y-get alt-point))

(pp "Sum pre deletion...")
(pp (tm/sum-point alt-point))

(pp alt-point)
(pp (tm/delete-point alt-point))

(pp alt-point)

# Ok, not great that its still callable, but what can we do?
(pp "Sum after deletion...")
(pp (tm/sum-point alt-point))

(pp "Test out typedef struct")
(def point-typedef (tm/new-pointed-t 1))
(tm/pointed-t-a-set point-typedef 44 )
(pp (tm/pointed-t-a-get point-typedef))

(pp "const test (enum)")
(pp (tm/const-TEN))
(pp (tm/const-TWENTY))
