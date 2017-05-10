MD_FILES := $(wildcard ../src/*.md)
HTML_FILES := $(subst ../src/, , $(patsubst %.md, %.html, $(MD_FILES)))
PDF_FILES := $(patsubst %.html,%.pdf,$(HTML_FILES))

THEME_DIR= ../theme/boring
SLIDE_COMPILER= landslide
PDF_COMPILER= wkhtmltopdf

.PHONY: all
all: $(HTML_FILES) $(PDF_FILES)

%.html: ../src/%.md
	cp $(<) $*.md
	$(SLIDE_COMPILER) -c -d $(@) -r -t $(THEME_DIR) $*.md
	rm $*.md

%.pdf: %.html
	$(PDF_COMPILER) --keep-relative-links $(<) $(@)
