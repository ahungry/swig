(use build/curl)

(def curl (curl-easy-init))
(def slist (new-curl-slist))

(curl-easy-setopt-string curl (const-CURLOPT-URL) "http://example.com")

(def slist2 (curl-slist-append slist "Shoesize: 10"))
#(curl-slist-append slist "Accept:")

(curl-easy-setopt-pointer curl (const-CURLOPT-HTTPHEADER) slist2)

(def res (curl-easy-perform curl))

(curl-easy-cleanup curl)
(curl-slist-free-all slist)

(pp res)
