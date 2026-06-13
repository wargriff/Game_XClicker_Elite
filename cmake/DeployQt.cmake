function(gmel_deploy_qt target_name)
    if(NOT WIN32 OR NOT GMEL_QT_PREFIX)
        return()
    endif()

    find_program(GMEL_WINDEPLOYQT windeployqt
        HINTS "${GMEL_QT_PREFIX}/bin"
        NO_DEFAULT_PATH)

    if(NOT GMEL_WINDEPLOYQT)
        find_program(GMEL_WINDEPLOYQT windeployqt)
    endif()

    if(GMEL_WINDEPLOYQT)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND "${GMEL_WINDEPLOYQT}"
                "$<TARGET_FILE:${target_name}>"
                --no-translations
                --no-compiler-runtime
            COMMENT "Deploiement Qt (DLL) -> build/$<CONFIG>/"
            VERBATIM)
    endif()
endfunction()
