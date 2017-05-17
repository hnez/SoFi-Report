# To me LaTeX seems to be held together by ducttape, faith and good luck.
# I replaced faith with hot glue to write this build system.
# It will most likely not work for you, heck it most likely wont
# even work for me in a few months.

LUALATEX= lualatex
LATEXRUN= ./latexrun
TEXINPUTS= .:..:../theme:

BIBTEX= biber

export TEXINPUTS

.PHONY: report
report:
	${LATEXRUN} --latex-cmd ${LUALATEX} --bibtex-cmd ${BIBTEX} --bibtex-args "--input-directory  ../" --verbose-cmds Report.tex
