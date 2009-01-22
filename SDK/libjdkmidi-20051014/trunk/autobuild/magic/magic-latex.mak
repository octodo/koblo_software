
LATEX_DOCS_TEX=$(call get_file_list,$(LIB_DOCS_DIR),tex)
LATEX_DOCS_TEX_TMP=$(addprefix $(LATEX_BUILD_DIR)/,$(call get_file_list,$(LIB_DOCS_DIR),tex))
LATEX_DOCS_PDF=$(addprefix $(OUTPUT_DOCS_DIR)/,$(LATEX_DOCS_TEX:.tex=.pdf))
LATEX_DOCS_HTML=$(addprefix $(OUTPUT_DOCS_DIR)/,$(LATEX_DOCS_TEX:.tex=.html))
LATEX_DOCS_CSS=$(addprefix $(OUTPUT_DOCS_DIR)/,$(LATEX_DOCS_TEX:.tex=.css))
DOCS=$(LATEX_DOCS_TEX_TMP) $(LATEX_DOCS_PDF) #$(LATEX_DOCS_HTML) $(LATEX_DOCS_CSS)


LATEX?=latex
MAKEINDEX?=makeindex
PDFLATEX?=pdflatex
DVIPS?=dvips
PS2PDF?=ps2pdf
HTLATEX?=htlatex

LATEX_OPTS+= --shell-escape
MAKEINDEX_OPTS=
DVIPS_OPTS+=-Ppdf
PS2PDF_OPTS+=
HTLATEX_OPTS+=
PDFLATEX_OPTS+= --shell-escape
LATEX_BUILD_DIR=$(BUILD_DIR)/latex
ALL_OUTPUT_DIRS+=$(LATEX_BUILD_DIR)

$(LATEX_BUILD_DIR)/%.tex : %.tex
	$(CP) $(<) $(@)

$(OUTPUT_DOCS_DIR)/%.dvi : $(LATEX_BUILD_DIR)/%.dvi
	$(CP) $(<) $(@)

$(OUTPUT_DOCS_DIR)/%.ps : $(LATEX_BUILD_DIR)/%.ps
	$(CP) $(<) $(@)

$(OUTPUT_DOCS_DIR)/%.pdf : $(LATEX_BUILD_DIR)/%.pdf
	$(CP) $(<) $(@)

$(OUTPUT_DOCS_DIR)/%.html : $(LATEX_BUILD_DIR)/%.html
	$(CP) $(<) $(@)

$(OUTPUT_DOCS_DIR)/%.css : $(LATEX_BUILD_DIR)/%.css
	$(CP) $(<) $(@)

$(LATEX_BUILD_DIR)/%.dvi : $(LATEX_BUILD_DIR)/%.tex
	(cd $(LATEX_BUILD_DIR) && \
	$(LATEX) $(LATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' && \
	$(LATEX) $(LATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' && \
	$(MAKEINDEX) $(MAKEINDEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' && \
	$(LATEX) $(LATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' && \
	$(LATEX) $(LATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' \
	)

$(LATEX_BUILD_DIR)/%.pdf : $(LATEX_BUILD_DIR)/%.tex
	(cd $(LATEX_BUILD_DIR) && \
	$(PDFLATEX) $(PDFLATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' && \
	$(PDFLATEX) $(PDFLATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' && \
	$(MAKEINDEX) $* && \
	$(PDFLATEX) $(PDFLATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' && \
	$(PDFLATEX) $(PDFLATEX_OPTS) '\newcommand{\topdir}{$(PROJECT_TOP_DIR)}\input{$*.tex}' \
	)

$(LATEX_BUILD_DIR)/%.ps : $(LATEX_BUILD_DIR)/%.dvi
	(cd $(LATEX_BUILD_DIR) &&  \
	$(DVIPS) $(DVIPS_OPTS) $* \
	)

$(LATEX_BUILD_DIR)/%.html $(LATEX_BUILD_DIR)/%.css : $(LATEX_BUILD_DIR)/%.tex
	(cd $(LATEX_BUILD_DIR) &&   \
	$(HTLATEX) $(HTLATEX_OPTS) $* \
	)	



