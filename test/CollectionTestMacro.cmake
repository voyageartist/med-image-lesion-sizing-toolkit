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
  ${TEMP}/LSMT${SEGMENTATION_METHOD_ID}_Test${DATASET_OBJECT_ID}.mha
  LSMT${SEGMENTATION_METHOD_ID}
  ${DATASET_ID}
  ${EXPECTED_VOLUME}
  ${TEMP}/VolumeEstimationA_${DATASET_OBJECT_ID}.txt
  )
endmacro(VOLUME_ESTIMATION_A)


macro(VOLUME_ESTIMATION_B DATASET_ID OBJECT_ID SEGMENTATION_METHOD_ID EXPECTED_VOLUME)
set(DATASET_OBJECT_ID ${DATASET_ID}_${OBJECT_ID})
set(ISO_VALUE -0.5) # Compensate for half-pixel shift in Canny-Edges
if(LSTK_USE_VTK)
  add_test(LSMTVEB${SEGMENTATION_METHOD_ID}_${DATASET_OBJECT_ID}
    ${CXX_TEST_PATH}/IsoSurfaceVolumeEstimation
    ${TEMP}/LSMT${SEGMENTATION_METHOD_ID}_Test${DATASET_OBJECT_ID}.mha
    ${ISO_VALUE}
    LSMT${SEGMENTATION_METHOD_ID}
    ${DATASET_ID}
    ${EXPECTED_VOLUME}
    ${TEMP}/VolumeEstimationB_${DATASET_OBJECT_ID}.txt
    )
endif(LSTK_USE_VTK)
endmacro(VOLUME_ESTIMATION_B)


macro(SEGMENTATION_SCREEN_SHOT DATASET_ID OBJECT_ID ISO_VALUE CONTOUR_ID)

set(DATASET_OBJECT_ID $