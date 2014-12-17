#############################################################################
# Makefile for building: Imageviewer.app/Contents/MacOS/Imageviewer
# Generated by qmake (3.0) (Qt 5.4.0)
# Project:  Imageviewer.pro
# Template: app
# Command: /Users/rainerschlonvoigt/Qt/5.4/clang_64/bin/qmake -spec macx-xcode -o Imageviewer.xcodeproj/project.pbxproj Imageviewer.pro
#############################################################################

MAKEFILE      = project.pbxproj

MOC       = /Users/rainerschlonvoigt/Qt/5.4/clang_64/bin/moc
UIC       = /Users/rainerschlonvoigt/Qt/5.4/clang_64/bin/uic
LEX       = flex
LEXFLAGS  = 
YACC      = yacc
YACCFLAGS = -d
DEFINES       = -DQT_NO_DEBUG -DQT_PRINTSUPPORT_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
INCPATH       = -I../../../Qt/5.4/clang_64/mkspecs/macx-clang -I. -I../../MasterThesis/source/Commonlib/include -I../../../Qt/5.4/clang_64/lib/QtPrintSupport.framework/Versions/5/Headers -I../../../Qt/5.4/clang_64/lib/QtWidgets.framework/Versions/5/Headers -I../../../Qt/5.4/clang_64/lib/QtGui.framework/Versions/5/Headers -I../../../Qt/5.4/clang_64/lib/QtCore.framework/Versions/5/Headers -I. -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.10.sdk/System/Library/Frameworks/AGL.framework/Headers -F/Users/rainerschlonvoigt/Qt/5.4/clang_64/lib
DEL_FILE  = rm -f
MOVE      = mv -f

IMAGES = 
PARSERS =
preprocess: $(PARSERS) compilers
clean preprocess_clean: parser_clean compiler_clean

parser_clean:
mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

check: first

compilers: moc_imageviewer.cpp moc_animateddrawer.cpp
compiler_objective_c_make_all:
compiler_objective_c_clean:
compiler_rcc_make_all:
compiler_rcc_clean:
compiler_moc_header_make_all: moc_imageviewer.cpp moc_animateddrawer.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_imageviewer.cpp moc_animateddrawer.cpp
moc_imageviewer.cpp: ../../../Qt/5.4/clang_64/lib/QtWidgets.framework/Versions/5/Headers/QMainWindow \
		../../../Qt/5.4/clang_64/lib/QtPrintSupport.framework/Versions/5/Headers/QPrinter \
		imageviewer.h
	/Users/rainerschlonvoigt/Qt/5.4/clang_64/bin/moc $(DEFINES) -D__APPLE__ -D__GNUC__=4 -I/Users/rainerschlonvoigt/Qt/5.4/clang_64/mkspecs/macx-clang -I/Users/rainerschlonvoigt/Projekte/CPU_GL/Imageviewer -I/Users/rainerschlonvoigt/Projekte/MasterThesis/source/Commonlib/include -I/Users/rainerschlonvoigt/Qt/5.4/clang_64/lib/QtPrintSupport.framework/Headers -I/Users/rainerschlonvoigt/Qt/5.4/clang_64/lib/QtWidgets.framework/Headers -I/Users/rainerschlonvoigt/Qt/5.4/clang_64/lib/QtGui.framework/Headers -I/Users/rainerschlonvoigt/Qt/5.4/clang_64/lib/QtCore.framework/Headers -F/Users/rainerschlonvoigt/Qt/5.4/clang_64/lib imageviewer.h -o moc_imageviewer.cpp

moc_animateddrawer.cpp: triangledb.h \
		../../MasterThesis/source/Commonlib/include/vec3d.h \
		framebuffer.h \
		../../../Qt/5.4/clang_64/lib/QtGui.framework/Versions/5/Headers/QImage \
		triangledrawer.h \
		../../MasterThesis/source/Commonlib/include/matrix4x4.h \
		imageviewer.h \
		../../../Qt/5.4/clang_64/lib/QtWidgets.framework/Versions/5/Headers/QMainWindow \
		../../../Qt/5.4/clang_64/lib/QtPrintSupport.framework/Versions/5/Headers/QPrinter \
		../../../Qt/5.4/clang_64/lib/QtCore.framework/Versions/5/Headers/QObject \
		animateddrawer.h
	/Users/rainerschlonvoigt/Qt/5.4/clang_64/bin/moc $(DEFINES) -D__APPLE__ -D__GNUC__=4 -I/Users/rainerschlonvoigt/Qt/5.4/clang_64/mkspecs/macx-clang -I/Users/rainerschlonvoigt/Projekte/CPU_GL/Imageviewer -I/Users/rainerschlonvoigt/Projekte/MasterThesis/source/Commonlib/include -I/Users/rainerschlonvoigt/Qt/5.4/clang_64/lib/QtPrintSupport.framework/Headers -I/Users/rainerschlonvoigt/Qt/5.4/clang_64/lib/QtWidgets.framework/Headers -I/Users/rainerschlonvoigt/Qt/5.4/clang_64/lib/QtGui.framework/Headers -I/Users/rainerschlonvoigt/Qt/5.4/clang_64/lib/QtCore.framework/Headers -F/Users/rainerschlonvoigt/Qt/5.4/clang_64/lib animateddrawer.h -o moc_animateddrawer.cpp

compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_rez_source_make_all:
compiler_rez_source_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean 

