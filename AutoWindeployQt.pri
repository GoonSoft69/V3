CONFIG += skip_target_version_ext

equals(TEMPLATE, lib) {
	EXTENSION = dll
} else {
	EXTENSION = exe
}

ARGUMENTS += \
	--no-quick-import \
	--no-translations \
	--no-system-d3d-compiler \
	--no-virtualkeyboard \
	--no-opengl-sw \

lessThan(QT_MAJOR_VERSION, 6) {
	ARGUMENTS += \
		--no-webkit2 \
		--no-angle \
}

CONFIG(release, debug | release) {
	QMAKE_POST_LINK += $$[QT_INSTALL_BINS]/windeployqt $$ARGUMENTS $$shell_path($$DESTDIR/$${TARGET}.$$EXTENSION)
}
