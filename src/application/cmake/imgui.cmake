include(FetchContent)

FetchContent_Declare(
	 imgui
	 GIT_REPOSITORY https://github.com/ocornut/imgui.git
	 GIT_TAG origin/docking
)

FetchContent_MakeAvailable(imgui)
FetchContent_GetProperties(imgui SOURCE_DIR IMGUI_SRC_DIR)

set(
    IMGUI_SRCFILES

    # Base Files
    ${IMGUI_SRC_DIR}/imconfig.h
    ${IMGUI_SRC_DIR}/imgui.h ${IMGUI_SRC_DIR}/imgui.cpp
    ${IMGUI_SRC_DIR}/imgui_demo.cpp
    ${IMGUI_SRC_DIR}/imgui_draw.cpp
    ${IMGUI_SRC_DIR}/imgui_internal.h
    ${IMGUI_SRC_DIR}/imgui_tables.cpp
    ${IMGUI_SRC_DIR}/imgui_widgets.cpp
    ${IMGUI_SRC_DIR}/imstb_rectpack.h
    ${IMGUI_SRC_DIR}/imstb_textedit.h
    ${IMGUI_SRC_DIR}/imstb_truetype.h

    # SDL3 & OpenGl files
    ${IMGUI_SRC_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_SRC_DIR}/backends/imgui_impl_sdl3.h
    ${IMGUI_SRC_DIR}/backends/imgui_impl_opengl3.cpp
    ${IMGUI_SRC_DIR}/backends/imgui_impl_opengl3.h
)

add_library(imgui ${IMGUI_SRCFILES})

target_include_directories(imgui PUBLIC ${SDL3_INCLUDE_DIRS} ${IMGUI_SRC_DIR} ${IMGUI_SRC_DIR}/backends)
