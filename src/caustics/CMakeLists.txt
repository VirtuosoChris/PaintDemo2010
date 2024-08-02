project(caustics)

#this package should include all the source code for the file
set(SOURCES caustics_example.cpp ScreenSpaceQuad.cpp radialWheel.h) 

#this is where you create "soaper_example, including any immediate link libraries"
add_executable(causticsDemo ${SOURCES})

target_link_libraries(causticsDemo se.core se.sim se.graphics.image se.graphics.cpGL)