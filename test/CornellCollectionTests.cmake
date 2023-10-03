
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
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0108/1.2.826.0.1.3680043.2.656.4.1.1.45/S02A06"
  159 251 0
  47 46 17
  1990.41
  )

TEST_CORNELL_DATASET(CornellZeroChange-ST0109-S03A01Full ST0109-S03A01
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0109/1.2.826.0.1.3680043.2.656.4.1.1.868/S03A01"
  125 233 129
  56 48 25
  2390.58
  )

TEST_CORNELL_DATASET(CornellZeroChange-ST0100-S03A01Full ST0109-S03A02
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0109/1.2.826.0.1.3680043.2.656.4.1.1.868/S03A02"
  126 256 0
  56 44 30
  2698.9
  )

#Fails in this case.
TEST_CORNELL_DATASET(CornellZeroChange-ST0110-S02A01Full ST0110-S02A02
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0110/1.2.826.0.1.3680043.2.656.4.1.1.58/S02A02"
  200 249 0
  68 85 23
  18078.2
  )

TEST_CORNELL_DATASET(CornellZeroChange-ST0111-S02A02Full ST0111-S02A02
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0111/1.2.826.0.1.3680043.2.656.4.1.1.62/S02A02"
  364 238 0
  69 55 17
  3496.34
  )

TEST_CORNELL_DATASET(CornellZeroChange-ST0111-S02A05Full ST0111-S02A05
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0111/1.2.826.0.1.3680043.2.656.4.1.1.62/S02A05"
  357 239 0
  77 58 19
  2484.37
  )

TEST_CORNELL_DATASET(CornellZeroChange-ST0112-S00A06Full ST0112-S00A06
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0112/1.2.826.0.1.3680043.2.656.4.1.1.749/S00A06"
  119 282 0
  61 58 13
  290.076
  )

TEST_CORNELL_DATASET(CornellZeroChange-ST0112-S00A07Full ST0112-S00A07
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0112/1.2.826.0.1.3680043.2.656.4.1.1.749/S00A07"
  118  273  0
  57 57 13
  297.629
  )


TEST_CORNELL_DATASET(CornellZeroChange-ST0113-S00A02Full ST0113-S00A02
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0113/1.2.826.0.1.3680043.2.656.4.1.1.794/S00A02"
  111 260 0
  53 58 17
  670.271
  )

TEST_CORNELL_DATASET(CornellZeroChange-ST0113-S00A05Full ST0113-S00A05
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0113/1.2.826.0.1.3680043.2.656.4.1.1.794/S00A05"
  107 249 0
  57 69 15
  654.427
  )


# Part solid case
TEST_CORNELL_DATASET(CornellZeroChange-ST0114-S02A01Full ST0114-S02A01
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0114/1.2.826.0.1.3680043.2.656.4.1.1.1444/S02A01"
  159  265  32 
  86 91 56
  3317.62  ) # solid volume was 2489.7mm^3

# Data characteristics completely different from its pair above such as being 5mm slice thickness etc..
# and having a very thin slab instead of the full dataset.. its unlikely that this is a fair comparison
TEST_CORNELL_DATASET(CornellZeroChange-ST0114-S02A02Full ST0114-S02A02
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0114/1.2.826.0.1.3680043.2.656.4.1.1.1444/S02A02"
  185  244  0
  72 70 5
  2864.81  ) # solid volume was 2120.05mm^3



TEST_CORNELL_DATASET(CornellZeroChange-ST0115-S02A01Full ST0115-S02A01
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0115/11.2.826.0.1.3680043.2.656.4.1.1.1491/S02A01"
  176  224  198
  54 52 34
  332.478  ) 

# Pair omitted.. its characteristics are very different.



# Data characteristics are very different... 
TEST_CORNELL_DATASET(CornellZeroChange-ST0117-S02A04Full ST0117-S02A04
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0117/1.2.826.0.1.3680043.2.656.4.1.1.2012/S02A04"
  130  283   0
  58 53 5
  422.54  ) 
TEST_CORNELL_DATASET(CornellZeroChange-ST0117-S02A01Full ST0117-S02A01
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0117/1.2.826.0.1.3680043.2.656.4.1.1.2012/S02A01"
  122  282  104
  57 59 34
  224.425  )



# Part solid case
TEST_CORNELL_DATASET(CornellZeroChange-ST0118-S02A03Full ST0118-S02A03
  "Cornell\ Zero\ Change\ Data/PRST-20080922/ST0118/1.2.826.0.1.3680043.2.656.4.1.1.2012/S02A03"
  383  242  0
  53 59 10
  1310.26  ) # solid part is 543.606

