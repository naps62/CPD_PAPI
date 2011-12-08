default: all

view: all open

all: gnuplot latex

gnuplot: roofline.gnuplot
	gnuplot roofline.gnuplot

latex: main.tex roofline.eps
	pdflatex --shell-escape main.tex > /dev/null

open:
	xdg-open main.pdf

clean:
	rm -rf *.log *.aux
