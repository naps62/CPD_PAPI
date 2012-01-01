#	Initial configurations
set term postscript eps enhanced clip
set output "joined.eps"
#set term epslatex
#set output "joined.eps"

#	Functions
#	--general purpose
min(a,b)	=	(a < b) ? a : b
max(a,b)	=	(a > b) ? a : b
frac(a,b)	=	a / b
rad2deg(r)	=	r * 180 / pi
deg2rad(d)	=	d * pi / 180
#	--labels
yabove(y)	=	y * 1.25
ybelow(y)	=	y * 0.85
xright(x)	=	x * 1.25

#	Constants
#	---User provided data
cpu_flop	=	4					#	maximum floating point throughput
cpu_freq	=	2.4					#	clock cycle frequency (GHz)
cpu_cores	=	2					#	number of cores
mem_width	=	8					#	memory bus width (Bytes)
mem_rate	=	0.800				#	memory clock rate (GHz)
mem_chan	=	2					#	number of memory channels

#	--window
x_min		=	0.0625
x_max		=	16
y_min		=	0.5
y_max		=	128

#	--roofline
cpu_peak	=	cpu_flop * cpu_freq * cpu_cores
									#	theoretical peak GFlop/s
mem_peak	=	mem_width * mem_rate * mem_chan
									#	peak memory bandwidth (GB/s)
mem_teta	=	45					#	memory roof angle (degrees)
mem_b		=	tan( deg2rad( mem_teta ) )
									#	slope
#	---> Ridge Point
ridge_x		=	cpu_peak / mem_peak
ridge_y		=	cpu_peak

#	More Functions
#	---> Memory roof
mem_a(m)	=	exp( log( cpu_peak ) - log( m ) * mem_b )
									#	y=ax^b when x=1
mem_x0(a)	=	exp( log( y_min / a ) / mem_b )
									#	y=ax^b when y=y_min

#	More Constants
#	-- roofline
#	---> Memory roof
mem_roof_a	=	mem_a( ridge_x )
									#	y=ax^b when x=1
mem_roof_x0	=	mem_x0( mem_roof_a )

#	Window
#	--x
set xrange [x_min:x_max]
set logscale x 2
#	--y
set yrange [y_min:y_max]
set logscale y 2

#	Functions yet again
#	--roofs
cpu_roof(x)	=	ridge_y
mem_roof(x)	=	mem_roof_a * x ** mem_b
roofline(x)	=	min( cpu_roof(x) , mem_roof(x) )

#	--ceilings
#	---> CPU
cpu_frac(x,y)	=	frac( cpu_roof(x), y )
cpu_half(x)	=	cpu_frac(x,2)
cpu_qrtr(x) = 	cpu_frac(x,4)
cpu_oct(x)	=	cpu_frac(x,8)
#	---> Memory
mem_frac(x,y)	=	frac( mem_roof(x), y )
mem_half(x)	=	mem_frac(x,2)
#mem_qrtr(x) = 	mem_frac(x,4)
#mem_oct(x)	=	mem_frac(x,8)

#	And Lastly, Constants. Again
#	--ceilings
#	---> Half memory ceiling
mem_half_a	=	mem_half(1)
mem_half_x0	=	mem_x0( mem_half_a )

#	Labels
set xlabel "Operational Intensity (Flops/Byte)"
set ylabel "Attainable GFlops/s"

#	Size
set size square

#	Styles
#	--lines
set style line 1 lt 0 lc rgb "gray40" lw 0.5	# grid lines
set style line 2 lt 1 lc rgb "black" lw 4		# roofs
set style line 3 lt 1 lc rgb "red" lw 2			# cpu ceilings
set style line 4 lt 1 lc rgb "dark-green" lw 2	# memory ceilings

#	X4 Zone
x4_cpu_peak	=	74
x4_mem_peak	=	16.6

x4_ridge_x	=	x4_cpu_peak / x4_mem_peak
x4_ridge_y	=	x4_cpu_peak

x4_mem_a(m)	=	exp( log( x4_ridge_y ) - log( m ) * mem_b )
x4_mem_x0(a)	=	exp( log( y_min / a ) / mem_b )

x4_mem_roof_a	=	x4_mem_a( x4_ridge_x )
x4_mem_roof_x0	=	x4_mem_x0( x4_mem_roof_a )

x4_cpu_roof(x)	=	x4_ridge_y
x4_mem_roof(x)	=	x4_mem_roof_a * x ** mem_b
x4_roofline(x)	=	min( x4_cpu_roof(x) , x4_mem_roof(x) )

x4_cpu_frac(x,y)	=	frac( x4_cpu_roof(x), y )
x4_cpu_half(x)	=	x4_cpu_frac(x,2)
x4_cpu_qrtr(x)	= 	x4_cpu_frac(x,4)
#x4_cpu_oct(x)	=	x4_cpu_frac(x,8)
x4_cpu_hexa(x)	=	x4_cpu_frac(x,16)

x4_mem_frac(x,y)	=	frac( x4_mem_roof(x), y )
x4_mem_half(x)	=	x4_mem_frac(x,2)
x4_mem_qrtr(x)	=	x4_mem_frac(x,4)

x4_mem_half_a	=	x4_mem_half(1)
x4_mem_half_x0	=	x4_mem_x0( x4_mem_half_a )

x4_mem_qrtr_a	=	x4_mem_qrtr(1)
x4_mem_qrtr_x0	=	x4_mem_x0( x4_mem_qrtr_a )

set style line 5 lt 1 lc rgb "gray" lw 4		# roofs
set style line 6 lt 1 lc rgb "orange" lw 2			# cpu ceilings
set style line 7 lt 1 lc rgb "green" lw 2	# memory ceilings
#	End X4 Zone

#	Other configurations
set nokey							#	removes function caption
set grid back linestyle 1			#	grid lines

#	Plot
set multiplot
#	--labels
set label 1 "Peak Floating-Point Performance" at x_max-1,yabove( cpu_roof(x_max) ) right
set label 2 "Peak Memory Bandwidth" at xright( max( mem_roof_x0 , x_min ) ),yabove( yabove( mem_roof( max( mem_roof_x0 , x_min ) ) ) ) left rotate by mem_teta
set label 3 "1. TLP (All cores used)" at x_max-1,ybelow( cpu_roof(x_max) ) right
set label 4 "2. Mul/Add balance" at x_max-1,ybelow( cpu_half(x_max) ) right
set label 5 "3. SIMD" at x_max-1,ybelow( cpu_qrtr(x_max) ) right
set label 6 "4. ILP only" at x_max-1,ybelow( cpu_oct(x_max) ) right
set label 7 "5. No Dual Channel" at xright( xright( max( mem_half_x0 , x_min ) ) ),yabove( mem_half( max( mem_half_x0 , x_min ) ) ) left rotate by mem_teta

#	Begin X4 Zone
plot min( x4_cpu_half(x), x4_mem_roof(x) ) ls 6
plot min( x4_cpu_qrtr(x), x4_mem_roof(x) ) ls 6
plot min( x4_cpu_hexa(x), x4_mem_roof(x) ) ls 6

plot min( x4_mem_half(x), x4_cpu_roof(x) ) ls 7
plot min( x4_mem_qrtr(x), x4_cpu_roof(x) ) ls 7

plot x4_roofline(x) ls 5
#	End X4 Zone

#	--ceilings
#	---> CPU
plot min( cpu_half(x), mem_roof(x) ) ls 3
plot min( cpu_qrtr(x), mem_roof(x) ) ls 3
plot min( cpu_oct(x), mem_roof(x) ) ls 3
#	---> Memory
plot min( mem_half(x), cpu_roof(x) ) ls 4

#	--roofline
plot roofline(x) ls 2

unset multiplot

#	Cleanup
set output
