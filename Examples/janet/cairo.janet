(import build/cairo :as cairo)

(def width 300)
(def height 300)

(def fmt (cairo/const-CAIRO-FORMAT-ARGB32))
(def surface (cairo/cairo-image-surface-create fmt width height))
(def ctx (cairo/cairo-create surface))

(pp ctx)
