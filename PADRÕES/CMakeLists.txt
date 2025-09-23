cmake_minimum_required(VERSION 3.16)
project(HidrometroWindows)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configurações para Windows
if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    add_definitions(-DUNICODE -D_UNICODE)
    add_definitions(-DNOMINMAX)
endif()

# Arquivos do projeto
set(SOURCES
        src/main.cpp
        src/Window.cpp
        src/Hidrometro.cpp
        src/Background.cpp
        src/SliderVazao.cpp
        src/ResourceManager.cpp
)

set(HEADERS
        include/Window.h
        include/Hidrometro.h
        include/Background.h
        include/SliderVazao.h
        include/ResourceManager.h
        include/Constants.h
)

# Criar executável
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Link com bibliotecas do Windows
if(WIN32)
    target_link_libraries(${PROJECT_NAME} user32 gdi32)
endif()

# Incluir diretórios
target_include_directories(${PROJECT_NAME} PRIVATE include)