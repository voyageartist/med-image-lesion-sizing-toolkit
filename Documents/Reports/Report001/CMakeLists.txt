project(Report001)

 
#
# These are source files needed for the report generation.
#

set(REPORT_SRCS
  algorithmic.sty
  algorithm.sty
  amssymb.sty
  fancyhdr.sty
  floatflt.sty
  fncychap.sty
  InsightArticle.cls
  InsightJournal.bib
  InsightJournal.ist
  InsightJournal.sty
  times.sty
  picins.sty
  )


#
#  Get the list of figures generated during testing
#
file(GLOB REPORT_GENERATED_FIGURES "${LesionSizingToolkit_BINARY_DIR}/Testing/Temporary/*.png")

set(PROJECT_REPORT_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
set(PROJECT_REPORT_BINARY_DIR "${PROJECT_BINARY_DIR}")

#
# This adds a custom command for each source file in REPORT_SRCS
# that copies the file from the source directory to the binary
# directory where the pdf will be generated.
#
foreach(SOURCE_FILE ${REPORT_SRCS})
   add_custom_command(
     OUTPUT   ${PROJECT_REPORT_BINARY_DIR}/${SOURCE_FILE}
     DEPENDS  ${LesionSizingToolkit_SOURCE_DIR}/Documents/Latex/${SOURCE_FILE}
     COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${LesionSizingToolkit_SOURCE_DIR}/Documents/Latex/${SOURCE_FILE}
        ${PROJECT_REPORT_BINARY_DIR}/${SOURCE_FILE}
    )
   set(COPY_RESULTS ${COPY_RESULTS} ${PROJECT_REPORT_BINARY_DIR}/${SOURCE_FILE})
endforeach(SOURCE_FILE ${REPORT_SRCS})

#
# This adds a custom command for each PNG image file in Testing/Temporary directory
# that copies the file from to the binary directory where the pdf will be generated.
#
foreach(SOURCE_FIGURE_FILE ${REPORT_GENERATED_FIGURES})
   get_filename_component(SOURCE_FIGURE_FILENAME ${SOURCE_FIGURE_FILE} NAME)
   set(DESTINATION_FIGURE_FILENAME ${PROJECT_REPORT_BINARY_DIR}/${SOURCE_FIGURE_FILENAME})
   add_custom_command(
     OUTPUT    ${DESTINATION_FIGURE_FILENAME}
     DEPENDS   ${SOURCE_FIGURE_FILE}
     COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${SOURCE_FIGURE_FILE}
        ${DESTINATION_FIGURE_FILENAME}
    )
   set(COPY_RESULTS ${COPY_RESULTS} ${DESTINATION_FIGURE_FILENAME})
endforeach(SOURCE_FIG