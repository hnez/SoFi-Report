THEME_DIR= ../theme/boring
SLIDE_COMPILER= landslide
NOTES_COMPILER= pandoc

.PHONY: all
all: sofi.html notes.html

sofi.html: ../src/sofi.md
	cp $(<) $*.md
	$(SLIDE_COMPILER) -c -d $(@) -r -t $(THEME_DIR) $*.md
	rm $*.md

notes.html: ../src/notes.md style.html.inc
	$(NOTES_COMPILER) -o $@ $< -H style.html.inc
