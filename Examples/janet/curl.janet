(import build/curl :as curl)

(def c (curl/curl-easy-init))

# (curl/curl-easy-setopt-string c (curl/const-CURLOPT-URL) "https://example.com")

# Use this url, as it does send a 302 from http to https
(curl/curl-easy-setopt-string c (curl/const-CURLOPT-URL) "http://ahungry.com")
(curl/curl-easy-setopt c (curl/const-CURLOPT-FOLLOWLOCATION) 1)

(def res (curl/curl-easy-perform c))

(curl/curl-easy-cleanup c)

(pp res)
