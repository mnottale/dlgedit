TEMPLATE = app
FORMS = dlgedit.ui MainWindow.ui ExprBuilder.ui
SOURCES = script.cpp dropzone.cpp flowlayout.cpp main.cpp previewer.cpp node.cpp dialog.cpp exprbuilder.cpp expr.cpp toolset.cpp
HEADERS = script.hh flowlayout.h main.hh previewer.hh node.hh dialog.hh imagelibrary.hh exprbuilder.hh expr.hh dropzone.hh toolset.hh
CONFIG += debug_and_release
QMAKE_CXXFLAGS += -g -std=c++17 -fPIC
QT += network widgets
