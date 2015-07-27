add_library(libsel4printf STATIC
    src/sel4_printf.c
)

target_compile_definitions(libsel4printf
    PUBLIC HAVE_SEL4_PRINTF
)

target_include_directories(libsel4printf SYSTEM
    PUBLIC "include"
)

target_link_libraries(libsel4printf
    libsel4
    libsel4string
    libsel4putchar)

