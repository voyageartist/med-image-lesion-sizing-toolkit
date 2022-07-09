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
     COMMAND ${CMAKE_COMMAND}