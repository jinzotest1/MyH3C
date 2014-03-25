project(MyH3C)
add_subdirectory(src bin)
set(SOURCE_LIST
  src/main.c src/myh3c.c src/lib/shortcuts/struct.c
  src/lib/daemon.c src/user/user.c)
add_executable(myh3c ${SOURCE_LIST})
