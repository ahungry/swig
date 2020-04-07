(declare-project
  :name "trivial-math"
  :description "Try out building wrapped math."
  :author "Matthew Carter"
  :license "GPLv3"
  :url "https://github.com/ahungry/janet-code/"
  :repo "git+https://github.com/ahungry/janet-code.git")

(declare-native
 # Apparently having a dash in name breaks the macros
  :name "trivialmath"
  :cflags ["-std=gnu99" "-Wall" "-Wextra"]
  :lflags ["-lm" "-ldl" "-lpthread"]
  :source @["trivial-math_wrap.c"])

# this 'works' (I am to the point of generating a sample png with it)
(declare-native
  :name "cairo"
  :cflags ["-std=gnu99" "-Wall" "-Wextra" "-I/usr/include/cairo"
           "-I/usr/include/glib-2.0" "-I/usr/lib/glib-2.0/include"
           "-I/usr/lib/libffi-3.2.1/include" "-I/usr/include/pixman-1"
           "-I/usr/include/freetype2" "-I/usr/include/libpng16"
           "-I/usr/include/harfbuzz"]
  :lflags ["-lm" "-ldl" "-lpthread" "-lcairo"]
  :source @["cairo_wrap.c"])

# Tried but doesn't quite work:
#   fuse    - function pointer issue
#   sqlite3 - ?
#   libcurl - variable arity issue (...)

# (declare-native
#  :name "curl"
#  :cflags ["-std=gnu99" "-Wall" "-Wextra"
#           ]
#  :lflags ["-lm" "-ldl" "-lpthread" "-lcurl"]
#  :source @["curl_wrap.c"])
