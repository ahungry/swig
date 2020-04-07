(import build/cairo :as cairo)

(def width 300)
(def height 300)

(def fmt (cairo/const-CAIRO-FORMAT-ARGB32))
(def surface (cairo/cairo-image-surface-create fmt width height))
(def ctx (cairo/cairo-create surface))

(pp ctx)

(cairo/cairo-set-line-width ctx 9)
(cairo/cairo-set-source-rgb ctx 0.69 0.19 0)
(cairo/cairo-translate ctx (/ width 2) (/ height 2))
(cairo/cairo-arc ctx 0 0 100 0 (* 2 3.1418))
(cairo/cairo-stroke-preserve ctx)
(cairo/cairo-fill ctx)

(cairo/cairo-surface-write-to-png surface "/tmp/swig-worked.png")
(pp "Worked?")
