#pragma once



#define DELOVI_DEBUGGING_ENABLED
#ifdef NDEBUG
#	undef DELOVI_DEBUGGING_ENABLED
#endif // NDEBUG

#ifdef DELOVI_DEBUGGING_ENABLED
#	ifdef QT_NO_DEBUG_OUTPUT
#		undef QT_NO_DEBUG_OUTPUT
#	endif
#else
#	ifndef QT_NO_DEBUG_OUTPUT
#		define QT_NO_DEBUG_OUTPUT
#	endif
#endif // DELOVI_DEBUGGING_ENABLED
// QT_NO_WARNING_OUTPUT
// QT_FATAL_WARNINGS ==> warning -> exit

#define DELOVI_VERSION_MAJOR unsigned int(1)
#define DELOVI_VERSION_MINOR unsigned int(0)
#define DELOVI_VERSION_BUILD unsigned int(0)

#define DELOVI_FILE_DIALOG_OPTIONS QFileDialog::DontUseNativeDialog
#define DELOVI_FILE_DIALOG_OPTIONS 0

#define DELOVI_PREFERENCES_FILENAME ".delovi.cfg"
