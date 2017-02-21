# Contributor: Erik Ogan <erik@stealthymonkeys.com>
# Maintainer: Erik Ogan <erik@stealthymonkeys.com>
pkgname=wait4ports
pkgver=0.0.4
pkgrel=0
pkgdesc="A small utility to wait for network peer availability."
url="https://github.com/erikogan/wait4ports"
arch="all"
license="MPL 2.0"
depends=""
makedepends=""
install=""
#subpackages="$pkgname-dev $pkgname-doc"
source="${pkgname}-${pkgver}.tar.gz::https://github.com/erikogan/wait4ports/archive/v${pkgver}.tar.gz"
builddir="$srcdir/$pkgname-$pkgver"


build() {
	cd "$builddir"
	make || return 1
}

package() {
	cd "$builddir"
	make PREFIX="$pkgdir" install || return 1
}

md5sums="7fcbad5c0ac70a088d1e6404aaf832c7  wait4ports-0.0.3.tar.gz"
sha256sums="7b4aaaea40397aa9712556fd7ab5b79dba3e4a16431dc2bd0d6245c573f5f43d  wait4ports-0.0.3.tar.gz"
sha512sums="05a6e4db291cd20fa3aa3d400e451596ff8dfe528c99afbd88c05106f6cd00480d77254e02dc6e3258eb3bd60b81a176395496b0cd4516bc0b44544b0d0c1bdb  wait4ports-0.0.3.tar.gz"
