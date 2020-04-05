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
