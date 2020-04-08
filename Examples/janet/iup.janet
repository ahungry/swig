(use build/iup)

(def iup (IupOpen (int-ptr) (char-ptr)))
(def label (IupLabel "Hello world from IUP."))
(def dialog (IupDialog (IupVbox label (int-ptr))))

(IupSetAttribute dialog "TITLE" "Hello World 2")

(IupShowXY dialog (const-IUP-CENTER) (const-IUP-CENTER))

(IupMainLoop)

#(IupMessage "Hello World 1" "Hello world from IUP.")
(IupClose)
