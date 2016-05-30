/*
This class has been directly used from https://github.com/sammyer/Chordroid/blob/master/src/com/chordroid/ProbTables.java
[Accessed 25th August 2013]

*/
#ifndef PROBABILITYTABLES
#define PROBABILITYTABLES

//bassProbTable is an array of probabilities that a note is the root note given the normalized chroma value of
//that note and of the fifth above that note.  values are rounded up to nearest tenth
//e.g. if norm'd chroma val of C=0.2 and G=0.33  look up bassProbTable[2][4] to get probability that C is root note
const float bassProbTable[11][11]={{0.02, 0.03, 0.04, 0.06, 0.08, 0.09, 0.09, 0.09, 0.09, 0.08, 0.1},
								   {0.03, 0.05, 0.07, 0.11, 0.14, 0.16, 0.16, 0.18, 0.2, 0.24, 0.32},
								   {0.05, 0.08, 0.11, 0.18, 0.24, 0.3, 0.36, 0.43, 0.46, 0.52, 0},
								   {0.09, 0.14, 0.22, 0.32, 0.42, 0.51, 0.57, 0.59, 0.54, 0, 0},
								   {0.14, 0.23, 0.35, 0.48, 0.58, 0.62, 0.66, 0.64, 0, 0, 0},
								   {0.2, 0.33, 0.47, 0.59, 0.66, 0.71, 0.71, 0, 0, 0, 0},
								   {0.27, 0.41, 0.57, 0.66, 0.74, 0.77, 0, 0, 0, 0, 0},
								   {0.34, 0.50, 0.68, 0.77, 0.80, 0, 0, 0, 0, 0, 0},
								   {0.41, 0.59, 0.74, 0.79, 0, 0, 0, 0, 0, 0, 0},
								   {0.5, 0.68, 0.8, 0, 0, 0, 0, 0, 0, 0, 0},
								   {0.59, 0.78, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

//chordGaussTable is the table of coefficients for the gaussian curve which approimates the pdf for a pitch class per chordType.
//The first array index is the chordType# and the second is the semitones from the root.
//For example, chordGaussTable[1][3] is the parameters for the pdf of the minor third in a minor chord (1=minor, 3=3 semitones up = m3)
//parameters are a,m,s,b where pdf(x)=a*e^(0.5((x-m)/s)^2)+b*e^(-7|x-m|)
const float chordGaussTable[6][12][4]={{{2.0088, 0.1303, 0.2104, 0.5096}, {4.4047, 0.0187, 0.0292, 1.4587}, {3.0630, 0.0309, 0.0834, 2.0546}, {3.9407, 0.0061, 0.0394, 1.7892}, {2.7331, 0.0520, 0.1581, 0.7108}, {3.1481, 0.0199, 0.0354, 2.4853}, {4.5306, 0.0120, 0.0346, 1.6424}, {2.1642, 0.1474, 0.1765, 0.5593}, {4.5339, 0.0170, 0.0327, 1.5210}, {3.3378, 0.0184, 0.0342, 2.6016}, {2.8700, 0.0206, 0.0375, 2.8664}, {3.2434, 0.0250, 0.0356, 2.3124}}, {{2.0825, 0.1500, 0.1939, 0.6136}, {4.5841, 0.0200, 0.0276, 1.1976}, {2.6452, 0.0300, 0.0460, 2.6167}, {2.1399, 0.0452, 0.2017, 1.0326}, {4.3150, 0.0183, 0.0455, 1.5766}, {4.0241, 0.0020, 0.0433, 2.5278}, {4.8395, 0.0108, 0.0283, 1.4890}, {2.0942, 0.1520, 0.1780, 0.7924}, {4.7829, 0.0181, 0.0272, 1.2261}, {4.8758, 0.0059, 0.0355, 1.7605}, {3.4530, -0.0376, 0.1261, 2.7241}, {4.4418, 0.0078, 0.0395, 1.2891}}, {{2.3706, -0.0525, 0.2686, 1.7738}, {4.2991, -0.0194, 0.0805, 1.9729}, {3.5591, 0.0294, 0.0231, 1.3993}, {2.5952, 0.0744, 0.1480, 0.7723}, {2.0533, 0.0383, 0.0217, 2.4693}, {5.1222, -0.0186, 0.0452, 2.3972}, {2.0987, -0.0765, 0.3410, 0.9359}, {4.3088, -0.0306, 0.1040, 2.3549}, {3.4031, 0.0291, 0.0232, 2.0206}, {0.7933, -0.0078, 0.2881, 2.3216}, {3.0507, -0.0200, 0.0856, 3.9497}, {3.9342, 0.0228, 0.0173, 1.3351}}, {{1.8573, 0.2287, 0.1734, 0.8760}, {5.3511, 0.0232, 0.0196, 2.0575}, {3.7005, 0.0314, 0.0195, 3.1548}, {3.9217, 0.0340, 0.0170, 1.9935}, {2.4570, 0.0873, 0.1612, 1.3074}, {4.5823, 0.0162, 0.0430, 1.4661}, {5.9839, 0.0101, 0.0222, 2.0137}, {2.9026, -0.0394, 0.1983, 2.1751}, {2.6534, -0.0267, 0.2390, 0.5885}, {6.5127, 0.0073, 0.0346, 1.2906}, {5.8188, 0.0258, 0.0300, 2.0566}, {4.4932, 0.0228, 0.0102, 2.1591}}, {{1.8829, 0.2390, 0.2224, 0.4466}, {4.4663, 0.0150, 0.0197, 1.4057}, {3.3100, 0.0508, 0.1069, 1.2335}, {7.2964, -0.0006, 0.0302, 1.6360}, {3.3253, -0.0102, 0.1054, 2.1211}, {2.7354, -0.0385, 0.2623, 0.5122}, {5.2451, 0.0057, 0.0334, 1.1639}, {1.2951, 0.1243, 0.0531, 3.6697}, {3.2416, 0.0215, 0.0193, 1.3984}, {7.6349, -0.0065, 0.0301, 2.4335}, {5.4852, -0.0039, 0.0206, 4.0150}, {3.8236, -0.0122, 0.0719, 2.3302}}, {{1.5482, 0.0231, 0.1030, 1.5772}, {1.7568, 0.0268, 0.0457, 1.9348}, {2.4729, -0.0171, 0.1716, 0.6756}, {2.4317, -0.0071, 0.0803, 1.5229}, {1.9558, 0.0506, 0.1550, 0.5947}, {1.4173, 0.0279, 0.0526, 1.8601}, {2.4411, -0.0128, 0.0681, 2.6279}, {1.2797, 0.0144, 0.0717, 2.4092}, {2.0983, 0.0259, 0.0389, 1.9460}, {1.9687, -0.0461, 0.1822, 1.9378}, {2.3472, -0.0030, 0.0605, 2.0159}, {2.4397, -0.0343, 0.1446, 1.4907}}};

//chordZeroTable represents the pdf between 0 and 0.01.  Because zero is disproportionately common, it causes a spike in the
//gaussian so we assume a constant probability rather than calculate the gaussian from the parameters in chordGaussTable.
//Also, this saves time, since we can just use a lookup table for the pdf rather than calculate a gaussian curve
const float chordZeroTable[6][12]={
	{11.8336, 61.4076, 27.7690, 58.5115, 20.2813, 46.6345, 58.2972, 11.0445, 58.4741, 46.1073, 41.6263, 47.7323},
	{7.1689, 65.5893, 39.6638, 18.9510, 52.0778, 47.6387, 64.8805, 7.4565, 64.8157, 57.6225, 31.0936, 63.1065},
	{20.3737, 48.0427, 65.3915, 21.2633, 47.7758, 57.9181, 24.3772, 39.0569, 57.2064, 46.2633, 34.0747, 70.1957},
	{6.2864, 59.3714, 41.6764, 56.4610, 8.9639, 53.4342, 59.8370, 23.2829, 25.0291, 58.6729, 44.5867, 60.7683},
	{3.3624, 69.1158, 23.1631, 65.8780, 39.7260, 24.1594, 68.6177, 5.8531, 72.8518, 59.7758, 47.8207, 49.5641},
	{53.1671, 59.1666, 45.1916, 61.3099, 43.2236, 59.0551, 53.7567, 51.1115, 59.0790, 45.0402, 60.4892, 49.4144}};

//chord types with their names and relative probabilities (numbers made up by Sam Myer, altered by Daniel Oluyomi)
//const char chordTypes[5][4]={"","m","dim","aug","sus"};
const float chordTypeProbs[5]={0.4, 0.5, 0.05, 0.01, 0.04};

#endif 