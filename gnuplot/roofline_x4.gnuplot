# initial config
set term postscript eps enhanced
set output 'roofline_x4.eps'
set nokey
set grid layerdefault   linetype 0 linewidth 1.000,  linetype 0 linewidth 1.000

set xlabel "Operational Intensity"
set ylabel "GFlops"

# sets log base 2 scale for both axis
set logscale x 2
set logscale y 2

# label offsets
L_MEM_X=0.125
L_MEM_ANG=32
L_MUL=1.15

# ragen of each axis
MAX_X=16
MAX_Y=128
set xrange [0.125:MAX_X]
set yrange [0.5:MAX_Y]

# CPU CONSTANTS
PEAK_GFLOPS=19.2
X4_PEAK_GFLOPS=74
NUM_CORES=2
X4_NUM_CORES=8


#ceilings
C_ALL_CORES		= 1
C_MUL_ADD_BAL	= NUM_CORES
C_SIMD			= 2 * C_MUL_ADD_BAL
C_ILP_ONLY		= 2 * C_SIMD

# MEM CONSTATS
PEAK_MEM_BW=12.8
X4_PEAK_MEM_BW=16.6
NUM_CHANNELS=2
# first ceiling, without multiple memory channels
C_NO_MULTI_CHANNEL	= NUM_CHANNELS

# FUNCTIONS
mem_roof(x)	= x * PEAK_MEM_BW
x4_mem_roof(x) = x * X4_PEAK_MEM_BW
cpu_roof	= PEAK_GFLOPS
x4_cpu_roof = X4_PEAK_GFLOPS

min(x, y)	= (x < y) ? x : y

cpu_ceiling(x, y)	= min(mem_roof(x), y)
x4_cpu_ceiling(x, y) = min(x4_mem_roof(x), y)
mem_ceiling(x)		= min(x, PEAK_GFLOPS)
x4_mem_ceiling(x)	= min(x, X4_PEAK_GFLOPS)
roofline(x, y)		= cpu_ceiling(x, y)
x4_roofline(x, y)	= x4_cpu_ceiling(x, y)

# LINE STYLES
LINE_ROOF=1
LINE_CEIL=2
X4_LINE_ROOF=3
X4_LINE_CEIL=4
set style line LINE_ROOF	lt 1 lw 6 lc rgb "#8B0000"
set style line LINE_CEIL	lt 1 lw 3 lc rgb "#8B0000"
set style line X4_LINE_ROOF lt 1 lw 6 lc rgb "blue"
set style line X4_LINE_CEIL lt 1 lw 3 lc rgb "blue"

# PLOTS
set multiplot

# X4 CEILINGS
plot x4_roofline(x, x4_cpu_roof) ls X4_LINE_ROOF
plot x4_cpu_ceiling(x, x4_cpu_roof / 2) ls X4_LINE_CEIL
plot x4_cpu_ceiling(x, x4_cpu_roof / 4) ls X4_LINE_CEIL
plot x4_cpu_ceiling(x, x4_cpu_roof / 16) ls X4_LINE_CEIL

# CPU CEILINGS
# All cores (same as roofline)
set label 3 "All cores used" at (MAX_X-1),(cpu_roof/L_MUL) right
plot cpu_ceiling(x, cpu_roof / C_ALL_CORES) ls LINE_CEIL

# MUL/ADD balance / only 1 core
set label 4 "Mul/Add balance, No TLP" at (MAX_X-1),((cpu_roof / C_MUL_ADD_BAL)/L_MUL) right
plot cpu_ceiling(x, cpu_roof / C_MUL_ADD_BAL) ls LINE_CEIL

# SIMD
set label 5 "With SIMD" at (MAX_X-1),((cpu_roof / C_SIMD)/L_MUL) right
plot cpu_ceiling(x, cpu_roof / C_SIMD) ls LINE_CEIL

# No paralellism
set label 6 "ILP Only" at (MAX_X-1),((cpu_roof / C_ILP_ONLY)/L_MUL) right
plot cpu_ceiling(x, cpu_roof / C_ILP_ONLY) ls LINE_CEIL

# MEM CEILINGS
# No dual channel
set label 7 "No Dual Channel" at (L_MEM_X),(mem_roof(L_MEM_X)/C_NO_MULTI_CHANNEL*L_MUL) rotate by L_MEM_ANG
plot mem_ceiling(mem_roof(x) / C_NO_MULTI_CHANNEL) ls LINE_CEIL

# ROOFLINE
set label 1 "Peak FP Performance" at (MAX_X-1),(PEAK_GFLOPS*L_MUL) right
set label 2 "Peak Mem Bandwidth" at L_MEM_X,mem_roof(L_MEM_X)*L_MUL rotate by L_MEM_ANG
plot roofline(x, cpu_roof) ls LINE_ROOF

unset multiplot
