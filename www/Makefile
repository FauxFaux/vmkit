

BIB_PAGES=Publications
BIB_FILES=$(patsubst %,%.php,$(BIB_PAGES) $(patsubst %,%_bib, $(BIB_PAGES)) $(patsubst %,%_abstracts, $(BIB_PAGES)))

.PHONY: all deploy tidy clean distclean
.SECONDARY: 
.SUFFIXES:

all: .biblio

.biblio: vmkit.bib ab-full.bib Makefile generate-biblio.sh
	./generate-biblio.sh vmkit.bib ab-full.bib 2000 2100 . && touch $@

deploy: tidy
	rsync -av * gaelthomas@scm.gforge.inria.fr:/home/groups/vmkit2/htdocs/

tidy:
	find . \( -iname "*~" -o -iname "\#*" -o -iname ".DS_Store" \) -exec rm -f {} \;

clean:

distclean:
	rm -f $(BIB_FILES)
