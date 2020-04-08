(use build/iup)

(pp (const-IUP-PRIMARY))

(def iup (IupOpen (int-ptr) (char-ptr)))

(IupMessage "Hello World 1" "Hello world from IUP.")

(IupClose)
