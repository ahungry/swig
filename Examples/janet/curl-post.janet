(import build/curl :as curl)

# Equivalent of this:
# curl https://httpbin.org/post -XPOST -d '{"x": 1}' -D- -H 'Content-Type: application/json'" "}

(def c (curl/curl-easy-init))

(def chunk (curl/new-curl-slist-null))
(def chunk2 (curl/curl-slist-append chunk "Content-Type: application/json"))
(def chunk3 (curl/curl-slist-append chunk2 "Accept:")) # nulls out the accept, so we take any

(curl/curl-easy-setopt-pointer c (curl/const-CURLOPT-HTTPHEADER) chunk3)

# Use this url, as it does send a 302 from http to https
(curl/curl-easy-setopt-string c (curl/const-CURLOPT-URL) "https://httpbin.org/post")
(curl/curl-easy-setopt c (curl/const-CURLOPT-FOLLOWLOCATION) 1)
#(curl/curl-easy-setopt c (curl/const-CURLOPT-POST) 1)
(curl/curl-easy-setopt c (curl/const-CURLOPT-VERBOSE) 1)

(pp "About to set data")

(def data "{\"x\": 1}")
(curl/curl-easy-setopt-string c (curl/const-CURLOPT-POSTFIELDS) data)
(curl/curl-easy-setopt c (curl/const-CURLOPT-POSTFIELDSIZE) (length data))


(pp "About to perform ")

(def res (curl/curl-easy-perform c))

(pp "Performed it...")

(curl/curl-easy-cleanup c)
(curl/curl-slist-free-all chunk)

(pp res)
