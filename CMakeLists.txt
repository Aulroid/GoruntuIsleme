cmake_minimum_required(VERSION 3.10)
project(GoruntuIsleme)

set(CMAKE_CXX_STANDARD 17)

# Homebrew ile kurulan OpenCV'yi bul
find_package(OpenCV REQUIRED)

# Proje içindeki tüm main.cpp dosyalarını bul (alt klasörler dahil)
file(GLOB_RECURSE SOURCES "main.cpp")

foreach(SOURCE ${SOURCES})
    # build veya gizli klasörleri atla
    if(SOURCE MATCHES "build" OR SOURCE MATCHES "\\.")
        continue()
    endif()

    # main.cpp'nin bulunduğu klasörün adını hedef (executable) adı yap
    get_filename_component(DIR_PATH ${SOURCE} DIRECTORY)
    get_filename_component(TARGET_NAME ${DIR_PATH} NAME)

    # Hedef oluştur ve kütüphaneleri bağla
    add_executable(${TARGET_NAME} ${SOURCE})
    target_link_libraries(${TARGET_NAME} ${OpenCV_LIBS})
endforeach()
