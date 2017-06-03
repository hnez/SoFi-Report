# To me LaTeX seems to be held together by ducttape, faith and good luck.
# I replaced faith with hot glue to write this build system.
# It will most likely not work for you, heck it most likely wont
# even work for me in a few months.

LUALATEX= lualatex
LATEXRUN= ./latexrun
TEXINPUTS= .:..:../theme:

BIBTEX= biber

export TEXINPUTS

all: stepone latex.out/Report.gls latex.out/Report.bbl steptwo

.PHONY: stepone
stepone:
	${LATEXRUN} --latex-cmd ${LUALATEX} --bibtex-cmd ${BIBTEX} --bibtex-args "--input-directory ../" Report.tex

.PHONY: steptwo
steptwo:
	${LATEXRUN} --latex-cmd ${LUALATEX} --bibtex-cmd ${BIBTEX} --bibtex-args "--input-directory ../" Report.tex

latex.out/Report.gls: latex.out/Report.aux
	cd latex.out; makeglossaries Report

latex.out/Report.bbl: latex.out/Report.bcf
	cd latex.out; ${BIBTEX} --input-directory ../ Report.bcf
