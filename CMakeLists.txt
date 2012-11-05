project(RazorRandr CXX)
cmake_minimum_required(VERSION 2.6)

subdirs(src)

install(FILES razorxrandr.desktop DESTINATION share/applications)
