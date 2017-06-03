system(rm -f Makefile) # Force always running of qmake

system(cd $$PWD ; mkdir -p bin/GameDebug/obj      bin/GameDebug/lib)
system(cd $$PWD ; mkdir -p bin/GameRelease/obj    bin/GameRelease/lib)
system(cd $$PWD ; mkdir -p bin/EditorDebug/obj    bin/EditorDebug/lib)
system(cd $$PWD ; mkdir -p bin/EditorRelease/obj  bin/EditorRelease/lib)
system(cd $$PWD ; sh ./scripts/createIncludeHeaders.sh)

TEMPLATE = subdirs

SUBDIRS = \
BangDataStructures \
BangLib \
BangMain \
BangPreprocessor

BangPreprocessor.depends += BangDataStructures
BangLib.depends          += BangDataStructures BangPreprocessor
BangMain.depends         += BangDataStructures BangLib
