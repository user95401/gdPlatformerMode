cmake_minimum_required(VERSION 3.10.0)
if(EXISTS ${LIBRARY_OUTPUT_PATH}/${PROJECT_NAME}.dll)
   file(COPY 
        about.md
        changelog.md
        logo.png
        mod.json
        ${LIBRARY_OUTPUT_PATH}/${PROJECT_NAME}.dll
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../)

    file(COPY resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../resources/)
    file(COPY  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/../${creator}.${PROJECT_NAME}.dll)
    file(RENAME ${CMAKE_CURRENT_BINARY_DIR}/../${PROJECT_NAME}.dll ${CMAKE_CURRENT_BINARY_DIR}/../${creator}.${PROJECT_NAME}.dll)

    file(ARCHIVE_CREATE
        OUTPUT "${CMAKE_CURRENT_LIST_DIR}/release/${creator}.${PROJECT_NAME}.geode"
        PATHS 
        about.md
        changelog.md
        logo.png
        mod.json
        resources/
        ${creator}.${PROJECT_NAME}.dll
        FORMAT "zip")

    file(COPY "${CMAKE_CURRENT_LIST_DIR}/release/${creator}.${PROJECT_NAME}.geode" DESTINATION 
    ${GEODE_MOD_OUTPUT_PATH})

    message("geode makeup done: see at ${GEODE_MOD_OUTPUT_PATH} or ${CMAKE_CURRENT_LIST_DIR}/release/${PROJECT_NAME}.geode")
else()
    message("geode makeup failed: ${LIBRARY_OUTPUT_PATH}/${PROJECT_NAME}.dll isnt exists yet.")
endif()
