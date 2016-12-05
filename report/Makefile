# Produce sensys-abstract compliant PDFs from all Markdown files in a directory
# Manuel Moraga | mmoraga@kth.se

# List files to be made
PDFS := $(patsubst %.Rmd,%.pdf,$(wildcard *.Rmd))

all : $(PDFS)

# Accepts PDF target with markdown syntax, makes them using pandoc
%.pdf : %.Rmd
		Rscript -e 'library(rmarkdown); render("$^")'

clean :
		rm $(PDFS)

open : report.pdf
	open $^

rebuild: clean all
