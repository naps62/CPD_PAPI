default: view

all: report slides

report: main.tex
	pdflatex --shell-escape main.tex

slides: slides.tex
	pdflatex --shell-escape slides.tex

view: report
	xdg-open main.pdf

view_slides: slides
	xdg-open slides.pdf

clean:
	rm -rf *.aux *.log *.out *.toc *.vrb 
