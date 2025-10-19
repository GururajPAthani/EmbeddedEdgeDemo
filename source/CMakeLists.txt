cmake_minimum_required(VERSION 3.7)
project(MyDDSProject C)

# 1. Find the CycloneDDS package
find_package(CycloneDDS REQUIRED)

# 2. Define and Compile the IDL file
# This step creates the MyData.c and MyData.h files in the build directory.
idlc_generate(
    TARGET MyData_lib         
    FILES MyData.idl          
)

# 3. Create the Publisher Executable
# FIX: Compile the generated MyData.c directly into the executable.
add_executable(
    MyPublisher
    publisher.c
    ${CMAKE_CURRENT_BINARY_DIR}/MyData.c 
)

# Link the Publisher to ONLY the core DDS library.
target_link_libraries(
    MyPublisher
    PRIVATE
    ddsc              # Core Cyclone DDS C library
)

# 4. Create the Subscriber Executable
# FIX: Compile the generated MyData.c directly into the executable.
add_executable(
    MySubscriber
    subscriber.c
    ${CMAKE_CURRENT_BINARY_DIR}/MyData.c 
)

# Link the Subscriber to ONLY the core DDS library.
target_link_libraries(
    MySubscriber
    PRIVATE
    ddsc              # Core Cyclone DDS C library
)

# 5. Add required include directory (for auto-generated headers)
# This is correct and necessary to find MyData.h
target_include_directories(
    MyPublisher
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)

target_include_directories(
    MySubscriber
    PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
)