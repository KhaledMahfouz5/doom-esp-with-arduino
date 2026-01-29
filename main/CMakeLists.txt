# main/CMakeLists.txt

idf_component_register(
    SRCS
        "app_main.c"
        "uart_control.c"
    INCLUDE_DIRS "."
)
