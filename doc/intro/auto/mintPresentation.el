(TeX-add-style-hook "mintPresentation"
 (lambda ()
    (TeX-add-symbols
     "mint")
    (TeX-run-style-hooks
     "graphicx"
     "listings"
     "xspace"
     "latex2e"
     "beamer12"
     "beamer"
     "12pt")))

