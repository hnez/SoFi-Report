LUALATEX= lualatex
TEX_OPTIONS= --output-format=pdf
TEXINPUTS= .:..:../theme:

export TEXINPUTS

DEPENDENCIES= $(wildcard chapters/*.tex)
DEPENDENCIES+= $(wildcard ../diagrams/*.tex)

.PHONY: all

all: Report.pdf

Report.pdf: Report.tex $(DEPENDENCIES)
	$(LUALATEX) $(TEX_OPTIONS) $(<) < /dev/null
