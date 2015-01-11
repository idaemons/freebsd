#!/bin/sh
#
# $FreeBSD$
#

set -e

export ASSUME_ALWAYS_YES="YES"
export PKG_DBDIR="/tmp/pkg"
export PERMISSIVE="YES"
export REPO_AUTOUPDATE="NO"
export PKGCMD="/usr/sbin/pkg -d"

_DVD_PACKAGES="archivers/unzip
devel/subversion
devel/subversion-static
emulators/linux_base-f10
misc/freebsd-doc-all
net/mpd5
net/rsync
ports-mgmt/pkg
ports-mgmt/portmaster
shells/bash
shells/zsh
security/sudo
sysutils/screen
www/firefox
www/links
x11-drivers/xf86-video-vmware
x11/gnome3
x11/kde4
x11/xorg"

# If NOPORTS is set for the release, do not attempt to build pkg(8).
if [ ! -f /usr/ports/Makefile ]; then
	echo "*** /usr/ports is missing!    ***"
	echo "*** Skipping pkg-stage.sh     ***"
	echo "*** Unset NOPORTS to fix this ***"
	exit 0
fi

if [ ! -x /usr/local/sbin/pkg ]; then
	/etc/rc.d/ldconfig restart
	/usr/bin/make -C /usr/ports/ports-mgmt/pkg install clean
fi

export DVD_DIR="dvd/packages"
export PKG_ABI=$(pkg config ABI)
export PKG_ALTABI=$(pkg config ALTABI 2>/dev/null)
export PKG_REPODIR="${DVD_DIR}/${PKG_ABI}"

/bin/mkdir -p ${PKG_REPODIR}
if [ ! -z "${PKG_ALTABI}" ]; then
	(cd ${DVD_DIR} && ln -s ${PKG_ABI} ${PKG_ALTABI})
fi

# Ensure the ports listed in _DVD_PACKAGES exist to sanitize the
# final list.
for _P in ${_DVD_PACKAGES}; do
	if [ -d "/usr/ports/${_P}" ]; then
		DVD_PACKAGES="${DVD_PACKAGES} ${_P}"
	else
		echo "*** Skipping nonexistent port: ${_P}"
	fi
done

# Make sure the package list is not empty.
if [ -z "${DVD_PACKAGES}" ]; then
	echo "*** The package list is empty."
	echo "*** Something is very wrong."
	# Exit '0' so the rest of the build process continues
	# so other issues (if any) can be addressed as well.
	exit 0
fi

# Print pkg(8) information to make debugging easier.
${PKGCMD} -vv
${PKGCMD} update -f
${PKGCMD} fetch -o ${PKG_REPODIR} -d ${DVD_PACKAGES}

${PKGCMD} repo ${PKG_REPODIR}

# Always exit '0', even if pkg(8) complains about conflicts.
exit 0
