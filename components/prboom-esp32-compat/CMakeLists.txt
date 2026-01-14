# components/prboom-esp32-compat/CMakeLists.txt

idf_component_register(
    SRCS
        gamepad.c
        i_main.c
        i_network.c
        i_sound.c
        i_system.c
        i_video.c
        psxcontroller.c
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
    -Wno-discarded-qualifiers
    -Wno-deprecated-declarations
    -Wno-unused-but-set-variable
    -Wno-unused-variable
    -Wno-unused-function
    -Wno-int-conversion
    -Wno-incompatible-pointer-types
)
