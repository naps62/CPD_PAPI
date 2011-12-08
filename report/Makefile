default: view_slides

all: report slides
	
report: main.tex roofline.eps x4_roofline.eps
	pdflatex --shell-escape main.tex

slides: slides.tex
	pdflatex --shell-escape slides.tex

view: report
	xdg-open report.pdf

view_slides: slides
	xdg-open slides.pdf

clean:
	rm -rf *.aux *.log *.out *.toc *.vrb 
