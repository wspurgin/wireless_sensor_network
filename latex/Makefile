
report.pdf : report.tex #chap*.tex
	pdflatex report.tex
	pdflatex report.tex
	bibtex report
	pdflatex report.tex

open : report.pdf
	open report.pdf

clean :
	\rm -f *.aux *.bbl *.blg *.dvi *.idx *.lof *.log *.lot *.toc report.pdf

