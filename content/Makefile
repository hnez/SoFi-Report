LUALATEX= lualatex
TEX_OPTIONS= --output-format=pdf
TEXINPUTS= .:..:../theme:

export TEXINPUTS

.PHONY: all

all: Report.pdf

%.pdf: %.tex
	$(LUALATEX) $(TEX_OPTIONS) $(<)
