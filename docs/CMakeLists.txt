project(paper NONE)
include(UseLATEX.cmake)
add_latex_document(
	paper.tex 
	BIBFILES references.bib
	IMAGE_DIRS images
	FORCE_PDF
)
