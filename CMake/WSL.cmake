function(is_wsl result)
    if (UNIX AND NOT APPLE)
        execute_process(
                COMMAND sh -c "test -d /proc/sys/fs/binfmt_misc && grep -q WSL /proc/version"
                RESULT_VARIABLE WSL_CHECK_RESULT
                OUTPUT_QUIET ERROR_QUIET
        )
        if (WSL_CHECK_RESULT EQUAL 0)
            set(${result} TRUE PARENT_SCOPE)
        else ()
            set(${result} FALSE PARENT_SCOPE)
        endif ()
    else ()
        set(${result} FALSE PARENT_SCOPE)
    endif ()
endfunction()