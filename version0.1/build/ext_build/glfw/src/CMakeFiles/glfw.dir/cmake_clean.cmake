file(REMOVE_RECURSE
  "libglfw3.pdb"
  "libglfw3.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang)
  include(CMakeFiles/glfw.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
