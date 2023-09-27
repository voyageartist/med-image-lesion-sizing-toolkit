
macro(TEST_CORNELL_DATASET MASTER_DATASET_ID DATASET_ID DATASET_DIRECTORY ROI_X ROI_Y ROI_Z ROI_DX ROI_DY ROI_DZ EXPECTED_VOLUME)

set(COLLECTION_PATH ${TEST_CORNELL_DATA_ROOT} )
set(DATASET_ROI ${TEMP}/${DATASET_ID}_ROI.mha)

CONVERT_DICOM_TO_META( ${COLLECTION_PATH} ${MASTER_DATASET_ID} ${DATASET_DIRECTORY} )
EXTRACT_REGION_OF_INTEREST(${MASTER_DATASET_ID} ${DATASET_ID} ${ROI_X} ${ROI_Y} ${ROI_Z} ${ROI_DX} ${ROI_DY} ${ROI_DZ} )
GENERATE_FEATURES( ${DATASET_ID} )
SCREEN_SHOT_FEATURES(  ${DATASET_ID} 001 )
COMPUTE_SEGMENTATIONS( ${DATASET_ID} 001 ${EXPECTED_VOLUME})

endmacro(TEST_CORNELL_DATASET)


TEST_CORNELL_DATASET(SL0074Full SL0074 
  "CornellPartSolid3/SL0074-20000101/SL0074/20000101-134856-2-1"
  100 170 180 
  100 120  64
  10000.0
  )

TEST_CORNELL_DATASET(SM0052Full SM0052 
  "SM0052-20000101/SM0052/20000101-154701-2-1"
  317   193   154
  111   116   85
  10000.0
  )

TEST_CORNELL_DATASET(SM0053Full SM0053 
  "SM0053-20000101/SM0053/20000101-123846-2-1"
  317   193   154  # MUST BE REPLACED
  111   116   85
  10000.0
  )

TEST_CORNELL_DATASET(SM0054Full  SM0054
  "SM0054-20000101/SM0054/20000101-141301-2-1"
  324 235 129
  124 103 54
  10000.0
  )

TEST_CORNELL_DATASET(SM0055Full  SM0055
  "CornellPartSolid2/SM0055-20000101/SM0055/20000101-122401-2-1"
  270 180 120 
  160 100  69 
  10000.0
  )

TEST_CORNELL_DATASET(SM0057Full  SM0057
  "SM0057-20000101/SM0057/20000101-113850-2-1"
  324 235 129  # MUST BE REPLACED
  124 103 54
  10000.0
  )

TEST_CORNELL_DATASET(SM0060Full SM0060
  "SM0060-20000101/SM0060/20000101-132756-2-1"
  324 235 129  # MUST BE REPLACED
  124 103 54
  10000.0
  )

TEST_CORNELL_DATASET(SM0061Full SM0061
  "SM0061-20000101/SM0061/20000101-150831-2-1"
  324 235 129  # MUST BE REPLACED
  124 103 54
  10000.0
  )

TEST_CORNELL_DATASET(SM0063Full  SM0063
  "SM0063-20000101/SM0063/20000101-084112-2-1"
  324 235 129  # MUST BE REPLACED
  124 103 54
  10000.0
  )

TEST_CORNELL_DATASET(SM0065Full SM0065
  "SM0065-20000101/SM0065/20000101-125137-2-1"
  324 235 129  # MUST BE REPLACED
  124 103 54
  10000.0
  )

TEST_CORNELL_DATASET(SM0066Full SM0066
  "SM0066-20000101/SM0066/20000101-085728-2-1"
  324 235 129  # MUST BE REPLACED
  124 103 54
  10000.0
  )

TEST_CORNELL_DATASET(SM0067Full  SM0067
  "SM0067-20000101/SM0067/20000101-133840-2-1"
  324 235 129  # MUST BE REPLACED
  124 103 54
  10000.0
  )

TEST_CORNELL_DATASET(SM0068Full SM0068
  "SM0068-20000101/SM0068/20000101-133453-2-1"
  324 235 129  # MUST BE REPLACED
  124 103 54
  10000.0
  )

TEST_CORNELL_DATASET(SS0026Full_a SS0026
  "SS0026/20000101-110155-0-2"
  68 158 57
  175 153 45
  10000.0
  )

TEST_CORNELL_DATASET(CornellZeroChange-ST0108-S02A02Full ST0108-S02A02
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0108/1.2.826.0.1.3680043.2.656.4.1.1.45/S02A02"
  149 251 0
  59 51 29
  1992.76
  )

TEST_CORNELL_DATASET(CornellZeroChange-ST0108-S02A06Full ST0108-S02A06
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0108/1.2.826.0.1.36800