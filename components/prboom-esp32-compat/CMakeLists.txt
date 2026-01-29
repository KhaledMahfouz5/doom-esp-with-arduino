# components/prboom-esp32-compat/CMakeLists.txt

idf_component_register(
    SRCS
        i_main.c
        i_network.c
        i_sound.c
	i_joystick.c
        i_system.c
        i_video.c
        spi_lcd.c
    INCLUDE_DIRS
        "include"
	"."
    REQUIRES
        prboom
        driver
        esp_system
        esp_timer
        esp_event
	spi_flash
        vfs
        fatfs
)

target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-unused-but-set-variable
    -Wno-unused-variable
    -Wno-unused-function
)
