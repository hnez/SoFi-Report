DIAGRAM_FILES := $(wildcard ../../../diagrams/*.tex)
RENDER_FILES := $(subst ../../../diagrams/, , $(patsubst %.tex, %.render.tex, $(DIAGRAM_FILES)))
PDF_FILES := $(patsubst %.render.tex, %.pdf, $(RENDER_FILES))
SVG_FILES := $(patsubst %.pdf, %.svg, $(PDF_FILES))

LUALATEX= lualatex
LATEXRUN= ./latexrun
TEXINPUTS= .:..:../../../:

export TEXINPUTS

.PHONY: all
all: ${SVG_FILES}

%.render.tex: ../../../diagrams/%.tex
	$(file > $(@),\RequirePackage{luatex85})
	$(file >> $(@),\documentclass{standalone})
	$(file >> $(@),\usepackage{amsmath})
	$(file >> $(@),\usepackage{siunitx})
	$(file >> $(@),\usepackage{verbatim})
	$(file >> $(@),\usepackage[usenames,dvipsnames]{xcolor})
	$(file >> $(@),\usepackage{bm})
	$(file >> $(@),\usepackage{tikz})
	$(file >> $(@),\usepackage{circuitikz})
	$(file >> $(@),\usetikzlibrary{shapes,arrows,positioning,calc})
	$(file >> $(@),\begin{document})
	$(file >> $(@), \input{$(<)})
	$(file >> $(@),\end{document})

%.pdf: %.render.tex
	${LATEXRUN} --latex-cmd ${LUALATEX} $(<) -o $(@)

%.svg: %.pdf
	pdf2svg $(<) $(@)
