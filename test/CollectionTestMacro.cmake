include( ParseArgumentsMacro.cmake )

#  Helper macro that will run the set of operations in a given dataset.
#
macro(FEATURE_SCREEN_SHOT DATASET_ID OBJECT_ID ISO_VALUE CONTOUR_ID)

set(DATASET_OBJECT_ID ${DATASET_ID}_${OBJECT_ID})
set(DATASET_ROI ${TEMP}/${DATASET_ID}_ROI.mha)

set(SEEDS_FILE ${TEST_DATA_ROOT}/Input/${DATASET_OBJECT_ID}_Seeds.txt)

add_test(SCRN_${CONTOUR_ID}_${DATASET_OBJECT_ID}
  ${CXX_TEST_PATH}/ViewImageSlicesAndSegmentationContours
  ${DATASET_ROI}
  ${SEEDS_FILE}
  ${ISO_VALUE}
  1
  ${TEMP}/${CONTOUR_ID}_Test${DATASET_OBJECT_ID}.png
  ${TEMP}/${CONTOUR_ID}_Test${DATASET_ID}.mha
  )

endmacro(FEATURE_SCREEN_SHOT)


macro(VOLUME_ESTIMATION_A DATASET_ID OBJECT_ID SEGMENTATION_METHOD_ID EXPECTED_VOLUME)
set(DATASET_OBJECT_ID ${DATASET_ID}_${OBJECT_ID})
add_test(LSMTVEA${SEGMENTATION_METHOD_ID}_${DATASET_OBJECT_ID}
  ${CXX_TEST_PATH}/itkGrayscaleImageSegmentationVolumeEstimatorTest2
  ${TEMP}